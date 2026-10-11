#pragma once
#include <BEMod.h>
#include "../SciterUI.h"

struct __启动窗口 : 窗口
{
	void 事件_创建完毕();

#pragma region 组件成员
	SciterUI zst, zst2;

	struct _按钮1 : 按钮 {
		void 事件_被单击();
	} 按钮1;

	struct _st : SciterUI {

		//回调事件里边的URL是以W版为主，而pld->outData取决于原始的网页数据（本质上是字节集）
		UINT 事件_资源加载(LPSCN_LOAD_DATA pld);
	} st;

#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
}; extern __启动窗口 _启动窗口;
