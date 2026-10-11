#include "控件类.h"

定义_枚举型(列表框边框, char,
	无边框 = 0, 凹入式 = 1, 凸出式 = 2, 浅凹入式 = 3, 镜框式 = 4, 单线边框式 = 5
);

class EXP 列表框 : public 控件类
{
public:
	struct 参数 : 控件类::参数 {
		列表框边框 边框 = 列表框边框::凹入式;
		bool 自动排序 = false;
		bool 多列 = false;
		int 行间距 = 1;
		颜色 文本颜色 = 颜色::黑色;
		颜色 背景颜色 = 颜色::白色;
		字体 字体;
		bool 允许选择多项 = false;
		int 现行选中项 = -1;
		自定数据 列表项目;
		自定数据 项目数值;
	} static _df;

	bool 创建(const 参数& cs, 容器类* 父窗口 = NULL, HWND 父句柄 = NULL) {
		控件类::_子初始(cs, 父窗口, 父句柄);

		边框 = cs.边框;
		自动排序 = cs.自动排序;
		多列 = cs.多列;
		行间距 = cs.行间距;
		文本颜色 = cs.文本颜色;
		背景颜色 = cs.背景颜色;
		允许选择多项 = cs.允许选择多项;
		现行选中项 = cs.现行选中项;
		列表项目 = cs.列表项目;
		项目数值 = cs.项目数值;

		背景颜色_(cs.背景颜色, false);
		_重建控件(true);
		置字体(cs.字体);

		return 窗口句柄 != NULL;
	}

	void 行间距_(int v) {
		行间距 = v;
		_更新行间距();
	}

	void 边框_(列表框边框 v) {
		边框 = v;
		if (!窗口句柄) return;
		LONG_PTR style = GetWindowLongPtrW(窗口句柄, GWL_STYLE);
		if (边框 == 列表框边框::单线边框式) style |= WS_BORDER;
		else style &= ~WS_BORDER;
		SetWindowLongPtrW(窗口句柄, GWL_STYLE, style);

		LONG_PTR exstyle = GetWindowLongPtrW(窗口句柄, GWL_EXSTYLE);
		exstyle &= ~(WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE | WS_EX_STATICEDGE | WS_EX_DLGMODALFRAME);
		exstyle |= _取扩展风格();
		SetWindowLongPtrW(窗口句柄, GWL_EXSTYLE, exstyle);

		SetWindowPos(窗口句柄, NULL, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
		RedrawWindow(窗口句柄, NULL, NULL, RDW_INVALIDATE | RDW_FRAME | RDW_UPDATENOW);
	}

	void 自动排序_(bool v) {
		if (自动排序 == v) return;
		自动排序 = v;
		_重建控件(false);
	}

	void 多列_(bool v) {
		if (多列 == v) return;
		多列 = v;
		_重建控件(false);
	}

	void 允许选择多项_(bool v) {
		if (允许选择多项 == v) return;
		允许选择多项 = v;
		_重建控件(false);
	}

	void 现行选中项_(int index) {
		现行选中项 = index;
		if (!窗口句柄) return;
		if (允许选择多项) {
			if (index >= 0) {
				SendMessageW(窗口句柄, LB_SETSEL, TRUE, index);
				SendMessageW(窗口句柄, LB_SETCARETINDEX, index, FALSE);
			} else {
				SendMessageW(窗口句柄, LB_SETSEL, FALSE, -1);
			}
		} else {
			SendMessageW(窗口句柄, LB_SETCURSEL, index, 0);
		}
	}

	int _取现行选中项() {
		if (!窗口句柄) return 现行选中项;
		if (允许选择多项) {
			int caret = (int)SendMessageW(窗口句柄, LB_GETCARETINDEX, 0, 0);
			return (caret >= 0) ? caret : -1;
		}
		return (int)SendMessageW(窗口句柄, LB_GETCURSEL, 0, 0);
	}

	void 文本颜色_(颜色 cr) {
		文本颜色 = cr;
		重画();
	}

	void 背景颜色_(颜色 cr, bool 是否重画 = true) {
		背景颜色 = cr;
		_背景画刷 = CreateSolidBrush(背景颜色);
		if (是否重画) 重画(nil, true);
	}

	void 列表项目_(const Bytes& data) {
		列表项目 = data;
		_重新加载列表数据();
	}

	void 项目数值_(const Bytes& data) {
		项目数值 = data;
		if (!窗口句柄) return;
		const int* pValues = (const int*)项目数值.buf;
		int valCount = (项目数值.buf && 项目数值.size >= sizeof(int)) ? (int)(项目数值.size / sizeof(int)) : 0;
		int total = 取项目数();
		for (int i = 0; i < total; i++) {
			int val = (i < valCount) ? pValues[i] : 0;
			SendMessageW(窗口句柄, LB_SETITEMDATA, i, val);
		}
	}

	int 加入项目(const StrW& 文本, int 数值 = 0) {
		int index = (int)SendMessageW(窗口句柄, LB_ADDSTRING, 0, (LPARAM)(const wchar_t*)文本);
		if (index >= 0 && 数值 != 0) {
			SendMessageW(窗口句柄, LB_SETITEMDATA, index, 数值);
		}
		return index;
	}

	int 插入项目(int 索引, const StrW& 文本, int 数值 = 0) {
		int index = (int)SendMessageW(窗口句柄, LB_INSERTSTRING, 索引, (LPARAM)(const wchar_t*)文本);
		if (index >= 0 && 数值 != 0) {
			SendMessageW(窗口句柄, LB_SETITEMDATA, index, 数值);
		}
		return index;
	}

	void 删除项目(int 索引) {
		SendMessageW(窗口句柄, LB_DELETESTRING, 索引, 0);
	}

	void 清空() {
		SendMessageW(窗口句柄, LB_RESETCONTENT, 0, 0);
	}

	int 取项目数() {
		return (int)SendMessageW(窗口句柄, LB_GETCOUNT, 0, 0);
	}

	StrW 取项目文本(int 索引) {
		int len = (int)SendMessageW(窗口句柄, LB_GETTEXTLEN, 索引, 0);
		if (len <= 0) return L"";
		StrW res(len);
		SendMessageW(窗口句柄, LB_GETTEXT, 索引, (LPARAM)(charW*)res);
		return res;
	}

	void 置项目文本(int 索引, const StrW& 文本) {
		if (索引 < 0 || 索引 >= 取项目数()) return;
		int val = 取项目数值(索引);
		bool isSel = 是否被选中(索引);
		删除项目(索引);
		int newIdx = 插入项目(索引, 文本, val);
		if (isSel) {
			置选中(newIdx >= 0 ? newIdx : 索引, true);
		}
	}

	int 取项目数值(int 索引) {
		LRESULT res = SendMessageW(窗口句柄, LB_GETITEMDATA, 索引, 0);
		if (res == LB_ERR) return 0;
		return (int)res;
	}

	void 置项目数值(int 索引, int 数值) {
		SendMessageW(窗口句柄, LB_SETITEMDATA, 索引, 数值);
	}

	bool 是否被选中(int 索引) {
		return SendMessageW(窗口句柄, LB_GETSEL, 索引, 0) > 0;
	}

	void 置选中(int 索引, bool 是否选中 = true) {
		if (允许选择多项) {
			SendMessageW(窗口句柄, LB_SETSEL, 是否选中 ? TRUE : FALSE, 索引);
		} else {
			SendMessageW(窗口句柄, LB_SETCURSEL, 是否选中 ? 索引 : -1, 0);
		}
	}

	int 取被选择总数() {
		if (允许选择多项) {
			return (int)SendMessageW(窗口句柄, LB_GETSELCOUNT, 0, 0);
		} else {
			return (SendMessageW(窗口句柄, LB_GETCURSEL, 0, 0) >= 0) ? 1 : 0;
		}
	}

	void 取被选中项(Arraybe<int>& out选中项) {
		out选中项.clear();
		if (!窗口句柄) return;
		if (允许选择多项) {
			int count = (int)SendMessageW(窗口句柄, LB_GETSELCOUNT, 0, 0);
			if (count > 0) {
				int* buf = new int[count];
				SendMessageW(窗口句柄, LB_GETSELITEMS, count, (LPARAM)buf);
				for (int i = 0; i < count; i++) {
					out选中项.push(buf[i]);
				}
				delete[] buf;
			}
		} else {
			int cur = (int)SendMessageW(窗口句柄, LB_GETCURSEL, 0, 0);
			if (cur >= 0) {
				out选中项.push(cur);
			}
		}
	}

	int 查找项目(const StrW& 欲查找文本, int 起始索引 = -1, bool 精确匹配 = false) {
		UINT msg = 精确匹配 ? LB_FINDSTRINGEXACT : LB_FINDSTRING;
		LRESULT res = SendMessageW(窗口句柄, msg, 起始索引, (LPARAM)(const wchar_t*)欲查找文本);
		return (res == LB_ERR) ? -1 : (int)res;
	}

	int 取顶端项目索引() {
		return (int)SendMessageW(窗口句柄, LB_GETTOPINDEX, 0, 0);
	}

	void 置顶端项目索引(int 索引) {
		SendMessageW(窗口句柄, LB_SETTOPINDEX, 索引, 0);
	}

	void 置字体(const 字体& f) {
		窗口基类::置字体(f);
		if (窗口句柄 && 行间距 > 0) {
			_更新行间距();
		}
	}
	字体 取字体() { return 窗口基类::取字体(); }
	HFONT 取字体句柄() { return 窗口基类::取字体句柄(); }

public:
	virtual void 事件_列表项被选择() {}
	virtual void 事件_双击选择() {}

	virtual LRESULT 挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	列表框边框 边框;
	bool 自动排序;
	bool 多列;
	int 行间距;
	颜色 文本颜色;
	颜色 背景颜色;
	bool 允许选择多项;
	int 现行选中项;
	Bytes 列表项目;
	Bytes 项目数值;

private:
	void _更新行间距() {
		if (!窗口句柄) return;
		HFONT hFont = 取字体句柄();
		if (!hFont) hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
		int fontH = 字体::取字体高度<D>(hFont, false);
		SendMessageW(窗口句柄, LB_SETITEMHEIGHT, 0, fontH + 行间距);
		重画(nil, true);
	}

	DWORD _取扩展风格() const {
		switch (边框) {
		case 列表框边框::凹入式:
			return WS_EX_CLIENTEDGE;
		case 列表框边框::凸出式:
			return WS_EX_WINDOWEDGE | WS_EX_DLGMODALFRAME;
		case 列表框边框::浅凹入式:
			return WS_EX_STATICEDGE;
		case 列表框边框::镜框式:
			return WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE;
		}
		return 0;
	}

	DWORD _取列表框风格() const {
		DWORD style = LBS_NOTIFY | LBS_HASSTRINGS | LBS_NOINTEGRALHEIGHT;
		if (自动排序) style |= LBS_SORT;
		if (多列) style |= (LBS_MULTICOLUMN | WS_HSCROLL);
		else style |= WS_VSCROLL;
		if (允许选择多项) style |= (LBS_MULTIPLESEL | LBS_EXTENDEDSEL);
		if (边框 == 列表框边框::单线边框式) style |= WS_BORDER;
		return style;
	}

	void _重新加载列表数据() {
		if (!窗口句柄) return;
		SendMessageW(窗口句柄, WM_SETREDRAW, FALSE, 0);
		清空();

		const int* pValues = (const int*)项目数值.buf;
		int valCount = (项目数值.buf && 项目数值.size >= sizeof(int)) ? (int)(项目数值.size / sizeof(int)) : 0;

		// 列表项目是 UTF-16LE、以 charW(0) 分隔的一维字符串数组。
		if (列表项目.buf && 列表项目.size >= sizeof(charW)) {
			int 项索引 = 0;
			for (BR 项 : SplitV2(列表项目, BNULLW)) {
				StrW 标题((const charW*)项.p, 项.size / sizeof(charW));
				int val = (项索引 < valCount) ? pValues[项索引] : 0;
				加入项目(标题, val);
				项索引++;
			}
		}

		if (现行选中项 >= 0) {
			现行选中项_(现行选中项);
		}

		SendMessageW(窗口句柄, WM_SETREDRAW, TRUE, 0);
		InvalidateRect(窗口句柄, NULL, TRUE);
	}

	void _重建控件(bool 首次创建 = false) {
		HWND hParent = NULL;
		int px = 0, py = 0, pw = 0, ph = 0;
		字体 当前字体;

		struct 项信息 {
			StrW 文本;
			int 数值;
			bool 是否选中;
		};
		Arraybe<项信息> 待恢复项目;

		if (首次创建) {
			hParent = 父窗口句柄;
			px = (int)dpi(左边); py = (int)dpi(顶边);
			pw = (int)dpi(宽度); ph = (int)dpi(高度);
		} else {
			if (!窗口句柄) return;

			当前字体 = 取字体();
			hParent = GetParent(窗口句柄);
			if (!hParent) hParent = 父窗口句柄;

			RECT rc;
			GetWindowRect(窗口句柄, &rc);
			if (hParent) MapWindowPoints(NULL, hParent, (LPPOINT)&rc, 2);
			px = rc.left; py = rc.top;
			pw = rc.right - rc.left; ph = rc.bottom - rc.top;

			LONG_PTR oldStyle = GetWindowLongPtrW(窗口句柄, GWL_STYLE);
			bool oldIsMulti = (oldStyle & (LBS_MULTIPLESEL | LBS_EXTENDEDSEL)) != 0;
			int oldCurSel = oldIsMulti ? -1 : (int)SendMessageW(窗口句柄, LB_GETCURSEL, 0, 0);

			int oldCount = 取项目数();
			for (int i = 0; i < oldCount; i++) {
				bool sel = oldIsMulti ? (SendMessageW(窗口句柄, LB_GETSEL, i, 0) > 0) : (i == oldCurSel);
				待恢复项目.push({ 取项目文本(i), 取项目数值(i), sel });
			}

			SetWindowLongPtrW(窗口句柄, GWLP_WNDPROC, (LONG_PTR)_原窗口过程);
			SetWindowLongPtrW(窗口句柄, GWLP_USERDATA, 0);
			HWND hOld = 窗口句柄;
			窗口句柄 = NULL;
			_窗口风格 = 0;
			DestroyWindow(hOld);
		}

		DWORD style = WS_CHILDWINDOW;
		if (可视) style |= WS_VISIBLE;
		if (可停留焦点_) style |= WS_TABSTOP;
		if (禁止) style |= WS_DISABLED;
		style |= _取列表框风格();
		DWORD exStyle = _取扩展风格();

		窗口句柄 = CreateWindowExW(exStyle, L"ListBox", L"", style,
			px, py, pw, ph, hParent, (HMENU)this, g_hInst, NULL);
		if (!窗口句柄) return;

		_窗口风格 = WS_CHILDWINDOW;
		if (可视) _窗口风格 |= WS_VISIBLE;
		if (可停留焦点_) _窗口风格 |= WS_TABSTOP;

		if (!首次创建) {
			置字体(当前字体);
		}
		控件类::_子类化();

		if (首次创建) {
			_重新加载列表数据();
		} else {
			for (int i = 0; i < 待恢复项目.count; i++) {
				int newIdx = 加入项目(待恢复项目[i].文本, 待恢复项目[i].数值);
				if (待恢复项目[i].是否选中) {
					if (允许选择多项) {
						置选中(newIdx >= 0 ? newIdx : i, true);
					} else {
						现行选中项_(newIdx >= 0 ? newIdx : i);
					}
				}
			}
		}

		_更新行间距();
		if (!首次创建) {
			重画(nil, true);
		}
	}
};

LRESULT 列表框::挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case BE_REFLECT + WM_COMMAND:
	{
		WORD code = HIWORD(wParam);
		if (code == LBN_SELCHANGE) {
			现行选中项 = _取现行选中项();
			事件_列表项被选择();
		} else if (code == LBN_DBLCLK) {
			事件_双击选择();
		}
		break;
	}
	case BE_REFLECT + WM_CTLCOLORLISTBOX:
	{
		HDC hdc = (HDC)wParam;
		SetTextColor(hdc, 文本颜色);
		SetBkColor(hdc, 背景颜色);
		if (!_背景画刷) _背景画刷 = CreateSolidBrush(背景颜色);
		return (LRESULT)(HBRUSH)_背景画刷;
	}
	}
	return 窗口基类::挂接消息(hwnd, msg, wParam, lParam);
}
