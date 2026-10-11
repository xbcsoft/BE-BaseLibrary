#include <BEMod.h>

struct __启动窗口 : 窗口
{
	LRESULT 事件_反馈(WPARAM wParam, LPARAM lParam)
	{
		调试输出(wParam, lParam);
		return 0;
	}

	void _取现行子夹按钮_被单击()
	{
		信息框(格式化文本<W>(L"索引(%d): ", 选择夹1.现行子夹)
			+选择夹1.取子夹标题(选择夹1.现行子夹));
	}

	void _切换子夹按钮_被单击()
	{
		int 下一个 = (选择夹1.现行子夹 + 1) % 2;
		选择夹1.现行子夹_(下一个);
	}

	void _隐藏自身按钮_被单击()
	{
		bool 新状态 = !选择夹1.隐藏自身;
		选择夹1.隐藏自身_(新状态);
		隐藏自身按钮.标题_(新状态 ? L"隐藏自身:开" : L"隐藏自身:关");
	}

	void _背景透明按钮_被单击()
	{
		static bool isTrans = false;
		isTrans = !isTrans;
		选择夹1.背景颜色_(isTrans ? 颜色::透明 : 颜色::白色);
		背景透明按钮.标题_(isTrans ? L"底色:透明" : L"底色:白色");
	}


#pragma region 组件成员
	struct :选择夹 {
	}选择夹1;
	struct :按钮 {
		void 事件_被单击() { _启动窗口._取现行子夹按钮_被单击(); }
	}取现行子夹按钮;

	struct _切换子夹按钮 : 按钮 {
		void 事件_被单击() { _启动窗口._切换子夹按钮_被单击(); }
	} 切换子夹按钮;

	struct _隐藏自身按钮 : 按钮 {
		void 事件_被单击() { _启动窗口._隐藏自身按钮_被单击(); }
	} 隐藏自身按钮;

	struct _背景透明按钮 : 按钮 {
		void 事件_被单击() { _启动窗口._背景透明按钮_被单击(); }
	} 背景透明按钮;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态);
} _启动窗口;