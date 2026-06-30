#include <BECore/中文核心.hpp>
#include "helper/窗口辅助核心.h"
#include "helper/句柄管理.h"
#include "helper/字体.h"
#include "helper/位图.h"
#include "helper/绘制上下文.h"

class EXP 窗口基类
{
public:
	HWND 窗口句柄 = NULL;
	HWND 父窗口句柄 = NULL;
	class 容器类* 父窗口 = NULL;

	~窗口基类() {
		销毁();
	}

	virtual bool 销毁() {
		if (窗口句柄) {
			if (_窗口风格 != 0)
				DestroyWindow(窗口句柄); //小心内部还会再触发WM_DESTROY，
			//所以如果已是WM_DESTROY则 _窗口风格 那边已经置为0
			窗口句柄 = NULL;
			return true;
		}
		return false;
	}

	/**取整个窗口相对于屏幕的矩形区域（虚拟单位）
	 * @return RECTF
	 */
	RECTF 取窗口矩形() {
		return rdpi(_取窗口矩形(), true);
	}

	template <class D>
	RECT 取窗口矩形() {
		return _取窗口矩形();
	}

	RECT _取窗口矩形() {
		RECT r; GetWindowRect(窗口句柄, &r);
		return r;
	}

	struct RectS { float x, y, w, h; };

	/**取整个窗口相对于屏幕的矩形区域（虚拟单位）
	* @return
	*/
	RectS 取窗口大小版矩形() {
		RectS r = _取窗口大小版矩形();
		return { rdpi(r.x), rdpi(r.y), rdpi(r.w), rdpi(r.h) };
	}

	template <class D>
	RectS 取窗口大小版矩形() {
		return _取窗口大小版矩形();
	}

	RectS _取窗口大小版矩形() {
		RECT r; GetWindowRect(窗口句柄, &r);
		return { (float)r.left, (float)r.top, (float)(r.right - r.left), (float)(r.bottom - r.top) };
	}

	/**取用户区大小（虚拟单位）
	 * @return SIZEF
	 */
	SIZEF 取用户区大小() {
		return rdpi(_取用户区大小());
	}

	template <class D>
	SIZE 取用户区大小() {
		return _取用户区大小();
	}

	SIZE _取用户区大小() {
		RECT r; GetClientRect(窗口句柄, &r);
		return { r.right, r.bottom };
	}

	/**取所在父窗口内的矩形（虚拟单位）
	 * @return RECTF
	 */
	RECTF 取父窗用户区内矩形() {
		return rdpi(_取父窗用户区内矩形(), true);
	}

	template <class D>
	RECT 取父窗用户区内矩形() {
		return _取父窗用户区内矩形();
	}

	RECT _取父窗用户区内矩形() {
		RECT r; GetWindowRect(窗口句柄, &r);
		MapWindowPoints(NULL, GetParent(窗口句柄), (LPPOINT)&r, 2);
		return r;
	}

	/**移动窗口（注：这些参数值均是虚拟单位，实际值则受dpi影响）
	 * @param x<可空>
	 * @param y<可空>
	 * @param w<可空>
	 * @param h<可空>
	 * @param 重画窗口 重画窗口
	 */
	void 移动(可空<float> x = 空, 可空<float> y = 空, 可空<float> w = 空, 可空<float> h = 空, bool 重画窗口 = true) {
		可空<float> _x, _y, _w, _h;
		if (x != 空) _x = dpi(x);
		if (y != 空) _y = dpi(y);
		if (w != 空) _w = dpi(w);
		if (h != 空) _h = dpi(h);
		_移动(_x, _y, _w, _h, 重画窗口);
	}

	template <class D>
	void 移动(可空<float> x = 空, 可空<float> y = 空, 可空<float> w = 空, 可空<float> h = 空, bool 重画窗口 = true) {
		_移动(x, y, w, h, 重画窗口);
	}

	void _移动(可空<float> x = 空, 可空<float> y = 空, 可空<float> w = 空, 可空<float> h = 空, bool 重画窗口 = true) {
		RECT r; int x_, y_, w_, h_;
		GetWindowRect(窗口句柄, &r);

		int origW = r.right - r.left;
		int origH = r.bottom - r.top;

		HWND hParent = GetParent(窗口句柄);
		if (hParent) MapWindowPoints(NULL, hParent, (LPPOINT)&r, 2);

		x_ = (x == 空 ? r.left : (int)x);
		y_ = (y == 空 ? r.top : (int)y);
		w_ = (w == 空 ? origW : (int)w);
		h_ = (h == 空 ? origH : (int)h);

		MoveWindow(窗口句柄, x_, y_, w_, h_, 重画窗口);
		左边 = rdpi((float)x_); 顶边 = rdpi((float)y_);
		宽度 = rdpi((float)w_); 高度 = rdpi((float)h_);
	}


	void 激活() {
		SetActiveWindow(窗口句柄);
	}

	void 获取焦点() {
		SetFocus(窗口句柄);
	}

	bool 可有焦点() {
		return GetFocus() == 窗口句柄;
	}

	void 窗口置父(HWND 父句柄) {
		父窗口句柄 = 父句柄;
		SetParent(窗口句柄, 父窗口句柄);
	}

	//在容器类那边实现
	void 窗口置父(容器类* 父容器, bool 加入父容器列表 = false);

	void 可视_(bool is = true) {
		可视 = is;
		ShowWindow(窗口句柄, is);
	}

	void 禁止_(bool is = true) {
		禁止 = is;
		EnableWindow(窗口句柄, !is);
	}

	/**制造窗口无效区，待下次消息循环后触发WM_PAINT中重绘（如需立即重绘需再自行调UpdateWindow）
	 * @param 客户区域<可空>
	 * @param 背景擦除=false
	 */
	void 重画(可空<RECTF> 客户区域 = 空, bool 背景擦除 = false) {
		可空<RECT> _客户区域;
		if (客户区域!=nil)_客户区域 = dpi(客户区域.val);
		_重画(_客户区域, 背景擦除);
	}

	template <class D>
	void 重画(可空<RECT> 客户区域 = 空, bool 背景擦除 = false) {
		return _重画(客户区域, 背景擦除);
	}

	void _重画(可空<RECT> 客户区域 = 空, bool 背景擦除 = false) {
		if (窗口句柄 == NULL)return;
		if (客户区域 == 空) {
			InvalidateRect(窗口句柄, NULL, 背景擦除);
		} else {
			InvalidateRect(窗口句柄, &客户区域, 背景擦除);
		}
	}

	/**强制刷新当前窗口及其所有可见的子窗口，让其背景和区域完全重画
	 */
	void 刷新显示() {
		背景颜色_(_取背景颜色(), true);
	}

	void 禁止重画(bool is) {
		发送消息(WM_SETREDRAW, is, 0);
	}

	/**改变窗口或窗口组件的现行所处层次
	 * @param 层次类型 参考-窗口层次::
	 */
	void 调整层次(char 层次类型 = 1) {
		HWND 层次 = (层次类型 == 窗口层次::顶层) ? HWND_TOP :
			(层次类型 == 窗口层次::底层) ? HWND_BOTTOM :
			(层次类型 == 窗口层次::最高层) ? HWND_TOPMOST : HWND_TOP;
		SetWindowPos(窗口句柄, 层次, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}

	BOOL 投递消息(UINT msg, WPARAM wParam, LPARAM lParam = 0) {
		return PostMessageW(窗口句柄, msg, wParam, lParam);
	}
	LRESULT 发送消息(UINT msg, WPARAM wParam, LPARAM lParam = 0) {
		return SendMessageW(窗口句柄, msg, wParam, lParam);
	}

	/**当且仅当标记未被置为字符串时可使用置整数
	 * @param a
	 * @return
	 */
	bool 标记_置整数(int a) {
		if (标记_._buf())return false;
		标记_.bytes.size = a;
		return true;
	}

	/**当且仅当标记未被置为字符串时可使用取整数
	 * @return
	 */
	int 标记_取整数() {
		if (标记_._buf())return 0;
		return 标记_.bytes.size;
	}

	void 弹出菜单(class 子菜单& menu, 可空<float> x = 空, 可空<float> y = 空);

	virtual bool 通用事件_鼠标左键被按下(int x, int y) {
		return true;
	}
	virtual bool 通用事件_鼠标左键被放开(int x, int y) {
		return true;
	}
	virtual bool 通用事件_鼠标左键双击(int x, int y) {
		return true;
	}
	virtual bool 通用事件_鼠标右键被按下(int x, int y) {
		return true;
	}
	virtual bool 通用事件_鼠标右键被放开(int x, int y) {
		return true;
	}
	virtual bool 通用事件_鼠标位置被移动(int x, int y) {
		return true;
	}

	/**若要同时获取控制键的状态可调 控制键_是否被按下()
	 * @param key
	 */
	virtual bool 通用事件_按下某键(BYTE key) {
		return true;
	}
	/**若要同时获取控制键的状态可调 控制键_是否被按下()
	 * @param key
	 */
	virtual bool 通用事件_放开某键(BYTE key) {
		return true;
	}

	virtual LRESULT 挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


public:
	/**安全获取临时窗口DC，出栈后自动释放DC句柄
	 * @return HandleDC
	 * */
	HandleDC _获取DC() {
		return HandleDC(窗口句柄);
	}

protected:
	WNDPROC _原窗口过程;
	DWORD _窗口风格;
	void _子入父列表(bool 静态组件不入父 = true);

	// 控件子类化的窗口过程
	static LRESULT CALLBACK _WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
		窗口基类* pThis = (窗口基类*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		return pThis->挂接消息(hwnd, msg, wp, lp);
	}

	void _子类化() {
		窗口基类* pThis = (窗口基类*)GetWindowLongPtrW(窗口句柄, GWLP_USERDATA);
		if (pThis!=this) {
			SetWindowLongPtrW(窗口句柄, GWLP_USERDATA, (LONG_PTR)this);
			_原窗口过程 = (WNDPROC)SetWindowLongPtrW(窗口句柄, GWLP_WNDPROC, (LONG_PTR)_WndProc);
		}
	}

	HandleBRUSH _背景画刷;

	friend HWND UI_寻找非透明背景祖先窗口(HWND 窗口句柄, 容器类** obj = 0);
	virtual 颜色 _取背景颜色(颜色** out = 0) { return 0; }
	//之所以这个设置成虚函数目的是有统一在外部枚举子窗口的接口一个个调用
	//但默认情况下该背景颜色不被公开而是看在最终子类上实现或继承或不实现
	virtual void 背景颜色_(颜色 cr, bool 是否重画 = true) {
		颜色* that = 0; _取背景颜色(&that);
		bool isDif = true;
		if (that) {
			if (*that != cr.val) {
				*that = cr.val;
			} else { isDif = false; }
		}

		if (cr.val == 颜色::透明) {
			if (是否重画) {
				RECT rc = { 0, 0, (int)dpi(宽度), (int)dpi(高度) };
				void UI_请求父窗绘制背景(HWND 窗口句柄, HDC hdc, const RECT& rc);
				UI_请求父窗绘制背景(窗口句柄, _获取DC(), rc);
			}
		} else {
			if (isDif)_背景画刷 = CreateSolidBrush(cr.val);
		}
		if (是否重画) {
			InvalidateRect(窗口句柄, NULL, true);
			RECT rcParent = SIZEToRECT(取用户区大小<D>());
			for (窗口基类* h : 窗口_枚举子窗口对象(窗口句柄)) {
				if (h->_取背景颜色()==颜色::透明 && IsWindowVisible(h->窗口句柄)) {
					RECT rcChild = h->取父窗用户区内矩形<D>(), rcIntersect;
					if (IntersectRect(&rcIntersect, &rcParent, &rcChild)) {
						h->背景颜色_(h->_取背景颜色(), true);
					}
				}
			}
		}
	}

	//字体根据继承类的需要再进行public转向调用该protected
	HandleFONT _字体句柄;
	void 置字体(const 字体& f) {
		LOGFONT edv = f.到LOGFONT();
		HFONT hfont = CreateFontIndirectW(&edv);
		if (!hfont) {
			_字体句柄.destroy();
			SendMessageW(窗口句柄, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), 1);
			return;
		}
		SendMessageW(窗口句柄, WM_SETFONT, (WPARAM)hfont, 1);
		_字体句柄 = hfont; //赋值时智能指针自带将旧句柄析构
		重画();
	}
	字体 取字体() {
		return 窗口_取字体(窗口句柄);
	}
	HFONT 取字体句柄() {
		HFONT hFont = 窗口_取字体句柄(窗口句柄);
		if (!hFont) hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
		return hFont;
	}

	virtual bool 通用事件_窗口重画(PAINTSTRUCT& ps) { return true; }
	virtual bool 通用事件_背景绘制(HDC hdc) { return true; }

public:
	float 左边, 顶边, 宽度, 高度;
	bool 可视, 禁止; StrW 标记_;
	bool _是否为窗口类对象 = false;
};

LRESULT 窗口基类::挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_WINDOWPOSCHANGED: {
		WINDOWPOS* lpwpos = (WINDOWPOS*)lParam;
		if (!(lpwpos->flags & SWP_NOMOVE)) {
			左边 = rdpi((float)lpwpos->x);
			顶边 = rdpi((float)lpwpos->y);
		}
		if (!(lpwpos->flags & SWP_NOSIZE)) {
			宽度 = rdpi((float)lpwpos->cx);
			高度 = rdpi((float)lpwpos->cy);
		}
		break;
	}
	case WM_DESTROY:
		_窗口风格 = 0;
		销毁();
		return 0;
	case WM_LBUTTONDOWN:
		if (!通用事件_鼠标左键被按下(LOWORD(lParam), HIWORD(lParam))) {
			ReleaseCapture();
			return 0;
		}
		break;
	case WM_LBUTTONUP:
		if (!通用事件_鼠标左键被放开(LOWORD(lParam), HIWORD(lParam))) return 0;
		break;
	case WM_LBUTTONDBLCLK:
		if (!通用事件_鼠标左键双击(LOWORD(lParam), HIWORD(lParam))) return 0;
		break;
	case WM_RBUTTONDOWN:
		if (!通用事件_鼠标右键被按下(LOWORD(lParam), HIWORD(lParam))) return 0;
		break;
	case WM_RBUTTONUP:
		if (!通用事件_鼠标右键被放开(LOWORD(lParam), HIWORD(lParam))) return 0;
		break;
	case WM_MOUSEMOVE:
		if (!通用事件_鼠标位置被移动(LOWORD(lParam), HIWORD(lParam))) return 0;
		break;
	case WM_KEYDOWN:
		if (!通用事件_按下某键(wParam)) return 0;
	case WM_KEYUP:
		if (!通用事件_放开某键(wParam)) return 0;
	case WM_PAINT: {
		PAINTSTRUCT ps;
		if (!通用事件_窗口重画(ps))return 0;
		break;
	}
	case WM_ERASEBKGND: {
		if (!通用事件_背景绘制((HDC)wParam))return 0;
		break;
	}
	}

	//return DefWindowProc(hwnd, msg, wp, lp);
	//在正式实例化创建时给出（如: 窗口.创建()令m_原窗口过程=DefWindowProc）
	//好处是可被允许动态子类化
	return CallWindowProc(_原窗口过程, hwnd, msg, wParam, lParam);
}


HWND UI_寻找非透明背景祖先窗口(HWND 窗口句柄, 容器类** obj) {
	HWND hTarget = GetParent(窗口句柄);
	while (hTarget) {
		窗口基类* _obj = 窗口_句柄取对象(hTarget);
		if (_obj && _obj->_取背景颜色() != 颜色::透明) {
			if (obj)*obj = (容器类*)_obj;
			break;
		}
		HWND hNext = GetParent(hTarget);
		if (!hNext) break; // 已是顶层
		hTarget = hNext;
	}
	return hTarget;
}

HBRUSH UI_请求父窗创建背景刷(HWND 窗口句柄) {
	HWND hTarget = UI_寻找非透明背景祖先窗口(窗口句柄);
	if (!hTarget) return NULL;

	RECT rcTarget;
	GetClientRect(hTarget, &rcTarget);
	int w = rcTarget.right;
	int h = rcTarget.bottom;
	if (w <= 0 || h <= 0) return NULL;

	HandleDC hdcRef(窗口句柄);
	HDC hdcMem = CreateCompatibleDC(hdcRef);
	HBITMAP hBmp = CreateCompatibleBitmap(hdcRef, w, h);
	HBITMAP hOldBmp = (HBITMAP)SelectObject(hdcMem, hBmp);

	// 让目标祖先把背景画到内存DC上
	SendMessageW(hTarget, WM_ERASEBKGND, (WPARAM)hdcMem, PRF_CLIENT);
	HBRUSH hBr = CreatePatternBrush(hBmp);

	SelectObject(hdcMem, hOldBmp);
	DeleteObject(hBmp);
	DeleteDC(hdcMem);

	return hBr;
}

void UI_请求父窗绘制背景(HWND 窗口句柄, HDC hdc, const RECT& rc) {
	HWND hTarget = UI_寻找非透明背景祖先窗口(窗口句柄);
	if (!hTarget) return;

	int nSavedDC = SaveDC(hdc);
	POINT pt = { 0, 0 };
	MapWindowPoints(窗口句柄, hTarget, &pt, 1);
	SetWindowOrgEx(hdc, pt.x, pt.y, NULL);
	IntersectClipRect(hdc, pt.x + rc.left, pt.y + rc.top, pt.x + rc.right, pt.y + rc.bottom);
	SendMessageW(hTarget, WM_ERASEBKGND, (WPARAM)hdc, 0);
	RestoreDC(hdc, nSavedDC);
}

bool 是否已创建(窗口基类& w)
{
	return IsWindow(w.窗口句柄);
}