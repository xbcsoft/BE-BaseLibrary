#include "控件类.h"

class EXP 按钮 :public 控件类
{
public:
	struct 参数 : 控件类::参数 {
		定义_子组件通用构造方法;
		StrW 标题 = L"按钮";
		字体 字体;
	}static _df;

	bool 创建(const 参数& cs, 容器类* 父窗口 = NULL, HWND 父句柄 = NULL) {
		控件类::_子初始(cs, 父窗口, 父句柄);

		标题 = cs.标题;
		窗口句柄 = CreateWindowExW(0, L"Button", 标题, _窗口风格,
			dpi(左边), dpi(顶边), dpi(宽度), dpi(高度), 父窗口句柄,
			(HMENU)this, g_hInst, NULL);
		if (!窗口句柄) return false;

		置字体(cs.字体);
		控件类::_子类化();
		return true;
	}

public:
	virtual void 事件_被单击() {}
	virtual LRESULT 挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
public:
	定义_标题成员;
	定义_字体成员;
};

LRESULT 按钮::挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case BE_REFLECT + WM_COMMAND: {
		事件_被单击();
		break;
	}
	}
	return 窗口基类::挂接消息(hwnd, msg, wParam, lParam);
}
