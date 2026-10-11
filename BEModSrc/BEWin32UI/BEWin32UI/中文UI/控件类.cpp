#include "窗口基类.h"
#include "容器类.h"
class 窗口;

class EXP 控件类 : public 窗口基类
{
public:
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
	void 可停留焦点(bool is) {
		if (is) {
			窗口_添加风格(窗口句柄, WS_TABSTOP);
		} else {
			窗口_移除风格(窗口句柄, WS_TABSTOP);
		}
	}
protected:
	void _子初始(const 参数& cs, 容器类* 父容器, HWND 父句柄, bool 禁止停留焦点 = false) {
		//有一种场景，让当前子组件的生命周期由实例父容器掌管，而窗口显示的置父却可以在别的窗口里
		父窗口句柄 = 父句柄 ? 父句柄 : ((父容器) ? 父容器->窗口句柄 : 0);
		if (父窗口句柄 == 0 && _g_interWnd) {
			// _g_interWnd 的对象布局会随公共基类调整而变化，不能再按固定指针偏移取 HWND。
			父窗口句柄 = ((窗口基类*)_g_interWnd)->窗口句柄;
		}
		this->父容器 = 父容器;
		左边 = cs.左边;
		顶边 = cs.顶边;
		宽度 = cs.宽度;
		高度 = cs.高度;
		可视 = cs.可视;
		禁止 = cs.禁止;
		if (禁止停留焦点) {
			可停留焦点_ = false;
		} else {
			可停留焦点_ = cs.可停留焦点;
			停留顺序_ = 0;
		}
		_窗口风格 = WS_CHILDWINDOW;
		if (可视)_窗口风格 |= WS_VISIBLE;
		if (可停留焦点_)_窗口风格 |= WS_TABSTOP;
		if (禁止)_窗口风格 |= WS_DISABLED;
		组件基类::_子入父列表();
	}
public:
	bool 可停留焦点_; int 停留顺序_;
};
