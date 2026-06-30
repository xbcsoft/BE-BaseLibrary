#pragma once
#include "子容器.hpp"

// 整合后的单一外形定义
定义_枚举型(外形式样, int,
	无边框 = 0,
	矩形 = 1,
	圆角矩形 = 2,
	椭圆 = 3,
	凹入式 = 4,
	凸出式 = 5
);

// 线型：对应 Win32 Pen Styles
定义_枚举型(外框线型, int, 实线 = PS_SOLID, 虚线 = PS_DASH, 点线 = PS_DOT, 次细实线 = PS_INSIDEFRAME);

class 外形框 : public 子容器<true>
{
public:
	struct 参数 : public 子容器<true>::参数 {
		定义_子组件通用构造方法;
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
	void 线条线型_(外框线型 t);
	void 线宽_(int w);
	void 线条颜色_(颜色 c);
	void 圆角半径_(int r);

	bool 通用事件_窗口重画(PAINTSTRUCT& ps);

public:
	外形式样 外形;
	外框线型 线型;
	int 线宽, 圆角半径;
	颜色 外框颜色;
	定义_背景颜色成员;
};
