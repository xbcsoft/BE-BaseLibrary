#include "控件类.h"

定义_枚举型(进度条方向, int, 横向 = 0, 纵向 = PBS_VERTICAL);
定义_枚举型(进度条显示方式, int, 分块 = 0, 平滑 = PBS_SMOOTH);

class EXP 进度条 : public 控件类
{
public:
	struct 参数 : 控件类::参数 {
		定义_子组件通用构造方法;
		进度条方向 方向 = 进度条方向::横向;
		进度条显示方式 显示方式 = 进度条显示方式::分块;
		int 最小位置 = 0;
		int 最大位置 = 100;
		int 位置 = 0;
	} static _df;

	bool 创建(const 参数& cs, 容器类* 父窗口 = NULL, HWND 父句柄 = NULL) {
		控件类::_子初始(cs, 父窗口, 父句柄);
		方向 = cs.方向;
		显示方式 = cs.显示方式;
		最小位置 = cs.最小位置;
		最大位置 = cs.最大位置;

		DWORD style = _窗口风格 | (int)方向 | (int)显示方式;

		窗口句柄 = CreateWindowExW(0, PROGRESS_CLASSW, L"", style,
			dpi(左边), dpi(顶边), dpi(宽度), dpi(高度), 父窗口句柄,
			(HMENU)this, g_hInst, NULL);
		if (!窗口句柄) return false;

		SendMessageW(窗口句柄, PBM_SETRANGE32, 最小位置, 最大位置);
		位置_(cs.位置);

		return true;
	}

	void 最小位置_(int v) {
		最小位置 = v;
		SendMessageW(窗口句柄, PBM_SETRANGE32, 最小位置, 最大位置);
	}

	void 最大位置_(int v) {
		最大位置 = v;
		SendMessageW(窗口句柄, PBM_SETRANGE32, 最小位置, 最大位置);
	}

	void 位置_(int v) {
		位置 = v;
		SendMessageW(窗口句柄, PBM_SETPOS, 位置, 0);
	}

	void 方向_(进度条方向 d) {
		方向 = d;
		DWORD style = GetWindowLongW(窗口句柄, GWL_STYLE);
		if (d == 进度条方向::纵向) {
			style |= PBS_VERTICAL;
		} else {
			style &= ~PBS_VERTICAL;
		}
		SetWindowLongW(窗口句柄, GWL_STYLE, style);
		SetWindowPos(窗口句柄, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	}

	void 显示方式_(进度条显示方式 s) {
		显示方式 = s;
		DWORD style = GetWindowLongW(窗口句柄, GWL_STYLE);
		if (s == 进度条显示方式::平滑) {
			style |= PBS_SMOOTH;
		} else {
			style &= ~PBS_SMOOTH;
		}
		SetWindowLongW(窗口句柄, GWL_STYLE, style);
		SetWindowPos(窗口句柄, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	}

public:
	进度条方向 方向;
	进度条显示方式 显示方式;
	int 最小位置;
	int 最大位置;
	int 位置;
};
