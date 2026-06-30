#pragma once
#include <BEMod.h>
#include "../扩展界面库一.h"

struct _窗口1 : 窗口
{
	void _导出按钮_被单击();

	void _载入按钮_被单击();

	void 事件_创建完毕();

#pragma region 组件成员
	树型框 树型框1;
	struct _导出按钮 : 按钮 {
		void 事件_被单击();
	} 导出按钮;

	struct _载入按钮 : 按钮 {
		void 事件_被单击();
	} 载入按钮;
#pragma endregion

	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
}; extern _窗口1 窗口1;
