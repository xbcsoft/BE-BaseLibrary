#pragma once
#include <BECore/中文核心.hpp>
#include "helper/窗口辅助核心.h"
#include "helper/句柄管理.h"
#include "helper/字体.h"
#include "helper/位图.h"
#include "helper/绘制上下文.h"

class 窗口基类
{
public:
	HWND 窗口句柄 = NULL;
	HWND 父窗口句柄 = NULL;
	class 容器类* 父窗口 = NULL;

	~窗口基类();

	virtual bool 销毁();

	/**取整个窗口相对于屏幕的矩形区域（虚拟单位）
	 * @return RECTF
	 */
	RECTF 取窗口矩形();

	template <class D>
	RECT 取窗口矩形() {
		return _取窗口矩形();
	}

	RECT _取窗口矩形();

	struct RectS { float x, y, w, h; };

	/**取整个窗口相对于屏幕的矩形区域（虚拟单位）
	* @return
	*/
	RectS 取窗口大小版矩形();

	template <class D>
	RectS 取窗口大小版矩形() {
		return _取窗口大小版矩形();
	}

	RectS _取窗口大小版矩形();

	/**取用户区大小（虚拟单位）
	 * @return SIZEF
	 */
	SIZEF 取用户区大小();

	template <class D>
	SIZE 取用户区大小() {
		return _取用户区大小();
	}

	SIZE _取用户区大小();

	/**取所在父窗口内的矩形（虚拟单位）
	 * @return RECTF
	 */
	RECTF 取父窗用户区内矩形();

	template <class D>
	RECT 取父窗用户区内矩形() {
		return _取父窗用户区内矩形();
	}

	RECT _取父窗用户区内矩形();

	/**移动窗口（注：这些参数值均是虚拟单位，实际值则受dpi影响）
	 * @param x<可空>
	 * @param y<可空>
	 * @param w<可空>
	 * @param h<可空>
	 * @param 重画窗口 重画窗口
	 */
	void 移动(可空<float> x = 空, 可空<float> y = 空, 可空<float> w = 空, 可空<float> h = 空, bool 重画窗口 = true);

	template <class D>
	void 移动(可空<float> x = 空, 可空<float> y = 空, 可空<float> w = 空, 可空<float> h = 空, bool 重画窗口 = true) {
		_移动(x, y, w, h, 重画窗口);
	}

	void _移动(可空<float> x = 空, 可空<float> y = 空, 可空<float> w = 空, 可空<float> h = 空, bool 重画窗口 = true);


	void 激活();

	void 获取焦点();

	bool 可有焦点();

	void 窗口置父(HWND 父句柄);

	//在容器类那边实现
	void 窗口置父(容器类* 父容器, bool 加入父容器列表 = false);

	void 可视_(bool is = true);

	void 禁止_(bool is = true);

	/**制造窗口无效区，待下次消息循环后触发WM_PAINT中重绘（如需立即重绘需再自行调UpdateWindow）
	 * @param 客户区域<可空>
	 * @param 背景擦除=false
	 */
	void 重画(可空<RECTF> 客户区域 = 空, bool 背景擦除 = false);

	template <class D>
	void 重画(可空<RECT> 客户区域 = 空, bool 背景擦除 = false) {
		return _重画(客户区域, 背景擦除);
	}

	void _重画(可空<RECT> 客户区域 = 空, bool 背景擦除 = false);

	/**强制刷新当前窗口及其所有可见的子窗口，让其背景和区域完全重画
	 */
	void 刷新显示();

	void 禁止重画(bool is);

	/**改变窗口或窗口组件的现行所处层次
	 * @param 层次类型 参考-窗口层次::
	 */
	void 调整层次(char 层次类型 = 1);

	BOOL 投递消息(UINT msg, WPARAM wParam, LPARAM lParam = 0);
	LRESULT 发送消息(UINT msg, WPARAM wParam, LPARAM lParam = 0);

	/**当且仅当标记未被置为字符串时可使用置整数
	 * @param a
	 * @return
	 */
	bool 标记_置整数(int a);

	/**当且仅当标记未被置为字符串时可使用取整数
	 * @return
	 */
	int 标记_取整数();

	void 弹出菜单(class 子菜单& menu, 可空<float> x = 空, 可空<float> y = 空);

	virtual bool 通用事件_鼠标左键被按下(int x, int y);
	virtual bool 通用事件_鼠标左键被放开(int x, int y);
	virtual bool 通用事件_鼠标左键双击(int x, int y);
	virtual bool 通用事件_鼠标右键被按下(int x, int y);
	virtual bool 通用事件_鼠标右键被放开(int x, int y);
	virtual bool 通用事件_鼠标位置被移动(int x, int y);

	/**若要同时获取控制键的状态可调 控制键_是否被按下()
	 * @param key
	 */
	virtual bool 通用事件_按下某键(BYTE key);
	/**若要同时获取控制键的状态可调 控制键_是否被按下()
	 * @param key
	 */
	virtual bool 通用事件_放开某键(BYTE key);

	virtual LRESULT 挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


public:
	/**安全获取临时窗口DC，出栈后自动释放DC句柄
	 * @return HandleDC
	 * */
	HandleDC _获取DC();

protected:
	WNDPROC _原窗口过程;
	DWORD _窗口风格;
	void _子入父列表(bool 静态组件不入父 = true);

	// 控件子类化的窗口过程
	static LRESULT CALLBACK _WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

	void _子类化();

	HandleBRUSH _背景画刷;

	friend HWND UI_寻找非透明背景祖先窗口(HWND 窗口句柄, 容器类** obj = 0);
	virtual 颜色 _取背景颜色(颜色** out = 0);
	//之所以这个设置成虚函数目的是有统一在外部枚举子窗口的接口一个个调用
	//但默认情况下该背景颜色不被公开而是看在最终子类上实现或继承或不实现
	virtual void 背景颜色_(颜色 cr, bool 是否重画 = true);

	//字体根据继承类的需要再进行public转向调用该protected
	HandleFONT _字体句柄;
	void 置字体(const 字体& f);
	字体 取字体();
	HFONT 取字体句柄();

	virtual bool 通用事件_窗口重画(PAINTSTRUCT& ps);
	virtual bool 通用事件_背景绘制(HDC hdc);

public:
	float 左边, 顶边, 宽度, 高度;
	bool 可视, 禁止; StrW 标记_;
	bool _是否为窗口类对象 = false;
};


HWND UI_寻找非透明背景祖先窗口(HWND 窗口句柄, 容器类** obj);

HBRUSH UI_请求父窗创建背景刷(HWND 窗口句柄);

void UI_请求父窗绘制背景(HWND 窗口句柄, HDC hdc, const RECT& rc);

bool 是否已创建(窗口基类& w);
