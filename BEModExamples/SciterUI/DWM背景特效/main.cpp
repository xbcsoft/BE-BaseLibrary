#include <BEWin32UI/runtime.h>
#include <SciterUI/SciterUI.h>
#include "窗口/窗口_Aero.h"
#include "窗口/窗口_Acrylic.h"

int main(int nCShow = SW_SHOWNORMAL, char** argVec = nullptr)
{
	全局初始化配置(GetModuleHandle(0), true);
	SciterUI::全局初始化();
	
	窗口_Aero.初显(nCShow).载入();
	窗口_Acrylic.初显(nCShow).载入();
	return Win32消息循环();
}

int WINAPI wWinMain(HINSTANCE, HINSTANCE, LPTSTR, int nCShow)
{
	return main(nCShow);
}