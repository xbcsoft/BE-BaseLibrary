#pragma once
#include "控件类.h"

定义_枚举型(列表框边框, char,
	无边框 = 0, 凹入式 = 1, 凸出式 = 2, 浅凹入式 = 3, 镜框式 = 4, 单线边框式 = 5
);

class 列表框 : public 控件类
{
public:
	struct 参数 : 控件类::参数 {
		列表框边框 边框 = 列表框边框::凹入式;
		bool 自动排序 = false;
		bool 多列 = false;
		int 行间距 = 1;
		颜色 文本颜色 = 颜色::黑色;
		颜色 背景颜色 = 颜色::白色;
		字体 字体;
		bool 允许选择多项 = false;
		int 现行选中项 = -1;
		自定数据 列表项目;
		自定数据 项目数值;
	} static _df;

	bool 创建(const 参数& cs, 容器类* 父窗口 = NULL, HWND 父句柄 = NULL);

	void 行间距_(int v);

	void 边框_(列表框边框 v);

	void 自动排序_(bool v);

	void 多列_(bool v);

	void 允许选择多项_(bool v);

	void 现行选中项_(int index);

	int _取现行选中项();

	void 文本颜色_(颜色 cr);

	void 背景颜色_(颜色 cr, bool 是否重画 = true);

	void 列表项目_(const Bytes& data);

	void 项目数值_(const Bytes& data);

	int 加入项目(const StrW& 文本, int 数值 = 0);

	int 插入项目(int 索引, const StrW& 文本, int 数值 = 0);

	void 删除项目(int 索引);

	void 清空();

	int 取项目数();

	StrW 取项目文本(int 索引);

	void 置项目文本(int 索引, const StrW& 文本);

	int 取项目数值(int 索引);

	void 置项目数值(int 索引, int 数值);

	bool 是否被选中(int 索引);

	void 置选中(int 索引, bool 是否选中 = true);

	int 取被选择总数();

	void 取被选中项(Arraybe<int>& out选中项);

	int 查找项目(const StrW& 欲查找文本, int 起始索引 = -1, bool 精确匹配 = false);

	int 取顶端项目索引();

	void 置顶端项目索引(int 索引);

	void 置字体(const 字体& f);
	字体 取字体();
	HFONT 取字体句柄();

public:
	virtual void 事件_列表项被选择();
	virtual void 事件_双击选择();

	virtual LRESULT 挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	列表框边框 边框;
	bool 自动排序;
	bool 多列;
	int 行间距;
	颜色 文本颜色;
	颜色 背景颜色;
	bool 允许选择多项;
	int 现行选中项;
	Bytes 列表项目;
	Bytes 项目数值;

private:
	void _更新行间距();

	DWORD _取扩展风格() const;

	DWORD _取列表框风格() const;

	void _重新加载列表数据();

	void _重建控件(bool 首次创建 = false);
};
