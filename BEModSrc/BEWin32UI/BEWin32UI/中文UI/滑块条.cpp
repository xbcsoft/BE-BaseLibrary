#include "控件类.h"

定义_枚举型(滑块条方向, int, 横向 = TBS_HORZ, 纵向 = TBS_VERT);
定义_枚举型(滑块条刻度类型, int, 下_右 = TBS_BOTTOM, 上_左 = TBS_TOP, 两端 = TBS_BOTH, 无刻度 = TBS_NOTICKS);

class EXP 滑块条 : public 控件类
{
public:
	struct 参数 : 控件类::参数 {
		定义_子组件通用构造方法;
		滑块条方向 方向 = 滑块条方向::横向;
		滑块条刻度类型 刻度类型 = 滑块条刻度类型::下_右;
		int 单位刻度值 = 1;
		bool 允许选择 = false;
		int 首选择位置 = 0;
		int 选择长度 = 0;
		int 页改变值 = 5;
		int 行改变值 = 1;
		int 最小位置 = 0;
		int 最大位置 = 10;
		int 位置 = 0;
	} static _df;

	bool 创建(const 参数& cs, 容器类* 父窗口 = NULL, HWND 父句柄 = NULL) {
		控件类::_子初始(cs, 父窗口, 父句柄);
		方向 = cs.方向;
		刻度类型 = cs.刻度类型;
		单位刻度值 = cs.单位刻度值;
		允许选择 = cs.允许选择;
		首选择位置 = cs.首选择位置;
		选择长度 = cs.选择长度;
		页改变值 = cs.页改变值;
		行改变值 = cs.行改变值;
		最小位置 = cs.最小位置;
		最大位置 = cs.最大位置;
		位置 = cs.位置;

		DWORD style = _窗口风格 | (int)方向 | (int)刻度类型 | 0x1000 /* TBS_TRANSPARENTBKGND */ | 0x0001 /* TBS_AUTOTICKS */;
		if (允许选择) style |= TBS_ENABLESELRANGE;

		窗口句柄 = CreateWindowExW(0, TRACKBAR_CLASSW, L"", style,
			dpi(左边), dpi(顶边), dpi(宽度), dpi(高度), 父窗口句柄,
			(HMENU)this, g_hInst, NULL);
		if (!窗口句柄) return false;

		SendMessageW(窗口句柄, TBM_SETRANGEMIN, FALSE, 最小位置);
		SendMessageW(窗口句柄, TBM_SETRANGEMAX, TRUE, 最大位置);
		SendMessageW(窗口句柄, TBM_SETTICFREQ, 单位刻度值, 0);
		if (允许选择) {
			SendMessageW(窗口句柄, TBM_SETSEL, TRUE, MAKELPARAM(首选择位置, 首选择位置 + 选择长度));
		}
		SendMessageW(窗口句柄, TBM_SETPAGESIZE, 0, 页改变值);
		SendMessageW(窗口句柄, TBM_SETLINESIZE, 0, 行改变值);
		SendMessageW(窗口句柄, TBM_SETPOS, TRUE, 位置);

		控件类::_子类化();
		return true;
	}

	void 方向_(滑块条方向 v) {
		方向 = v;
		DWORD style = GetWindowLongW(窗口句柄, GWL_STYLE);
		style &= ~(TBS_HORZ | TBS_VERT);
		style |= (int)v;
		SetWindowLongW(窗口句柄, GWL_STYLE, style);
		SetWindowPos(窗口句柄, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	}

	void 刻度类型_(滑块条刻度类型 v) {
		刻度类型 = v;
		DWORD style = GetWindowLongW(窗口句柄, GWL_STYLE);
		style &= ~(TBS_BOTTOM | TBS_TOP | TBS_BOTH | TBS_NOTICKS);
		style |= (int)v | 0x0001 /* TBS_AUTOTICKS */;
		SetWindowLongW(窗口句柄, GWL_STYLE, style);
		SetWindowPos(窗口句柄, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	}

	void 单位刻度值_(int v) {
		单位刻度值 = v;
		SendMessageW(窗口句柄, TBM_SETTICFREQ, v, 0);
	}

	void 允许选择_(bool v) {
		允许选择 = v;
		DWORD style = GetWindowLongW(窗口句柄, GWL_STYLE);
		if (v) style |= TBS_ENABLESELRANGE;
		else style &= ~TBS_ENABLESELRANGE;
		SetWindowLongW(窗口句柄, GWL_STYLE, style);
		SetWindowPos(窗口句柄, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	}

	void 首选择位置_(int v) {
		首选择位置 = v;
		if (允许选择) SendMessageW(窗口句柄, TBM_SETSEL, TRUE, MAKELPARAM(v, v + 选择长度));
	}

	void 选择长度_(int v) {
		选择长度 = v;
		if (允许选择) SendMessageW(窗口句柄, TBM_SETSEL, TRUE, MAKELPARAM(首选择位置, 首选择位置 + v));
	}

	void 页改变值_(int v) {
		页改变值 = v;
		SendMessageW(窗口句柄, TBM_SETPAGESIZE, 0, v);
	}

	void 行改变值_(int v) {
		行改变值 = v;
		SendMessageW(窗口句柄, TBM_SETLINESIZE, 0, v);
	}

	void 最小位置_(int v) {
		最小位置 = v;
		SendMessageW(窗口句柄, TBM_SETRANGEMIN, TRUE, v);
	}

	void 最大位置_(int v) {
		最大位置 = v;
		SendMessageW(窗口句柄, TBM_SETRANGEMAX, TRUE, v);
	}

	void 位置_(int v) {
		位置 = v;
		SendMessageW(窗口句柄, TBM_SETPOS, TRUE, v);
	}

public:
	virtual void 事件_位置被改变() {}

	virtual LRESULT 挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		switch (msg) {
		case BE_REFLECT + WM_HSCROLL:
		case BE_REFLECT + WM_VSCROLL: {
			位置 = SendMessageW(窗口句柄, TBM_GETPOS, 0, 0);
			事件_位置被改变();
			break;
		}
		}
		return 窗口基类::挂接消息(hwnd, msg, wParam, lParam);
	}

public:
	滑块条方向 方向;
	滑块条刻度类型 刻度类型;
	int 单位刻度值;
	bool 允许选择;
	int 首选择位置;
	int 选择长度;
	int 页改变值;
	int 行改变值;
	int 最小位置;
	int 最大位置;
	int 位置;
};
