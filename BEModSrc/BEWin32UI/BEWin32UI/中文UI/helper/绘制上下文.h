#pragma once
#include "字体.h"
#include "位图.h"

struct 画笔 {
	HPEN h;
	bool _接管生命周期;

	画笔();
	画笔(HPEN h, bool 是否接管生命周期 = false);
	画笔(HandlePEN&& hp);
	~画笔();

	void 挂接(HPEN h, bool 是否接管生命周期 = false);
	void 挂接(HandlePEN&& hp);
	void 销毁();
	operator HPEN();

	static HandlePEN 创建(COLORREF 颜色, int 宽度 = 1, int 样式 = PS_SOLID);
};

struct 画刷 {
	HBRUSH h;
	bool _接管生命周期;

	画刷();
	画刷(HBRUSH h, bool 是否接管生命周期 = false);
	画刷(HandleBRUSH&& hb);
	~画刷();

	void 挂接(HBRUSH h, bool 是否接管生命周期 = false);
	void 挂接(HandleBRUSH&& hb);
	void 销毁();
	operator HBRUSH();

	static HandleBRUSH 创建纯色(COLORREF 颜色);
	static HandleBRUSH 创建阴影(int 阴影样式, COLORREF 颜色);
	static HandleBRUSH 创建纹理(HBITMAP 位图句柄);
	static HBRUSH 获取透明画刷();
};

/**绘制上下文 注意对select进的GDI对象句柄其生命周期由外部负责
 */
struct 绘制上下文 {
	HDC hdc = NULL;
	PAINTSTRUCT* ps = NULL;
	HWND hwnd = NULL;
	HGDIOBJ old_font = NULL;
	HGDIOBJ old_pen = NULL;
	HGDIOBJ old_brush = NULL;
	HGDIOBJ old_bmp = NULL;
	HandleFONT _font;

	绘制上下文() = default;
	绘制上下文(HDC hdc);

	/**使用HWND构造时自动开始BeginPaint，析构时自动EndPaint
	 * @param ps
	 */
	绘制上下文(HWND hwnd, PAINTSTRUCT& ps);

	/**结束绘制 出栈时也会自动调用此函数
	 */
	void 结束绘制();

	绘制上下文& 选择字体(HFONT font);
	绘制上下文& 选择字体(const 字体& font);
	绘制上下文& 选择位图(HBITMAP bmp);

	绘制上下文& 选择画刷(HBRUSH brush);

	绘制上下文& 选择画笔(HPEN pen);

	/**置背景模式，影响绘制文本轮廓的空白
	 * @param mode OPAQUE：用当前DC的背景色填充虚线画笔、阴影刷子以及字符的空隙；
			  TRANSPARENT：透明处理，即不作颜色填充（窗口背景原先什么色那就什么色）
	 * @return
	 */
	绘制上下文& 置背景模式(int mode);

	绘制上下文& 背景颜色(DWORD color);

	绘制上下文& 置文本颜色(DWORD color);

	~绘制上下文();

	绘制上下文(绘制上下文&& other) noexcept;

	绘制上下文& operator=(绘制上下文&& other) noexcept;

	void 绘制文本(const StrW& s, float x, float y);

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
	void 绘制文本(const StrW& s, const RECTF& rect, UINT format = DT_LEFT | DT_TOP | DT_SINGLELINE);

	void 绘制位图(HBITMAP 位图句柄, float x, float y, 可空<float> w = nil, 可空<float> h = nil, float srcX = 0, float srcY = 0);

};
