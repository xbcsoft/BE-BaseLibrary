#pragma once
#include <BEMod.h>

struct _窗口01_DOM基础 : 窗口
{
	SciterDom dom;
	void 事件_创建完毕();

	void 子iframe内执行JS();

	void DOM对象返回再投入();

#pragma region 组件成员
	struct _st :SciterUI {
	} st;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
}; extern _窗口01_DOM基础 窗口01_DOM基础;
