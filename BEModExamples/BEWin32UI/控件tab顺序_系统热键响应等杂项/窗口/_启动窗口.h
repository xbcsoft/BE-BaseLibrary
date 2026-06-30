#pragma once
#include <BEMod.h>

struct __启动窗口 : 窗口
{
	void 事件_创建完毕();

	void 事件_快捷键(WORD cmdID);

#pragma region 组件成员
	struct :按钮 {
	}按钮1;
	struct :按钮 {
	}按钮2;
	struct :按钮 {
	}按钮3;
	struct :按钮 {
	}按钮4;
	struct :按钮 {
	}按钮5;
	struct :按钮 {
	}按钮6;
	struct _分组框1 : 分组框 {
		
	}分组框1;
	struct : 分组框 {

	}分组框2;
	struct :按钮 {
	}按钮7;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
}; extern __启动窗口 _启动窗口;
