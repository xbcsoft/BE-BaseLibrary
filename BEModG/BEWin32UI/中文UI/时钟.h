#pragma once
#include "组件基类.h"
#include "窗口.h"

class 时钟 : public 组件基类
{
public:
	struct 参数 :组件基类::参数 {
		int 时钟周期 = 0;
	} static _df;

	bool 创建(const 参数& cs, 容器类* 父容器 = NULL);

	bool 销毁(bool 同步 = false) override;

	void 时钟周期_(int 周期);

private:
	static void CALLBACK _时钟_TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
public:
	virtual void 事件_周期事件();
	~时钟();
public:
	int 时钟周期 = 0;
};
