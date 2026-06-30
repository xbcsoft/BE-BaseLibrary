#include "容器类.h"

class EXP 分组框 : public 容器类
{
public:
	struct 参数 : 容器类::参数 {
		定义_子组件通用构造方法;
		StrW 标题;
		字体 字体;
		颜色 文本颜色 = 颜色::黑色;
		颜色 背景颜色 = 颜色::默认底色;
	} static _df;

	bool 创建(const 参数& cs, 容器类* 父窗口 = NULL, HWND 父句柄 = NULL) {
		容器类::_子初始(cs, 父窗口, 父句柄);
		文本颜色_ = cs.文本颜色;
		标题 = cs.标题;
		背景颜色_(cs.背景颜色, false);

		_窗口风格 = _窗口风格 | BS_GROUPBOX | WS_CLIPCHILDREN;

		//WS_EX_CONTROLPARENT（支持让该容器内的子控件穿透tab焦点导航）

		//原生 BS_GROUPBOX 标题始终保持空字符串：
		//空标题下系统会绘制连续的 etched 边框，标题文字由我们在 WM_PAINT 中自绘叠加，
		//这样既避开了系统标题无法改文本颜色/字体的限制，也避免了用透明子标签
		//在 WS_CLIPCHILDREN 下因尺寸变化导致父边框残留像素的问题。
		窗口句柄 = CreateWindowExW(WS_EX_CONTROLPARENT, L"Button", L"", _窗口风格,
			dpi(左边), dpi(顶边), dpi(宽度), dpi(高度), 父窗口句柄,
			(HMENU)this, g_hInst, NULL);

		if (!窗口句柄) return false;

		容器类::_子类化();
		置字体(cs.字体);

		return true;
	}

	void 文本颜色(颜色 cr) {
		文本颜色_ = cr; 重画();
	}

public:
	StrW 标题;
	void 标题_(const StrW& title) { 标题 = title; 重画(); }
	定义_字体成员;
	颜色 文本颜色_;
	定义_背景颜色成员;

	bool 通用事件_背景绘制(HDC hdc)
	{
		RECT rc; GetClientRect(窗口句柄, &rc);
		if (背景颜色 != 颜色::透明) {
			FillRect(hdc, &rc, _背景画刷);
		}
		return false;
	}

	virtual LRESULT 挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		if (msg == WM_PAINT) {
			// 1. 让 BS_GROUPBOX 先画连续的 etched 边框
			//    （客户区底色已由前置 WM_ERASEBKGND → 通用事件_背景绘制 铺好）
			LRESULT ret = CallWindowProcW(_原窗口过程, hwnd, msg, wParam, lParam);

			// 2. 在标题区抹平边框 + 写我们自己的文字
			HandleDC hdc(hwnd);
			StrW 绘制文本 = 标题;
			SIZE sz = 文本测量(取字体句柄()).测量<D>(绘制文本);
			if (sz.cx <= 0) return ret; // 空标题：保留系统画的连续 etched 边框
			RECT rcTitle = { dpi(9), 0, dpi(9) + sz.cx, sz.cy };

			IntersectClipRect(hdc, rcTitle.left, rcTitle.top, rcTitle.right, rcTitle.bottom);

			// 抹平标题区的 etched 边框：不透明用自身背景色，透明用父窗口背景
			if (背景颜色 != 颜色::透明) {
				FillRect(hdc, &rcTitle, _背景画刷);
			} else {
				RECT rc = { 0, 0, (int)dpi(宽度), (int)dpi(高度) };
				UI_请求父窗绘制背景(窗口句柄, hdc, rc); //这里是必须的，因为我们务必要覆盖原生控件已经画过的内容
			}

			// 写标题文字
			绘制上下文 dc(hdc);
			dc.选择字体(取字体句柄());
			dc.置文本颜色(文本颜色_);
			DrawTextW(hdc, 绘制文本, -1, &rcTitle, DT_LEFT | DT_TOP | DT_SINGLELINE);
			return ret;
		}
		return 容器类::挂接消息(hwnd, msg, wParam, lParam);
	}
};