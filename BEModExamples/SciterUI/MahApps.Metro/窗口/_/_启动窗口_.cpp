#include "../_启动窗口.h"

void __启动窗口::载入(窗口* 父窗, bool 模态)
{
	if (窗口句柄)return;
	窗口::参数 cs{ 0, 0, 1108*0.8f, 790*0.8f };
	cs.最大化按钮 = true;
	cs.最小化按钮 = true;
	cs.边框 = 窗口边框::普通可调边框;
	窗口::创建(cs);
	
	this->完毕(模态);
}

void __启动窗口::完毕(bool 模态)
{
	SciterUI::全局初始化();
	SciterUI::参数 cs;
	cs.文件_html = "index.html";
	st.创建(cs, this);
	窗口::完毕(模态);
}
