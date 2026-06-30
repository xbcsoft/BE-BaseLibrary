#include <BEMod.h>

struct __启动窗口 : 窗口
{
	void 事件_创建完毕()
	{
		调试输出("你好啊白易语言");
	}

	bool 通用事件_背景绘制(HDC hdc)
	{
		绘制上下文 dc(hdc);
		dc.绘制位图(位图::从内存创建(R::背景图), 0, 0);
		return false; //这里要阻止向上传导
	}

#pragma region 组件成员
	struct _图片框1 : 图片框 {
	}图片框1;
	struct : 图片框 {
	}图片框2;
	struct : 图片框 {
	}图片框3;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
} _启动窗口;