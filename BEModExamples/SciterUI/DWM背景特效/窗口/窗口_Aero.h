#pragma once
#include <BEMod.h>
#include "../SciterDWM.h"

struct _窗口_Aero : 窗口
{
	void 事件_创建完毕();

#pragma region 组件成员
	struct _st :SciterUI {

	} st;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
}; extern _窗口_Aero 窗口_Aero;
