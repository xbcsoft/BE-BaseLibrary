#include "组件基类.h"
#include "窗口.h"

class EXP 时钟 : public 组件基类
{
public:
	struct 参数 :组件基类::参数 {
		int 时钟周期 = 0;
	} static _df;

	bool 创建(const 参数& cs, 容器类* 父容器 = NULL) {
		左边 = cs.左边;
		顶边 = cs.顶边;
		宽度 = cs.宽度;
		高度 = cs.高度;
		this->父容器 = 父容器;
		_子入父列表();
		时钟周期_(cs.时钟周期);
		return true;
	}

	bool 销毁(bool 同步 = false) override {
		if (时钟周期 <= 0) return false;
		时钟周期_(0);
		return true;
	}

	void 时钟周期_(int 周期) {
		HWND hwGlobal = _g_interWnd ? _g_interWnd->窗口句柄 : NULL;
		if (周期 > 0) {
			时钟周期 = 周期;
			SetTimer(hwGlobal, (UINT_PTR)this, 时钟周期, _时钟_TimerProc);
		} else {
			if (hwGlobal) KillTimer(hwGlobal, (UINT_PTR)this);
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
	~时钟() { 销毁(true); }
public:
	int 时钟周期 = 0;
};
