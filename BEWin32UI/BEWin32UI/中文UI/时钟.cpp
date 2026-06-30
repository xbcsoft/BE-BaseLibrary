#include "控件类.h"
#include "窗口.h"

class EXP 时钟 : public 控件类
{
public:
	struct 参数 : 控件类::参数 {
		定义_子组件通用构造方法;
		int 时钟周期 = 0;
	} static _df;

	bool 创建(const 参数& cs, 容器类* 父窗口 = NULL, HWND 父句柄 = NULL) {
		控件类::_子初始(cs, 父窗口, 父句柄);
		时钟周期 = cs.时钟周期;
		时钟周期_(时钟周期);
		return true;
	}

	void 时钟周期_(int 周期) {
		HWND hwGlobal = _g_interWnd ? _g_interWnd->窗口句柄 : NULL;
		if (时钟周期 > 0) {
			时钟周期 = 周期;
			SetTimer(hwGlobal, (UINT_PTR)this, 时钟周期, _时钟_TimerProc);
		} else {
			KillTimer(hwGlobal, (UINT_PTR)this);
			时钟周期 = 0;
		}
	}

private:
	static void CALLBACK _时钟_TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
		时钟* pThis = (时钟*)idEvent;
		if (pThis) {
			pThis->事件_周期事件();
		}
	}
public:
	virtual void 事件_周期事件() {}
	~时钟() { 时钟周期_(0); }
public:
	int 时钟周期;
};
