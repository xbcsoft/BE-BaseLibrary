#pragma once

#pragma region 扩展界面库一
//#version 1.0
#include <扩展界面库一/扩展界面库一.h>
#ifndef _LIB
#ifdef _DEBUG
#ifdef _WIN64
#ifdef _DLL
#pragma comment(lib,"扩展界面库一/x64/Debug/扩展界面库一.lib")
#else
#pragma comment(lib,"扩展界面库一/x64/Debug/扩展界面库一_static.lib")
#endif
#else
#ifdef _DLL
#pragma comment(lib,"扩展界面库一/Debug/扩展界面库一.lib")
#else
#pragma comment(lib,"扩展界面库一/Debug/扩展界面库一_static.lib")
#endif
#endif
#else
#ifdef _WIN64
#ifdef _DLL
#pragma comment(lib,"扩展界面库一/x64/Release/扩展界面库一.lib")
#else
#pragma comment(lib,"扩展界面库一/x64/Release/扩展界面库一_static.lib")
#endif
#else
#ifdef _DLL
#pragma comment(lib,"扩展界面库一/Release/扩展界面库一.lib")
#else
#pragma comment(lib,"扩展界面库一/Release/扩展界面库一_static.lib")
#endif
#endif
#endif
#endif
#pragma endregion

