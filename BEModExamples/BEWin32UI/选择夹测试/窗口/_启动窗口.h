#pragma once
#include <BEMod.h>

struct __启动窗口 : 窗口
{
	LRESULT 事件_反馈(WPARAM wParam, LPARAM lParam);

	void _取现行子夹按钮_被单击();

	void _切换子夹按钮_被单击();

	void _隐藏自身按钮_被单击();

	void _背景透明按钮_被单击();


#pragma region 组件成员
	struct :选择夹 {
	}选择夹1;
	struct :按钮 {
		void 事件_被单击();
	}取现行子夹按钮;

	struct _切换子夹按钮 : 按钮 {
		void 事件_被单击();
	} 切换子夹按钮;

	struct _隐藏自身按钮 : 按钮 {
		void 事件_被单击();
	} 隐藏自身按钮;

	struct _背景透明按钮 : 按钮 {
		void 事件_被单击();
	} 背景透明按钮;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态);
}; extern __启动窗口 _启动窗口;
