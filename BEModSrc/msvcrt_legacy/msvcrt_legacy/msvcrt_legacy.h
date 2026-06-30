/**@ModuleTitle: CRT传统兼容
*  @version:     1.0
*  @platform:    win32(x86|x64)
*  @compiler:    source
*  @author:      xbcsoft
*  @datetime:    2026-03-25
*  @description: 适用于VS2015以后对老式msvcrt的兼容层支持
*/
#include "stdafx.h"

#pragma comment(lib, "legacy_stdio_definitions.lib")

#if defined(_M_IX86)
#pragma comment(linker, "/alternatename:__imp____iob_func=___iob_func")
#elif defined(_M_X64)
#pragma comment(linker, "/alternatename:__imp___iob_func=__iob_func")
#endif



#ifndef _LIB
#ifdef _DEBUG
#ifdef _WIN64
#ifdef _DLL
#pragma comment(lib,"msvcrt_legacy/x64/Debug/msvcrt_legacy.lib")
#else
#pragma comment(lib,"msvcrt_legacy/x64/Debug/msvcrt_legacy_static.lib")
#endif
#else
#ifdef _DLL
#pragma comment(lib,"msvcrt_legacy/Debug/msvcrt_legacy.lib")
#else
#pragma comment(lib,"msvcrt_legacy/Debug/msvcrt_legacy_static.lib")
#endif
#endif
#else
#ifdef _WIN64
#ifdef _DLL
#pragma comment(lib,"msvcrt_legacy/x64/Release/msvcrt_legacy.lib")
#else
#pragma comment(lib,"msvcrt_legacy/x64/Release/msvcrt_legacy_static.lib")
#endif
#else
#ifdef _DLL
#pragma comment(lib,"msvcrt_legacy/Release/msvcrt_legacy.lib")
#else
#pragma comment(lib,"msvcrt_legacy/Release/msvcrt_legacy_static.lib")
#endif
#endif
#endif
#endif