#pragma once
#include <BEMod.h>

struct __启动窗口 : 窗口
{
	void 事件_创建完毕();

	bool 通用事件_窗口重画(PAINTSTRUCT & ps);

	bool 通用事件_背景绘制(HDC hdc);

#pragma region 组件成员

#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
}; extern __启动窗口 _启动窗口;
