#pragma once
#include <BEMod.h>

void CALLBACK my文件拖放回调(const wchar_t* 文件列表);

struct __启动窗口 : 窗口
{
	void 事件_创建完毕();

#pragma region 组件成员

#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
}; extern __启动窗口 _启动窗口;
