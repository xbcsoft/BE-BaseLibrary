#pragma once

#include <BECore/BECore.h>

#ifdef _DEBUG
#pragma comment(lib,"BECore/Debug/BECore.lib")
#else
#pragma comment(lib,"BECore/Release/BECore.lib")
#endif