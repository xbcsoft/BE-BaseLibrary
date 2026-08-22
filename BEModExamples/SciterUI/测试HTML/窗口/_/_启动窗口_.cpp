#include "../_启动窗口.h"

void __启动窗口::载入(窗口* 父窗, bool 模态)
{
	if (窗口句柄)return;

	窗口::参数 cs{ 0, 0, 380, 250 };
	//提供原生能力的支持！注意只有改为普通可调的边框后才能进行拉伸
	cs.最大化按钮 = true; //提供双击标题栏自动最大化
	cs.最小化按钮 = true; //提供任务栏可进行最小化
	cs.边框 = 窗口边框::普通可调边框;
	窗口::创建(cs);

	this->完毕(模态);
}

void __启动窗口::完毕(bool 模态)
{
	SciterUI::参数 cs;
	//本工程已将启动目录设置为$(ProjectDir)\out
#ifdef _DEBUG
	cs.文件_html = "自定义选择框与单选框.html";
#else
	cs.内存_zip = R::reszip;
#endif
	//打包内存zip可用out\__!packZipLib.bat

	st.创建(cs, this);
	窗口::完毕(模态);
}
