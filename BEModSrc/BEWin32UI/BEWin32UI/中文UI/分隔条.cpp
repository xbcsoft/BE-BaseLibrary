#include "控件类.h"

定义_枚举型(分隔条方向, char,
	水平 = 0, 垂直 = 1
);

class EXP 分隔条 : public 控件类
{
public:
	struct 参数 : 控件类::参数 {
		分隔条方向 方向 = 分隔条方向::水平;
		颜色 线条颜色 = 颜色::浅灰;
		颜色 点燃颜色 = 颜色(RGB(0, 120, 215));
		float 线条粗细 = 1;
		bool 可拖动 = true;
		bool 自动移动自身 = false;
	} static _df;

	分隔条方向 方向;
	颜色 线条颜色;
	颜色 点燃颜色;
	float 线条粗细;
	bool 可拖动;
	bool 自动移动自身;

	bool 创建(const 参数& cs, 容器类* 父窗口 = NULL, HWND 父句柄 = NULL) {
		控件类::_子初始(cs, 父窗口, 父句柄, true);
		方向 = cs.方向;
		线条颜色 = cs.线条颜色;
		点燃颜色 = cs.点燃颜色;
		线条粗细 = cs.线条粗细 < 0 ? 0 : cs.线条粗细;
		可拖动 = cs.可拖动;
		自动移动自身 = cs.自动移动自身;

		窗口句柄 = CreateWindowExW(0, L"STATIC", L"", _窗口风格 | SS_NOTIFY,
			dpi(左边), dpi(顶边), dpi(宽度), dpi(高度), 父窗口句柄,
			(HMENU)this, g_hInst, NULL);
		if (!窗口句柄) return false;

		控件类::_子类化();
		// CreateWindowExW 创建 STATIC 时的首次绘制发生在子类化之前；
		// 子类化完成后主动制造无效区，确保首次 WM_PAINT 进入本控件的自绘过程。
		重画(nil, true);
		return true;
	}

	void 方向_(分隔条方向 v) {
		方向 = v;
		重画(nil, true);
	}
	void 线条颜色_(颜色 v) {
		线条颜色 = v;
		重画(nil, true);
	}
	void 点燃颜色_(颜色 v) {
		点燃颜色 = v;
		if (_鼠标在内 || _正在拖动) 重画(nil, true);
	}
	void 线条粗细_(float v) {
		线条粗细 = v < 0 ? 0 : v;
		重画(nil, true);
	}
	void 可拖动_(bool v) {
		可拖动 = v;
		if (!v && _正在拖动) {
			_正在拖动 = false;
			if (GetCapture() == 窗口句柄) ReleaseCapture();
		}
	}
	void 自动移动自身_(bool v) {
		自动移动自身 = v;
	}

	virtual void 事件_位置被改变(float 新位置) {}
	virtual void 事件_拖动结束(float 最终位置) {}

	LRESULT 挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

private:
	void _绘制(HDC hdc) {
		RECT rc{};
		GetClientRect(窗口句柄, &rc);
		UI_请求父窗绘制背景(窗口句柄, hdc, rc);

		颜色 当前颜色 = (_鼠标在内 || _正在拖动) ? 点燃颜色 : 线条颜色;
		if (当前颜色 == 颜色::透明 || 线条粗细 <= 0) return;

		int 粗细 = dpi(线条粗细);
		if (粗细 < 1) 粗细 = 1;
		RECT line = rc;
		if (方向 == 分隔条方向::水平) {
			int y = (rc.bottom - rc.top - 粗细) / 2;
			line.top = y;
			line.bottom = y + 粗细;
		} else {
			int x = (rc.right - rc.left - 粗细) / 2;
			line.left = x;
			line.right = x + 粗细;
		}
		HBRUSH brush = CreateSolidBrush(当前颜色.val);
		FillRect(hdc, &line, brush);
		DeleteObject(brush);
	}

	bool _鼠标在内 = false;
	bool _正在拖动 = false;
	POINT _拖动起始屏幕点{};
	float _拖动起始位置 = 0;
	float _拖动当前位置 = 0;

	void _开始跟踪鼠标() {
		TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, 窗口句柄, 0 };
		TrackMouseEvent(&tme);
	}
	void _刷新鼠标状态(bool 在内) {
		if (_鼠标在内 == 在内) return;
		_鼠标在内 = 在内;
		重画(nil, true);
	}
	float _当前位置() const {
		RECT rc{};
		GetWindowRect(窗口句柄, &rc);
		HWND parent = GetParent(窗口句柄);
		if (parent) MapWindowPoints(NULL, parent, (LPPOINT)&rc, 2);
		return 方向 == 分隔条方向::水平
			? rdpi((float)rc.top) : rdpi((float)rc.left);
	}
	void _拖动到(POINT 屏幕点) {
		if (!_正在拖动 || !可拖动) return;
		int deltaPx = 方向 == 分隔条方向::水平
			? 屏幕点.y - _拖动起始屏幕点.y
			: 屏幕点.x - _拖动起始屏幕点.x;
		float 新位置 = _拖动起始位置 + rdpi((float)deltaPx);

		HWND parent = GetParent(窗口句柄);
		if (parent) {
			RECT rc{};
			GetClientRect(parent, &rc);
			float 上限 = 方向 == 分隔条方向::水平
				? rdpi((float)(rc.bottom - rc.top)) - 高度
				: rdpi((float)(rc.right - rc.left)) - 宽度;
			if (上限 < 0) 上限 = 0;
			if (新位置 < 0) 新位置 = 0;
			if (新位置 > 上限) 新位置 = 上限;
		}

		if (新位置 == _拖动当前位置) return;
		_拖动当前位置 = 新位置;
		if (自动移动自身) {
			if (方向 == 分隔条方向::水平)
				移动(空, 新位置, 空, 空, true);
			else
				移动(新位置, 空, 空, 空, true);
		}
		事件_位置被改变(新位置);
	}
};

LRESULT 分隔条::挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_ERASEBKGND:
		return 1;
	case WM_PAINT: {
		PAINTSTRUCT ps;
		绘制上下文 dc(hwnd, ps);
		_绘制(dc.hdc);
		return 1;
	}
	case WM_MOUSEMOVE: {
		if (!_鼠标在内) {
			_刷新鼠标状态(true);
			_开始跟踪鼠标();
		}
		if (_正在拖动) {
			POINT pt{};
			GetCursorPos(&pt);
			_拖动到(pt);
		}
		return 0;
	}
	case WM_MOUSELEAVE:
		if (!_正在拖动) _刷新鼠标状态(false);
		return 0;
	case WM_LBUTTONDOWN:
		if (可拖动) {
			SetCapture(hwnd);
			_正在拖动 = true;
			GetCursorPos(&_拖动起始屏幕点);
			_拖动起始位置 = _当前位置();
			_拖动当前位置 = _拖动起始位置;
			_刷新鼠标状态(true);
		}
		return 0;
	case WM_LBUTTONUP:
		if (_正在拖动) {
			POINT pt{};
			GetCursorPos(&pt);
			_拖动到(pt);
			float 最终位置 = _拖动当前位置;
			_正在拖动 = false;
			if (GetCapture() == hwnd) ReleaseCapture();
			事件_拖动结束(最终位置);
			RECT rc{};
			GetClientRect(hwnd, &rc);
			POINT cp{};
			GetCursorPos(&cp);
			ScreenToClient(hwnd, &cp);
			_刷新鼠标状态(PtInRect(&rc, cp) != FALSE);
		}
		return 0;
	case WM_CAPTURECHANGED:
		if ((HWND)lParam != hwnd) {
			_正在拖动 = false;
			重画(nil, true);
		}
		break;
	case WM_SETCURSOR:
		if (LOWORD(lParam) == HTCLIENT) {
			SetCursor(LoadCursor(NULL,
				方向 == 分隔条方向::水平 ? IDC_SIZENS : IDC_SIZEWE));
			return TRUE;
		}
		break;
	}
	return 控件类::挂接消息(hwnd, msg, wParam, lParam);
}
