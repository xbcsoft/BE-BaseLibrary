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



#pragma comment(lib, "xmllite.lib")
#pragma comment(lib, "shlwapi.lib")

class XMLite
{
public:
	struct XmlNode {
		StrW Name;
		StrW Text;
		HashTbe<StrW, StrW> Attributes;
		XmlNode* Parent = nullptr;
		Arraybe<XmlNode*> Children;

		~XmlNode();
	};

	const XmlNode* _取根节点指针() const;

private:
	XmlNode* m_root = nullptr;

public:
	XMLite();

	~XMLite();

	// 禁用拷贝
	XMLite(const XMLite&) = delete;
	XMLite& operator=(const XMLite&) = delete;

	// 移动构造
	XMLite(XMLite&& other) noexcept;

	// 移动赋值
	XMLite& operator=(XMLite&& other) noexcept;

	void _销毁();

	void 清除();

	bool 导入(c_StrU8 XML数据);

	bool 导入W(c_StrW 宽字符文本);

	StrU8 导出() const;

	StrW 导出W() const;

	Bytes _导出为UNICODE() const;

	StrU8 取父节点名(c_StrU8 节点路径);

	StrU8 取根节点名();

	int 取子节点名称(c_StrU8 节点路径, Arraybe<StrU8>& 子节点名称);

	bool 删除节点(c_StrU8 节点路径);

	bool 添加节点(c_StrU8 节点路径, c_StrU8 节点名称, 可空<c_StrU8> 节点内容 = nil);

	StrU8 取节点文本(c_StrU8 节点路径);

	bool 置节点文本(c_StrU8 节点路径, c_StrU8 节点内容);

	int 取子节点数(c_StrU8 节点路径);



private:
	XmlNode* _FindNode(const StrW& path);

	void _WriteNode(void* writerHandle, XmlNode* node) const;

	friend AutoStr __AutoStr__(const XMLite& xml);
};

AutoStr __AutoStr__(const XMLite& xml);
