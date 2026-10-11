#include <BEWin32UI/runtime.h>

#if !defined(_WINDLL) && !defined(_LIB)
int main(int nCShow = SW_SHOWNORMAL, char** argVec = nullptr)
{
	全局初始化配置(GetModuleHandle(0), true);

	_启动窗口.初显(nCShow).载入();
	return Win32消息循环();
}

int WINAPI wWinMain(HINSTANCE, HINSTANCE, LPTSTR, int nCShow)
{
	return main(nCShow);
}
#endif

#if defined(_WINDLL)
#define _BE_FB //若当前控件项目依赖FB库则该宏阻止它二次引入定义（因为在VSUISDK中有内联）
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
		模块名 = "扩展界面库一"; //必须与当前工程中所开发的模块名一致
		//如果你的模块工程一开始打算制作多控件那么模块取名上可不与具体控件名一致

		分组名 = "扩展组件"; //可随意，白易不做约束，会将已有组名的控件一同加入进去
		SDK主版本 = 1; //声明所需最低要求的SDK版本（主版本不一致时将直接不支持）
		SDK次版本 = 0; //声明所需最低要求的SDK版本（次版本可作自由更动）
		作者 = "小白菜软件(xbcsoft)";
		日期 = "2026-10-01";
		备注 = "https://wiki.xbcsoft.com/BEVCpp/"; //这个就随意了

#define _____________________________________________________________________
		int i = 0;
		interfaceUI[i].名称 = L"超级列表框";
		interfaceUI[i].图标 = ICON::超列;
		interfaceUI[i].图标大小 = sizeof(ICON::超列);
		interfaceUI[i].属性个数 = sizeof(META_超级列表框) / sizeof(UI属性元信息);
		interfaceUI[i].属性元信息 = META_超级列表框;
		interfaceUI[i].fn创建 = UI_超级列表框_创建;
		interfaceUI[i].fn置属性 = UI_超级列表框_置属性;

		//如果一个DLL有多个控件的话
		i = 1;
		interfaceUI[i].名称 = L"树型框";
		interfaceUI[i].图标 = ICON::树型;
		interfaceUI[i].图标大小 = sizeof(ICON::树型);
		interfaceUI[i].属性个数 = sizeof(META_树型框) / sizeof(UI属性元信息);
		interfaceUI[i].属性元信息 = META_树型框;
		interfaceUI[i].fn创建 = UI_树型框_创建;
		interfaceUI[i].fn置属性 = UI_树型框_置属性;

		fn全局属性面板 = [](LPCWSTR 组件名称, LPCWSTR 属性名,
			LPCWSTR 当前属性值, LPCWSTR 即将改变到属性值, BEIDEFN ide方法) -> bool
		{
			StrW 属性名W = 属性名, 组件名称W = 组件名称;
			using namespace IDE方法;
			// 注：即将改变到属性值在设计上有二义性（需优先判断当前是否为UI事件）

			if (即将改变到属性值 == UI事件类型::响应属性) {
				if (组件名称W == L"超级列表框") {
					Bytes* 类型值 = ide方法(取属性值, FB(L"类型"));
					int 类型 = 类型值 && 类型值->buf ? _wtoi((charW*)类型值->buf) : 0;//到整数((charW*)类型值->buf);//类型值 && 类型值->buf ? _wtoi((charW*)类型值->buf) : 0;
					bool 图标模式 = 类型 == 0 || 类型 == 1;
					bool 报表模式 = 类型 == 3;
					delete 类型值; //注意必须对IDE方法()所有的返回值进行内存释放
					ide方法(启用禁用属性, FB(L"图标对齐方式", 图标模式));
					ide方法(启用禁用属性, FB(L"自动排列图标", 图标模式));
					ide方法(启用禁用属性, FB(L"标题自动换行", 图标模式));
					ide方法(启用禁用属性, FB(L"报表列", 报表模式));
					ide方法(启用禁用属性, FB(L"无表头", 报表模式));
					ide方法(启用禁用属性, FB(L"表头图片组", 报表模式));
					ide方法(启用禁用属性, FB(L"表头可单击", 报表模式));
					ide方法(启用禁用属性, FB(L"整行选择", 报表模式));
					ide方法(启用禁用属性, FB(L"显示表格线", 报表模式));
					ide方法(启用禁用属性, FB(L"表列可拖动", 报表模式));
				}
				return true;
			}
			if (即将改变到属性值 != UI事件类型::自定数据) return true;

			// 若取自定义数据则需要动态调用IDE方法
			Bytes* 当前数据 = ide方法(取属性值, FB(属性名W));
			Bytes* 编辑结果 = NULL;
			if (组件名称W == L"超级列表框" && 属性名W == L"报表列") {
				编辑结果 = UI超级列表框二维编辑器实现::编辑报表列(当前数据, ide方法);
			} else if (组件名称W == L"超级列表框" && 属性名W == L"表项") {
				编辑结果 = UI超级列表框二维编辑器实现::编辑表项(当前数据, ide方法);
			} else if (组件名称W == L"超级列表框"
				&& (属性名W == L"图片组" || 属性名W == L"状态图片组"
					|| 属性名W == L"表头图片组")) {
				编辑结果 = UI_图片组_编辑窗口(当前数据, 属性名W);
			} else if (组件名称W == L"树型框" && 属性名W == L"项目") {
				编辑结果 = UI树型框二维编辑器实现::编辑项目(当前数据, ide方法);
			} else if (组件名称W == L"树型框"
				&& (属性名W == L"图片组" || 属性名W == L"检查框图片组")) {
				编辑结果 = UI_图片组_编辑窗口(当前数据, 属性名W);
			}
			delete 当前数据;
			if (!编辑结果) return false;
			ide方法(置属性值, FB(属性名W, *编辑结果));
			delete 编辑结果;
			return true;
		};
	}
};

constexpr UI库信息 g_UI库信息;

EXPC UI库信息* UI_Info() {
	return (UI库信息*)&g_UI库信息;
}

#endif