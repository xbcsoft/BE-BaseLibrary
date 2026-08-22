#include <BEMod.h>

struct __启动窗口 : 窗口
{
	void 事件_创建本体()
	{
		SciterUI::参数 cs;
#ifdef _DEBUG
		cs.文件_html = "index.html";
#else
		cs.内存_zip = R::reszip;
#endif
		st.创建(cs, this);
	}

	void 事件_创建完毕()
	{
		调试输出("你好啊白易语言");
	}

#pragma region 组件成员
	struct _st : SciterUI {
	} st;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
} _启动窗口;