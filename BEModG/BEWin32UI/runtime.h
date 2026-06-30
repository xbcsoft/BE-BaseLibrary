#include "BEWin32UI.h"

//如需要引入资源/图标
//主程序入口务必要在最前面引入
//#include "resource.h" 而后才到
//#include <BEWin32UI/runtime.h>
int __获取图标ID() {
#ifndef IDI_ICON1
#define IDI_ICON1 0
#endif // !IDI_ICON1
	return IDI_ICON1;
}

struct __启动窗口 :窗口 { };
extern __启动窗口 _启动窗口;