#pragma once
#include "窗口基类.h"
#include "容器类.h"
class 窗口;

class 控件类 : public 窗口基类
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

	void _子初始(const 参数& cs, 容器类* 父窗口, HWND 父句柄, bool 禁止停留焦点 = false);

public:
	void 可停留焦点(bool is);
public:
	bool 可停留焦点_; int 停留顺序_;
};
