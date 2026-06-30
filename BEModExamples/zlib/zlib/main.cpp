#include "stdafx.h"
#include "BEMod/BEMod.h"

//需要引进curl库

// Gzip 压缩函数
字节集 Gzip压缩(const 字节集& 待压缩数据) {
	if (待压缩数据._size() == 0) return 字节集();

	z_stream strm;
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;

	// windowBits = 15 + 16 表示开启 gzip 格式支持
	if (deflateInit2(&strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 15 + 16, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
		return 字节集();
	}

	strm.avail_in = (uInt)待压缩数据._size();
	strm.next_in = (Bytef*)待压缩数据._buf();

	字节集 结果字节集;
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
字节集 Gzip解压(const 字节集& 待解压数据) {
	if (待解压数据._size() == 0) return 字节集();

	z_stream strm;
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = (uInt)待解压数据._size();
	strm.next_in = (Bytef*)待解压数据._buf();

	// windowBits = 15 + 16 表示开启 gzip 格式支持
	if (inflateInit2(&strm, 15 + 16) != Z_OK) return 字节集();

	字节集 结果字节集;
	unsigned char 临时缓冲区[16384];
	int 状态;

	do {
		strm.avail_out = sizeof(临时缓冲区);
		strm.next_out = 临时缓冲区;
		状态 = inflate(&strm, Z_NO_FLUSH);

		if (状态 == Z_NEED_DICT || 状态 == Z_DATA_ERROR || 状态 == Z_MEM_ERROR) {
			inflateEnd(&strm);
			return 字节集();
		}

		size_t 已解压大小 = sizeof(临时缓冲区) - strm.avail_out;
		if (已解压大小 > 0) {
			结果字节集.append(临时缓冲区, 已解压大小);
		}

	} while (状态 != Z_STREAM_END);

	inflateEnd(&strm);
	return 结果字节集;
}

void main()
{
	文本型 待测试文本 = "Hello, BEMod! Gzip Compression and Decompression Test with full C++ Mangling!";
	字节集 原始数据 = 到字节集(待测试文本);

	printf("原始数据大小: %d，内容: %s\n", 原始数据._size(), (char*)待测试文本);

	字节集 已压缩 = Gzip压缩(原始数据);
	printf("压缩后大小: %d\n", 已压缩._size());

	if (已压缩._size() > 0) {
		字节集 已解压 = Gzip解压(已压缩);

		if (已解压._size() > 0) {
			文本型 结果 = 到文本(已解压);
			printf("解压成功！内容: %s\n", (char*)结果);
		} else {
			printf("解压失败！\n");
		}
	} else {
		printf("压缩失败！\n");
	}

	// system("pause");
}

