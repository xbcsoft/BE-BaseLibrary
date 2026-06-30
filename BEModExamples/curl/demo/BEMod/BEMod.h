#pragma once

#pragma region curl_native
//#version 8.11.0
#include <curl_native/curl_native.h>
#ifndef _LIB
#ifdef _WIN64
#pragma comment(lib,"curl_native/x64/Release/libcurl.lib")
#pragma comment(lib,"curl_native/x64/Release/zs.lib")
#else
#pragma comment(lib,"curl_native/Release/libcurl.lib")
#pragma comment(lib,"curl_native/Release/zs.lib")
#endif
#endif
#pragma endregion

