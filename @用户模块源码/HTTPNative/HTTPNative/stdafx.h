#pragma once

#include <stdio.h>
//#include <windows.h>
#include "BEMod/BEMod.h"

/**
 * 根据 HTTP Header 判断并进行 Gzip 解压
 */
inline Bytes _HTTPNative_GZIP判断(const StrA& header, const Bytes& body) {
	if (header.len() == 0 || body.size == 0) return body;
	const char* p = header;
	bool gzip = false;
	for (int i = 0; i <= (int)header.len() - 24; ++i) {
		if (memcmp(p + i, "\r\nContent-Encoding: gzip", 24) == 0) {
			gzip = true;
			break;
		}
	}
	if (gzip) {
		return BEdecode::GZIP(body);
	}
	return body;
}