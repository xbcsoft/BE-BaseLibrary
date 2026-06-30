#pragma once
#include <BEWin32Base/BEWin32Base.h>

//用于消息反射的安全前缀标示（类似于MFC的WM_REFLECT_BASE或WTL of OCM__BASE）
//能确保控件在接收到反射消息时其原生态处理机制(DefWindowProc)不会出现误会并导致死循环反弹
#define BE_REFLECT (WM_APP + 0x1000)
//仿易语言中的标签反馈事件消息号
#define WM_反馈 32885

struct D {}; //用于设备单位的绘制

struct RECTF { float left, top, right, bottom; };
struct SIZEF { float cx, cy; };

#define 定义_子组件通用构造方法 参数(){}\
参数(float x,float y,float w,float h,bool show=true){左边=x;顶边=y;宽度=w;高度=h;可视=show;\
}

#define 定义_字体成员 void 置字体(const 字体& f){窗口基类::置字体(f);}\
字体 取字体(){return 窗口基类::取字体();}\
HFONT 取字体句柄(){return 窗口基类::取字体句柄();\
}

#define 定义_标题成员 StrW 标题;\
void 标题_(const StrW& title){ 标题 = title; 窗口_置标题(窗口句柄, title); 重画(); }

#define 定义_背景颜色成员 颜色 背景颜色;\
颜色 _取背景颜色(颜色** out=0){ if(out)*out=&背景颜色; return 背景颜色; }\
void 背景颜色_(颜色 cr, bool 是否重画 = true) {\
窗口基类::背景颜色_(cr,是否重画); \
}

#define _SUB_ 

#define _水平居中(宽度) ((rdpi(GetSystemMetrics(SM_CXSCREEN)) - 宽度) / 2)
#define _垂直居中(高度) ((rdpi(GetSystemMetrics(SM_CYSCREEN)) - 高度) / 2)

#define _消息循环处理过程 \
if (msg.hwnd) { \
	HWND h = GetActiveWindow(); \
	if (h){ \
		窗口基类* _wBase = (窗口基类*)GetWindowLongPtrW(h, GWLP_USERDATA); \
		if (_wBase && _wBase->_是否为窗口类对象){ \
			窗口* _wObj = (窗口*)_wBase; \
			if (TranslateAccelerator(h, _wObj->_wa.hAccel, &msg) \
			 ||(_wObj->子控件焦点导航 && IsDialogMessage(h, &msg)) )\
				goto __消息循环处理完毕; \
		} \
	} \
} \
TranslateMessage(&msg); \
DispatchMessage(&msg); \
__消息循环处理完毕:;


定义_枚举型(窗口层次, char,
	顶层 = 1, 底层 = 2, 最高层 = 3, 次高层 = 4
);

定义_枚举型(控制键, char,
	Shift键 = FSHIFT, Ctrl键 = FCONTROL, Alt键 = FALT
);

定义_枚举型(横向对齐, char,
	左对齐 = 0, 居中 = 1, 右对齐 = 2
);

定义_枚举型(纵向对齐, char,
	顶对齐 = 0, 居中 = 1, 底对齐 = 2
);