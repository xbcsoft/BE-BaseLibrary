#include "../窗口01_DOM基础.h"

void _窗口01_DOM基础::载入(窗口* 父窗, bool 模态)
{
	if (窗口句柄)return;

	窗口::参数 cs{ 0, 0, 380+5, 250+15 };
	cs.最小化按钮 = true;
	cs.最大化按钮 = true;
	cs.边框 = 窗口边框::普通可调边框;
	窗口01_DOM基础.创建(cs, 父窗, 模态);
	
	this->完毕(模态);
}

void _窗口01_DOM基础::完毕(bool 模态)
{
	SciterUI::参数 cs;
	cs.文件_html = "index.html";
	st.创建(cs, this);
	窗口::完毕(模态);
}
