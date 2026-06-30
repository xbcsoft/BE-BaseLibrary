#pragma once
#include "stdafx.h"

namespace {
//为什么要用它？（在 HTTP 原始报中的作用）
//在 TCP 通讯中，数据是按流式（Stream）到达的。你的 HTTP原始报服务器 会把收到的所有字节先塞进 httpBuffer。
//
//场景：假设缓冲区里现在有 1500 字节的数据，客户端那边可能在一个TCP包中一次性给出几个HTTP请求报文
//处理：_处理HTTP包 函数经过分析，发现前 800 字节正好是一个完整的 HTTP 请求。
//移除：处理完这 800 字节后，你需要把它们从缓冲区里删掉，剩下的 700 字节（可能是下一个请求的一部分）要挪到缓冲区的最开头。
//调用：_buffer_del(buffer, 0, totalNeeded);

void _buffer_del(Bytes& b, int start, int len)
{
	if (start < 0 || len <= 0 || start + len >(int)b.size) return;
	memmove((char*)b.buf + start, (char*)b.buf + start + len, b.size - (start + len));
	b.size -= len;
}
}

//此服务器实现单线程单客户同步阻塞性能低下，仅用作参考及简单测试，切勿应用在生产环境
class HTTP原始报服务器 : public TCP服务器
{
public:
	typedef void(*事件_客户进来)(HTTP原始报服务器& 服务器, SOCKET 客户句柄, int 客户端口);
	typedef void(*事件_客户数据到来)(HTTP原始报服务器& 服务器, SOCKET 客户句柄, c_Bytes 接收数据);
	typedef void(*事件_客户断开)(HTTP原始报服务器& 服务器, SOCKET 客户句柄, int 客户端口);

	inline static const char* 默认协议头 =
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html; charset=UTF-8\r\n"
		"Access-Control-Allow-Origin:*\r\n"
		"\r\n";

	bool 创建(int 端口,
		事件_客户进来 客户进来 = nullptr,
		事件_客户数据到来 客户数据到来 = nullptr,
		事件_客户断开 客户断开 = nullptr,
		bool 以新线程启动 = false,
		const char* 绑定监听网卡IP = "",
		int 接收缓冲区大小 = 20480)
	{
		return TCP服务器::创建(端口,
			(TCP服务器::事件_客户进来)客户进来,
			(TCP服务器::事件_客户数据到来)客户数据到来,
			(TCP服务器::事件_客户断开)客户断开,
			以新线程启动, 绑定监听网卡IP, 接收缓冲区大小);
	}

	/**发送 HTTP 响应，自动修正 Content-Length
	 * @param 客户句柄
	 * @param 协议头<可空>=HTTP原始报服务器::默认协议头
	 * @param 正文内容={}
	 * @return
	 */
	bool HTTP发送(SOCKET 客户句柄, 可空<c_StrU8> 协议头 = nil, c_Bytes 正文内容 = {})
	{
		StrA header;
		if (协议头 == nil) {
			header = 默认协议头;
		} else {
			header = (const char*)协议头.val;
		}

		if (header.len()==0) return false;
		const char* pHead = header;

		// 寻找 Header 结束符
		const char* pEnd = strstr(pHead, "\r\n\r\n");
		if (!pEnd) pEnd = strstr(pHead, "\n\n");

		StrA correctedHeader;
		if (pEnd) {
			int headLen = (int)(pEnd - pHead);
			correctedHeader = _BSA(pHead, headLen);
			// 确保 Content-Length 另起一行
			if (headLen > 0 && pHead[headLen - 1] != '\n') correctedHeader += "\r\n";
			correctedHeader += sprintF("Content-Length: %d\r\n\r\n", (int)正文内容.size);
		} else {
			correctedHeader = header;
			if (header.len() > 0 && pHead[header.len() - 1] != '\n') correctedHeader += "\r\n";
			correctedHeader += sprintF("Content-Length: %d\r\n\r\n", (int)正文内容.size);
		}

		// 使用【四参数构造函数】快速拼凑 Header + Body
		Bytes fullPack(correctedHeader._buf(), correctedHeader.len(),
			正文内容.buf, 正文内容.size);

		return 发送数据(客户句柄, fullPack);
	}

protected:
	virtual void _TCP_客户进入(SOCKET sClient, int 客户端口) override
	{
		if (_投参.客户进来) _投参.客户进来(*this, sClient, 客户端口);

		Bytes recvBuf(_投参.接收缓冲区大小);
		Bytes httpBuffer;

		while (true) {
			int r = recv(sClient, (char*)recvBuf.buf, _投参.接收缓冲区大小, 0);
			if (r <= 0) break;

			httpBuffer.append(recvBuf.buf, r);
			_处理HTTP包(httpBuffer, sClient, _投参.客户数据到来);
		}

		if (_投参.客户断开) _投参.客户断开(*this, sClient, 客户端口);
		closesocket(sClient);
	}

	void _处理HTTP包(Bytes& buffer, SOCKET sClient, TCP服务器::事件_客户数据到来 fn客户数据到来)
	{
		while (buffer.size > 0) {
			const char* p = (const char*)buffer.buf;
			int k = -1;
			// 寻找 HTTP/1.1 (或 HTTP/1.0)
			for (int i = 0; i <= (int)buffer.size - 8; ++i) {
				if (memcmp(p + i, "HTTP/1.", 7) == 0) {
					k = i;
					break;
				}
			}
			if (k == -1) break;

			int k_rnrn = -1;
			for (int i = k; i <= (int)buffer.size - 4; ++i) {
				if (memcmp(p + i, "\r\n\r\n", 4) == 0) {
					k_rnrn = i;
					break;
				}
			}
			if (k_rnrn == -1) break;

			int k1 = -1;
			for (int i = k; i <= k_rnrn - 16; ++i) {
				if (memcmp(p + i, "Content-Length: ", 16) == 0) {
					k1 = i;
					break;
				}
			}

			if (k1 == -1) {
				int packLen = k_rnrn + 4;
				if (fn客户数据到来) fn客户数据到来(*this, sClient, Bytes(buffer.buf, BRef, packLen));
				_buffer_del(buffer, 0, packLen);
				continue;
			}

			k1 += 16;
			int k2 = -1;
			for (int i = k1; i <= k_rnrn; ++i) {
				if (p[i] == '\r' && p[i + 1] == '\n') {
					k2 = i;
					break;
				}
			}
			if (k2 == -1) break;

			char tmp[32] = { 0 };
			int numLen = k2 - k1;
			if (numLen > 31) numLen = 31;
			memcpy(tmp, p + k1, numLen);
			int contentLen = atoi(tmp);

			int headerLen = k_rnrn + 4;
			int totalNeeded = headerLen + contentLen;

			if ((int)buffer.size < totalNeeded) break;

			if (fn客户数据到来) {
				Bytes body = _HTTPNative_GZIP判断(_BSA(buffer.buf, headerLen), Bytes(buffer.buf + headerLen, BRef, contentLen));
				if (body.buf != (byte*)buffer.buf + headerLen) {
					// 发生了解压，重新拼凑报文（Header + 解压后的 Body）
					fn客户数据到来(*this, sClient, Bytes(buffer, headerLen, body, body.size));
				} else {
					fn客户数据到来(*this, sClient, Bytes(buffer.buf, BRef, totalNeeded));
				}
			}

			_buffer_del(buffer, 0, totalNeeded);
		}
	}
};
