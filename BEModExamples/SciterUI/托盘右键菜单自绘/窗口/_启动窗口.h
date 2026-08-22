#pragma once
#include <BEMod.h>

struct __启动窗口 : 窗口
{
	SciterDom dom;

	void 事件_创建本体();

	void 事件_创建完毕();

	/**
	 * 托盘事件响应
	 * @param 操作类型 1: 单击左键, 2: 双击, 3: 单击右键
	 */
	void 事件_托盘(int 操作类型);

	bool 事件_被关闭();
#pragma region 组件成员
	SciterUI st;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
}; extern __启动窗口 _启动窗口;
