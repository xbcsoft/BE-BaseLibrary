#include "../_启动窗口.h"

void __启动窗口::载入(窗口* 父窗, bool 模态)
{
	if (窗口句柄)return;

	窗口::参数 cs{ 0, 0, 540, 400 };
	cs.最大化按钮 = false;
	cs.最小化按钮 = true;
	窗口::创建(cs);
	this->完毕(模态);
}

void __启动窗口::完毕(bool 模态)
{
	SciterUI::参数 cs;
	cs.文件_html = "index.html";
	cs.是否有启动动画 = true; //DWM特效会干扰我们的启动动画
	//这里true后暂且可视为假且关闭DWM，由JS内部准备好快照表面后才主动可视为真
	st.创建(cs, this);

	推迟调用子程序(100, [this]() { //如有需要可再加回去（100ms足矣等待动画完毕）
		st.支持DWM动效(true); //主要用于支持最小化/还原动画、贴边最大化
	});

	窗口::完毕(模态);
}
