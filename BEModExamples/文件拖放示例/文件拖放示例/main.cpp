#include <BEWin32UI/runtime.h>

int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPTSTR lpCmdLine, int nCmdShow)
{
	全局初始化配置(hInstance, true);
	_启动窗口.初显(nCmdShow).载入();

	return Win32消息循环();
}

#ifdef _CWinDbg
int main()
{
	全局初始化配置(GetModuleHandle(0), true);
	_启动窗口.初显(true).载入();
	return Win32消息循环();
}
#endif