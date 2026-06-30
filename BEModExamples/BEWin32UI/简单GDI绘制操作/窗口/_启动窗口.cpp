#include <BEMod.h>

struct __启动窗口 : 窗口
{
	void 事件_创建完毕()
	{
		调试输出("你好啊白易语言");
	}

	bool 通用事件_窗口重画(PAINTSTRUCT & ps)
	{
		绘制上下文 dc(窗口句柄, ps);
		dc.选择字体(字体(L"微软雅黑", 12));
		dc.置文本颜色(颜色::白色);
		dc.置背景模式(TRANSPARENT);
		dc.绘制文本(L"囧", 30, 30);

		HandlePEN pen = CreatePen(PS_SOLID, 1, 颜色::黄色);
		dc.选择画笔(pen);
		dc.选择画刷(绘制上下文::获取透明画刷());
		RoundRect(dc.hdc, 100, 20, 300, 200, 10, 10); //注意这些都是设备像素(按需dpi进行转换)

		return true;
	}

	bool 通用事件_背景绘制(HDC hdc)
	{
		绘制上下文 dc(hdc);
		dc.绘制位图(位图::从内存创建(R::背景图), 0, 0);
		return false; //这里要阻止向上传导
	}

#pragma region 组件成员

#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
} _启动窗口;