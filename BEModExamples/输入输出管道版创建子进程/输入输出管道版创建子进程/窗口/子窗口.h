#pragma once
#include <BEMod.h>

struct _子窗口 : 窗口
{
	static void OnOutputReceived(const StrA& output);

	void 事件_首次激活();

	void 事件_被销毁();

	void _计算按钮_被单击();

#pragma region 组件成员
	编辑框 输入编辑框;

	struct : 按钮 {
		void 事件_被单击();
	} 计算按钮;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);

	HANDLE _hStdinWrite = NULL;
}; extern _子窗口 子窗口;
