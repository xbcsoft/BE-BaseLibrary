#include <BEMod.h>
#include "托盘菜单.h"

struct __启动窗口 : 窗口
{
	SciterDom dom;

	void 事件_创建本体()
	{
		// 初始化创建自绘托盘菜单子窗口
		窗口::参数 menuCs;
		menuCs.宽度 = 160;
		menuCs.高度 = 165;
		menuCs.总在最前 = true;
		menuCs.可视 = false;
		_托盘菜单.创建(menuCs, this);

		// 一次性注入统一的菜单指令处理函数
		_托盘菜单.dom.注入JS函数("onMenuCommand", [this](SciterObj& arg) {
			StrA cmd = arg;
			if (cmd == "show-main") {
				激活(true);
			} else if (cmd == "hide-main") {
				可视_(false);
			} else if (cmd == "toggle-topmost") {
				总在最前_(!总在最前);
				return (bool)总在最前;
			} else if (cmd == "about") {
				信息框("托盘右键菜单自绘\n采用白易与SciterUI引擎\n支持自定义弹出菜单与托盘交互。", "关于");
			} else if (cmd == "quit") {
				置托盘图标(NULL);
				销毁();
			}
			return true;
		});
	}

	void 事件_创建完毕()
	{
		// 初始化系统托盘图标
		置托盘图标(LoadIconW(NULL, IDI_APPLICATION), L"白易 SciterUI · 托盘右键菜单自绘");
	}

	/**
	 * 托盘事件响应
	 * @param 操作类型 1: 单击左键, 2: 双击, 3: 单击右键
	 */
	void 事件_托盘(int 操作类型)
	{
		if (操作类型 == 1 || 操作类型 == 2) { // 单击左键或双击：激活还原主窗口
			激活(true);
		} else if (操作类型 == 3) { // 右键单击：业务端更新置顶状态并弹出自绘托盘菜单
			_托盘菜单.dom.调用JS函数("updateMenuState", (bool)总在最前);
			_托盘菜单.弹出();
		}
	}

	bool 事件_被关闭()
	{// 击右上角关闭按钮时，自动隐藏并最小化至托盘
		可视_(false);
		return false;
	}
#pragma region 组件成员
	SciterUI st;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
} _启动窗口;