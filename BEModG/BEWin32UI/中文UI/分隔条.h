#pragma once
#include "控件类.h"

定义_枚举型(分隔条方向, char,
	水平 = 0, 垂直 = 1
);

class 分隔条 : public 控件类
{
public:
	struct 参数 : 控件类::参数 {
		分隔条方向 方向 = 分隔条方向::水平;
		颜色 线条颜色 = 颜色::浅灰;
		颜色 点燃颜色 = 颜色(RGB(0, 120, 215));
		float 线条粗细 = 1;
		bool 可拖动 = true;
		bool 自动移动自身 = false;
	} static _df;

	分隔条方向 方向;
	颜色 线条颜色;
	颜色 点燃颜色;
	float 线条粗细;
	bool 可拖动;
	bool 自动移动自身;

	bool 创建(const 参数& cs, 容器类* 父窗口 = NULL, HWND 父句柄 = NULL);

	void 方向_(分隔条方向 v);
	void 线条颜色_(颜色 v);
	void 点燃颜色_(颜色 v);
	void 线条粗细_(float v);
	void 可拖动_(bool v);
	void 自动移动自身_(bool v);

	virtual void 事件_位置被改变(float 新位置);
	virtual void 事件_拖动结束(float 最终位置);

	LRESULT 挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

private:
	void _绘制(HDC hdc);

	bool _鼠标在内 = false;
	bool _正在拖动 = false;
	POINT _拖动起始屏幕点{};
	float _拖动起始位置 = 0;
	float _拖动当前位置 = 0;

	void _开始跟踪鼠标();
	void _刷新鼠标状态(bool 在内);
	float _当前位置() const;
	void _拖动到(POINT 屏幕点);
};
