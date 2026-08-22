#include "../窗口03_JS模块化动态页面.h"

void _窗口03_JS模块化动态页面::载入(窗口* 父窗, bool 模态)
{
	if (窗口句柄)return;
	窗口::参数 cs{ 0, 0, 780, 520 };
	cs.标题 = L"SciterUI - JS模块化动态页面";
	窗口03_JS模块化动态页面.创建(cs, 父窗, 模态);

	按钮::参数 btn_cs{ 590, 250, 160, 40 };
	btn_cs.标题 = L"C++修改User";
	按钮1.创建(btn_cs, this);

	this->完毕(模态);
}

void _窗口03_JS模块化动态页面::完毕(bool 模态)
{
	SciterUI::参数 cs;
	cs.文件_html = "动态页面测试/index.html";
	st.创建(cs, this);
	窗口::完毕(模态);
}
