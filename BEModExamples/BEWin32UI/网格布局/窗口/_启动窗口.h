#pragma once
#include <BEMod.h>

struct __启动窗口 : 窗口
{
	网格布局 布局;

	void 事件_创建完毕();

	void 事件_尺寸被改变();

	void _按钮1_被单击();

	void _按钮2_被单击();

#pragma region 组件成员
	struct _按钮1 : 按钮 {
		void 事件_被单击();
	} 按钮1;

	struct _按钮2 : 按钮 {
		void 事件_被单击();
	} 按钮2;

	编辑框 编辑框_导航;
	编辑框 编辑框_内容;
	标签 标签_状态栏;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
}; extern __启动窗口 _启动窗口;
