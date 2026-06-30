#include <BEMod.h>

struct __启动窗口 : 窗口
{
	void 事件_创建完毕()
	{
		调试输出("你好啊白易语言");
	}

#pragma region 组件成员
	struct _外形框_测试 :外形框 {
	}外形框_测试;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
} _启动窗口;