#include "../窗口00_JS基础.h"

void _窗口00_JS基础::载入(窗口* 父窗, bool 模态)
{
	if (窗口句柄)return;
	窗口::创建({});
	
	this->完毕(模态);
}

void _窗口00_JS基础::完毕(bool 模态)
{
	SciterUI::全局初始化();
	SciterUI::参数 cs;
	cs.内存_html = "<html></html>";
	st.创建(cs, this);
	窗口::完毕(模态);
}
