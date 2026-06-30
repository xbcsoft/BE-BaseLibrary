#pragma once
#include "标签.h"

class 超级链接框 : public 标签
{
public:
	struct 参数 : 控件类::参数 {
		定义_子组件通用构造方法;
		StrW 标题 = L"超级链接框";
		StrW 地址; //支持http://, mailto://
		颜色 文本颜色 = RGB(0, 0, 255);
		颜色 访问后的颜色 = RGB(128, 0, 128);
		颜色 热点颜色 = RGB(0, 102, 204);
		颜色 背景颜色 = 颜色::透明;
		标签效果 效果 = 标签效果::通常;
		标签边框 边框 = 标签边框::无边框;
		字体 字体;
		bool 是否自动折行 = false;
		横向对齐 横向对齐方式 = 横向对齐::左对齐;
		纵向对齐 纵向对齐方式 = 纵向对齐::顶对齐;
	}static _df;

	bool 创建(const 参数& cs, 容器类* 父窗口 = NULL, HWND 父句柄 = NULL);

	void 地址_(const StrW& addr);

	void 访问后的颜色_(颜色 cr);

	void 热点颜色_(颜色 cr);

public:
	virtual LRESULT 挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
	void 打开链接();

public:
	StrW 地址;
	颜色 访问后的颜色;
	颜色 热点颜色;
	颜色 初始颜色;
	bool 鼠标悬停 = false;
	bool 已访问 = false;
};
