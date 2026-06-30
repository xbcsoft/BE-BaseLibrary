#include "控件类.h"

定义_枚举型(组合框类型, char,
	可编辑列表式 = 0,
	可编辑下拉式 = 1,
	不可编辑下拉式 = 2
);

class EXP 组合框 : public 控件类
{
public:
	struct 参数 : 控件类::参数 {
		定义_子组件通用构造方法;
		组合框类型 类型 = 组合框类型::不可编辑下拉式;
		StrW 内容 = L"";
		int 最大文本长度 = 0;
		bool 自动排序 = false;
		int 行间距 = 1;
		颜色 文本颜色 = 颜色::黑色;
		颜色 背景颜色 = 颜色::白色;
		字体 字体;
		int 现行选中项 = -1;
	} static _df;

	bool 创建(const 参数& cs, 容器类* 父窗口 = NULL, HWND 父句柄 = NULL) {
		控件类::_子初始(cs, 父窗口, 父句柄);
		
		类型 = cs.类型;
		内容 = cs.内容;
		最大文本长度 = cs.最大文本长度;
		自动排序 = cs.自动排序;
		行间距 = cs.行间距;
		文本颜色 = cs.文本颜色;
		现行选中项 = cs.现行选中项;

		_窗口风格 |= _取组合框风格() | WS_VSCROLL;
		
		背景颜色_(cs.背景颜色, false);

		窗口句柄 = CreateWindowExW(WS_EX_CLIENTEDGE, L"ComboBox", L"", _窗口风格,
			dpi(左边), dpi(顶边), dpi(宽度), dpi(高度), 父窗口句柄,
			(HMENU)this, g_hInst, NULL);

		if (!窗口句柄) return false;

		置字体(cs.字体);
		
		if (最大文本长度 > 0) {
			SendMessageW(窗口句柄, CB_LIMITTEXT, 最大文本长度, 0);
		}
		
		if (行间距 > 0) {
			SendMessageW(窗口句柄, CB_SETITEMHEIGHT, 0, cs.字体.取字体高度<D>(false)+行间距);
		}

		if (现行选中项 >= 0) {
			现行选中项_(现行选中项);
		}
		if (类型 != 组合框类型::不可编辑下拉式 && 内容!=L"") {
			内容_(内容);
		}

		控件类::_子类化();
		return true;
	}

	void 行间距_() {
		SendMessageW(窗口句柄, CB_SETITEMHEIGHT, 0,
			字体::取字体高度<D>(取字体句柄(), false)+行间距);
	}

	void 内容_(const StrW& txt) {
		内容 = txt; 窗口_置标题(窗口句柄, txt);
	}

	StrW _取内容() {
		return 窗口_取标题(窗口句柄);
	}

	void 现行选中项_(int index) {
		现行选中项 = index; SendMessageW(窗口句柄, CB_SETCURSEL, index, 0);
	}

	int _取现行选中项() {
		return (int)SendMessageW(窗口句柄, CB_GETCURSEL, 0, 0);
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

	int 取项目数() {
		return (int)SendMessageW(窗口句柄, CB_GETCOUNT, 0, 0);
	}

	void 加入项目(const StrW& 文本, int 数值 = 0) {
		int index = (int)SendMessageW(窗口句柄, CB_ADDSTRING, 0, (LPARAM)(charW*)文本);
		if (index >= 0 && 数值 != 0) {
			SendMessageW(窗口句柄, CB_SETITEMDATA, index, 数值);
		}
	}

	void 插入项目(int 索引, const StrW& 文本, int 数值 = 0) {
		int index = (int)SendMessageW(窗口句柄, CB_INSERTSTRING, 索引, (LPARAM)(charW*)文本);
		if (index >= 0 && 数值 != 0) {
			SendMessageW(窗口句柄, CB_SETITEMDATA, index, 数值);
		}
	}

	void 删除项目(int 索引) {
		SendMessageW(窗口句柄, CB_DELETESTRING, 索引, 0);
	}

	void 清空() {
		SendMessageW(窗口句柄, CB_RESETCONTENT, 0, 0);
	}

	StrW 取项目文本(int 索引) {
		int len = (int)SendMessageW(窗口句柄, CB_GETLBTEXTLEN, 索引, 0);
		if (len <= 0) return L"";
		StrW res(len);
		SendMessageW(窗口句柄, CB_GETLBTEXT, 索引, (LPARAM)(charW*)res);
		return res;
	}

	void 置项目文本(int 索引, const StrW& 文本) {
		int num = 取项目数值(索引);
		删除项目(索引);
		插入项目(索引, 文本, num);
	}

	int 取项目数值(int 索引) {
		LRESULT res = SendMessageW(窗口句柄, CB_GETITEMDATA, 索引, 0);
		if (res == CB_ERR) return 0;
		return (int)res;
	}

	void 置项目数值(int 索引, int 数值) {
		SendMessageW(窗口句柄, CB_SETITEMDATA, 索引, 数值);
	}

public:
	virtual void 事件_列表项被选择() {}
	virtual void 事件_编辑内容被改变() {}
	virtual void 事件_将弹出列表() {}
	virtual void 事件_列表被关闭() {}
	virtual void 事件_双击选择() {}

	virtual LRESULT 挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	组合框类型 类型;
	StrW 内容;
	int 最大文本长度;
	bool 自动排序;
	int 行间距;
	颜色 文本颜色;
	颜色 背景颜色;
	定义_字体成员;
	int 现行选中项;

protected:
	DWORD _取组合框风格() const {
		DWORD style = 0;
		switch (类型) {
		case 组合框类型::可编辑列表式: style |= CBS_SIMPLE; break;
		case 组合框类型::不可编辑下拉式: style |= CBS_DROPDOWNLIST; break;
		case 组合框类型::可编辑下拉式: 
		default: style |= CBS_DROPDOWN; break;
		}
		if (自动排序) style |= CBS_SORT;
		// 如果需要自绘等，可以加入 CBS_HASSTRINGS | CBS_OWNERDRAWFIXED
		return style;
	}
};

LRESULT 组合框::挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case BE_REFLECT + WM_COMMAND:
	{
		WORD code = HIWORD(wParam);
		if (code == CBN_SELCHANGE) {
			现行选中项 = _取现行选中项();
			内容 = _取内容();
			事件_列表项被选择();
		} else if (code == CBN_EDITCHANGE) {
			内容 = _取内容();
			事件_编辑内容被改变();
		} else if (code == CBN_DROPDOWN) {
			事件_将弹出列表();
		} else if (code == CBN_CLOSEUP) {
			事件_列表被关闭();
		} else if (code == CBN_DBLCLK) {
			事件_双击选择();
		}
		break;
	}
	case BE_REFLECT + WM_CTLCOLOREDIT:
	case BE_REFLECT + WM_CTLCOLORLISTBOX:
	case BE_REFLECT + WM_CTLCOLORSTATIC:
	{
		HDC hdc = (HDC)wParam;
		SetTextColor(hdc, 文本颜色);
		SetBkColor(hdc, 背景颜色);
		return (LRESULT)(HBRUSH)_背景画刷;
	}
	}
	return 窗口基类::挂接消息(hwnd, msg, wParam, lParam);
}
