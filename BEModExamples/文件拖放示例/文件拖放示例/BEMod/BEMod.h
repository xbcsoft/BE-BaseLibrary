#pragma once

#pragma region BEWin32UI
//#version 0.6
#include <BEWin32UI/BEWin32UI.h>
#ifndef _LIB
#ifdef _DEBUG
#pragma comment(lib,"BEWin32UI/Debug/BEWin32UI.lib")
#else
#pragma comment(lib,"BEWin32UI/Release/BEWin32UI.lib")
#endif
#endif
#pragma endregion

#pragma region 文件拖放
//#version 1.0
#include "文件拖放/文件拖放.h"
#pragma endregion

