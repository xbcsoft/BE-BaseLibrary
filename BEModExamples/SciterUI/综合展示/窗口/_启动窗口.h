#pragma once
#include <BEMod.h>

struct __启动窗口 : 窗口
{
	SciterDom dom;
	UINT_PTR timerId = 0;

	void 事件_创建本体();
	void 事件_创建完毕();

	void 事件_尺寸被改变();

	void 事件_将被销毁();

	bool 保存截图到文件();

	/**若要同时获取控制键的状态可调 控制键_是否被下()
	 * @param key
	 */
	bool 通用事件_按下某键(BYTE key);

#pragma region 组件成员
	SciterUI st;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
}; extern __启动窗口 _启动窗口;
