#pragma once
#include "控件类.h"

定义_枚举型(组合框类型, char,
	可编辑列表式 = 0,
	可编辑下拉式 = 1,
	不可编辑下拉式 = 2
);

class 组合框 : public 控件类
{
public:
	struct 参数 : 控件类::参数 {
		定义_子组件通用构造方法;
		组合框类型 类型 = 组合框类型::不可编辑下拉式;
		StrW 内容 = L"";
		int 最大文本长度 = 0;
		bool 自动排序 = false;
		int 行间距 = 1;
		颜色 文本颜色 = 颜色::黑色;
		颜色 背景颜色 = 颜色::白色;
		字体 字体;
		int 现行选中项 = -1;
	} static _df;

	bool 创建(const 参数& cs, 容器类* 父窗口 = NULL, HWND 父句柄 = NULL);

	void 行间距_();

	void 内容_(const StrW& txt);

	StrW _取内容();

	void 现行选中项_(int index);

	int _取现行选中项();

	void 文本颜色_(颜色 cr);

	void 背景颜色_(颜色 cr, bool 是否重画 = true);

	int 取项目数();

	void 加入项目(const StrW& 文本, int 数值 = 0);

	void 插入项目(int 索引, const StrW& 文本, int 数值 = 0);

	void 删除项目(int 索引);

	void 清空();

	StrW 取项目文本(int 索引);

	void 置项目文本(int 索引, const StrW& 文本);

	int 取项目数值(int 索引);

	void 置项目数值(int 索引, int 数值);

public:
	virtual void 事件_列表项被选择();
	virtual void 事件_编辑内容被改变();
	virtual void 事件_将弹出列表();
	virtual void 事件_列表被关闭();
	virtual void 事件_双击选择();

	virtual LRESULT 挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	组合框类型 类型;
	StrW 内容;
	int 最大文本长度;
	bool 自动排序;
	int 行间距;
	颜色 文本颜色;
	颜色 背景颜色;
	定义_字体成员;
	int 现行选中项;

protected:
	DWORD _取组合框风格() const;
};
