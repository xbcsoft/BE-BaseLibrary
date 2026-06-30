#include "控件类.h"

定义_枚举型(编辑框边框, char,
	无边框 = 0, 凹入式 = 1, 凸出式 = 2, 浅凹入式 = 3, 镜框式 = 4, 单线边框式 = 5
);

定义_枚举型(编辑框滚动条, char,
	无 = 0, 横向滚动条 = 1, 纵向滚动条 = 2, 横向及纵向滚动条 = 3
);

定义_枚举型(编辑框输入方式, char,
	通常方式 = 0, 只读方式 = 1, 密码输入 = 2, 整数文本输入 = 3, 小数文本输入 = 4,
	输入字节 = 5, 输入短整数 = 6, 输入整数 = 7, 输入长整数 = 8, 输入小数 = 9,
	输入双精度小数 = 10, 输入日期时间 = 11
);

定义_枚举型(编辑框转换方式, char,
	无 = 0, 大写到小写 = 1, 小写到大写 = 2
);

定义_枚举型(编辑框调节器方式, char,
	无调节器 = 0, 自动调节器 = 1, 手动调节器 = 2
);

static void _更新编辑框格式矩形(HWND hwnd) {
	RECT rcClient;
	GetClientRect(hwnd, &rcClient);
	int clientHeight = rcClient.bottom - rcClient.top;

	HFONT hFont = (HFONT)SendMessageW(hwnd, WM_GETFONT, 0, 0);
	if (!hFont) hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

	HDC hdc = GetDC(hwnd);
	HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
	TEXTMETRICW tm;
	GetTextMetricsW(hdc, &tm);
	SelectObject(hdc, hOldFont);
	ReleaseDC(hwnd, hdc);

	int textHeight = tm.tmHeight;
	if (clientHeight > textHeight) {
		int yOffset = (clientHeight - textHeight) / 2;
		RECT rcFormat = rcClient;
		rcFormat.top = yOffset;
		rcFormat.bottom = yOffset + textHeight;
		SendMessageW(hwnd, EM_SETRECT, 0, (LPARAM)&rcFormat);
	} else {
		SendMessageW(hwnd, EM_SETRECT, 0, (LPARAM)&rcClient);
	}
}

class EXP 编辑框 :public 控件类
{
public:
	struct 参数 : 控件类::参数 {
		定义_子组件通用构造方法;
		StrW 内容 = L"";
		编辑框边框 边框 = 编辑框边框::凹入式;
		字体 字体;
		颜色 文本颜色 = 颜色::黑色;
		颜色 背景颜色 = 颜色::白色;
		bool 隐藏选择 = true;
		int 最大允许长度 = 0;
		bool 是否允许多行 = false;
		编辑框滚动条 滚动条 = 编辑框滚动条::无;
		横向对齐 对齐方式 = 横向对齐::左对齐;
		编辑框输入方式 输入方式 = 编辑框输入方式::通常方式;
		StrW 密码遮盖字符 = L"*";
		编辑框转换方式 转换方式 = 编辑框转换方式::无;
		编辑框调节器方式 调节器方式 = 编辑框调节器方式::无调节器;
		int 调节器底限值 = -32767;
		int 调节器上限值 = 32767;
	}static _df;

	bool 创建(const 参数& cs, 容器类* 父窗口 = NULL, HWND 父句柄 = NULL) {
		控件类::_子初始(cs, 父窗口, 父句柄);

		边框 = cs.边框;
		文本颜色 = cs.文本颜色;
		背景颜色 = cs.背景颜色;
		隐藏选择 = cs.隐藏选择;
		最大允许长度 = cs.最大允许长度;
		是否允许多行 = cs.是否允许多行;
		滚动条 = cs.滚动条;
		对齐方式 = cs.对齐方式;
		输入方式 = cs.输入方式;
		密码遮盖字符 = cs.密码遮盖字符;
		转换方式 = cs.转换方式;
		调节器方式 = cs.调节器方式;
		调节器底限值 = cs.调节器底限值;
		调节器上限值 = cs.调节器上限值;
		内容 = cs.内容;

		DWORD style = _窗口风格 | _取编辑风格();
		if (边框 == 编辑框边框::单线边框式) {
			style |= WS_BORDER;
		}
		DWORD exStyle = _取扩展风格();

		窗口句柄 = CreateWindowExW(exStyle, L"Edit", cs.内容, style,
			dpi(左边), dpi(顶边), dpi(宽度), dpi(高度), 父窗口句柄,
			(HMENU)this, g_hInst, NULL);
		if (!窗口句柄) return false;

		背景颜色_(背景颜色);
		置字体(cs.字体);
		最大允许长度_(最大允许长度);
		密码遮盖字符_(密码遮盖字符);
		控件类::_子类化();
		if (!是否允许多行 && 输入方式 != 编辑框输入方式::密码输入) {
			_更新编辑框格式矩形(窗口句柄);
		}
		return true;
	}

	void 内容_(const StrW& txt) {
		内容 = txt;
		if (窗口句柄) 窗口_置标题(窗口句柄, txt);
	}
	StrW _取内容() {
		return 窗口_取标题(窗口句柄);
	}
	int 取内容长度() {
		if (窗口句柄) {
			return GetWindowTextLengthW(窗口句柄);
		}
		return (int)内容.len();
	}

	void 加入文本(const StrW& 欲加入文本) {
		int len = 取内容长度();
		SendMessageW(窗口句柄, EM_SETSEL, (WPARAM)len, (LPARAM)len);
		SendMessageW(窗口句柄, EM_REPLACESEL, FALSE, (LPARAM)(const wchar_t*)欲加入文本);
	}

	void 边框_(编辑框边框 v) {
		边框 = v;
		_同步样式();
	}
	void 文本颜色_(颜色 cr) {
		文本颜色 = cr;
		重画();
	}

	颜色 _取背景颜色(颜色** out = 0) {
		return 背景颜色;
	}

	//编辑框的背景颜色较为特殊，需要采用背景画刷来实现
	void 背景颜色_(颜色 cr, bool 是否重画 = true) {
		背景颜色 = cr;
		if (背景颜色 == 颜色::透明) {
			_背景画刷 = UI_请求父窗创建背景刷(窗口句柄);
		} else {
			_背景画刷 = CreateSolidBrush(背景颜色);
		}
		if (是否重画)重画(nil, true);
	}

	void 隐藏选择_(bool is) {
		隐藏选择 = is;
		_同步样式();
	}
	void 最大允许长度_(int len) {
		最大允许长度 = len;
		SendMessageW(窗口句柄, EM_LIMITTEXT, (WPARAM)(len < 0 ? 0 : len), 0);
	}
	void 是否允许多行_(bool is) {
		if (是否允许多行 == is) return;
		是否允许多行 = is;
		_重建控件();
	}
	void 滚动条_(编辑框滚动条 v) {
		if (滚动条 == v) return;
		滚动条 = v;
		_重建控件();
	}
	void 对齐方式_(横向对齐 v) {
		对齐方式 = v;
		_同步样式();
	}
	void 输入方式_(编辑框输入方式 v) {
		输入方式 = v;
		_同步样式();
		密码遮盖字符_(密码遮盖字符);
	}
	void 密码遮盖字符_(const StrW& v) {
		密码遮盖字符 = v;
		if (!窗口句柄) return;
		wchar_t ch = 0;
		if (输入方式 == 编辑框输入方式::密码输入) {
			const wchar_t* p = v;
			if (p && p[0]) ch = p[0];
			else ch = L'*';
		}
		SendMessageW(窗口句柄, EM_SETPASSWORDCHAR, (WPARAM)ch, 0);
		重画();
	}
	void 转换方式_(编辑框转换方式 v) {
		转换方式 = v;
		_同步样式();
	}
	void 调节器方式_(编辑框调节器方式 v) {
		调节器方式 = v;
	}
	void 调节器底限值_(int v) {
		调节器底限值 = v;
	}
	void 调节器上限值_(int v) {
		调节器上限值 = v;
	}

	void 置起始选择位置(int pos) {
		if (pos < 0) {
			SendMessageW(窗口句柄, EM_SETSEL, (WPARAM)-1, (LPARAM)-1);
			return;
		}
		DWORD s = 0, e = 0;
		SendMessageW(窗口句柄, EM_GETSEL, (WPARAM)&s, (LPARAM)&e);
		int len = (int)e - (int)s;
		if (len < 0) len = 0;
		SendMessageW(窗口句柄, EM_SETSEL, (WPARAM)pos, (LPARAM)(pos + len));
	}
	int 取起始选择位置() {
		DWORD s = 0, e = 0;
		SendMessageW(窗口句柄, EM_GETSEL, (WPARAM)&s, (LPARAM)&e);
		return (int)s;
	}

	/**置被选择字符数
	 * @param len 若给-1相当于全选，0相当于取消选择
	 * @param i<可空> 如果为空则从当前光标处开始
	 */
	void 置被选择字符数(int len, 可空<int> i = 空) {
		DWORD s = 0, e = 0;
		if (i == 空) {
			SendMessageW(窗口句柄, EM_GETSEL, (WPARAM)&s, (LPARAM)&e);
		} else {
			s = (DWORD)(int)i;
		}
		SendMessageW(窗口句柄, EM_SETSEL, (WPARAM)s, (LPARAM)(s + len));
		获取焦点();
	}
	int 取被选择字符数() {
		DWORD s = 0, e = 0;
		SendMessageW(窗口句柄, EM_GETSEL, (WPARAM)&s, (LPARAM)&e);
		return (int)e - (int)s;
	}

	/**替换当前被选中文本
	 * @param txt
	 */
	void 置被选择文本(const StrW& txt) {
		SendMessageW(窗口句柄, EM_REPLACESEL, TRUE, (LPARAM)(charW*)txt);
	}
	/**获取当前被选中的文本内容
	 * @return 选中的文本，若无选择则返回空
	 */
	StrW 取被选择文本() {
		DWORD s = 0, e = 0;
		SendMessageW(窗口句柄, EM_GETSEL, (WPARAM)&s, (LPARAM)&e);
		if (s == e) return L"";
		return 取文本子串<W>(_不即时写内容属性_ ? _取内容() : 内容, s, e - s);
	}

public:
	virtual void 事件_内容被改变() {}
	virtual void 事件_调节钮被按下(int 按钮值) {}
	virtual LRESULT 挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	定义_字体成员;
	StrW 内容;
	bool _不即时写内容属性_ = false;
	编辑框边框 边框;
	颜色 文本颜色;
	颜色 背景颜色;
	bool 隐藏选择;
	int 最大允许长度;
	bool 是否允许多行;
	编辑框滚动条 滚动条;
	横向对齐 对齐方式;
	编辑框输入方式 输入方式;
	StrW 密码遮盖字符;
	编辑框转换方式 转换方式;
	编辑框调节器方式 调节器方式;
	int 调节器底限值;
	int 调节器上限值;

protected:

	DWORD _取编辑风格() {
		DWORD style = 0;
		if (是否允许多行) {
			style |= ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN;
			if (滚动条 == 编辑框滚动条::横向滚动条 || 滚动条 == 编辑框滚动条::横向及纵向滚动条) {
				style |= WS_HSCROLL | ES_AUTOHSCROLL;
			}
			if (滚动条 == 编辑框滚动条::纵向滚动条 || 滚动条 == 编辑框滚动条::横向及纵向滚动条) {
				style |= WS_VSCROLL;
			}
		} else {
			style |= ES_AUTOHSCROLL;
			if (输入方式 != 编辑框输入方式::密码输入) {
				style |= ES_MULTILINE;
			}
		}

		if (对齐方式 == 横向对齐::居中) style |= ES_CENTER;
		else if (对齐方式 == 横向对齐::右对齐) style |= ES_RIGHT;
		else style |= ES_LEFT;

		if (!隐藏选择) style |= ES_NOHIDESEL;

		switch (输入方式) {
		case 编辑框输入方式::只读方式:
			style |= ES_READONLY;
			break;
		case 编辑框输入方式::密码输入:
			style |= ES_PASSWORD;
			break;
		case 编辑框输入方式::整数文本输入:
		case 编辑框输入方式::输入字节:
		case 编辑框输入方式::输入短整数:
		case 编辑框输入方式::输入整数:
		case 编辑框输入方式::输入长整数:
			style |= ES_NUMBER;
			break;
		}

		if (转换方式 == 编辑框转换方式::大写到小写) style |= ES_LOWERCASE;
		else if (转换方式 == 编辑框转换方式::小写到大写) style |= ES_UPPERCASE;
		return style;
	}

	DWORD _取扩展风格() {
		switch (边框) {
		case 编辑框边框::凹入式:
			return WS_EX_CLIENTEDGE;
		case 编辑框边框::凸出式:
			return WS_EX_WINDOWEDGE | WS_EX_DLGMODALFRAME;
		case 编辑框边框::浅凹入式:
			return WS_EX_STATICEDGE;
		case 编辑框边框::镜框式:
			return WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE;
		}
		return 0;
	}

	void _重建控件() {
		if (!窗口句柄) return;

		// 1. 保存重建前的所有运行时状态
		内容 = 窗口_取标题(窗口句柄);
		字体 当前字体 = 取字体();

		// 取当前在父窗口客户区的实际像素位置和尺寸（原始像素，不经 dpi 转换）
		RECT rc;
		GetWindowRect(窗口句柄, &rc);
		HWND hParent = GetParent(窗口句柄);
		if (hParent) MapWindowPoints(NULL, hParent, (LPPOINT)&rc, 2);
		int px = rc.left, py = rc.top;
		int pw = rc.right - rc.left, ph = rc.bottom - rc.top;

		// 2. 取消子类化，恢复原始窗口过程，避免 DestroyWindow 时走我们的 WM_DESTROY 逻辑
		SetWindowLongPtrW(窗口句柄, GWLP_WNDPROC, (LONG_PTR)_原窗口过程);
		SetWindowLongPtrW(窗口句柄, GWLP_USERDATA, 0);

		// 3. 销毁旧窗口
		//    由于已恢复原始过程，WM_DESTROY 不再回调我们，_窗口风格 不会被清零，
		//    但 窗口基类::销毁() 不会被触发，所以手动清理句柄即可。
		HWND hOld = 窗口句柄;
		窗口句柄 = NULL;           // 先置空，防止析构二次销毁
		_窗口风格 = 0;             // 同上
		DestroyWindow(hOld);

		// 4. 用新样式重建
		DWORD style = WS_CHILDWINDOW;
		if (可视) style |= WS_VISIBLE;
		if (可停留焦点_) style |= WS_TABSTOP;
		style |= _取编辑风格();
		if (边框 == 编辑框边框::单线边框式) style |= WS_BORDER;
		DWORD exStyle = _取扩展风格();

		窗口句柄 = CreateWindowExW(exStyle, L"Edit", 内容, style,
			px, py, pw, ph, hParent, (HMENU)this, g_hInst, NULL);
		if (!窗口句柄) return;

		// 恢复 _窗口风格 成员（子初始时已设，但重建后需重新设）
		_窗口风格 = WS_CHILDWINDOW;
		if (可视) _窗口风格 |= WS_VISIBLE;
		if (可停留焦点_) _窗口风格 |= WS_TABSTOP;

		// 5. 恢复运行时属性
		背景颜色_(背景颜色, false);
		置字体(当前字体);
		最大允许长度_(最大允许长度);
		密码遮盖字符_(密码遮盖字符);

		// 6. 重新子类化
		控件类::_子类化();
		if (!是否允许多行 && 输入方式 != 编辑框输入方式::密码输入) {
			_更新编辑框格式矩形(窗口句柄);
		}

		重画(nil, true);
	}

	void _同步样式() {
		if (!窗口句柄) return;

		DWORD editStyle = _取编辑风格();
		LONG_PTR style = GetWindowLongPtrW(窗口句柄, GWL_STYLE);
		if ((style & ES_MULTILINE) != (editStyle & ES_MULTILINE)) {
			_重建控件();
			return;
		}

		// 锁定重画以防止多次样式修改造成的严重闪烁
		SendMessageW(窗口句柄, WM_SETREDRAW, FALSE, 0);

		style &= ~(WS_BORDER | WS_HSCROLL | WS_VSCROLL |
			ES_LEFT | ES_CENTER | ES_RIGHT | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL |
			ES_WANTRETURN | ES_NOHIDESEL | ES_PASSWORD | ES_NUMBER |
			ES_LOWERCASE | ES_UPPERCASE);
		style |= (editStyle & ~ES_READONLY);
		// 只有单线边框式才显式携带 WS_BORDER
		if (边框 == 编辑框边框::单线边框式) style |= WS_BORDER;

		SetWindowLongPtrW(窗口句柄, GWL_STYLE, style);
		SendMessageW(窗口句柄, EM_SETREADONLY, (editStyle & ES_READONLY) ? TRUE : FALSE, 0);

		LONG_PTR exstyle = GetWindowLongPtrW(窗口句柄, GWL_EXSTYLE);
		exstyle &= ~(WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE | WS_EX_STATICEDGE | WS_EX_DLGMODALFRAME);
		exstyle |= _取扩展风格();
		SetWindowLongPtrW(窗口句柄, GWL_EXSTYLE, exstyle);

		SetWindowPos(窗口句柄, NULL, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

		// 恢复重画，并执行一次干净的强制重绘
		SendMessageW(窗口句柄, WM_SETREDRAW, TRUE, 0);
		RedrawWindow(窗口句柄, NULL, NULL, RDW_INVALIDATE | RDW_FRAME | RDW_UPDATENOW | RDW_ALLCHILDREN);
	}

	bool _校验输入字符(WPARAM wParam) {
		if ((GetKeyState(VK_CONTROL) & 0x8000) != 0) return true;
		if (wParam < 0x20) return true;

		StrW currentText = _不即时写内容属性_ ? _取内容() : 内容;

		if (输入方式 == 编辑框输入方式::小数文本输入 ||
			输入方式 == 编辑框输入方式::输入小数 ||
			输入方式 == 编辑框输入方式::输入双精度小数) {
			wchar_t ch = (wchar_t)wParam;
			if (ch >= L'0' && ch <= L'9') return true;
			if (ch == L'.') {
				const wchar_t* p = currentText;
				return (!p || wcschr(p, L'.') == NULL);
			}
			if (ch == L'-') {
				DWORD s = 0, e = 0;
				SendMessageW(窗口句柄, EM_GETSEL, (WPARAM)&s, (LPARAM)&e);
				const wchar_t* p = currentText;
				return s == 0 && (!p || wcschr(p, L'-') == NULL);
			}
			return false;
		}

		if (输入方式 == 编辑框输入方式::整数文本输入 ||
			输入方式 == 编辑框输入方式::输入字节 ||
			输入方式 == 编辑框输入方式::输入短整数 ||
			输入方式 == 编辑框输入方式::输入整数 ||
			输入方式 == 编辑框输入方式::输入长整数) {
			wchar_t ch = (wchar_t)wParam;
			if (ch >= L'0' && ch <= L'9') return true;
			if (ch == L'-') {
				DWORD s = 0, e = 0;
				SendMessageW(窗口句柄, EM_GETSEL, (WPARAM)&s, (LPARAM)&e);
				const wchar_t* p = currentText;
				return s == 0 && (!p || wcschr(p, L'-') == NULL);
			}
			return false;
		}

		if (输入方式 == 编辑框输入方式::输入日期时间) {
			wchar_t ch = (wchar_t)wParam;
			if ((ch >= L'0' && ch <= L'9') || ch == L'-' || ch == L'/' || ch == L':' || ch == L' ' || ch == L'.') {
				return true;
			}
			return false;
		}

		return true;
	}
};

LRESULT 编辑框::挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_SIZE:
	{
		LRESULT res = 窗口基类::挂接消息(hwnd, msg, wParam, lParam);
		if (!是否允许多行 && 输入方式 != 编辑框输入方式::密码输入) {
			_更新编辑框格式矩形(hwnd);
		}
		return res;
	}
	case WM_SETFONT:
	{
		LRESULT res = 窗口基类::挂接消息(hwnd, msg, wParam, lParam);
		if (!是否允许多行 && 输入方式 != 编辑框输入方式::密码输入) {
			_更新编辑框格式矩形(hwnd);
		}
		return res;
	}
	case WM_CHAR:
		if (!是否允许多行 && wParam == VK_RETURN) {
			return 0;
		}
		if (!_校验输入字符(wParam)) return 0;
		break;
	case BE_REFLECT + WM_COMMAND:
	{
		WORD code = HIWORD(wParam);
		if (背景颜色 == 颜色::透明 &&
			(code == EN_VSCROLL || code == EN_HSCROLL || code == EN_UPDATE)) {
			//EN_UPDATE 在文本改变且即将重画时触发
			//EN_V/HSCROLL 在文字内容输入不下内部滚动时触发
			重画(nil, true);
		}
		if (code == EN_CHANGE) {
			StrW txt = 窗口_取标题(窗口句柄);
			if (!是否允许多行) {
				if (寻找文本<W>(txt, L"\r") != -1 || 寻找文本<W>(txt, L"\n") != -1) {
					StrW cleanContent = 子文本替换<W>(txt, L"\r", L"");
					cleanContent = 子文本替换<W>(cleanContent, L"\n", L"");
					
					bool oldVal = _不即时写内容属性_;
					_不即时写内容属性_ = true;
					if (oldVal) {
						窗口_置标题(窗口句柄, cleanContent);
					} else {
						内容_(cleanContent);
					}
					_不即时写内容属性_ = oldVal;
					txt = cleanContent;
				}
			}
			if (!_不即时写内容属性_) {
				内容 = txt;
			}
			事件_内容被改变();
		}
		break;
	}
	case WM_HSCROLL:
	case WM_VSCROLL:
	{   //先让系统处理滚动（移动文本和滚动条偏移）
		LRESULT res = CallWindowProcW(_原窗口过程, hwnd, msg, wParam, lParam);
		if (背景颜色 == 颜色::透明) {
			重画(nil, true);
		}
		return res;
	}

	case BE_REFLECT + WM_CTLCOLOREDIT:
	case BE_REFLECT + WM_CTLCOLORSTATIC:
	{
		if (msg == BE_REFLECT + WM_CTLCOLORSTATIC && !IsWindowEnabled(窗口句柄)) {
			HDC hdc = (HDC)wParam;
			SetTextColor(hdc, GetSysColor(COLOR_GRAYTEXT));
			SetBkColor(hdc, GetSysColor(COLOR_3DFACE));
			return (LRESULT)GetSysColorBrush(COLOR_3DFACE);
		}
		HDC hdc = (HDC)wParam;
		SetTextColor(hdc, 文本颜色);
		if (背景颜色 == 颜色::透明) {
			SetBkMode(hdc, TRANSPARENT);
			POINT pt = { 0, 0 };
			MapWindowPoints(窗口句柄, UI_寻找非透明背景祖先窗口(窗口句柄), &pt, 1);
			SetBrushOrgEx(hdc, -pt.x, -pt.y, NULL);
		} else {
			SetBkColor(hdc, 背景颜色);
		}
		return (LRESULT)(HBRUSH)_背景画刷;
	}
	}
	return 窗口基类::挂接消息(hwnd, msg, wParam, lParam);
}