#include <BEMod.h>

struct __启动窗口 : 窗口
{
	static void 按钮1_移入事件(HWND hWnd)
	{
		_启动窗口.按钮1.标题_(L"移入");
		printf("移入");
	}
	static void 按钮1_移出事件(HWND hWnd)
	{
		_启动窗口.按钮1.标题_(L"移出");
		printf("移出");
	}

	void 事件_创建完毕()
	{
		//调试输出("你好啊白易语言");
		控件_添加鼠标移入移出事件(按钮1.窗口句柄, 按钮1_移入事件, 按钮1_移出事件);
	}

	void _按钮1_被单击()
	{
		信息框(123);
	}

#pragma region 组件成员
	struct _按钮1 : 按钮 {
		void 事件_被单击() { _启动窗口._按钮1_被单击(); }
	}按钮1;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
} _启动窗口;