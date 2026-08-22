#include <BEWin32UI/runtime.h>
#include <SciterUI/SciterUI.h>

int main(int nCShow = SW_SHOWNORMAL, char** argVec = nullptr)
{
	全局初始化配置(GetModuleHandle(0), true);
	SciterUI::全局初始化();
	
	_启动窗口.初显(nCShow).载入();
	return Win32消息循环();
}

int WINAPI wWinMain(HINSTANCE, HINSTANCE, LPTSTR, int nCShow)
{
	return main(nCShow);
}