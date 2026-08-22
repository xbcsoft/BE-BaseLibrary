#include "../窗口02_JS异步协程.h"

void _窗口02_JS异步协程::载入(窗口* 父窗, bool 模态)
{
	if (窗口句柄)return;
	窗口::参数 cs{ 0, 0, 960, 720 };
	cs.最小化按钮 = true;
	窗口02_JS异步协程.创建(cs, 父窗, 模态);

	
	
	this->完毕(模态);
}
void _窗口02_JS异步协程::完毕(bool 模态)
{
	SciterUI::全局初始化();
	SciterUI::参数 cs;
	cs.文件_html = "异步并发测试.html";
	st.创建(cs, this);
	窗口::完毕(模态);
}
