#include <BEWin32UI/runtime.h>
#include <SciterUI/SciterUI.h>
#include "窗口\窗口00_JS基础.h"
#include "窗口\窗口02_JS异步协程.h"
#include "窗口\窗口01_DOM基础.h"
#include "窗口\窗口03_JS模块化动态页面.h"

int main(int nCShow = SW_SHOWNORMAL, char** argVec = nullptr)
{
	全局初始化配置(GetModuleHandle(0), true);
	SciterUI::全局初始化();

	//窗口00_JS基础.载入();
	//窗口01_DOM基础.载入();
	//窗口02_JS异步协程.载入();
	窗口03_JS模块化动态页面.载入();

	return Win32消息循环();
}

int WINAPI wWinMain(HINSTANCE, HINSTANCE, LPTSTR, int nCShow)
{
	return main(nCShow);
}