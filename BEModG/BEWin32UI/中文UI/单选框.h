#pragma once
#include "控件类.h"

/*WS_GROUP 风格：一组单选框中的第一个通常带有 WS_GROUP 风格，标志着一组的开始，
  仅限在同一个父窗口内，紧随其后的所有控件，只要它们没有 WS_GROUP 风格，
  它们就会被系统自动划归到单选框 A 所开启的这个组里，
  直到遇到下一个带有 WS_GROUP 风格的控件为止；
  如果单选框在不同的父窗口里则天然存在隔离。
  注：一个窗口中若第一个单选框即便不设置WS_GROUP，也视为往后的全部为同一组。
*/

class 单选框 : public 控件类
{
public:
	struct 参数 : 控件类::参数 {
		定义_子组件通用构造方法;
		StrW 标题 = L"单选框";
		字体 字体;
		颜色 文本颜色 = 颜色::黑色;
		颜色 背景颜色 = 颜色::默认底色;
		bool 标题居左 = false;
		bool 选中 = false;
		横向对齐 横向对齐方式 = 横向对齐::左对齐;
		纵向对齐 纵向对齐方式 = 纵向对齐::居中;
	} static _df;

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
	定义_标题成员;
	定义_字体成员;
	bool 选中;
	颜色 文本颜色;
	定义_背景颜色成员;
	bool 标题居左 = false;
	横向对齐 横向对齐方式;
	纵向对齐 纵向对齐方式;
};
