#include "控件类.h"

/*WS_GROUP 风格：一组单选框中的第一个通常带有 WS_GROUP 风格，标志着一组的开始，
  仅限在同一个父窗口内，紧随其后的所有控件，只要它们没有 WS_GROUP 风格，
  它们就会被系统自动划归到单选框 A 所开启的这个组里，
  直到遇到下一个带有 WS_GROUP 风格的控件为止；
  如果单选框在不同的父窗口里则天然存在隔离。
  注：一个窗口中若第一个单选框即便不设置WS_GROUP，也视为往后的全部为同一组。
*/

class EXP 单选框 : public 控件类
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

	bool 创建(const 参数& cs, 容器类* 父窗口 = NULL, HWND 父句柄 = NULL) {
		控件类::_子初始(cs, 父窗口, 父句柄);
		文本颜色 = cs.文本颜色;
		标题居左 = cs.标题居左;
		横向对齐方式 = cs.横向对齐方式;
		纵向对齐方式 = cs.纵向对齐方式;
		选中 = cs.选中;

		_窗口风格 = _窗口风格 | BS_AUTORADIOBUTTON | BS_NOTIFY;
		if (标题居左) _窗口风格 |= BS_LEFTTEXT;

		if (横向对齐方式 == 横向对齐::居中) _窗口风格 |= BS_CENTER;
		else if (横向对齐方式 == 横向对齐::右对齐) _窗口风格 |= BS_RIGHT;
		else _窗口风格 |= BS_LEFT;

		if (纵向对齐方式 == 纵向对齐::居中) _窗口风格 |= BS_VCENTER;
		else if (纵向对齐方式 == 纵向对齐::底对齐) _窗口风格 |= BS_BOTTOM;
		else _窗口风格 |= BS_TOP;
		背景颜色_(cs.背景颜色, false);
		标题 = cs.标题;
		// 传入空标题 L""，防止系统原生过程绘制任何文字，避免重叠
		窗口句柄 = CreateWindowExW(0, L"Button", cs.标题, _窗口风格,
			dpi(左边), dpi(顶边), dpi(宽度), dpi(高度), 父窗口句柄,
			(HMENU)this, g_hInst, NULL);

		if (!窗口句柄) return false;
		if (选中) SendMessageW(窗口句柄, BM_SETCHECK, BST_CHECKED, 0);
		置字体(cs.字体);
		控件类::_子类化();
		return true;
	}

	void 文本颜色_(颜色 cr) {
		文本颜色 = cr;
		重画();
	}

	void 选中_(bool isChecked) {
		选中 = isChecked;
		SendMessageW(窗口句柄, BM_SETCHECK, isChecked ? BST_CHECKED : BST_UNCHECKED, 0);
	}

	bool _取选中() {
		return SendMessageW(窗口句柄, BM_GETCHECK, 0, 0) == BST_CHECKED;
	}

	void 标题居左_(bool isLeft) {
		标题居左 = isLeft;
		int style = GetWindowLongW(窗口句柄, GWL_STYLE);
		if (isLeft) style |= BS_LEFTTEXT; else style &= ~BS_LEFTTEXT;
		SetWindowLongW(窗口句柄, GWL_STYLE, style);
		重画();
	}

	void 横向对齐方式_(横向对齐 mode) {
		横向对齐方式 = mode;
		int style = GetWindowLongW(窗口句柄, GWL_STYLE);
		style &= ~BS_CENTER;
		if (mode == 横向对齐::居中) style |= BS_CENTER;
		else if (mode == 横向对齐::右对齐) style |= BS_RIGHT;
		else style |= BS_LEFT;
		SetWindowLongW(窗口句柄, GWL_STYLE, style);
		重画();
	}

	void 纵向对齐方式_(纵向对齐 mode) {
		纵向对齐方式 = mode;
		int style = GetWindowLongW(窗口句柄, GWL_STYLE);
		style &= ~BS_VCENTER;
		if (mode == 纵向对齐::居中) style |= BS_VCENTER;
		else if (mode == 纵向对齐::底对齐) style |= BS_BOTTOM;
		else style |= BS_TOP;
		SetWindowLongW(窗口句柄, GWL_STYLE, style);
		重画();
	}

public:
	virtual void 事件_被单击() {}
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
LRESULT 单选框::挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_PAINT: {
		SetPropW(GetParent(hwnd), L"BE_PAINT_CLIENT", hwnd);
		if (文本颜色 == 颜色::黑色) break;

		RECT rcClient;
		GetClientRect(hwnd, &rcClient);
		
		// 划分出文字所在的大区域 rRightSide（完全接管该区域，不让系统绘制任何东西）
		RECT rRightSide = rcClient;
		if (标题居左) {
			rRightSide.right -= dpi(14); // 扩张接管区域至 14，彻底吞噬掉系统的原生焦点框
		} else {
			rRightSide.left += dpi(14);
		}
		
		// 核心黑科技：强制系统裁剪掉该区域，彻底阻止系统画出任何黑字和错位的焦点框
		ValidateRect(hwnd, &rRightSide);

		// 1. 让系统先画，系统现在只会画出未被 Validate 的小圆圈（单选框按钮）
		LRESULT ret = CallWindowProcW((WNDPROC)_原窗口过程, hwnd, msg, wParam, lParam);

		// 2. 获取DC，由我们全面接管该区域的背景、文字、焦点框的绘制
		HandleDC hdc(hwnd);
		HFONT hFont = 取字体句柄();
		HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

		// 补画背景
		if (背景颜色 == 颜色::透明) {
			UI_请求父窗绘制背景(hwnd, hdc, rRightSide);
		} else {
			FillRect(hdc, &rRightSide, _背景画刷);
		}

		SIZE szText;
		GetTextExtentPoint32W(hdc, 标题, 标题.len(), &szText);

		RECT rFill = rRightSide;

		// 垂直居中计算 (保持整个客户端高度居中计算，以保证与控件整体对齐)
		int textHeight = szText.cy;
		int clientHeight = rcClient.bottom - rcClient.top;
		if (纵向对齐方式 == 纵向对齐::居中) {
			rFill.top = rcClient.top + (clientHeight - textHeight) / 2;
			rFill.bottom = rFill.top + textHeight;
		} else if (纵向对齐方式 == 纵向对齐::底对齐) {
			rFill.top = rcClient.bottom - textHeight;
			rFill.bottom = rcClient.bottom;
		} else {
			rFill.top = rcClient.top;
			rFill.bottom = rcClient.top + textHeight;
		}

		// 水平位置计算
		int textWidth = szText.cx;
		if (横向对齐方式 == 横向对齐::居中) {
			rFill.left = rRightSide.left + (rRightSide.right - rRightSide.left - textWidth) / 2;
		} else if (横向对齐方式 == 横向对齐::右对齐) {
			rFill.left = rRightSide.right - textWidth - dpi(2);
		} else {
			rFill.left = rRightSide.left + dpi(2); // 增加 2 像素边距，防止焦点框隐身于文字笔画中
		}
		rFill.right = rFill.left + textWidth;

		// 3. 手动补画焦点虚线框（此时 DC 仍为默认黑字白底状态，可以直接画出标准虚线）
		if ((GetFocus() == hwnd) && !(SendMessageW(hwnd, WM_QUERYUISTATE, 0, 0) & 1 /*UISF_HIDEFOCUS*/)) {
			RECT rcFocus = rFill;
			rcFocus.left -= dpi(1);
			rcFocus.top -= dpi(1);
			rcFocus.right += dpi(1);
			rcFocus.bottom += dpi(1);
			DrawFocusRect(hdc, &rcFocus);
		}

		// 4. 绘制我们自己的文字，带上指定的颜色
		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, 文本颜色);

		UINT format = DT_SINGLELINE;
		if (横向对齐方式 == 横向对齐::居中) format |= DT_CENTER;
		else if (横向对齐方式 == 横向对齐::右对齐) format |= DT_RIGHT;
		else format |= DT_LEFT;

		if (纵向对齐方式 == 纵向对齐::居中) format |= DT_VCENTER;
		else if (纵向对齐方式 == 纵向对齐::底对齐) format |= DT_BOTTOM;
		else format |= DT_TOP;

		DrawTextW(hdc, 标题, -1, &rFill, format);

		SelectObject(hdc, hOldFont);
		return ret;
	}
	case BE_REFLECT + WM_PRINTCLIENT:
		if (背景颜色 == 颜色::透明) return -1;
		break;
	case BE_REFLECT + WM_CTLCOLORSTATIC:
		return (LRESULT)_背景画刷;
	case BE_REFLECT + WM_COMMAND:
		if (HIWORD(wParam) == BN_CLICKED) {
			选中 = _取选中();
			事件_被单击();
		}
		break;
	}
	return 窗口基类::挂接消息(hwnd, msg, wParam, lParam);
}
