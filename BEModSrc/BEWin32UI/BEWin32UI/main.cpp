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
#include "中文UI/dllmeta/标签.hpp"
#include "中文UI/dllmeta/编辑框.hpp"
#include "中文UI/dllmeta/选择框.hpp"
#include "中文UI/dllmeta/窗口.hpp"
#include "中文UI/dllmeta/选择夹.hpp"
#include "中文UI/dllmeta/分组框.hpp"
#include "中文UI/dllmeta/列表框.hpp"
#include "中文UI/dllmeta/超级链接框.hpp"
#include "中文UI/dllmeta/单选框.hpp"
#include "中文UI/dllmeta/滑块条.hpp"
#include "中文UI/dllmeta/分隔条.hpp"
#include "中文UI/dllmeta/进度条.hpp"
#include "中文UI/dllmeta/时钟.hpp"
#include "中文UI/dllmeta/图片框.hpp"
#include "中文UI/dllmeta/外形框.hpp"
#include "中文UI/dllmeta/组合框.hpp"

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
	UI库组件信息 interfaceUI[17]{};

	constexpr UI库信息() {
		模块名 = "BEWin32UI";
		分组名 = "基本组件";
		SDK主版本 = 1; //声明所需最低要求的SDK版本（主版本不一致时将直接不支持）
		SDK次版本 = 2; //使用 UI多夹类型 1.2：现行子夹及设计时客户区命中
		作者 = "小白菜软件(xbcsoft)";
		日期 = "2026-10-01";
		备注 = "https://wiki.xbcsoft.com/BEVCpp/";
#define _____________________________________________________________________
		int i = 0;
		interfaceUI[i].名称 = L"窗口";
		interfaceUI[i].图标 = ICON::_指针;
		interfaceUI[i].图标大小 = sizeof(ICON::_指针);
		interfaceUI[i].属性个数 = sizeof(META_窗口) / sizeof(UI属性元信息);
		interfaceUI[i].属性元信息 = META_窗口;
		interfaceUI[i].fn创建 = UI_窗口_创建;
		interfaceUI[i].fn置属性 = UI_窗口_置属性;

		i = 1;
		interfaceUI[i].名称 = L"按钮";
		interfaceUI[i].图标 = ICON::按钮;
		interfaceUI[i].图标大小 = sizeof(ICON::按钮);
		interfaceUI[i].属性个数 = sizeof(META_按钮) / sizeof(UI属性元信息);
		interfaceUI[i].属性元信息 = META_按钮;
		interfaceUI[i].fn创建 = UI_按钮_创建;
		interfaceUI[i].fn置属性 = UI_按钮_置属性;

		i = 2;
		interfaceUI[i].名称 = L"编辑框";
		interfaceUI[i].图标 = ICON::编辑框;
		interfaceUI[i].图标大小 = sizeof(ICON::编辑框);
		interfaceUI[i].属性个数 = sizeof(META_编辑框) / sizeof(UI属性元信息);
		interfaceUI[i].属性元信息 = META_编辑框;
		interfaceUI[i].fn创建 = UI_编辑框_创建;
		interfaceUI[i].fn置属性 = UI_编辑框_置属性;

		i = 3;
		interfaceUI[i].名称 = L"标签";
		interfaceUI[i].图标 = ICON::标签;
		interfaceUI[i].图标大小 = sizeof(ICON::标签);
		interfaceUI[i].属性个数 = sizeof(META_标签) / sizeof(UI属性元信息);
		interfaceUI[i].属性元信息 = META_标签;
		interfaceUI[i].fn创建 = UI_标签_创建;
		interfaceUI[i].fn置属性 = UI_标签_置属性;

		i = 4;
		interfaceUI[i].名称 = L"超级链接框";
		interfaceUI[i].图标 = ICON::超级链接框;
		interfaceUI[i].图标大小 = sizeof(ICON::超级链接框);
		interfaceUI[i].属性个数 = sizeof(META_超级链接框) / sizeof(UI属性元信息);
		interfaceUI[i].属性元信息 = META_超级链接框;
		interfaceUI[i].fn创建 = UI_超级链接框_创建;
		interfaceUI[i].fn置属性 = UI_超级链接框_置属性;

		i = 5;
		interfaceUI[i].名称 = L"单选框";
		interfaceUI[i].图标 = ICON::单选框;
		interfaceUI[i].图标大小 = sizeof(ICON::单选框);
		interfaceUI[i].属性个数 = sizeof(META_单选框) / sizeof(UI属性元信息);
		interfaceUI[i].属性元信息 = META_单选框;
		interfaceUI[i].fn创建 = UI_单选框_创建;
		interfaceUI[i].fn置属性 = UI_单选框_置属性;

		i = 6;
		interfaceUI[i].名称 = L"分组框";
		interfaceUI[i].图标 = ICON::分组框;
		interfaceUI[i].图标大小 = sizeof(ICON::分组框);
		interfaceUI[i].属性个数 = sizeof(META_分组框) / sizeof(UI属性元信息);
		interfaceUI[i].属性元信息 = META_分组框;
		interfaceUI[i].组件类型 = UI类型::容器;
		interfaceUI[i].fn创建 = UI_分组框_创建;
		interfaceUI[i].fn置属性 = UI_分组框_置属性;

		i = 7;
		interfaceUI[i].名称 = L"滑块条";
		interfaceUI[i].图标 = ICON::滑块条;
		interfaceUI[i].图标大小 = sizeof(ICON::滑块条);
		interfaceUI[i].属性个数 = sizeof(META_滑块条) / sizeof(UI属性元信息);
		interfaceUI[i].属性元信息 = META_滑块条;
		interfaceUI[i].fn创建 = UI_滑块条_创建;
		interfaceUI[i].fn置属性 = UI_滑块条_置属性;

		i = 8;
		interfaceUI[i].名称 = L"进度条";
		interfaceUI[i].图标 = ICON::进度条;
		interfaceUI[i].图标大小 = sizeof(ICON::进度条);
		interfaceUI[i].属性个数 = sizeof(META_进度条) / sizeof(UI属性元信息);
		interfaceUI[i].属性元信息 = META_进度条;
		interfaceUI[i].fn创建 = UI_进度条_创建;
		interfaceUI[i].fn置属性 = UI_进度条_置属性;

		i = 9;
		interfaceUI[i].名称 = L"列表框";
		interfaceUI[i].图标 = ICON::列表框;
		interfaceUI[i].图标大小 = sizeof(ICON::列表框);
		interfaceUI[i].属性个数 = sizeof(META_列表框) / sizeof(UI属性元信息);
		interfaceUI[i].属性元信息 = META_列表框;
		interfaceUI[i].fn创建 = UI_列表框_创建;
		interfaceUI[i].fn置属性 = UI_列表框_置属性;

		i = 10;
		interfaceUI[i].名称 = L"时钟";
		interfaceUI[i].组件类型 = UI类型::虚拟控件;
		interfaceUI[i].图标 = ICON::时钟;
		interfaceUI[i].图标大小 = sizeof(ICON::时钟);
		interfaceUI[i].属性个数 = sizeof(META_时钟) / sizeof(UI属性元信息);
		interfaceUI[i].属性元信息 = META_时钟;
		interfaceUI[i].fn创建 = UI_时钟_创建;
		interfaceUI[i].fn置属性 = UI_时钟_置属性;

		i = 11;
		interfaceUI[i].名称 = L"图片框";
		interfaceUI[i].图标 = ICON::图片框;
		interfaceUI[i].图标大小 = sizeof(ICON::图片框);
		interfaceUI[i].属性个数 = sizeof(META_图片框) / sizeof(UI属性元信息);
		interfaceUI[i].属性元信息 = META_图片框;
		interfaceUI[i].组件类型 = UI类型::容器;
		interfaceUI[i].fn创建 = UI_图片框_创建;
		interfaceUI[i].fn置属性 = UI_图片框_置属性;

		i = 12;
		interfaceUI[i].名称 = L"外形框";
		interfaceUI[i].图标 = ICON::外形框;
		interfaceUI[i].图标大小 = sizeof(ICON::外形框);
		interfaceUI[i].属性个数 = sizeof(META_外形框) / sizeof(UI属性元信息);
		interfaceUI[i].属性元信息 = META_外形框;
		interfaceUI[i].组件类型 = UI类型::容器;
		interfaceUI[i].fn创建 = UI_外形框_创建;
		interfaceUI[i].fn置属性 = UI_外形框_置属性;

		i = 13;
		interfaceUI[i].名称 = L"选择夹";
		interfaceUI[i].图标 = ICON::选择夹;
		interfaceUI[i].图标大小 = sizeof(ICON::选择夹);
		interfaceUI[i].属性个数 = sizeof(META_选择夹) / sizeof(UI属性元信息);
		interfaceUI[i].属性元信息 = META_选择夹;
		interfaceUI[i].组件类型 = UI类型::多夹;
		interfaceUI[i].fn创建 = UI_选择夹_创建;
		interfaceUI[i].fn置属性 = UI_选择夹_置属性;

		i = 14;
		interfaceUI[i].名称 = L"选择框";
		interfaceUI[i].图标 = ICON::选择框;
		interfaceUI[i].图标大小 = sizeof(ICON::选择框);
		interfaceUI[i].属性个数 = sizeof(META_选择框) / sizeof(UI属性元信息);
		interfaceUI[i].属性元信息 = META_选择框;
		interfaceUI[i].fn创建 = UI_选择框_创建;
		interfaceUI[i].fn置属性 = UI_选择框_置属性;

		i = 15;
		interfaceUI[i].名称 = L"组合框";
		interfaceUI[i].图标 = ICON::组合框;
		interfaceUI[i].图标大小 = sizeof(ICON::组合框);
		interfaceUI[i].属性个数 = sizeof(META_组合框) / sizeof(UI属性元信息);
		interfaceUI[i].属性元信息 = META_组合框;
		interfaceUI[i].fn创建 = UI_组合框_创建;
		interfaceUI[i].fn置属性 = UI_组合框_置属性;

		i = 16;
		interfaceUI[i].名称 = L"分隔条";
		interfaceUI[i].图标 = ICON::分隔条;
		interfaceUI[i].图标大小 = sizeof(ICON::分隔条);
		interfaceUI[i].属性个数 = sizeof(META_分隔条) / sizeof(UI属性元信息);
		interfaceUI[i].属性元信息 = META_分隔条;
		interfaceUI[i].fn创建 = UI_分隔条_创建;
		interfaceUI[i].fn置属性 = UI_分隔条_置属性;
#define _____________________________________________________________________

		fn全局属性面板 = [](LPCWSTR 组件名称, LPCWSTR 属性名,
			LPCWSTR 当前属性值, LPCWSTR 即将改变到属性值, BEIDEFN ide方法) -> bool
		{
			StrW 属性名W = 属性名, 组件名称W = 组件名称;
			using namespace IDE方法;

			if (即将改变到属性值 == UI事件类型::自定数据) {
				if (组件名称W == L"选择夹" && 属性名W == L"子夹管理") {
					Bytes* 当前数据 = ide方法(取属性值, FB(L"子夹管理"));
					Bytes* 编辑结果 = UI_一维数组_编辑窗口(
						当前数据, false, L"子夹管理", L"子夹");
					delete 当前数据;
					if (!编辑结果) return false;
					ide方法(置属性值, FB(L"子夹管理", *编辑结果));
					delete 编辑结果;
					return true;
				}
				if (组件名称W == L"列表框" && 属性名W == L"列表项目") {
					Bytes* 当前数据 = ide方法(取属性值, FB(L"列表项目"));
					Bytes* 编辑结果 = UI_一维数组_编辑窗口(
						当前数据, false, L"列表项目管理", L"项目");
					delete 当前数据;
					if (!编辑结果) return false;
					ide方法(置属性值, FB(L"列表项目", *编辑结果));
					delete 编辑结果;
					return true;
				}
				if (组件名称W == L"列表框" && 属性名W == L"项目数值") {
					Bytes* 当前数据 = ide方法(取属性值, FB(L"项目数值"));
					Bytes* 编辑结果 = UI_一维数组_编辑窗口(
						当前数据, true, L"项目数值管理", L"数值");
					delete 当前数据;
					if (!编辑结果) return false;
					ide方法(置属性值, FB(L"项目数值", *编辑结果));
					delete 编辑结果;
					return true;
				}
				return false;
			}
			
			if (属性名W == L"图标ID" && 当前属性值 == StrW(L"0")) {
				Bytes* r = ide方法(取属性值, FB(L"标题"));
				delete r;
			}

			if (属性名W == L"标题" && 当前属性值 == StrW(L"111")) {
				ide方法(置属性值, FB(L"标题", L"新标题啊"));
				return false;
			}
			
			if (组件名称W == L"窗口" && 属性名W == L"控制按钮") {
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
