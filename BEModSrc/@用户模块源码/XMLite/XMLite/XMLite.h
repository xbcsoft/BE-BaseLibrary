#pragma once
/**@ModuleTitle:
*  @version:     1.0
*  @platform:    win32(x86|x64)
*  @compiler:    source
*  @author:
*  @datetime:
*  @description: 基于 XmlLite 实现的轻量级 XML DOM 操作类
*/
#include "BEMod/BEMod.h"
#include <xmllite.h>
#include <shlwapi.h>

#pragma comment(lib, "xmllite.lib")
#pragma comment(lib, "shlwapi.lib")

class XMLite {
public:
	struct XmlNode {
		StrW Name;
		StrW Text;
		HashTbe<StrW, StrW> Attributes;
		XmlNode* Parent = nullptr;
		Arraybe<XmlNode*> Children;

		~XmlNode() {
			for (auto* child : Children) {
				delete child;
			}
		}
	};

	const XmlNode* _取根节点指针() const {
		return m_root;
	}

private:
	XmlNode* m_root = nullptr;

public:
	XMLite() {
		m_root = new XmlNode();
		m_root->Name = L"#document";
	}

	~XMLite() {
		_销毁();
	}

	// 禁用拷贝
	XMLite(const XMLite&) = delete;
	XMLite& operator=(const XMLite&) = delete;

	// 移动构造
	XMLite(XMLite&& other) noexcept {
		m_root = other.m_root;
		other.m_root = nullptr;
	}

	// 移动赋值
	XMLite& operator=(XMLite&& other) noexcept {
		if (this != &other) {
			_销毁();
			m_root = other.m_root;
			other.m_root = nullptr;
		}
		return *this;
	}

	void _销毁() {
		if (m_root) {
			delete m_root;
			m_root = nullptr;
		}
	}

	void 清除() {
		_销毁();
		m_root = new XmlNode();
		m_root->Name = L"#document";
	}

	bool 导入(c_StrU8 XML数据) {
		return 导入W(AtoW(XML数据));
	}

	bool 导入W(c_StrW 宽字符文本) {
		清除();
		if (宽字符文本.len() == 0) return false;

		IStream* pStream = SHCreateMemStream((const BYTE*)(const wchar_t*)宽字符文本, 宽字符文本.bytes.size);
		if (!pStream) return false;

		IXmlReader* pReader = nullptr;
		if (FAILED(CreateXmlReader(__uuidof(IXmlReader), (void**)&pReader, nullptr))) {
			pStream->Release();
			return false;
		}

		if (FAILED(pReader->SetInput(pStream))) {
			pReader->Release();
			pStream->Release();
			return false;
		}

		XmlNode* current = m_root;
		XmlNodeType nodeType;
		while (pReader->Read(&nodeType) == S_OK) {
			switch (nodeType) {
			case XmlNodeType_Element: {
				const wchar_t* name;
				pReader->GetLocalName(&name, nullptr);
				XmlNode* node = new XmlNode();
				node->Name = name;
				node->Parent = current;
				current->Children.push(node);

				if (pReader->MoveToFirstAttribute() == S_OK) {
					do {
						const wchar_t* attrName;
						const wchar_t* attrValue;
						if (SUCCEEDED(pReader->GetLocalName(&attrName, nullptr)) &&
							SUCCEEDED(pReader->GetValue(&attrValue, nullptr))) {
							node->Attributes[StrW(attrName)] = StrW(attrValue);
						}
					} while (pReader->MoveToNextAttribute() == S_OK);
					pReader->MoveToElement();
				}

				if (!pReader->IsEmptyElement()) {
					current = node;
				}
				break;
			}
			case XmlNodeType_Text: {
				const wchar_t* text;
				pReader->GetValue(&text, nullptr);
				current->Text += StrW(text);
				break;
			}
			case XmlNodeType_EndElement: {
				if (current->Parent) {
					current = current->Parent;
				}
				break;
			}
			default:
				break;
			}
		}

		pReader->Release();
		pStream->Release();
		return true;
	}

	StrU8 导出() const {
		return WtoU8(导出W());
	}

	StrW 导出W() const {
		Bytes b = _导出为UNICODE();
		if (b.size >= 2 && b.buf[0] == 0xFF && b.buf[1] == 0xFE) {
			// 跳过 UTF-16 LE BOM
			return StrW((charW*)(b.buf + 2), (b.size - 2) / 2);
		}
		return BSW<true>(b);
	}

	Bytes _导出为UNICODE() const {
		if (!m_root) return Bytes();

		IStream* pStream = nullptr;
		if (FAILED(CreateStreamOnHGlobal(NULL, TRUE, &pStream))) return Bytes();

		IXmlWriter* pWriter = nullptr;
		if (FAILED(CreateXmlWriter(__uuidof(IXmlWriter), (void**)&pWriter, nullptr))) {
			pStream->Release();
			return Bytes();
		}

		IXmlWriterOutput* pOutput = nullptr;
		if (SUCCEEDED(CreateXmlWriterOutputWithEncodingName(pStream, nullptr, L"utf-16", &pOutput))) {
			pWriter->SetOutput(pOutput);
			pOutput->Release();
		} else {
			pWriter->SetOutput(pStream);
		}

		pWriter->SetProperty(XmlWriterProperty_Indent, TRUE);

		for (auto* child : m_root->Children) {
			_WriteNode(pWriter, child);
		}

		pWriter->Flush();

		LARGE_INTEGER liZero = { 0 };
		ULARGE_INTEGER uliPos = { 0 };
		pStream->Seek(liZero, STREAM_SEEK_CUR, &uliPos);

		HGLOBAL hGlobal;
		GetHGlobalFromStream(pStream, &hGlobal);
		void* ptr = GlobalLock(hGlobal);

		Bytes res;
		if (ptr && uliPos.QuadPart > 0) {
			res.reset((int)uliPos.QuadPart);
			memcpy(res.buf, ptr, (size_t)uliPos.QuadPart);
		}
		if (ptr) GlobalUnlock(hGlobal);

		pWriter->Release();
		pStream->Release();
		return res;
	}

	StrU8 取父节点名(c_StrU8 节点路径) {
		XmlNode* node = _FindNode(U8toW(节点路径));
		if (node && node->Parent && node->Parent != m_root) {
			return WtoU8(node->Parent->Name);
		}
		return "";
	}

	StrU8 取根节点名() {
		if (m_root && m_root->Children.count > 0) {
			return WtoU8(m_root->Children[0]->Name);
		}
		return "";
	}

	int 取子节点名称(c_StrU8 节点路径, Arraybe<StrU8>& 子节点名称) {
		子节点名称.clear();
		XmlNode* node = _FindNode(U8toW(节点路径));
		if (node) {
			for (auto* child : node->Children) {
				子节点名称.push(WtoU8(child->Name));
			}
		}
		return 子节点名称.count;
	}

	bool 删除节点(c_StrU8 节点路径) {
		XmlNode* node = _FindNode(U8toW(节点路径));
		if (node && node->Parent) {
			XmlNode* parent = node->Parent;
			for (int i = 0; i < parent->Children.count; i++) {
				if (parent->Children[i] == node) {
					parent->Children.del(i);
					delete node;
					return true;
				}
			}
		}
		return false;
	}

	bool 添加节点(c_StrU8 节点路径, c_StrU8 节点名称, 可空<c_StrU8> 节点内容 = nil) {
		StrW path = (节点路径.len() == 0) ? StrW(L"/") : U8toW(节点路径);
		XmlNode* parent = _FindNode(path);
		if (!parent) return false;

		XmlNode* node = new XmlNode();
		node->Name = U8toW(节点名称);
		if (节点内容 != nil) {
			node->Text = U8toW(节点内容);
		}
		node->Parent = parent;
		parent->Children.push(node);
		return true;
	}

	StrU8 取节点文本(c_StrU8 节点路径) {
		XmlNode* node = _FindNode(U8toW(节点路径));
		if (node) return WtoU8(node->Text);
		return "";
	}

	bool 置节点文本(c_StrU8 节点路径, c_StrU8 节点内容) {
		XmlNode* node = _FindNode(U8toW(节点路径));
		if (node) {
			node->Text = U8toW(节点内容);
			return true;
		}
		return false;
	}

	int 取子节点数(c_StrU8 节点路径) {
		XmlNode* node = _FindNode(U8toW(节点路径));
		if (node) return node->Children.count;
		return 0;
	}



private:
	XmlNode* _FindNode(const StrW& path) {
		if (!m_root) return nullptr;
		if (path.len() == 0 || path == L"/") return m_root;

		Arraybe<StrW> parts;
		Split(path, L"/", parts);

		XmlNode* current = m_root;
		for (auto& part : parts) {
			if (part.len() == 0) continue;

			bool found = false;
			for (auto* child : current->Children) {
				if (child->Name == part) {
					current = child;
					found = true;
					break;
				}
			}
			if (!found) return nullptr;
		}
		return current;
	}

	void _WriteNode(IXmlWriter* pWriter, XmlNode* node) const {
		pWriter->WriteStartElement(nullptr, node->Name, nullptr);

		for (auto& attr : node->Attributes) {
			pWriter->WriteAttributeString(nullptr, attr.key, nullptr, attr.value);
		}

		if (node->Text.len() > 0) {
			pWriter->WriteString(node->Text);
		}

		for (auto* child : node->Children) {
			_WriteNode(pWriter, child);
		}

		pWriter->WriteEndElement();
	}

	friend AutoStr __AutoStr__(const XMLite& xml) {
		return xml.导出();
	}
};
