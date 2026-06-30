#pragma once
#include <BEMod.h>

struct __启动窗口 : 窗口
{
	void _打开计算窗口按钮_被单击();

#pragma region 组件成员
	struct : 按钮 {
		void 事件_被单击();
	} 打开计算窗口按钮;

	编辑框 结果编辑框;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
}; extern __启动窗口 _启动窗口;
