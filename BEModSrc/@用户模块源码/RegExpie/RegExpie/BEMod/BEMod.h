#pragma once

#pragma region BECore
//#version 0.6
#include <BECore/BECore.h>
#ifndef _LIB
#ifdef _DEBUG
#ifdef _WIN64
#ifdef _DLL
#pragma comment(lib,"BECore/x64/Debug/BECore.lib")
#else
#pragma comment(lib,"BECore/x64/Debug/BECore_static.lib")
#endif
#else
#ifdef _DLL
#pragma comment(lib,"BECore/Debug/BECore.lib")
#else
#pragma comment(lib,"BECore/Debug/BECore_static.lib")
#endif
#endif
#else
#ifdef _WIN64
#ifdef _DLL
#pragma comment(lib,"BECore/x64/Release/BECore.lib")
#else
#pragma comment(lib,"BECore/x64/Release/BECore_static.lib")
#endif
#else
#ifdef _DLL
#pragma comment(lib,"BECore/Release/BECore.lib")
#else
#pragma comment(lib,"BECore/Release/BECore_static.lib")
#endif
#endif
#endif
#endif
#pragma endregion

