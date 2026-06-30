#pragma once
#include "控件类.h"

定义_枚举型(标签效果, char,
	通常 = 0, 凹入 = 1, 凸出 = 2, 阴影 = 3
);

定义_枚举型(标签边框, char,
	无边框 = 0, 凹入式 = 1, 凸出式 = 2, 浅凹入式 = 3, 镜框式 = 4, 单线边框式 = 5, 渐变镜框式 = 6
);

class 标签 :public 控件类
{
public:
	struct 参数 : 控件类::参数 {
		定义_子组件通用构造方法;
		StrW 标题 = L"标签";
		标签效果 效果 = 标签效果::通常;
		标签边框 边框 = 标签边框::无边框;
		字体 字体;
		颜色 文本颜色 = 颜色::黑色;
		颜色 背景颜色 = 颜色::默认底色;
		bool 是否自动折行 = false;
		横向对齐 横向对齐方式 = 横向对齐::左对齐;
		纵向对齐 纵向对齐方式 = 纵向对齐::顶对齐;
	}static _df;

	bool 创建(const 参数& cs, 容器类* 父窗口 = NULL, HWND 父句柄 = NULL);

	void 标题_(const StrW& tit);

	void 文本颜色_(颜色 cr);

	void 横向对齐方式_(横向对齐 mode);

	void 是否自动折行_(bool is);

	void 纵向对齐方式_(纵向对齐 mode);

	void 效果_(标签效果 eff);

	void 边框_(标签边框 bor);
public:
	定义_字体成员;
	StrW 标题;
	标签效果 效果;
	标签边框 边框;
	颜色 文本颜色;
	定义_背景颜色成员;
	横向对齐 横向对齐方式;
	纵向对齐 纵向对齐方式;
	bool 是否自动折行;

	virtual LRESULT 事件_反馈(WPARAM wParam, LPARAM lParam);

	LRESULT 挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};
