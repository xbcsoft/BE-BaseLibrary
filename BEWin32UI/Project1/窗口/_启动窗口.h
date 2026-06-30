#pragma once
#include <BEMod.h>

struct __启动窗口 : 窗口
{
	void 事件_创建完毕();
	bool 事件_重画(PAINTSTRUCT & ps);

#pragma region 组件成员
	struct _按钮1 : 按钮
	{
	}按钮1;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
}; extern __启动窗口 _启动窗口;
