#pragma once
#include <BEMod.h>

struct __启动窗口 : 窗口
{
	SciterDom dom;

	void 事件_创建完毕();

#pragma region 组件成员
	struct _st : SciterUI {
		UINT 事件_资源加载(LPSCN_LOAD_DATA pld);
	} st;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
}; extern __启动窗口 _启动窗口;
