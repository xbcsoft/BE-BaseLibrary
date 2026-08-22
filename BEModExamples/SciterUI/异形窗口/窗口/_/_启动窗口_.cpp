#include "../_启动窗口.h"

void __启动窗口::载入(窗口* 父窗, bool 模态)
{
	if (窗口句柄)return;
	窗口::参数 cs;
	cs.最小化按钮 = true;
	窗口::创建(cs);
	
	
	窗口::完毕(模态);
}