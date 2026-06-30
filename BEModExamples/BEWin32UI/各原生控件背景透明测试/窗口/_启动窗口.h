#pragma once
#include <BEMod.h>

struct __启动窗口 : 窗口
{
	void 事件_创建完毕();

	HandleBITMAP hbp;
	bool 通用事件_背景绘制(HDC hdc);

	bool 通用事件_鼠标左键被按下(int x, int y);

	bool 通用事件_鼠标左键被放开(int x, int y);


#pragma region 组件成员
	struct _图片框1 : 图片框 {
	}图片框1;
	struct : 图片框 {
	}图片框2;
	struct _分组框1 : 分组框 {
	}分组框1;
	struct : 标签 {
	}标签1;
	struct : 单选框 {
	}单选框1;
	struct : 选择框 {
	}选择框1;
	struct : 编辑框 {
	}编辑框1;
	struct : 编辑框 {
	}编辑框2;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
}; extern __启动窗口 _启动窗口;
