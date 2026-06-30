#pragma once
#include "容器类.h"

class 分组框 : public 容器类
{
public:
	struct 参数 : 容器类::参数 {
		定义_子组件通用构造方法;
		StrW 标题;
		字体 字体;
		颜色 文本颜色 = 颜色::黑色;
		颜色 背景颜色 = 颜色::默认底色;
	} static _df;

	bool 创建(const 参数& cs, 容器类* 父窗口 = NULL, HWND 父句柄 = NULL);

	void 文本颜色(颜色 cr);

public:
	StrW 标题;
	void 标题_(const StrW& title);
	定义_字体成员;
	颜色 文本颜色_;
	定义_背景颜色成员;

	bool 通用事件_背景绘制(HDC hdc);

	virtual LRESULT 挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};
