#pragma once
#include <BEMod.h>

struct __启动窗口 : 窗口
{
	SciterDom dom;
	void 事件_菜单项被单击(int 菜单ID);

	void 事件_快捷键(WORD cmdID);

	void 事件_创建完毕();

#pragma region 组件成员
	菜单 主菜单;
	struct _st :SciterUI {

	} st;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
}; extern __启动窗口 _启动窗口;
