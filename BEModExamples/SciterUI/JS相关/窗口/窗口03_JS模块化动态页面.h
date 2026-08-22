#pragma once
#include <BEMod.h>

struct _窗口03_JS模块化动态页面 : 窗口
{
	SciterDom dom;
	void 事件_创建完毕();

	void _按钮1_被单击();
	
#pragma region 组件成员
	struct _st :SciterUI {
	} st;
	struct _按钮1 : 按钮 {
		void 事件_被单击();
	}按钮1;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
}; extern _窗口03_JS模块化动态页面 窗口03_JS模块化动态页面;
