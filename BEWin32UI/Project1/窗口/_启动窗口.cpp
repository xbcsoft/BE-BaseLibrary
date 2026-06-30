#include <BEMod.h>

struct __启动窗口 : 窗口
{
	void 事件_创建完毕()
	{
		调试输出("你好啊白易语言");
	}
	bool 事件_重画(PAINTSTRUCT & ps)
	{
		HDC hdc = BeginPaint(窗口句柄, &ps);
		//绘制位图(hdc, 位图::从内存创建(R::测试),150,10);
		EndPaint(窗口句柄, &ps);
		return true;
	}

#pragma region 组件成员
	struct _按钮1 : 按钮
	{
	}按钮1;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
} _启动窗口;