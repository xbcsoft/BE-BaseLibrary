#include <BEMod.h>

struct _窗口03_JS模块化动态页面 : 窗口
{
	SciterDom dom;
	void 事件_创建完毕()
	{
		dom = st.取文档模型();
		标题_(L"SciterUI - JS模块化动态页面与C++交互测试");
		dom.嵌入原生控件("#btn_cpp", 按钮1.窗口句柄);
	}

	void _按钮1_被单击()
	{
		//dom.执行JS脚本("subPageInstance.changeUser('白菜叔叔给糖 #来自C++原生按钮修改！');");
		//还可以下面的对象级方法调用：
		dom.执行JS脚本("subPageInstance").方法("changeUser", "白菜叔叔 #来自C++原生按钮修改！");
	}
	
#pragma region 组件成员
	struct _st :SciterUI {
	} st;
	struct _按钮1 : 按钮 {
		void 事件_被单击() { 窗口03_JS模块化动态页面._按钮1_被单击(); }
	}按钮1;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
} 窗口03_JS模块化动态页面;