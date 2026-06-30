#pragma once

#pragma region 全局量区(SymbolsG)
#include <BEWin32Base/BEWin32Base.h>
extern HINSTANCE g_hInst;
extern char* g_hInstEnd;
extern float g_dpi;
extern float _g_dpi_bak;
extern int _g_wndCount;
class 窗口;
extern 窗口* _g_interWnd;
#pragma endregion

#pragma region BEWin32Base
//#version 0.6
#include <BEWin32Base/BEWin32Base.h>
#ifndef _LIB
#ifdef _DEBUG
#ifdef _WIN64
#ifdef _DLL
#pragma comment(lib,"BEWin32Base/x64/Debug/BEWin32Base.lib")
#else
#pragma comment(lib,"BEWin32Base/x64/Debug/BEWin32Base_static.lib")
#endif
#else
#ifdef _DLL
#pragma comment(lib,"BEWin32Base/Debug/BEWin32Base.lib")
#else
#pragma comment(lib,"BEWin32Base/Debug/BEWin32Base_static.lib")
#endif
#endif
#else
#ifdef _WIN64
#ifdef _DLL
#pragma comment(lib,"BEWin32Base/x64/Release/BEWin32Base.lib")
#else
#pragma comment(lib,"BEWin32Base/x64/Release/BEWin32Base_static.lib")
#endif
#else
#ifdef _DLL
#pragma comment(lib,"BEWin32Base/Release/BEWin32Base.lib")
#else
#pragma comment(lib,"BEWin32Base/Release/BEWin32Base_static.lib")
#endif
#endif
#endif
#endif
#pragma endregion
