#include "runtime.h"

#if !defined(_WINDLL) && !defined(_LIB)
int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPTSTR lpCmdLine, int nCmdShow)
{
	全局初始化配置(hInstance, true);
	_启动窗口.初显(nCmdShow).载入();

	return Win32消息循环();
}
#endif


#if defined(_WINDLL)
#include "中文UI/dllmeta/按钮.hpp"
#include "中文UI/dllmeta/编辑框.hpp"
#include "中文UI/dllmeta/窗口.hpp"
#include "中文UI/dllmeta/选择夹.hpp"

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


#pragma comment(lib,"BEMod/ICON.lib")
#include "BEMod/ICON.h"

struct UI库信息 : _UI库信息 {
	int 组件个数 = sizeof(interfaceUI) / sizeof(interfaceUI[0]);
	UI库组件信息 interfaceUI[16]{};

	constexpr UI库信息() {
		模块名 = "BEWin32UI";
		分组名 = "基本组件";
		SDK主版本 = 1; //声明所需最低要求的SDK版本（主版本不一致时将直接不支持）
		SDK次版本 = 0; //声明所需最低要求的SDK版本（次版本可作自由更动）
		作者 = "小白菜软件(xbcsoft)";
		日期 = "2026-04-06";

#define _____________________________________________________________________
		int i = 0;
		interfaceUI[i].名称 = L"窗口";
		interfaceUI[i].图标 = ICON::_指针;
		interfaceUI[i].图标大小 = sizeof(ICON::_指针);
		interfaceUI[i].属性个数 = sizeof(META_窗口) / sizeof(UI属性元信息);
		interfaceUI[i].属性元信息 = META_窗口;
		interfaceUI[i].fn创建 = (void*)UI_窗口_创建;
		interfaceUI[i].fn置属性 = (void*)UI_窗口_置属性;

		i = 1;
		interfaceUI[i].名称 = L"按钮";
		interfaceUI[i].图标 = ICON::按钮;
		interfaceUI[i].图标大小 = sizeof(ICON::按钮);
		interfaceUI[i].属性个数 = sizeof(META_按钮) / sizeof(UI属性元信息);
		interfaceUI[i].属性元信息 = META_按钮;
		interfaceUI[i].fn创建 = (void*)UI_按钮_创建;
		interfaceUI[i].fn置属性 = (void*)UI_按钮_置属性;

		i = 2;
		interfaceUI[i].名称 = L"编辑框";
		interfaceUI[i].图标 = ICON::编辑框;
		interfaceUI[i].图标大小 = sizeof(ICON::编辑框);
		interfaceUI[i].属性个数 = sizeof(META_编辑框) / sizeof(UI属性元信息);
		interfaceUI[i].属性元信息 = META_编辑框;
		interfaceUI[i].fn创建 = (void*)UI_编辑框_创建;
		interfaceUI[i].fn置属性 = (void*)UI_编辑框_置属性;

		i = 3;
		interfaceUI[i].名称 = L"标签";
		interfaceUI[i].图标 = ICON::标签;
		interfaceUI[i].图标大小 = sizeof(ICON::标签);

		i = 4;
		interfaceUI[i].名称 = L"超级链接框";
		interfaceUI[i].图标 = ICON::超级链接框;
		interfaceUI[i].图标大小 = sizeof(ICON::超级链接框);

		i = 5;
		interfaceUI[i].名称 = L"单选框";
		interfaceUI[i].图标 = ICON::单选框;
		interfaceUI[i].图标大小 = sizeof(ICON::单选框);

		i = 6;
		interfaceUI[i].名称 = L"分组框";
		interfaceUI[i].图标 = ICON::分组框;
		interfaceUI[i].图标大小 = sizeof(ICON::分组框);

		i = 7;
		interfaceUI[i].名称 = L"滑块条";
		interfaceUI[i].图标 = ICON::滑块条;
		interfaceUI[i].图标大小 = sizeof(ICON::滑块条);

		i = 8;
		interfaceUI[i].名称 = L"进度条";
		interfaceUI[i].图标 = ICON::进度条;
		interfaceUI[i].图标大小 = sizeof(ICON::进度条);

		i = 9;
		interfaceUI[i].名称 = L"列表框";
		interfaceUI[i].图标 = ICON::列表框;
		interfaceUI[i].图标大小 = sizeof(ICON::列表框);

		i = 10;
		interfaceUI[i].名称 = L"时钟";
		interfaceUI[i].图标 = ICON::时钟;
		interfaceUI[i].图标大小 = sizeof(ICON::时钟);

		i = 11;
		interfaceUI[i].名称 = L"图片框";
		interfaceUI[i].图标 = ICON::图片框;
		interfaceUI[i].图标大小 = sizeof(ICON::图片框);
		interfaceUI[i].组件类型 = UI类型::容器;

		i = 12;
		interfaceUI[i].名称 = L"外形框";
		interfaceUI[i].图标 = ICON::外形框;
		interfaceUI[i].图标大小 = sizeof(ICON::外形框);
		interfaceUI[i].组件类型 = UI类型::容器;

		i = 13;
		interfaceUI[i].名称 = L"选择夹";
		interfaceUI[i].图标 = ICON::选择夹;
		interfaceUI[i].图标大小 = sizeof(ICON::选择夹);
		interfaceUI[i].属性个数 = sizeof(META_按钮) / sizeof(UI属性元信息);
		interfaceUI[i].属性元信息 = META_按钮;
		interfaceUI[i].组件类型 = UI类型::多夹;

		i = 14;
		interfaceUI[i].名称 = L"选择框";
		interfaceUI[i].图标 = ICON::选择框;
		interfaceUI[i].图标大小 = sizeof(ICON::选择框);

		i = 15;
		interfaceUI[i].名称 = L"组合框";
		interfaceUI[i].图标 = ICON::组合框;
		interfaceUI[i].图标大小 = sizeof(ICON::组合框);
#define _____________________________________________________________________

		fn全局属性面板 = [](LPWSTR 组件名称, LPWSTR 属性名,
			LPWSTR 当前属性值, LPWSTR 即将改变到属性值, BEIDEFN ide方法) -> bool
		{
			using namespace IDE方法;
			
			if (属性名==StrW(L"图标ID") && 当前属性值==StrW(L"0")) {
				Bytes* r = ide方法(取属性值, FB(L"标题"));
				delete r;
			}

			if (属性名==StrW(L"标题") && 当前属性值==StrW(L"111")) {
				ide方法(置属性值, FB(L"标题", L"新标题啊"));
				return false;
			}
			
			if (组件名称==StrW(L"窗口") && 属性名==StrW(L"控制按钮")) {
				bool enable = 即将改变到属性值==StrW(L"真");
				ide方法(启用禁用属性, FB(L"最小化按钮", enable));
				ide方法(启用禁用属性, FB(L"最大化按钮", enable));
			}
			return true;
		};
	}
};

constexpr UI库信息 g_UI库信息;

EXPC UI库信息* UI_Info(){
	return (UI库信息*)&g_UI库信息;
}

#endif
