#pragma once
#include "stdafx.h"

extern HMODULE g_sciterModule;

/**
 * 直接复用 Sciter 5.0.3.15(x86/x64) 内部的 ZIP 读取能力。
 * 这里只实现 SciterUI 加载内存资源所需的最小功能，不替代现有 Zip 类。
 */
class SciterZip
{
	struct 内存流
	{
		const byte* 数据 = nullptr;
		size_t 大小 = 0;
		size_t 位置 = 0;
	};

#ifdef _WIN64
	struct 文件回调
	{
		void* 打开64;
		void* 读取;
		void* 写入;
		void* 取位置64;
		void* 置位置64;
		void* 关闭;
		void* 取错误;
		void* 附加参数;
		void* 打开32;
		void* 取位置32;
		void* 置位置32;
	};

	using FN打开归档 = void* (__fastcall*)(const void*, 文件回调*, int);
	using FN取当前信息 = int(__fastcall*)(void*, void*, void*, char*, unsigned int,
		void*, unsigned int, void*, unsigned int);
	using FN打开当前成员 = int(__fastcall*)(void*, int*, int*, int, const char*);
#else
	struct 文件回调
	{
		void* 打开;
		void* 读取;
		void* 写入;
		void* 取位置;
		void* 置位置;
		void* 关闭;
		void* 取错误;
		void* 附加参数;
	};

	using FN打开归档 = void* (__fastcall*)(const char*, 文件回调*);
#endif
	using FN移动成员 = int(__fastcall*)(void*);
	using FN读取当前成员 = int(__fastcall*)(void*, void*);
	using FN关闭当前成员 = int(__fastcall*)(void*);
#ifdef _WIN64
	using FN释放 = void(__fastcall*)(void*);
#else
	using FN释放 = void(__cdecl*)(void*);
#endif

	Bytes _数据;
	内存流 _流;
	void* _归档 = nullptr;
	FN打开归档 _打开归档 = nullptr;
#ifdef _WIN64
	FN取当前信息 _取当前信息 = nullptr;
	FN打开当前成员 _打开当前成员 = nullptr;
#else
	void* _取当前信息 = nullptr;
	void* _打开当前成员 = nullptr;
#endif
	FN移动成员 _首成员 = nullptr;
	FN移动成员 _下一成员 = nullptr;
	FN读取当前成员 _读取当前成员 = nullptr;
	FN关闭当前成员 _关闭当前成员 = nullptr;
	FN释放 _释放 = nullptr;

	static bool _成员路径相同(const char* 归档路径, const char* 请求路径);

	static void* __cdecl _流打开(void* 附加参数, const char*, int);

	static unsigned long __cdecl _流读取(void*, void* 文件, void* 缓冲区, unsigned long 欲读大小);

	static long __cdecl _流取位置(void*, void* 文件);

	static long __cdecl _流置位置(void*, void* 文件, unsigned long 偏移, int 起点);

#ifdef _WIN64
	static unsigned long long __cdecl _流取位置64(void*, void* 文件);

	static long __cdecl _流置位置64(void*, void* 文件, unsigned long long 偏移, int 起点);
#endif

	static int __cdecl _流关闭(void*, void*);
#ifndef _WIN64
	// 这两个入口由 Sciter 的 LTCG 生成：ECX/EDX 传前两项，但栈参数由调用者清理，
	// 无法用 MSVC 的标准函数指针调用约定准确表达，只在 x86 下做最小桥接。
	static int _调用取当前信息(void* 函数, void* 归档, void* 信息, char* 名字);

	static int _调用打开当前成员(void* 函数, void* 归档);
#endif

	void _绑定接口();

public:
	~SciterZip();

	bool 打开(Bytes& 内存数据);

	void 关闭();

	bool 是否已打开() const;

	Bytes 读取成员(c_StrU8 相对路径);

	static Bytes& _修复(Bytes& 数据);

};
