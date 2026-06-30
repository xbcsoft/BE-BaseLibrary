#pragma once
#include "窗口基类.h"
class 窗口;

//1.该容器类机制意义在于保存动态内存申请的控件让其父容器销毁时自动将它们安全释放内存
//注意：容器类不一定采用Win32原生父子窗口之间的关系（也就是不一定将子窗口进行置父）
//2.以及接管一些Win32控件它必须由父容器来进行处理的消息由白易再给它反射转发回去
class 容器类 :public 窗口基类
{
public:
	struct 参数 {
		float 左边;
		float 顶边;
		float 宽度;
		float 高度;
		bool 可视 = true;
		bool 禁止 = false;
	};

	Arraybe<窗口基类*> _组件列表;

	~容器类();

	bool 销毁();

	void 销毁异步();

	virtual LRESULT 挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	operator 窗口*();

protected:
	//继承自容器类的容器应当实现【背景颜色】（不过目前已经取消这一设定，放在窗口基类中了默认是背景黑色）
	//virtual 颜色 _取背景颜色(颜色** out = 0) = 0;
	//virtual void 背景颜色_(颜色 cr, bool 是否重画 = true) = 0;

	//WS_CLIPCHILDREN，当父窗口绘制时排除所有子窗口的区域，不在子窗口所在区域绘制内容（避免父窗口的重绘覆盖子窗口）
	//WS_CLIPSIBLINGS，当同级（同父窗口）窗口重叠时，当前窗口的绘制区域不包含同级兄弟窗口区域（避免兄弟窗口重叠时的绘制冲突）
	bool _子初始(const 参数& cs, 容器类* 父窗口, HWND 父句柄);
};
