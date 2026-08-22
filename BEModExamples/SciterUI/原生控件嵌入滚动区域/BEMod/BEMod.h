#pragma once

#pragma region 全局量区(SymbolsG)
#include "R.h"
#pragma endregion

#pragma region BEWin32UI
//#version 0.1
#include <BEWin32UI/BEWin32UI.h>
#ifndef _LIB
#ifdef _DEBUG
#ifdef _WIN64
#ifdef _DLL
#pragma comment(lib,"BEWin32UI/x64/Debug/BEWin32UI.lib")
#else
#pragma comment(lib,"BEWin32UI/x64/Debug/BEWin32UI_static.lib")
#endif
#else
#ifdef _DLL
#pragma comment(lib,"BEWin32UI/Debug/BEWin32UI.lib")
#else
#pragma comment(lib,"BEWin32UI/Debug/BEWin32UI_static.lib")
#endif
#endif
#else
#ifdef _WIN64
#ifdef _DLL
#pragma comment(lib,"BEWin32UI/x64/Release/BEWin32UI.lib")
#else
#pragma comment(lib,"BEWin32UI/x64/Release/BEWin32UI_static.lib")
#endif
#else
#ifdef _DLL
#pragma comment(lib,"BEWin32UI/Release/BEWin32UI.lib")
#else
#pragma comment(lib,"BEWin32UI/Release/BEWin32UI_static.lib")
#endif
#endif
#endif
#endif
#pragma endregion

#pragma region SciterUI
//#version 1.0
#include <SciterUI/SciterUI.h>
#ifndef _LIB
#ifdef _DEBUG
#ifdef _WIN64
#pragma comment(lib,"SciterUI/x64/Debug/SciterUI.lib")
#else
#endif
#else
#endif
#endif
#pragma endregion

