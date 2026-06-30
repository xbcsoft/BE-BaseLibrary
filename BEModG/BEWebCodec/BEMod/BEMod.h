#pragma once

#pragma region BE_zlib
//#version 1.0
#include <BEWebCodec/BEMod/BE_zlib/BE_zlib.h>
#ifndef _LIB
#ifdef _WIN64
#ifdef _DLL
#pragma comment(lib,"BEWebCodec/BEMod/BE_zlib/x64/Release/BE_zlib.lib")
#else
#pragma comment(lib,"BEWebCodec/BEMod/BE_zlib/x64/Release/BE_zlib_static.lib")
#endif
#else
#ifdef _DLL
#pragma comment(lib,"BEWebCodec/BEMod/BE_zlib/Release/BE_zlib.lib")
#else
#pragma comment(lib,"BEWebCodec/BEMod/BE_zlib/Release/BE_zlib_static.lib")
#endif
#endif
#endif
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

