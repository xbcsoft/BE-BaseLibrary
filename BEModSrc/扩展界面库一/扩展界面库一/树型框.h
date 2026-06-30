#pragma once
#include "stdafx.h"
#include <CommCtrl.h>

定义_枚举型(树型框边框, char,
	无边框 = 0, 凹入式 = 1, 凸出式 = 2, 浅凹入式 = 3, 镜框式 = 4, 单线边框式 = 5
);

class 树型框 : public 控件类
{
public:
	struct 参数 : 控件类::参数 {
		定义_子组件通用构造方法;
		树型框边框 边框 = 树型框边框::凹入式;
		颜色 文本颜色 = 颜色::黑色;
		颜色 背景颜色 = 颜色::白色;
		bool 显示加减框 = true;
		bool 显示连线 = true;
		bool 显示根部线 = true;
		bool 允许编辑 = false;
		bool 始终显示选择项 = false;
		int 现行选中项 = -1;
		int 行高 = -1;
		字体 字体;
		Bytes 图片组;
		Bytes 项目;
		bool 是否有检查框 = false;
		Bytes 检查框图片组;
		int 检查框状态数 = 2;
	} static _df;

	树型框边框 边框;
	颜色 文本颜色;
	颜色 背景颜色;
	bool 显示加减框;
	bool 显示连线;
	bool 显示根部线;
	bool 允许编辑;
	bool 始终显示选择项;
	int 现行选中项;
	int 行高;
	bool 是否有检查框;
	int 检查框状态数;
	StrW 结束编辑文本;

	定义_字体成员;

	图片组 _图片组;
	图片组 _检查框图片组;

	bool 创建(const 参数& cs, 容器类* 父窗口 = NULL, HWND 父句柄 = NULL);

	bool 销毁();

	void 边框_(树型框边框 v);

	void 文本颜色_(颜色 cr);

	void 背景颜色_(颜色 cr, bool 是否重画 = true);

	void 显示加减框_(bool is);

	void 显示连线_(bool is);

	void 显示根部线_(bool is);

	void 允许编辑_(bool is);

	void 始终显示选择项_(bool is);

	void 现行选中项_(int index);

	void 行高_(int h);

	void 是否有检查框_(bool is);

	void 检查框状态数_(int num);

	void 置图片组(图片组& imageList);

	void 置检查框图片组(图片组& imageList);

	int 取项目数();

	int 取缩进层次(int 项目索引);

	StrW 取项目文本(int 项目索引);

	int 取图片(int 项目索引);

	int 取选中图片(int 项目索引);

	int 取项目数值(int 项目索引);

	bool 置项目文本(int 项目索引, StrW 欲置入的项目文本);

	bool 置图片(int 项目索引, int 图片索引);

	bool 置选中图片(int 项目索引, int 图片索引);

	bool 置项目数值(int 项目索引, int 欲置入的项目数值 = 0);

	void 加粗(int 项目索引);

	void 取消加粗(int 项目索引);

	bool 是否已加粗(int 项目索引);

	void 扩展(int 项目索引);

	void 收缩(int 项目索引);

	void 保证显示(int 项目索引);

	void 进入编辑(int 项目索引);

	int 加入项目(int 父项目索引 = -1, StrW 项目文本 = L"", int 图片索引 = 0, int 选中图片索引 = -1, int 项目数值 = 0, bool 是否加粗 = false, int 检查框状态 = 0);

	bool 删除项目(int 项目索引);

	void 清空();

	int 取父项目(int 项目索引);

	bool 是否有子项目(int 项目索引);

	bool 是否已扩展(int 项目索引);

	int 取检查框状态(int 项目索引);

	void 置检查框状态(int 项目索引, int 检查框状态, bool 连同子项目 = false);

	bool 取加粗状态(int 项目索引);

	void 收缩项目(int 项目索引);

	Bytes 导出项目();

	void 载入项目(const Bytes& dat);

	virtual void 事件_项目被选择(int 选择方式);
	virtual void 事件_双击项目();
	virtual bool 事件_开始编辑(int 项目索引);
	virtual bool 事件_结束编辑(int 项目索引);
	virtual bool 事件_即将扩展(int 项目索引);
	virtual bool 事件_即将收缩(int 项目索引);
	virtual void 事件_右键单击项目();
	virtual void 事件_检查框状态被改变(int 表项索引, int 检查框状态);

	virtual LRESULT 挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	void _置树检查框状态(HTREEITEM hItem, int 检查框状态, bool 连同子项目);

	HTREEITEM _取项目句柄(int index);

	int _取项目索引(HTREEITEM hTarget);

	void _同步样式();

	DWORD _取基础风格();

	DWORD _取扩展风格();
};
