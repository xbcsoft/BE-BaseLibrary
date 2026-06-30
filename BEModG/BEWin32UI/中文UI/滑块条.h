#pragma once
#include "控件类.h"

定义_枚举型(滑块条方向, int, 横向 = TBS_HORZ, 纵向 = TBS_VERT);
定义_枚举型(滑块条刻度类型, int, 下_右 = TBS_BOTTOM, 上_左 = TBS_TOP, 两端 = TBS_BOTH, 无刻度 = TBS_NOTICKS);

class 滑块条 : public 控件类
{
public:
	struct 参数 : 控件类::参数 {
		定义_子组件通用构造方法;
		滑块条方向 方向 = 滑块条方向::横向;
		滑块条刻度类型 刻度类型 = 滑块条刻度类型::下_右;
		int 单位刻度值 = 1;
		bool 允许选择 = false;
		int 首选择位置 = 0;
		int 选择长度 = 0;
		int 页改变值 = 5;
		int 行改变值 = 1;
		int 最小位置 = 0;
		int 最大位置 = 10;
		int 位置 = 0;
	} static _df;

	bool 创建(const 参数& cs, 容器类* 父窗口 = NULL, HWND 父句柄 = NULL);

	void 方向_(滑块条方向 v);

	void 刻度类型_(滑块条刻度类型 v);

	void 单位刻度值_(int v);

	void 允许选择_(bool v);

	void 首选择位置_(int v);

	void 选择长度_(int v);

	void 页改变值_(int v);

	void 行改变值_(int v);

	void 最小位置_(int v);

	void 最大位置_(int v);

	void 位置_(int v);

public:
	virtual void 事件_位置被改变();

	virtual LRESULT 挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	滑块条方向 方向;
	滑块条刻度类型 刻度类型;
	int 单位刻度值;
	bool 允许选择;
	int 首选择位置;
	int 选择长度;
	int 页改变值;
	int 行改变值;
	int 最小位置;
	int 最大位置;
	int 位置;
};
