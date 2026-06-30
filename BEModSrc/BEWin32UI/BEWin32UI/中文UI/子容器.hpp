#pragma once
#include "容器类.h"

template <bool 背景是否自定义 = false>
class 子容器 : public 容器类
{
public:
	struct 参数 : 容器类::参数 {
		定义_子组件通用构造方法;
	};

	bool 创建(const 参数& cs, 容器类* 父窗口 = NULL, HWND 父句柄 = NULL) {
		if (!容器类::_子初始(cs, 父窗口, 父句柄)) return false;

		// 子容器默认作为透明背景或自行绘制的载体
		窗口句柄 = CreateWindowExW(WS_EX_CONTROLPARENT, L"Static", L"", _窗口风格,
			(int)dpi(左边), (int)dpi(顶边), (int)dpi(宽度), (int)dpi(高度), 父窗口句柄, (HMENU)this, g_hInst, NULL);

		if constexpr (!背景是否自定义) {
			窗口基类::_子类化();
		}

		return 窗口句柄!=NULL;
	}

	颜色 _取背景颜色(颜色** out = 0) override {
		return 颜色::透明;
	}

	virtual void 背景颜色_(颜色 cr, bool 是否重画 = true) override {
		窗口基类::背景颜色_(cr, 是否重画);
	}

	LRESULT 挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		if constexpr (!背景是否自定义) {
			switch (msg) {
			case WM_ERASEBKGND: { //这个是防止按钮本身出现黑边框
				HDC hdc = (HDC)wParam;
				RECT rc = { 0, 0, (int)dpi(宽度), (int)dpi(高度) };
				UI_请求父窗绘制背景(窗口句柄, hdc, rc);
				ValidateRect(hwnd, NULL); //主动报告系统此区域有效
				return 1;
			}
			case WM_PAINT: { //这个是防止仅重画而没有背景擦除
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(窗口句柄, &ps);
				RECT rc = { 0, 0, (int)dpi(宽度), (int)dpi(高度) };
				UI_请求父窗绘制背景(窗口句柄, hdc, rc);
				EndPaint(窗口句柄, &ps);
				return 1;
			}
			}
		}
		return 容器类::挂接消息(hwnd, msg, wParam, lParam);
	}
};