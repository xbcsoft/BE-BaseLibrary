#pragma once
#include "../BEWin32UI.h"
#include "../resource.h"

struct __启动窗口 : 窗口
{
	void 事件_菜单项被单击(int 菜单ID);

	void 事件_快捷键(WORD cmdID);

	void 事件_创建完毕();

	/**事件_托盘
	 * @param 操作类型 1、#单击左键；2、#双击；3、#单击右键。
	 */
	void 事件_托盘(int 操作类型);

#pragma region 组件成员
	菜单 主菜单;
	子菜单 托盘菜单;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
}; extern __启动窗口 _启动窗口;
