#pragma once
#include "控件类.h"
#include "窗口.h"

class 时钟 : public 控件类
{
public:
	struct 参数 : 控件类::参数 {
		定义_子组件通用构造方法;
		int 时钟周期 = 0;
	} static _df;

	bool 创建(const 参数& cs, 容器类* 父窗口 = NULL, HWND 父句柄 = NULL);

	void 时钟周期_(int 周期);

private:
	static void CALLBACK _时钟_TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
public:
	virtual void 事件_周期事件();
	~时钟();
public:
	int 时钟周期;
};
