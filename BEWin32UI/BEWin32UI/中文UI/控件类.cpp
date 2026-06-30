#include "窗口基类.h"
#include "容器类.h"
class 窗口;

class EXP 控件类 : public 窗口基类
{
protected:
	struct 参数 {
		float 左边;
		float 顶边;
		float 宽度;
		float 高度;
		bool 可视 = true;
		bool 禁止 = false;
		bool 可停留焦点 = true;
		int 停留顺序 = 0;
	};

	void _子初始(const 参数& cs, 容器类* 父窗口, HWND 父句柄, bool 禁止停留焦点 = false) {
		//有一种场景，让当前子组件的生命周期由实例父窗口掌管，而窗口显示的置父却可以在别的窗口里
		父窗口句柄 = 父句柄 ? 父句柄 : ((父窗口) ? 父窗口->窗口句柄 : 0);
		if (父窗口句柄 == 0) { 父窗口句柄 = (HWND)*(((HWND*)_g_interWnd)+1); }
		this->父窗口 = 父窗口;
		左边 = cs.左边;
		顶边 = cs.顶边;
		宽度 = cs.宽度;
		高度 = cs.高度;
		可视 = cs.可视;
		if (禁止停留焦点) {
			可停留焦点_ = false;
		} else {
			可停留焦点_ = cs.可停留焦点;
			停留顺序_ = 0;
		}
		_窗口风格 = WS_CHILDWINDOW;
		if (可视)_窗口风格 |= WS_VISIBLE;
		if (可停留焦点_)_窗口风格 |= WS_TABSTOP;
		窗口基类::_子入父列表();
	}

public:
	void 可停留焦点(bool is) {
		if (is) {
			窗口_添加风格(窗口句柄, WS_TABSTOP);
		} else {
			窗口_移除风格(窗口句柄, WS_TABSTOP);
		}
	}
public:
	bool 可停留焦点_; int 停留顺序_;
};
