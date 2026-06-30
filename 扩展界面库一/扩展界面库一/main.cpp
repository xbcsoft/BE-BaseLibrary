#include <BEWin32UI/runtime.h>
#define _CWinDbg

#if !defined(_WINDLL) && !defined(_LIB)
int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE hPrev,
	LPTSTR lpCmdLine, int nCmdShow)
{
	全局初始化配置(hInstance, true);
	_启动窗口.初显(nCmdShow).载入();

	return Win32消息循环();
}

#ifdef _CWinDbg
int main() //窗口程序调试使用cw_printf,cw_print(条件空定义)
{ //#define cw_printf printf 、#define cw_print be::print
	全局初始化配置(GetModuleHandle(0), true);
	_启动窗口.初显(true).载入();
	return Win32消息循环();
}
#endif

#endif


#if defined(_WINDLL)
#include "stdafx.h" //这里边引出的你要自己包含原版扩展界面库一.hpp
#include "dllmeta/超级列表框.hpp"
#include "dllmeta/树型框.hpp"

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		全局初始化配置(hModule, true);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}


struct UI库信息 : _UI库信息 {
	int 组件个数 = sizeof(interfaceUI) / sizeof(interfaceUI[0]);
	UI库组件信息 interfaceUI[2]{}; //你当前工程制作的控件个数

	constexpr UI库信息() {
		模块名 = "扩展界面库一"; //必须与主模块名一致，如果你的模块工程一开始打算制作多控件
		//那么则建议工程名与模块名上就取得类似【白易扩展界面库一】这样的命名
		//（但工程这里已经写死扩展界面库一了就没办法）
		分组名 = "扩展组件"; //可随意，白易不做约束，会将已有组名的控件一同加入进去
		SDK主版本 = 1; //声明所需最低要求的SDK版本（主版本不一致时将直接不支持）
		SDK次版本 = 0; //声明所需最低要求的SDK版本（次版本可作自由更动）
		作者 = "小白菜软件(xbcsoft)";
		日期 = "2026-04-06";

#define _____________________________________________________________________
		int i = 0;
		interfaceUI[i].名称 = L"超级列表框";
		//interfaceUI[i].图标 = ICON::_指针; //0.5白易暂时不需要则注释
		//interfaceUI[i].图标大小 = sizeof(ICON::_指针);
		interfaceUI[i].属性个数 = sizeof(META_超级列表框) / sizeof(UI属性元信息);
		interfaceUI[i].属性元信息 = META_超级列表框;
		interfaceUI[i].fn创建 = (void*)UI_超级列表框_创建;
		interfaceUI[i].fn置属性 = (void*)UI_超级列表框_置属性;

		//如果一个DLL有多个控件的话
		i = 1;
		interfaceUI[i].名称 = L"树型框";
		//interfaceUI[i].图标 = ICON::按钮;
		//interfaceUI[i].图标大小 = sizeof(ICON::按钮);
		interfaceUI[i].属性个数 = sizeof(META_树型框) / sizeof(UI属性元信息);
		interfaceUI[i].属性元信息 = META_树型框;
		interfaceUI[i].fn创建 = (void*)UI_树型框_创建;
		interfaceUI[i].fn置属性 = (void*)UI_树型框_置属性;
	}
};

constexpr UI库信息 g_UI库信息;

EXPC UI库信息* UI_Info() {
	return (UI库信息*)&g_UI库信息;
}

#endif