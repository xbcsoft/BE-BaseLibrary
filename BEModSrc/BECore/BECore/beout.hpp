#pragma once
#include "behelper.hpp"

#ifdef _WIN32
extern "C" {
	__declspec(dllimport)
		void __stdcall OutputDebugStringW(const wchar_t* lpOutputString);
	__declspec(dllimport)
		int __stdcall SetConsoleOutputCP(unsigned int wCodePageID);
}
#define EXP __declspec(dllexport)
#define EXPC extern "C" __declspec(dllexport)
#endif

//供窗口程序用作控制台打印的调试使用
#ifdef _CWinDbg
#define cw_printf printf
#define cw_print be::print
#else
#define cw_printf(...)
#define cw_print(...)
#endif


template<typename T, typename>
inline AutoStr __AutoStr__(const T& v) {
	return AutoStr(const_cast<T&>(v));
}
inline AutoStr __AutoStr__(const StrA& v) {
	AutoStr s; s += "\""; s += v; s += "\""; return s;
}
inline AutoStr __AutoStr__(const StrU8& v) {
	AutoStr s; s += "\""; s += v; s += "\""; return s;
}
inline AutoStr __AutoStr__(const StrW& v) {
	AutoStr s; s += "\""; s += WtoU8(v); s += "\""; return s;
}
//对常规字符串指针不进行重载，否则打印到控制台上很难看
//inline AutoStr __AutoStr__(const char* v) {
//	AutoStr s; s += "\""; s += v; s += "\""; return s;
//}
//inline AutoStr __AutoStr__(const charW* v) {
//	AutoStr s; s += "\""; s += v; s += "\""; return s;
//}
template <int N>
inline AutoStr __AutoStr__(byte(&arr)[N]) {
	return AutoStr(jzjj(arr));
}

// 支持 Arraybe 的自动流式输出
template<typename T_ELE, int STACK_CAP>
inline AutoStr __AutoStr__(const Arraybe<T_ELE, STACK_CAP>& arr) {
	AutoStr s;
	s += "Array:";
	s += ToStr(arr.count);
	s += "{";
	for (int i = 0; i < arr.count; i++) {
		s += __AutoStr__(arr[i]);
		if (i != arr.count - 1) s += ",";
	}
	s += "}";
	return s;
}

// 支持 HashTbe 的自动流式输出
template<typename K, typename V>
inline AutoStr __AutoStr__(const HashTbe<K, V>& hash) {
	AutoStr s;
	s += "{";
	bool first = true;
	for (auto& node : hash) {
		if (!first) s += ", ";
		s += __AutoStr__(node.key);
		s += ":";
		s += __AutoStr__(node.value);
		first = false;
	}
	s += "}";
	return s;
}

// 支持 NilOpt (可空) 的自动流式输出
template<typename T>
inline AutoStr __AutoStr__(const NilOpt<T>& opt) {
	if (opt.has) return __AutoStr__(opt.val);
	return "nil";
}
template<typename T>
inline AutoStr __AutoStr__(const NilOpt<T&>& opt) {
	if (opt.p) return __AutoStr__(*opt.p);
	return "nil";
}
template<typename T>
inline AutoStr __AutoStr__(const NilOpt<const T&>& opt) {
	if (opt.has) return __AutoStr__(*opt.p);
	return "nil";
}

namespace be {
template <typename T>
inline void _sprint_helper(AutoStr& result, T&& t) {
	result += __AutoStr__(t);
}
template <typename T, typename... Args>
inline void _sprint_helper(AutoStr& result, T&& t, Args&&... args) {
	result += __AutoStr__(t);
	result += " | ";
	_sprint_helper(result, args...);
}

#ifdef _DEBUG
template <typename... Args>
void debug(Args&&... args) {
	AutoStr result;
	_sprint_helper(result, args...);
	result += "\n";
#ifdef _WIN32
	OutputDebugStringW(U8toW(result.str));
#else
	printf("%s", (const char*)result);
#endif
}
#else
inline void debug(...) {}
#endif // DEBUG

template <typename... Args>
void print(Args&&... args) {
	AutoStr result; _sprint_helper(result, args...);
	result += "\n";
	printf("%s", (const char*)result);
}

struct Endl {} const endl;
template <bool IsDebugout>
struct _OStream {
	_OStream& operator<<(const AutoStr& v) {
#ifdef _WIN32
		if constexpr (IsDebugout) {
			OutputDebugStringW(U8toW(v));
			return *this;
		}
#endif
		printf("%s", (const char*)v);
		return *this;
	}
	_OStream& operator<<(Endl) {
		return *this << "\n";
	}
};

// be::cout << any << be::endl;
static _OStream<false> cout;
static _OStream<true> dout;
}


//Example DIY Struct - 只需定义 AutoStr 即可
//struct Point {
//	int x, y;
//	Point(int x, int y) : x(x), y(y) {}
//
//	// 只需要定义 U8 版，W 版可按需转换
//	operator AutoStr() {
//		return sprintF("Point{%d,%d}", x, y);
//	}
//};
