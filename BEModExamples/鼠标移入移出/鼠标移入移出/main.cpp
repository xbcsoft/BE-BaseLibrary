#include <BEWin32UI/runtime.h>

int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPTSTR lpCmdLine, int nCmdShow)
{
	全局初始化配置(hInstance, true);
	_启动窗口.初显(nCmdShow).载入();

	return Win32消息循环();
}

#if DBG==1
int main() //窗口程序的调试请使用dbg_printf、dbg_print(在beout.hpp)
{ //#define dbg_printf printf 、#define dbg_print be::print
	全局初始化配置(GetModuleHandle(0), true);
	_启动窗口.初显(true).载入();
	return Win32消息循环();
}
#endif