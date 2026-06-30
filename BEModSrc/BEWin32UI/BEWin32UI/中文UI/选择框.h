#pragma once
#include "控件类.h"

class 选择框 : public 控件类
{
public:
	struct 参数 : 控件类::参数 {
		定义_子组件通用构造方法;
		StrW 标题 = L"选择框";
		字体 字体;
		颜色 文本颜色 = 颜色::黑色;
		颜色 背景颜色 = 颜色::默认底色;
		bool 标题居左 = false;
		bool 选中 = false;
		横向对齐 横向对齐方式 = 横向对齐::左对齐;
		纵向对齐 纵向对齐方式 = 纵向对齐::居中;
	} static _df;
	定义_标题成员;
	定义_字体成员;

	bool 创建(const 参数& cs, 容器类* 父窗口 = NULL, HWND 父句柄 = NULL);

	void 文本颜色_(颜色 cr);

	void 选中_(bool isChecked);

	bool _取选中();

	void 标题居左_(bool isLeft);

	void 横向对齐方式_(横向对齐 mode);

	void 纵向对齐方式_(纵向对齐 mode);

public:
	virtual void 事件_被单击();
	virtual LRESULT 挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	bool 选中;
	颜色 文本颜色;
	定义_背景颜色成员;
	bool 标题居左;
	横向对齐 横向对齐方式;
	纵向对齐 纵向对齐方式;
};
