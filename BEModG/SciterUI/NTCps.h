#pragma once
#include "stdafx.h"

// 扩展名为 .ntcps 的单成员 CAB/LZX 压缩与内存解压包装类
class NTCps {
public:
	// 直接把一个文件压缩为 LZX CAB 文件。
	static bool 压缩(c_StrX 待压缩文件, c_StrX 压缩包路径, int 压缩等级 = 6);

	// 直接从内存中的单成员 CAB 解压到内存。
	static bool 解压(c_Bytes 待解压数据, Bytes& 解压后数据);

	static HMODULE 加载DLL(c_Bytes compressed, c_StrX filename = "");
};
