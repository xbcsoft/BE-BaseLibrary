#include "字体.h"
#include "位图.h"

struct EXP 画笔 {
	HPEN h;
	bool _接管生命周期;

	画笔() { h = NULL; _接管生命周期 = false; }
	画笔(HPEN h, bool 是否接管生命周期 = false) {
		挂接(h, 是否接管生命周期);
	}
	画笔(HandlePEN&& hp) {
		挂接((HandlePEN&&)hp);
	}
	~画笔() {
		销毁();
	}

	void 挂接(HPEN h, bool 是否接管生命周期 = false) {
		this->h = h;
		this->_接管生命周期 = 是否接管生命周期;
	}
	void 挂接(HandlePEN&& hp) {
		this->h = hp._move();
		this->_接管生命周期 = true;
	}
	void 销毁() {
		if (_接管生命周期 && h) {
			DeleteObject(h);
			h = NULL;
		}
	}
	operator HPEN() {
		return h;
	}

	static HandlePEN 创建(COLORREF 颜色, int 宽度 = 1, int 样式 = PS_SOLID) {
		return CreatePen(样式, 宽度, 颜色);
	}
};

struct EXP 画刷 {
	HBRUSH h;
	bool _接管生命周期;

	画刷() { h = NULL; _接管生命周期 = false; }
	画刷(HBRUSH h, bool 是否接管生命周期 = false) {
		挂接(h, 是否接管生命周期);
	}
	画刷(HandleBRUSH&& hb) {
		挂接((HandleBRUSH&&)hb);
	}
	~画刷() {
		销毁();
	}

	void 挂接(HBRUSH h, bool 是否接管生命周期 = false) {
		this->h = h;
		this->_接管生命周期 = 是否接管生命周期;
	}
	void 挂接(HandleBRUSH&& hb) {
		this->h = hb._move();
		this->_接管生命周期 = true;
	}
	void 销毁() {
		if (_接管生命周期 && h) {
			DeleteObject(h);
			h = NULL;
		}
	}
	operator HBRUSH() {
		return h;
	}

	static HandleBRUSH 创建纯色(COLORREF 颜色) {
		return CreateSolidBrush(颜色);
	}
	static HandleBRUSH 创建阴影(int 阴影样式, COLORREF 颜色) {
		return CreateHatchBrush(阴影样式, 颜色);
	}
	static HandleBRUSH 创建纹理(HBITMAP 位图句柄) {
		return CreatePatternBrush(位图句柄);
	}
	static HBRUSH 获取透明画刷() {
		static HBRUSH nullbrush;
		if (!nullbrush)nullbrush = (HBRUSH)GetStockObject(NULL_BRUSH);
		return nullbrush;
	}
};

/**绘制上下文 注意对select进的GDI对象句柄其生命周期由外部负责
 */
struct EXP 绘制上下文 {
	HDC hdc = NULL;
	PAINTSTRUCT* ps = NULL;
	HWND hwnd = NULL;
	HGDIOBJ old_font = NULL;
	HGDIOBJ old_pen = NULL;
	HGDIOBJ old_brush = NULL;
	HGDIOBJ old_bmp = NULL;
	HandleFONT _font;

	绘制上下文() = default;
	绘制上下文(HDC hdc) : hdc(hdc) {
		置背景模式(TRANSPARENT);
	}

	/**使用HWND构造时自动开始BeginPaint，析构时自动EndPaint
	 * @param ps
	 */
	绘制上下文(HWND hwnd, PAINTSTRUCT& ps) :hwnd(hwnd), ps(&ps) {
		hdc = BeginPaint(hwnd, &ps);
		置背景模式(TRANSPARENT);
	}

	/**结束绘制 出栈时也会自动调用此函数
	 */
	void 结束绘制() {
		if (hdc) {
			if (old_font) { SelectObject(hdc, old_font); old_font = NULL; }
			if (old_pen) { SelectObject(hdc, old_pen); old_pen = NULL; }
			if (old_brush) { SelectObject(hdc, old_brush); old_brush = NULL; }
			if (old_bmp) { SelectObject(hdc, old_bmp); old_bmp = NULL; }
			hdc = NULL;
		}
		if (hwnd) {
			EndPaint(hwnd, ps);
			hwnd = NULL;
		}
	}

	绘制上下文& 选择字体(HFONT font) {
		if (hdc && font) {
			HGDIOBJ old = SelectObject(hdc, font);
			if (!old_font) old_font = old;
		}
		return *this;
	}
	绘制上下文& 选择字体(const 字体& font) {
		_font = font.到字体句柄(); 选择字体(_font);
		return *this;
	}
	绘制上下文& 选择位图(HBITMAP bmp) {
		if (hdc && bmp) {
			HGDIOBJ old = SelectObject(hdc, bmp);
			if (!old_bmp) old_bmp = old;
		}
		return *this;
	}

	绘制上下文& 选择画刷(HBRUSH brush) {
		if (hdc && brush) {
			HGDIOBJ old = SelectObject(hdc, brush);
			if (!old_brush) old_brush = old;
		}
		return *this;
	}

	绘制上下文& 选择画笔(HPEN pen) {
		if (hdc && pen) {
			HGDIOBJ old = SelectObject(hdc, pen);
			if (!old_pen) old_pen = old;
		}
		return *this;
	}

	/**置背景模式，影响绘制文本轮廓的空白
	 * @param mode OPAQUE：用当前DC的背景色填充虚线画笔、阴影刷子以及字符的空隙；
			  TRANSPARENT：透明处理，即不作颜色填充（窗口背景原先什么色那就什么色）
	 * @return
	 */
	绘制上下文& 置背景模式(int mode) {
		SetBkMode(hdc, mode);
		return *this;
	}

	绘制上下文& 背景颜色(DWORD color) {
		SetBkColor(hdc, color);
		return *this;
	}

	绘制上下文& 置文本颜色(DWORD color) {
		SetTextColor(hdc, color);
		return *this;
	}

	~绘制上下文() {
		结束绘制();
	}

	绘制上下文(绘制上下文&& other) noexcept : hdc(other.hdc), ps(other.ps),
		old_font(other.old_font), old_pen(other.old_pen), old_brush(other.old_brush), old_bmp(other.old_bmp) {
		other.hdc = NULL;
		other.ps = NULL;
		other.old_font = NULL;
		other.old_pen = NULL;
		other.old_brush = NULL;
		other.old_bmp = NULL;
	}

	绘制上下文& operator=(绘制上下文&& other) noexcept {
		if (this != &other) {
			结束绘制();
			hdc = other.hdc;
			ps = other.ps;
			old_font = other.old_font;
			old_pen = other.old_pen;
			old_brush = other.old_brush;
			old_bmp = other.old_bmp;
			连续赋值(NULL, other.hdc, other.ps, other.old_font,
				other.old_pen, other.old_brush, other.old_bmp);
		}
		return *this;
	}

	void 绘制文本(const StrW& s, float x, float y) {
		TextOutW(hdc, (int)dpi(x), (int)dpi(y), s, s.len());
	}

	/**绘制文本（矩形区域）
	 * @param s 待绘制的文本
	 * @param rect 目标矩形区域（虚拟单位，内部会自动转换 DPI）
	 * @param format=DT_LEFT|DT_TOP|DT_SINGLELINE 绘制格式标志（可选以下位组合）：
	 *  - DT_LEFT / DT_CENTER / DT_RIGHT: 水平 左 / 中 / 右 对齐
	 *  - DT_TOP / DT_VCENTER / DT_BOTTOM: 垂直 顶 / 中 / 底 对齐（垂直居中/底部对齐通常需配合 DT_SINGLELINE）
	 *  - DT_SINGLELINE: 单行显示（忽略换行符）
	 *  - DT_WORDBREAK: 自动换行（若文本超出宽度则在单词间换行）
	 *  - DT_NOCLIP: 不裁剪超出区域的部分
	 *  - DT_NOPREFIX: 禁止处理 & 符号（不显示下划线）
	 *  - DT_END_ELLIPSIS: 文本超出时末尾显示省略号
	 */
	void 绘制文本(const StrW& s, const RECTF& rect, UINT format = DT_LEFT | DT_TOP | DT_SINGLELINE) {
		DrawTextW(hdc, s, s.len(), &dpi(rect), format);
	}

	void 绘制位图(HBITMAP 位图句柄, float x, float y, 可空<float> w = nil, 可空<float> h = nil, float srcX = 0, float srcY = 0) {
		if (!位图句柄) return;
		SIZE sz = 位图::取尺寸(位图句柄);
		int destW = (w == 空) ? sz.cx : (int)dpi(w);
		int destH = (h == 空) ? sz.cy : (int)dpi(h);
		HDC hdcMem = CreateCompatibleDC(hdc);
		HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, 位图句柄);
		if (destW == sz.cx && destH == sz.cy && srcX == 0 && srcY == 0) {
			BitBlt(hdc, (int)dpi(x), (int)dpi(y), destW, destH, hdcMem, 0, 0, SRCCOPY);
		} else {
			// 设置高质量缩放模式，消除缩放边角脏点和锯齿
			SetStretchBltMode(hdc, HALFTONE);
			SetBrushOrgEx(hdc, 0, 0, NULL);
			StretchBlt(hdc, (int)dpi(x), (int)dpi(y), destW, destH, hdcMem, (int)srcX, (int)srcY, (int)(sz.cx - srcX), (int)(sz.cy - srcY), SRCCOPY);
		}
		SelectObject(hdcMem, hbmOld);
		DeleteDC(hdcMem);
	}

};