#include "../窗口1.h"
#include "../../resource.h"

void _窗口1::载入(窗口* 父窗, bool 模态)
{
	if (窗口句柄)return;

	窗口::参数 cs; cs.标题 = L"RC资源菜单示例";
	窗口::创建(cs, 父窗, 模态);

	// 直接挂接RC资源中定义的 IDR_MENU1
	主菜单.挂接资源(IDR_MENU1, this);

	//这个菜单可以在RC资源那边查询相关菜单ID

	窗口::完毕(模态);
}
