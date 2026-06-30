#include "控件类.h"

定义_枚举型(标签效果, char,
	通常 = 0, 凹入 = 1, 凸出 = 2, 阴影 = 3
);

定义_枚举型(标签边框, char,
	无边框 = 0, 凹入式 = 1, 凸出式 = 2, 浅凹入式 = 3, 镜框式 = 4, 单线边框式 = 5, 渐变镜框式 = 6
);

class EXP 标签 :public 控件类
{
public:
	struct 参数 : 控件类::参数 {
		定义_子组件通用构造方法;
		StrW 标题 = L"标签";
		标签效果 效果 = 标签效果::通常;
		标签边框 边框 = 标签边框::无边框;
		字体 字体;
		颜色 文本颜色 = 颜色::黑色;
		颜色 背景颜色 = 颜色::默认底色;
		bool 是否自动折行 = false;
		横向对齐 横向对齐方式 = 横向对齐::左对齐;
		纵向对齐 纵向对齐方式 = 纵向对齐::顶对齐;
	}static _df;

	bool 创建(const 参数& cs, 容器类* 父窗口 = NULL, HWND 父句柄 = NULL) {
		控件类::_子初始(cs, 父窗口, 父句柄, true);
		效果 = cs.效果;
		边框 = cs.边框;
		文本颜色 = cs.文本颜色;
		横向对齐方式 = cs.横向对齐方式;
		是否自动折行 = cs.是否自动折行;
		纵向对齐方式 = cs.纵向对齐方式;
		标题 = cs.标题;
		背景颜色_(cs.背景颜色, false);

		// 直接使用系统 STATIC 类，子类化后由当前类接管 WM_PAINT 自绘文本。
		窗口句柄 = CreateWindowExW(0, L"STATIC", L"", _窗口风格 | SS_NOTIFY,
			dpi(左边), dpi(顶边), dpi(宽度), dpi(高度), 父窗口句柄,
			(HMENU)this, g_hInst, NULL);

		if (!窗口句柄) return false;
		控件类::_子类化();
		置字体(cs.字体);
		return true;
	}

	void 标题_(const StrW& tit) {
		标题 = tit;
		//背景颜色(背景颜色_);
		重画(nil, true);
	}

	void 文本颜色_(颜色 cr) {
		文本颜色 = cr; 重画();
	}

	void 横向对齐方式_(横向对齐 mode) {
		横向对齐方式 = mode; 重画();
	}

	void 是否自动折行_(bool is) {
		是否自动折行 = is; 重画();
	}

	void 纵向对齐方式_(纵向对齐 mode) {
		纵向对齐方式 = mode; 重画();
	}

	void 效果_(标签效果 eff) {
		效果 = eff; 重画();
	}

	void 边框_(标签边框 bor) {
		边框 = bor; 重画();
	}
public:
	定义_字体成员;
	StrW 标题;
	标签效果 效果;
	标签边框 边框;
	颜色 文本颜色;
	定义_背景颜色成员;
	横向对齐 横向对齐方式;
	纵向对齐 纵向对齐方式;
	bool 是否自动折行;

	virtual LRESULT 事件_反馈(WPARAM wParam, LPARAM lParam) { return 0; }

	LRESULT 挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg) {
		case WM_GETTEXTLENGTH:
			return (LRESULT)标题.len();
		case WM_GETTEXT: {
			int len = 标题.len();
			int cap = (int)wParam - 1;
			if (cap < 0) return 0;
			if (len > cap) len = cap;
			memcpy((wchar_t*)lParam, (charW*)标题, len * sizeof(wchar_t));
			((wchar_t*)lParam)[len] = 0;
			return len;
		}
		case WM_ERASEBKGND:
			return 1;
		case WM_PAINT: {
			PAINTSTRUCT ps;
			绘制上下文 dc(hwnd, ps);
			HDC hdc = dc.hdc; RECT rc;
			GetClientRect(窗口句柄, &rc);

			// 1. 绘制背景
			if (背景颜色 != 颜色::透明) {
				FillRect(hdc, &rc, _背景画刷);
			}

			// 2. 绘制边框
			if (边框 == 标签边框::凹入式) {
				DrawEdge(hdc, &rc, EDGE_SUNKEN, BF_RECT);
				InflateRect(&rc, -dpi(2), -dpi(2));
			} else if (边框 == 标签边框::浅凹入式) {
				// 类似编辑框的一像素凹陷效果
				DrawEdge(hdc, &rc, BDR_SUNKENOUTER, BF_RECT);
				InflateRect(&rc, -dpi(1), -dpi(1));
			} else if (边框 == 标签边框::单线边框式) {
				HBRUSH hBr = CreateSolidBrush(RGB(128, 128, 128));
				FrameRect(hdc, &rc, hBr);
				DeleteObject(hBr);
				InflateRect(&rc, -dpi(1), -dpi(1));
			} else if (边框 == 标签边框::镜框式) {
				DrawEdge(hdc, &rc, EDGE_ETCHED, BF_RECT);
				InflateRect(&rc, -dpi(2), -dpi(2));
			} else if (边框 == 标签边框::凸出式) {
				DrawEdge(hdc, &rc, EDGE_RAISED, BF_RECT);
				InflateRect(&rc, -dpi(2), -dpi(2));
			}

			// 3. 计算对齐参数
			UINT format = DT_EXTERNALLEADING;
			if (横向对齐方式 == 横向对齐::居中) format |= DT_CENTER;
			else if (横向对齐方式 == 横向对齐::右对齐) format |= DT_RIGHT;
			else format |= DT_LEFT;

			if (纵向对齐方式 == 纵向对齐::居中) format |= DT_VCENTER;
			else if (纵向对齐方式 == 纵向对齐::底对齐) format |= DT_BOTTOM;
			else format |= DT_TOP;

			if (!是否自动折行) {
				format |= DT_SINGLELINE;
			} else {
				format |= DT_WORDBREAK;
				if (纵向对齐方式 != 纵向对齐::顶对齐) {
					RECT rcCalc = rc;
					DrawTextW(hdc, 标题, -1, &rcCalc, format | DT_CALCRECT);
					int h = rcCalc.bottom - rcCalc.top;
					if (纵向对齐方式 == 纵向对齐::居中) rc.top = (rc.bottom - rc.top - h) / 2;
					else rc.top = rc.bottom - h;
					rc.bottom = rc.top + h;
				}
			}

			// 4. 绘制特效
			if (效果 == 标签效果::阴影) {
				SetTextColor(hdc, RGB(128, 128, 128));
				RECT rcEff = rc; OffsetRect(&rcEff, dpi(1), dpi(1));
				DrawTextW(hdc, 标题, -1, &rcEff, format);
			} else if (效果 == 标签效果::凹入) {
				SetTextColor(hdc, RGB(255, 255, 255));
				RECT rcEff = rc; OffsetRect(&rcEff, dpi(1), dpi(1));
				DrawTextW(hdc, 标题, -1, &rcEff, format);
				SetTextColor(hdc, RGB(128, 128, 128));
				rcEff = rc; OffsetRect(&rcEff, dpi(-1), dpi(-1));
				DrawTextW(hdc, 标题, -1, &rcEff, format);
			} else if (效果 == 标签效果::凸出) {
				SetTextColor(hdc, RGB(128, 128, 128));
				RECT rcEff = rc; OffsetRect(&rcEff, dpi(1), dpi(1));
				DrawTextW(hdc, 标题, -1, &rcEff, format);
				SetTextColor(hdc, RGB(255, 255, 255));
				rcEff = rc; OffsetRect(&rcEff, dpi(-1), dpi(-1));
				DrawTextW(hdc, 标题, -1, &rcEff, format);
			}

			// 5. 绘制主文字
			dc.选择字体(取字体句柄());
			dc.置文本颜色(文本颜色);
			DrawTextW(hdc, 标题, -1, &rc, format);

			return 1;
		}
		case 32885: //消息反馈事件（仿易语言）
			return 事件_反馈(wParam, lParam);
		}
		return 控件类::挂接消息(hwnd, msg, wParam, lParam);
	}
};