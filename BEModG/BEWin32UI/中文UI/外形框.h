#pragma once
#include "子容器.hpp"

定义_枚举型(外形式样, int,
	无边框 = 0,
	矩形 = 1,
	圆角矩形 = 2,
	椭圆 = 3,
	凹入式 = 4,
	凸出式 = 5
);

// 线型：对应 Win32 Pen Styles
定义_枚举型(外框线型, int,
	无 = -1,
	实线 = PS_SOLID,
	划线 = PS_DASH,
	点线 = PS_DOT,
	点划线 = PS_DASHDOT,
	双点划线 = PS_DASHDOTDOT
);

class 外形框 : public 子容器<true>
{
public:
	struct 参数 : public 子容器<true>::参数 {
		外形式样 外形 = 外形式样::矩形;
		外框线型 线型 = 外框线型::实线;
		int 线宽 = 1;
		颜色 线条颜色 = 颜色::黑色;
		颜色 背景颜色 = 颜色::默认底色;
		int 圆角半径 = 10;
	} static _df;

	bool 创建(const 参数& cs, 容器类* 父窗口 = NULL, HWND 父句柄 = NULL);

	// 属性设置函数
	void 外形_(外形式样 s);
	void 线型_(外框线型 t);
	void 线宽_(int w);
	void 线条颜色_(颜色 c);
	void 圆角半径_(int r);

	bool 通用事件_窗口重画(PAINTSTRUCT& ps);

	// 透明子控件（如单选框、选择框）会通过 WM_ERASEBKGND 请求父容器
	// 提供它们所在位置的底图。外形框的背景原先只在 WM_PAINT 中绘制，
	// 因而作为父容器时原生 Static 过程会把黑色底图交给透明子控件。
	bool 通用事件_背景绘制(HDC hdc) override;

public:
	外形式样 外形;
	外框线型 线型;
	int 线宽, 圆角半径;
	颜色 外框颜色;
	定义_背景颜色成员;
};
