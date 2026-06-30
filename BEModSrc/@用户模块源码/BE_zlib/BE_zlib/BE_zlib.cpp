/**@ModuleTitle:
*  @version:     1.0
*  @platform:    win32(x86|x64)
*  @compiler:    msvc141
*  @author:
*  @datetime:
*  @description:
*/
#include "stdafx.h"
#include "native\zlib.h"

//#beExcludeNamespaceSymbol(Bytes)

Bytes GZIP压缩(const Bytes& 待压缩数据) {
	if (待压缩数据._size() == 0) return Bytes();

	z_stream strm;
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;

	// windowBits = 15 + 16 表示开启 gzip 格式支持
	if (deflateInit2(&strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 15 + 16, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
		return Bytes();
	}

	strm.avail_in = (uInt)待压缩数据._size();
	strm.next_in = (Bytef*)待压缩数据._buf();

	Bytes 结果字节集;
	unsigned char 临时缓冲区[16384];
	int 状态;

	do {
		strm.avail_out = sizeof(临时缓冲区);
		strm.next_out = 临时缓冲区;
		状态 = deflate(&strm, Z_FINISH);

		size_t 已压缩大小 = sizeof(临时缓冲区) - strm.avail_out;
		if (已压缩大小 > 0) {
			结果字节集.append(临时缓冲区, 已压缩大小);
		}
	} while (状态 != Z_STREAM_END && 状态 == Z_OK);

	deflateEnd(&strm);
	return 结果字节集;
}

// Gzip 解压函数
Bytes GZIP解压(const Bytes& 待解压数据) {
	if (待解压数据._size() == 0) return Bytes();

	z_stream strm;
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = (uInt)待解压数据._size();
	strm.next_in = (Bytef*)待解压数据._buf();

	// windowBits = 15 + 16 表示开启 gzip 格式支持
	if (inflateInit2(&strm, 15 + 16) != Z_OK) return Bytes();

	Bytes 结果字节集;
	unsigned char 临时缓冲区[16384];
	int 状态;

	do {
		strm.avail_out = sizeof(临时缓冲区);
		strm.next_out = 临时缓冲区;
		状态 = inflate(&strm, Z_NO_FLUSH);

		if (状态 == Z_NEED_DICT || 状态 == Z_DATA_ERROR || 状态 == Z_MEM_ERROR) {
			inflateEnd(&strm);
			return Bytes();
		}

		size_t 已解压大小 = sizeof(临时缓冲区) - strm.avail_out;
		if (已解压大小 > 0) {
			结果字节集.append(临时缓冲区, 已解压大小);
		}

	} while (状态 != Z_STREAM_END);

	inflateEnd(&strm);
	return 结果字节集;
}