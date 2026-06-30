#pragma once



#pragma region 全局量区(SymbolsG)
#pragma endregion

#pragma region OpenSSL_native
//#version 0.9.8k
//#beinclude "$(_BEMod_)\OpenSSL_native"
#include <OpenSSL_native/OpenSSL_native.h>
#ifndef _LIB
#ifdef _WIN64
#ifdef _DLL
#pragma comment(lib,"OpenSSL_native/x64/Release/libeay32.lib")
#pragma comment(lib,"OpenSSL_native/x64/Release/ssleay32.lib")
#else
#pragma comment(lib,"OpenSSL_native/x64/Release/libeay32_static.lib")
#pragma comment(lib,"OpenSSL_native/x64/Release/ssleay32_static.lib")
#endif
#else
#ifdef _DLL
#pragma comment(lib,"OpenSSL_native/Release/libeay32.lib")
#pragma comment(lib,"OpenSSL_native/Release/ssleay32.lib")
#else
#pragma comment(lib,"OpenSSL_native/Release/libeay32_static.lib")
#pragma comment(lib,"OpenSSL_native/Release/ssleay32_static.lib")
#endif
#endif
#endif
#pragma endregion

