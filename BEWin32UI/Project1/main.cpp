#include "resource.h"
#include <BEWin32UI/runtime.h>

int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE hPre,
	LPTSTR lpCmdLine, int nCmdShow)
{
	全局初始化配置(hInstance, true);
	_启动窗口.初显(nCmdShow).载入();

	return Win32消息循环();
}