#pragma once
#include "控件类.h"

定义_枚举型(进度条方向, int, 横向 = 0, 纵向 = PBS_VERTICAL);
定义_枚举型(进度条显示方式, int, 分块 = 0, 平滑 = PBS_SMOOTH);

class 进度条 : public 控件类
{
public:
	struct 参数 : 控件类::参数 {
		定义_子组件通用构造方法;
		进度条方向 方向 = 进度条方向::横向;
		进度条显示方式 显示方式 = 进度条显示方式::分块;
		int 最小位置 = 0;
		int 最大位置 = 100;
		int 位置 = 0;
	} static _df;

	bool 创建(const 参数& cs, 容器类* 父窗口 = NULL, HWND 父句柄 = NULL);

	void 最小位置_(int v);

	void 最大位置_(int v);

	void 位置_(int v);

	void 方向_(进度条方向 d);

	void 显示方式_(进度条显示方式 s);

public:
	进度条方向 方向;
	进度条显示方式 显示方式;
	int 最小位置;
	int 最大位置;
	int 位置;
};
