#include <BEMod.h>

struct __启动窗口 : 窗口
{
	LRESULT 事件_反馈(WPARAM wParam, LPARAM lParam)
	{
		调试输出(wParam, lParam);
		return 0;
	}
	void _按钮2_被单击()
	{
		信息框(静态按钮2.标题);
	}

#pragma region 组件成员
	标签 静态标签1;        //第一种，直接加，通常是不需要任何回调事件

	struct : 按钮 {        //第二种，匿名类继承，已经可以加虚函数覆盖了
		void 事件_被单击() { 信息框(标题); }
	}静态按钮1;

	struct _按钮2 : 按钮 { //第三种，就是在第二种基础上有正规类名
		void 事件_被单击() {
			_启动窗口._按钮2_被单击(); //还可以进行事件转发
			//这样在那边的函数里就能直接访问启动窗口的成员了
		}
	}静态按钮2;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
} _启动窗口;