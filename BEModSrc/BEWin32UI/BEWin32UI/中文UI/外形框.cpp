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

class EXP 外形框 : public 子容器<true>
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

	bool 创建(const 参数& cs, 容器类* 父窗口 = NULL, HWND 父句柄 = NULL) {
		if (!子容器<true>::创建(cs, 父窗口, 父句柄)) return false;
		外形 = cs.外形;
		线型 = cs.线型;
		线宽 = cs.线宽;
		外框颜色 = cs.线条颜色;
		圆角半径 = cs.圆角半径;
		背景颜色_(cs.背景颜色, false);
		容器类::_子类化();
		return true;
	}

	// 属性设置函数
	void 外形_(外形式样 s) { 外形 = s; 重画(); }
	void 线型_(外框线型 t) { 线型 = t; 重画(); }
	void 线宽_(int w) { 线宽 = w; 重画(); }
	void 线条颜色_(颜色 c) { 外框颜色 = c; 重画(); }
	void 圆角半径_(int r) { 圆角半径 = r; 重画(); }

	bool 通用事件_窗口重画(PAINTSTRUCT& ps)
	{
		HDC hdc = BeginPaint(窗口句柄, &ps);
		RECT rc = { 0, 0, (int)dpi(宽度), (int)dpi(高度) };

		// 1. 绘制背景
		if (背景颜色 != 颜色::透明) {
			FillRect(hdc, &rc, _背景画刷);
		} else {
			UI_请求父窗绘制背景(窗口句柄, hdc, rc);
		}

		// 2. 绘制样式
		if (外形 == 外形式样::无边框) goto RETN_FALSE;

		// 系统效果类样式
		if (外形 == 外形式样::凹入式) {
			DrawEdge(hdc, &rc, EDGE_SUNKEN, BF_RECT);
			goto RETN_FALSE;
		}
		if (外形 == 外形式样::凸出式) {
			DrawEdge(hdc, &rc, EDGE_RAISED, BF_RECT);
			goto RETN_FALSE;
		}

		// 自绘形状类样式。使用几何画笔，保证划线/点线类在线宽>1时也保持样式。
		HPEN hPen = NULL;
		if (线型 != 外框线型::无) {
			LOGBRUSH lb = { BS_SOLID, (COLORREF)外框颜色, 0 };
			DWORD style = PS_GEOMETRIC | PS_ENDCAP_FLAT | PS_JOIN_MITER | (DWORD)线型;
			int penWidth = (int)dpi(线宽);
			if (penWidth < 1) penWidth = 1;
			hPen = ExtCreatePen(style, penWidth, &lb, 0, NULL);
		}
		HPEN hOldPen = hPen ? (HPEN)SelectObject(hdc, hPen) : (HPEN)SelectObject(hdc, GetStockObject(NULL_PEN));
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));

		if (外形 == 外形式样::矩形) {
			Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
		} else if (外形 == 外形式样::圆角矩形) {
			RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, dpi(圆角半径), dpi(圆角半径));
		} else if (外形 == 外形式样::椭圆) {
			Ellipse(hdc, rc.left, rc.top, rc.right, rc.bottom);
		}

		SelectObject(hdc, hOldPen);
		SelectObject(hdc, hOldBrush);
		if (hPen) DeleteObject(hPen);
	RETN_FALSE:
		EndPaint(窗口句柄, &ps);
		return false;
	}

	// 透明子控件（如单选框、选择框）会通过 WM_ERASEBKGND 请求父容器
	// 提供它们所在位置的底图。外形框的背景原先只在 WM_PAINT 中绘制，
	// 因而作为父容器时原生 Static 过程会把黑色底图交给透明子控件。
	bool 通用事件_背景绘制(HDC hdc) override
	{
		RECT rc;
		GetClientRect(窗口句柄, &rc);
		if (背景颜色 == 颜色::透明) {
			UI_请求父窗绘制背景(窗口句柄, hdc, rc);
		} else {
			FillRect(hdc, &rc, _背景画刷);
		}
		return false;
	}

public:
	外形式样 外形;
	外框线型 线型;
	int 线宽, 圆角半径;
	颜色 外框颜色;
	定义_背景颜色成员;
};
