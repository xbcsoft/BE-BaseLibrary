#pragma once
#include <BEMod.h>

struct __启动窗口 : 窗口
{
	static void 按钮1_移入事件(HWND hWnd);
	static void 按钮1_移出事件(HWND hWnd);

	void 事件_创建完毕();

	void _按钮1_被单击();

#pragma region 组件成员
	struct _按钮1 : 按钮 {
		void 事件_被单击();
	}按钮1;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
}; extern __启动窗口 _启动窗口;
