#include "stdafx.h"

namespace {
#ifndef _WIN64
#pragma pack(push, 4)
#endif
struct NTCps错误 {
	int 操作;
	int 类型;
	BOOL 有错误;
};

struct NTCps通知信息 {
	long 大小;
	char* 文本1;
	char* 文本2;
	char* 文本3;
	void* 自定义参数;
	INT_PTR 文件句柄;
	USHORT 日期;
	USHORT 时间;
	USHORT 属性;
	USHORT 包标识;
	USHORT 包索引;
	USHORT 文件夹索引;
	int 错误;
};
#ifndef _WIN64
#pragma pack(pop)
#endif

using NTCps分配回调 = void* (__cdecl*)(ULONG);
using NTCps释放回调 = void(__cdecl*)(void*);
using NTCps打开回调 = INT_PTR(__cdecl*)(char*, int, int);
using NTCps读取回调 = UINT(__cdecl*)(INT_PTR, void*, UINT);
using NTCps写入回调 = UINT(__cdecl*)(INT_PTR, void*, UINT);
using NTCps关闭回调 = int(__cdecl*)(INT_PTR);
using NTCps定位回调 = long(__cdecl*)(INT_PTR, long, int);
using NTCps通知回调 = INT_PTR(__cdecl*)(int, NTCps通知信息*);
using NTCps创建函数 = void* (__cdecl*)(NTCps分配回调, NTCps释放回调, NTCps打开回调,
	NTCps读取回调, NTCps写入回调, NTCps关闭回调, NTCps定位回调, int, NTCps错误*);
using NTCps解压函数 = BOOL(__cdecl*)(void*, char*, char*, int, NTCps通知回调, void*, void*);
using NTCps销毁函数 = BOOL(__cdecl*)(void*);

struct NTCps解压上下文 {
	const Bytes* 输入;
	Bytes* 输出;
	size_t 输入位置;
	size_t 输出位置;
};

//thread_local NTCps解压上下文* 当前NTCps任务 = nullptr;
//如果你能保证：
//不会从工作线程调用 NTCps::解压()；
//不会同时解压两个包；
//解压回调不会发生嵌套；
NTCps解压上下文* 当前NTCps任务 = nullptr;

void* __cdecl NTCps分配(ULONG 大小) { return malloc(大小); }
void __cdecl NTCps释放(void* 地址) { free(地址); }

INT_PTR __cdecl NTCps打开(char*, int, int) {
	当前NTCps任务->输入位置 = 0;
	return 1;
}

UINT __cdecl NTCps读取(INT_PTR, void* 缓冲区, UINT 大小) {
	size_t 剩余 = 当前NTCps任务->输入->size - 当前NTCps任务->输入位置;
	if (大小 > 剩余) 大小 = (UINT)剩余;
	memcpy(缓冲区, 当前NTCps任务->输入->buf + 当前NTCps任务->输入位置, 大小);
	当前NTCps任务->输入位置 += 大小;
	return 大小;
}

UINT __cdecl NTCps写入(INT_PTR, void* 缓冲区, UINT 大小) {
	memcpy(当前NTCps任务->输出->buf + 当前NTCps任务->输出位置, 缓冲区, 大小);
	当前NTCps任务->输出位置 += 大小;
	return 大小;
}

int __cdecl NTCps关闭(INT_PTR) { return 0; }

long __cdecl NTCps定位(INT_PTR 句柄, long 距离, int 起点) {
	size_t* 位置 = 句柄 == 1 ? &当前NTCps任务->输入位置 : &当前NTCps任务->输出位置;
	long 基准 = 起点 == SEEK_SET ? 0 : 起点 == SEEK_CUR ? (long)*位置 :
		(long)(句柄 == 1 ? 当前NTCps任务->输入->size : 当前NTCps任务->输出->size);
	return (long)(*位置 = 基准 + 距离);
}

INT_PTR __cdecl NTCps通知(int 类型, NTCps通知信息* 信息) {
	NTCps解压上下文* 任务 = (NTCps解压上下文*)信息->自定义参数;
	if (类型 == 2) { // fdintCOPY_FILE
		任务->输出->reset((size_t)信息->大小);
		任务->输出位置 = 0;
		return 2;
	}
	return 类型 == 3; // fdintCLOSE_FILE_INFO
}

bool 运行NTCps压缩(c_StrW 文件名, c_StrW 命令参数) {
	return 运行(文件名, 命令参数, true, 1);
}

void 破坏NTCps文件头(c_StrW 文件名) {
	HANDLE 文件 = CreateFileW(文件名, GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
	DWORD 假头 = 0x2153504E, 已写入;
	WriteFile(文件, &假头, sizeof(假头), &已写入, nullptr); // "NPS!"
	CloseHandle(文件);
}
}

// 扩展名为 .ntcps 的单成员 CAB/LZX 压缩与内存解压包装类
class EXP NTCps {
public:
	// 直接把一个文件压缩为 LZX CAB 文件。
	static bool 压缩(c_StrX 待压缩文件, c_StrX 压缩包路径, int 压缩等级 = 6) {
		wchar_t 系统目录[MAX_PATH];
		if (!GetSystemDirectoryW(系统目录, MAX_PATH)) return false;
		if (压缩等级 < 0) 压缩等级 = 0;
		if (压缩等级 > 10) 压缩等级 = 10;
		int LZX窗口 = 15 + (压缩等级 * 6 + 5) / 10;
		StrW 输出路径 = (const wchar_t*)压缩包路径;
		DeleteFileW(输出路径);
		StrW 程序 = StrW(系统目录) + L"\\makecab.exe";
		StrW 参数 = sprintF<W>(L"/D CompressionType=LZX /D CompressionMemory=%d /D Cabinet=on /D Compress=on \"", LZX窗口) +
			StrW((const wchar_t*)待压缩文件) + L"\" \"" + 输出路径 + L"\"";
		bool 成功 = 运行NTCps压缩(程序, 参数);
		if (成功) 破坏NTCps文件头(输出路径);
		return 成功;
	}

	// 直接从内存中的单成员 CAB 解压到内存。
	static bool 解压(c_Bytes 待解压数据, Bytes& 解压后数据) {
		解压后数据.reset();
		Bytes NTCps数据 = 待解压数据;
		*(DWORD*)NTCps数据.buf = 0x4643534D; // 恢复 "MSCF"
		HMODULE Cabinet = LoadLibraryW(L"cabinet.dll");
		auto 创建 = (NTCps创建函数)GetProcAddress(Cabinet, "FDICreate");
		auto 解包 = (NTCps解压函数)GetProcAddress(Cabinet, "FDICopy");
		auto 销毁 = (NTCps销毁函数)GetProcAddress(Cabinet, "FDIDestroy");

		NTCps错误 错误 = {};
		void* fdi = 创建(NTCps分配, NTCps释放, NTCps打开, NTCps读取,
			NTCps写入, NTCps关闭, NTCps定位, -1, &错误);
		NTCps解压上下文 任务 = { &NTCps数据, &解压后数据, 0, 0 };
		当前NTCps任务 = &任务;
		bool 成功 = 解包(fdi, (char*)"data.ntcps", (char*)"", 0, NTCps通知, nullptr, &任务);
		当前NTCps任务 = nullptr;
		销毁(fdi);
		FreeLibrary(Cabinet);
		return 成功;
	}

	static HMODULE 加载DLL(c_Bytes compressed, c_StrX filename = "") {
		Bytes uncompressed;
		if (!解压(compressed, uncompressed)) {
			return NULL;
		}
		StrA target_path;
		if (!filename) {
			target_path = 取临时文件名(取特定目录(目录::临时文件目录));
		} else if (是否为相对路径(filename)) {
			target_path = 取特定目录(目录::临时文件目录) + "\\" + filename;
		} else {
			target_path = filename;
		}
		if (!target_path) {
			return NULL;
		}
		写到文件Ex(target_path, uncompressed); // 无论写入成功与否均尝试加载（写入失败可能是已加锁，此时直接加载已有文件）
		动态库 dll;
		if (dll.加载(target_path)) {
			HMODULE hMod = dll.hModule;
			dll.hModule = NULL; // 脱离接管，防止析构时卸载
			return hMod;
		}
		return NULL;
	}
};
