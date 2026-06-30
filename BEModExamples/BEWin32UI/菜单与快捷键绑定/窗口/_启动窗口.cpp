#include <BEMod.h>
#include "窗口1.h"
#include "../resource.h"

struct __启动窗口 : 窗口
{
	void 事件_菜单项被单击(int 菜单ID)
	{
		switch (菜单ID)
		{
		case 1002:
			窗口1.载入(this, true);
			break;
		case 10086+1:
		case 1003: // 退出
		case ID_40003: // 退出
			销毁();
			break;
		default:
			信息框("你点击了菜单 ID: "+到文本(菜单ID));
			break;
		}
	}

	void 事件_快捷键(WORD cmdID) {
		// 可以将菜单绑定的快捷键直接转发给菜单点击事件处理
		事件_菜单项被单击(cmdID);
	}

	void 事件_创建完毕()
	{
		// 注册快捷键绑定到和菜单相同的 ID (1001 为"新建")
		快捷键_注册('N', 控制键::Ctrl键, 1001);

		托盘菜单.创建();
		托盘菜单.添加项(10086, L"显示窗口");
		托盘菜单.添加项(10086+1, L"退出");
		置托盘图标(图标::从内存创建(R::myico), L"白易你好");
	}

	/**事件_托盘
	 * @param 操作类型 1、#单击左键；2、#双击；3、#单击右键。
	 */
	void 事件_托盘(int 操作类型)
	{
		if (操作类型==3) {
			弹出菜单(托盘菜单);
		}
	}

#pragma region 组件成员
	菜单 主菜单;
	子菜单 托盘菜单;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
} _启动窗口;