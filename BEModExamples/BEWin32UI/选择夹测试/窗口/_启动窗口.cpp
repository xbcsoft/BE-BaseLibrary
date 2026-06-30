#include <BEMod.h>

struct __启动窗口 : 窗口
{
	void _按钮1_被单击()
	{
		选择夹1.背景颜色_(颜色::默认底色);
		信息框(选择夹1.取子夹标题(选择夹1.现行子夹));
	}

	LRESULT 事件_反馈(WPARAM wParam, LPARAM lParam)
	{
		调试输出(wParam, lParam);
		return 0;
	}


#pragma region 组件成员
	struct _选择夹1 :选择夹{
	}选择夹1;

	struct _按钮1 :按钮 {
		void 事件_被单击() { _启动窗口._按钮1_被单击(); }
	}按钮1;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态);
} _启动窗口;