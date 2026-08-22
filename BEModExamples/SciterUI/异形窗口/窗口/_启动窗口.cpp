#include <BEMod.h>

struct __启动窗口 : 窗口
{
	void 事件_创建本体()
	{
		SciterUI::全局初始化();
		SciterUI::参数 cs;
		cs.文件_html = "index.html";
		st.创建(cs, this);
	}
	void 事件_创建完毕()
	{
		
	}

#pragma region 组件成员
	SciterUI st;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
} _启动窗口;