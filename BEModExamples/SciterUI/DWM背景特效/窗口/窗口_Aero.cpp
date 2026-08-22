#include <BEMod.h>
#include "../SciterDWM.h"

struct _窗口_Aero : 窗口
{
	void 事件_创建完毕()
	{
		st.取文档模型().置页面标题("Sciter-毛玻璃特效");
		RECTF r = this->取窗口矩形(); 移动(r.left-宽度/2-5);

		// 1. 应用毛玻璃特效
		SciterDWM背景特效(st, SciterDWMType::Aero);

		// 2. <html window-frame="extended"> 可还原到系统原生边框阴影
		边框 = 窗口边框::普通可调边框; //注意创建那边必须要先填无边框而后这里恢复
	}

#pragma region 组件成员
	struct _st :SciterUI {

	} st;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
} 窗口_Aero;