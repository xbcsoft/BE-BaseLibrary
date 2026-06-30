/**@ModuleTitle: Web常用编解码
*  @version:     1.0
*  @platform:    win32(x86|x64)
*  @compiler:    source
*  @author:      
*  @datetime:    
*  @description:
*/
#include "stdafx.h"
#pragma comment(lib, "crypt32.lib")
namespace { using namespace _MBEWebCodec; }


namespace BEdecode
{
/**
* URL解码 (公开函数)
* 支持 %XX 和 + 号还原
*/
StrA URL(const StrA& 欲解码文本) {
	size_t len = 欲解码文本.len();
	if (len == 0) return StrA("");

	const char* p = 欲解码文本;
	Bytes buf(len); byte* out = buf.buf;
	size_t outIdx = 0;

	for (size_t i = 0; i < len; ++i) {
		if (p[i] == '%') {
			if (i + 2 < len) {
				char hex[3] = { p[i + 1], p[i + 2], 0 };
				out[outIdx++] = (byte)strtol(hex, nullptr, 16);
				i += 2;
			}
		} else if (p[i] == '+') {
			out[outIdx++] = ' ';
		} else {
			out[outIdx++] = (byte)p[i];
		}
	}
	out[outIdx] = '\0';
	buf.size = outIdx;
	return (StrA&)buf;
}

/**
 * Base64 解码 (Win32 API)
 */
Bytes Base64(const StrA& 待解码文本) {
	if (待解码文本.len() == 0) return Bytes();
	DWORD outLen = 0;
	// 第一次调用获取所需长度
	if (!CryptStringToBinaryA(待解码文本, (DWORD)待解码文本.len(), CRYPT_STRING_BASE64, NULL, &outLen, NULL, NULL))
		return Bytes();

	Bytes res(outLen);
	if (!CryptStringToBinaryA(待解码文本, (DWORD)待解码文本.len(), CRYPT_STRING_BASE64, res.buf, &outLen, NULL, NULL))
		return Bytes();

	res.size = outLen;
	return res;
}

Bytes GZIP(const Bytes& 待解压数据) {
	return GZIP解压(待解压数据);
}

}

namespace BEencode
{
/**
 * URL编码 (公开函数)
 * 将特殊字符转换为 %XX 格式，空格转换为 +
 */
StrA URL(const StrA& 欲编码文本) {
	size_t len = 欲编码文本.len();
	if (len == 0) return StrA("");

	const char* p = 欲编码文本;
	// 预分配最大可能空间（每个字符可能变为 %XX，即 3 倍）
	Bytes buf(len * 3); byte* out = buf.buf;
	size_t outIdx = 0;

	static const char* hex = "0123456789ABCDEF";

	for (size_t i = 0; i < len; ++i) {
		unsigned char c = (unsigned char)p[i];
		if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') ||
			c == '-' || c == '_' || c == '.' || c == '~') {
			out[outIdx++] = c;
		} else if (c == ' ') {
			out[outIdx++] = '+';
		} else {
			out[outIdx++] = '%';
			out[outIdx++] = hex[c >> 4];
			out[outIdx++] = hex[c & 0x0F];
		}
	}
	out[outIdx] = '\0';
	buf.size = outIdx;
	return (StrA&)buf;
}

/**
 * Base64 编码 (Win32 API)
 */
StrA Base64(const Bytes& 待编码数据) {
	if (待编码数据.size == 0) return "";
	DWORD outLen = 0;
	// 第一次调用获取所需长度 (CRYPT_STRING_NOCRLF 去除换行符)
	if (!CryptBinaryToStringA(待编码数据.buf, (DWORD)待编码数据.size, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, NULL, &outLen))
		return "";

	StrA res(outLen - 1); // CryptBinaryToStringA 返回的长度包含空终止符
	if (!CryptBinaryToStringA(待编码数据.buf, (DWORD)待编码数据.size, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, (char*)res._buf(), &outLen))
		return "";

	return res;
}

Bytes GZIP(const Bytes& 待压缩数据) {
	return GZIP压缩(待压缩数据);
}
}

#include "HTTP键值对.hpp"