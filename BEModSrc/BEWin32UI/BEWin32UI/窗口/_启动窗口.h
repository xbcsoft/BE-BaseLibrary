#pragma once
#include "../BEWin32UI.h"

struct __启动窗口 : 窗口
{
	void 事件_创建完毕();

	void _按钮1_被单击();

#pragma region 组件成员
	struct : 外形框 {
	}外形框1;
	struct : 单选框 {
	}单选框1;
	struct : 选择框 {
	}选择框1;
	struct : 单选框 {
	}单选框2;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
}; extern __启动窗口 _启动窗口;
