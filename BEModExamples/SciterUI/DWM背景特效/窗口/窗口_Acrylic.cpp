#include <BEMod.h>
#include "../SciterDWM.h"

struct _窗口_Acrylic : 窗口
{
	void 事件_创建完毕()
	{
		st.取文档模型().置页面标题("Sciter-亚克力特效");
		RECTF r = this->取窗口矩形(); 移动(r.left+宽度/2+5);

		// 1. 应用亚克力特效
		SciterDWM背景特效(st, SciterDWMType::Acrylic);

		// 2. <html window-frame="extended"> 可还原到系统原生边框阴影
		边框 = 窗口边框::普通可调边框; //注意创建那边必须要先填无边框而后这里恢复

		// 3. 建议Acrylic(亚克力)模式情况下禁用DWM的最大化、最小化还原等动画（因为可能会发生透光闪烁）
		//    若需要解决可能要hook还原状态时设置CSS背景为纯色、等DWM动画完毕后再重新设置透明度
		//    （因此执行起来难度较大，如有需求可自行尝试，下面直接禁用会更干脆达到无穿帮效果）
		//SciterDWM禁用原生动画(st);
	}

#pragma region 组件成员
	struct _st :SciterUI {

	} st;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
} 窗口_Acrylic;