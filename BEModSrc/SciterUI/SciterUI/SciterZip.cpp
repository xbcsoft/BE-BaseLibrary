#include "stdafx.h"

extern HMODULE g_sciterModule;

/**
 * 直接复用 Sciter 5.0.3.15(x86/x64) 内部的 ZIP 读取能力。
 * 这里只实现 SciterUI 加载内存资源所需的最小功能，不替代现有 Zip 类。
 */
class EXP SciterZip
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

	static bool _成员路径相同(const char* 归档路径, const char* 请求路径)
	{
		if (!归档路径 || !请求路径) return false;
		for (;;) {
			unsigned char a = (unsigned char)*归档路径++;
			unsigned char b = (unsigned char)*请求路径++;
			if (a == '\\') a = '/';
			if (b == '\\') b = '/';
			if (a != b) return false;
			if (a == 0) return true;
		}
	}

	static void* __cdecl _流打开(void* 附加参数, const char*, int)
	{
		auto* 流 = (内存流*)附加参数;
		流->位置 = 0;
		return 流;
	}

	static unsigned long __cdecl _流读取(void*, void* 文件, void* 缓冲区, unsigned long 欲读大小)
	{
		auto* 流 = (内存流*)文件;
		if (!流 || 流->位置 >= 流->大小) return 0;
		size_t 剩余 = 流->大小 - 流->位置;
		size_t 实际 = 剩余 < 欲读大小 ? 剩余 : 欲读大小;
		CopyMemory(缓冲区, 流->数据 + 流->位置, 实际);
		流->位置 += 实际;
		return (unsigned long)实际;
	}

	static long __cdecl _流取位置(void*, void* 文件)
	{
		auto* 流 = (内存流*)文件;
		return 流 ? (long)流->位置 : -1;
	}

	static long __cdecl _流置位置(void*, void* 文件, unsigned long 偏移, int 起点)
	{
		auto* 流 = (内存流*)文件;
		if (!流) return -1;

		LONGLONG 新位置;
		if (起点 == 0) 新位置 = 偏移;
		else if (起点 == 1) 新位置 = (LONGLONG)流->位置 + (long)偏移;
		else if (起点 == 2) 新位置 = (LONGLONG)流->大小 + (long)偏移;
		else return -1;

		if (新位置 < 0 || (unsigned long long)新位置 > 流->大小) return -1;
		流->位置 = (size_t)新位置;
		return 0;
	}

#ifdef _WIN64
	static unsigned long long __cdecl _流取位置64(void*, void* 文件)
	{
		auto* 流 = (内存流*)文件;
		return 流 ? (unsigned long long)流->位置 : (unsigned long long)-1;
	}

	static long __cdecl _流置位置64(void*, void* 文件, unsigned long long 偏移, int 起点)
	{
		auto* 流 = (内存流*)文件;
		if (!流) return -1;

		LONGLONG 新位置;
		if (起点 == 0) 新位置 = (LONGLONG)偏移;
		else if (起点 == 1) 新位置 = (LONGLONG)流->位置 + (LONGLONG)偏移;
		else if (起点 == 2) 新位置 = (LONGLONG)流->大小 + (LONGLONG)偏移;
		else return -1;

		if (新位置 < 0 || (unsigned long long)新位置 > 流->大小) return -1;
		流->位置 = (size_t)新位置;
		return 0;
	}
#endif

	static int __cdecl _流关闭(void*, void*) { return 0; }
#ifndef _WIN64
	// 这两个入口由 Sciter 的 LTCG 生成：ECX/EDX 传前两项，但栈参数由调用者清理，
	// 无法用 MSVC 的标准函数指针调用约定准确表达，只在 x86 下做最小桥接。
	static int _调用取当前信息(void* 函数, void* 归档, void* 信息, char* 名字)
	{
		int 返回值;
		__asm {
			push 名字
			mov edx, 信息
			mov ecx, 归档
			call 函数
			add esp, 4
			mov 返回值, eax
		}
		return 返回值;
	}

	static int _调用打开当前成员(void* 函数, void* 归档)
	{
		int 返回值;
		__asm {
			push 0
			push 0
			push 0
			xor edx, edx
			mov ecx, 归档
			call 函数
			add esp, 12
			mov 返回值, eax
		}
		return 返回值;
	}
#endif

	void _绑定接口()
	{
		auto* 模块 = (byte*)g_sciterModule;
#ifdef _WIN64
		_打开归档 = (FN打开归档)(模块 + 0x4FD214);
		_取当前信息 = (FN取当前信息)(模块 + 0x4FD728);
		_首成员 = (FN移动成员)(模块 + 0x4FDB8C);
		_下一成员 = (FN移动成员)(模块 + 0x4FDBEC);
		_打开当前成员 = (FN打开当前成员)(模块 + 0x4FDEC0);
		_读取当前成员 = (FN读取当前成员)(模块 + 0x4FE264);
		_关闭当前成员 = (FN关闭当前成员)(模块 + 0x4FE5BC);
		_释放 = (FN释放)(模块 + 0x597E30);
#else
		_打开归档 = (FN打开归档)(模块 + 0x39CEA6);
		_取当前信息 = 模块 + 0x39D2E6;
		_首成员 = (FN移动成员)(模块 + 0x39D389);
		_下一成员 = (FN移动成员)(模块 + 0x39D3D6);
		_打开当前成员 = 模块 + 0x39D67B;
		_读取当前成员 = (FN读取当前成员)(模块 + 0x39D91E);
		_关闭当前成员 = (FN关闭当前成员)(模块 + 0x39DBEC);
		_释放 = (FN释放)(模块 + 0x4135A0);
#endif
	}

public:
	~SciterZip() { 关闭(); }

	bool 打开(Bytes& 内存数据)
	{
		关闭();
		if (!内存数据) return false;

		if (内存数据[0] == 0xF5) { //是否被混淆
			_修复(内存数据);
		}

		_绑定接口();
		_数据 = 内存数据;
		_流.数据 = _数据.buf;
		_流.大小 = _数据.size;
		_流.位置 = 0;

		文件回调 回调 = {};
#ifdef _WIN64
		回调.打开64 = (void*)_流打开;
		回调.读取 = (void*)_流读取;
		回调.取位置64 = (void*)_流取位置64;
		回调.置位置64 = (void*)_流置位置64;
		回调.关闭 = (void*)_流关闭;
		回调.附加参数 = &_流;
		_归档 = _打开归档(nullptr, &回调, 0);
#else
		回调.打开 = (void*)_流打开;
		回调.读取 = (void*)_流读取;
		回调.取位置 = (void*)_流取位置;
		回调.置位置 = (void*)_流置位置;
		回调.关闭 = (void*)_流关闭;
		回调.附加参数 = &_流;
		_归档 = _打开归档("", &回调);
#endif
		if (!_归档) {
			_数据.reset();
			_流 = {};
			return false;
		}
		return true;
	}

	void 关闭()
	{
		if (_归档) {
			auto* 归档 = (byte*)_归档;
#ifdef _WIN64
			using FN关闭文件 = int(__fastcall*)(void*, void*);
			auto 关闭文件 = *(FN关闭文件*)(归档 + 0x28);
			if (关闭文件) 关闭文件(*(void**)(归档 + 0x38), *(void**)(归档 + 0x60));
#else
			using FN关闭文件 = int(__cdecl*)(void*, void*);
			auto 关闭文件 = *(FN关闭文件*)(归档 + 0x14);
			if (关闭文件) 关闭文件(*(void**)(归档 + 0x1C), *(void**)(归档 + 0x30));
#endif
			if (_释放) _释放(_归档);
			_归档 = nullptr;
		}
		_流 = {};
		_数据.reset();
	}

	bool 是否已打开() const
	{
		return _归档 != nullptr;
	}

	Bytes 读取成员(c_StrU8 相对路径)
	{
		if (!_归档 || !相对路径 || _首成员(_归档) != 0) return Bytes();

		char 当前名字[MAX_PATH];
#ifdef _WIN64
		byte 当前信息[0x58];
#else
		DWORD 当前信息[20];
#endif
		for (;;) {
#ifdef _WIN64
			if (_取当前信息(_归档, 当前信息, nullptr, 当前名字, MAX_PATH,
				nullptr, 0, nullptr, 0) != 0) return Bytes();
#else
			if (_调用取当前信息(_取当前信息, _归档, 当前信息, 当前名字) != 0) return Bytes();
#endif

			// Windows Compress-Archive 会使用反斜杠，Sciter URI 使用正斜杠。
			if (_成员路径相同(当前名字, (char*)相对路径)) {
#ifdef _WIN64
				if (_打开当前成员(_归档, nullptr, nullptr, 0, nullptr) != 0) return Bytes();
#else
				if (_调用打开当前成员(_打开当前成员, _归档) != 0) return Bytes();
#endif

				Bytes 结果;
				byte 缓冲区[0x2000];
				for (;;) {
					int 已读 = _读取当前成员(_归档, 缓冲区);
					if (已读 < 0) {
						_关闭当前成员(_归档);
						return Bytes();
					}
					if (已读 == 0) break;
					结果.append(缓冲区, 已读);
				}
				_关闭当前成员(_归档);
				return 结果;
			}

			if (_下一成员(_归档) != 0) break;
		}
		return Bytes();
	}

	static Bytes& _修复(Bytes& 数据) {
		DWORD 中央目录 = *(DWORD*)(数据.buf + 数据.size - 6);
		// 1. 破坏/恢复 文件开头第 0 字节
		数据.buf[0] ^= 0xA5;
		// 2. 破坏/恢复 整个中央目录区（从中央目录起始位置一直到 EOCD 偏移处）
		for (DWORD i = 中央目录; i < 数据.size - 6; i++) {
			数据.buf[i] ^= 0xA5;
		}
		return 数据;
	}

};
