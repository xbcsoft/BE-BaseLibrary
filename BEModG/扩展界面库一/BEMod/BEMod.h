#pragma once

#pragma region BEWin32UI
//#version 0.6
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
#pragma comment(lib,"BEWin32UI/Release/BEWin32UI.lib")
#endif
#endif
#endif
#pragma endregion

#pragma region FB
//#version 1.0
#include <FB/FB.h>
#pragma endregion

