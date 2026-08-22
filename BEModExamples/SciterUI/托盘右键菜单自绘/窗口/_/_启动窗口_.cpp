#include "../_启动窗口.h"

void __启动窗口::载入(窗口* 父窗, bool 模态)
{
	if (窗口句柄)return;

	窗口::参数 cs;
	cs.标题 = L"白易 SciterUI · 托盘右键菜单自绘";
	cs.最大化按钮 = true;
	cs.最小化按钮 = true;
	窗口::创建(cs);

	this->完毕(模态);
}

void __启动窗口::完毕(bool 模态)
{
	SciterUI::参数 cs;
	cs.文件_html = "index.html";
	st.创建(cs, this);
	dom = st.取文档模型();
	窗口::完毕(模态);
}
