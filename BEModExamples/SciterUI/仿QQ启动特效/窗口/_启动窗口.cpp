#include <BEMod.h>

struct __启动窗口 : 窗口
{
	void 事件_创建完毕()
	{
		SciterDom dom;
		dom = st.取文档模型();
		//若原生层未注入此函数，则JS那边会直接播退出动画后关闭窗口
		//dom.注入JS函数("NativeConfirmClose", [this](可空<SciterObj&>) {
		//	return SciterObj(信息框(L"确定要关闭窗口吗？", L"关闭确认",
		//		MB::确认取消, 窗口句柄) == MB::确认取消);
		//});
	}

#pragma region 组件成员
	struct _st :SciterUI {
	} st;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
} _启动窗口;
