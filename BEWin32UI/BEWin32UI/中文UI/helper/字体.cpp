#include "句柄管理.h"
#include "窗口辅助核心.h"

struct EXP 字体
{
	StrW 字体名称;
	int 字体大小 = 9; //pt为单位
	bool 下划线 = false;
	bool 删除线 = false;
	bool 倾斜 = false;
	bool 加粗 = false;
	int 角度 = 0;

	字体() { 字体名称 = (const charW*)获取默认逻辑字体().lfFaceName; }
	字体(可空<c_StrW> 字体名称, int 字体大小 = 9, bool 下划线 = false, bool 删除线 = false,
		bool 倾斜 = false, bool 加粗 = false, int 角度 = 0)
		: 字体大小(字体大小), 下划线(下划线), 删除线(删除线),
		倾斜(倾斜), 加粗(加粗), 角度(角度)
	{
		if (字体名称==空)this->字体名称 = (const charW*)获取默认逻辑字体().lfFaceName;
		else { this->字体名称 = 字体名称; }
	}

	int _字体像素; //-px为单位，内部使用
	//注：该px是字体实际字形对应的原生设备像素（若计算整体行距的情况不要使用此值，还需加上内部行距）

	static LOGFONT 获取默认逻辑字体() {
		static LOGFONT lf; if (lf.lfHeight==0)
			GetObject((HFONT)GetStockObject(DEFAULT_GUI_FONT), sizeof(lf), &lf);
		return lf;
	}

	/**到字体句柄
	 * @return 务必使用HandleFONT去接
	 */
	HandleFONT 到字体句柄() const {
		return CreateFontIndirectW(&到LOGFONT());
	}

	void 从LOGFONT创建(LOGFONT& lf) {
		字体名称 = lf.lfFaceName;
		_字体像素 = lf.lfHeight;
		字体大小 = -MulDiv(lf.lfHeight, 72, GetDeviceCaps(_hdc, LOGPIXELSY));
		角度 = lf.lfEscapement;
		加粗 = lf.lfWeight>400;
		倾斜 = lf.lfItalic!=0;
		下划线 = lf.lfUnderline!=0;
		删除线 = lf.lfStrikeOut!=0;
	}
	LOGFONT 到LOGFONT() const
	{
		LOGFONT lf = { 0 };
		Bytes facename(lf.lfFaceName, BRef);
		facename.copyFrom(SR(字体名称));

		lf.lfItalic = 倾斜;
		lf.lfUnderline = 下划线;
		lf.lfStrikeOut = 删除线;
		lf.lfEscapement = 角度;
		lf.lfWeight = 加粗 ? FW_BOLD : FW_NORMAL;
		lf.lfHeight = -MulDiv(字体大小, GetDeviceCaps(_hdc, LOGPIXELSY), 72);
		return lf;
	}

	/**取字体高度（虚拟单位）
	 * @param 是否为净高=false 为假则为包括字体内行距在内的高度，为真则净高度（字体大小的像素版）
	 * @return
	 */
	int 取字体高度(bool 是否为净高 = true) const {
		return rdpi(_取字体高度(是否为净高));
	}

	template <class D>
	int 取字体高度(bool 是否为净高 = true) const {
		return _取字体高度(是否为净高);
	}

	int _取字体高度(bool 是否为净高 = true) const {
		if (是否为净高) {
			return MulDiv(字体大小, GetDeviceCaps(_hdc, LOGPIXELSY), 72);
		} else {
			return _取字体高度(到字体句柄(), false);
		}
	}

	static int 取字体高度(HFONT hFont, bool 是否为净高 = true) {
		return rdpi(_取字体高度(hFont, 是否为净高));
	}

	template <class D>
	static int 取字体高度(HFONT hFont, bool 是否为净高 = true) {
		return _取字体高度(hFont, 是否为净高);
	}

	static int _取字体高度(HFONT hFont, bool 是否为净高 = true) {
		if (是否为净高) {
			LOGFONT lf = { 0 };
			if (GetObjectW(hFont, sizeof(LOGFONT), &lf)) {
				return abs(lf.lfHeight);
			}
			return 0;
		} else {
			HDC hdcTemp = GetDC(NULL);
			HGDIOBJ hOldFont = SelectObject(hdcTemp, hFont);
			TEXTMETRIC tm;
			GetTextMetricsW(hdcTemp, &tm);
			SelectObject(hdcTemp, hOldFont);
			ReleaseDC(NULL, hdcTemp);
			return tm.tmHeight;
		}
	}
	static HDC _hdc;
}; HDC 字体::_hdc;

HFONT 窗口_取字体句柄(HWND hwnd) {
	return (HFONT)SendMessageW(hwnd, WM_GETFONT, 0, 0);
}
字体 窗口_取字体(HWND hwnd) {
	HFONT hfont = (HFONT)SendMessageW(hwnd, WM_GETFONT, 0, 0);
	LOGFONT lf = { 0 };
	if (!hfont || !GetObject(hfont, sizeof(lf), &lf)) {
		lf = 字体::获取默认逻辑字体();
	}
	字体 f; f.从LOGFONT创建(lf);
	return f;
}

struct EXP 文本测量 {
	HandleCDC hdc;
	HFONT hfont; HFONT hOld;
	文本测量(HFONT hfont) :hdc(字体::_hdc), hfont(hfont) {
		hOld = (HFONT)SelectObject(hdc, hfont);
	}
	文本测量(const 字体& font) :hdc(字体::_hdc) {
		hOld = (HFONT)SelectObject(hdc, hfont);
	}
	SIZEF 测量(const StrW& text) {
		return rdpi(_测量(text));
	}

	template <class D>
	SIZE 测量(const StrW& text) {
		return _测量(text);
	}

	SIZE _测量(const StrW& text) {
		SIZE sz = { 0, 0 };
		GetTextExtentPoint32W(hdc, text, text.len(), &sz);
		return sz;
	}
	~文本测量() {
		SelectObject(hdc, hOld);
	}
};