#pragma once
#include "控件类.h"

class 按钮 :public 控件类
{
public:
	struct 参数 : 控件类::参数 {
		定义_子组件通用构造方法;
		StrW 标题 = L"按钮";
		字体 字体;
	}static _df;

	bool 创建(const 参数& cs, 容器类* 父窗口 = NULL, HWND 父句柄 = NULL);

public:
	virtual void 事件_被单击();
	virtual LRESULT 挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
public:
	定义_标题成员;
	定义_字体成员;
};
