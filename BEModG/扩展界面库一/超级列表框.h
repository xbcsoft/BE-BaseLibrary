#pragma once
#include "stdafx.h"
#include <CommCtrl.h>

定义_枚举型(超级列表框边框, char,
	无边框 = 0, 凹入式 = 1, 凸出式 = 2, 浅凹入式 = 3, 镜框式 = 4, 单线边框式 = 5
);

定义_枚举型(超级列表框类型, char,
	大图标列表框 = 0, 小图标列表框 = 1, 普通列表框 = 2, 报表列表框 = 3
);

定义_枚举型(超级列表框对齐方式, char,
	顶部对齐 = 0, 左边对齐 = 1
);

定义_枚举型(超级列表框排序方式, char,
	不排序 = 0, 正向排序 = 1, 逆向排序 = 2
);

class 超级列表框 : public 控件类
{
public:
	struct 参数 : 控件类::参数 {
		定义_子组件通用构造方法;
		超级列表框边框 边框 = 超级列表框边框::凹入式;
		颜色 文本颜色 = 颜色::黑色;
		颜色 文本背景色 = 颜色::透明;
		颜色 背景颜色 = 颜色::白色;
		字体 字体;
		超级列表框类型 类型 = 超级列表框类型::报表列表框;
		超级列表框对齐方式 图标对齐方式 = 超级列表框对齐方式::顶部对齐;
		bool 自动排列图标 = true;
		bool 标题自动换行 = true;
		bool 无表头 = false;
		bool 表头可单击 = true;
		bool 整行选择 = false;
		bool 显示表格线 = false;
		bool 表列可拖动 = false;
		bool 热点跟踪 = false;
		bool 手形指针 = false;
		bool 标注非热点 = false;
		bool 标注热点 = false;
		bool 允许编辑 = false;
		超级列表框排序方式 排序方式 = 超级列表框排序方式::不排序;
		bool 是否有检查框 = false;
		bool 平面滚动条 = false;
		bool 单一选择 = false;
		bool 始终显示选择项 = false;
		int 现行选中项 = -1;
		Bytes 报表列;
		Bytes 表项;
	} static _df;

	超级列表框边框 边框;
	颜色 文本颜色;
	颜色 文本背景色;
	颜色 背景颜色;
	超级列表框类型 类型;
	超级列表框对齐方式 图标对齐方式;
	bool 自动排列图标;
	bool 标题自动换行;
	bool 无表头;
	bool 表头可单击;
	bool 整行选择;
	bool 显示表格线;
	bool 表列可拖动;
	bool 热点跟踪;
	bool 手形指针;
	bool 标注非热点;
	bool 标注热点;
	bool 允许编辑;
	超级列表框排序方式 排序方式;
	bool 是否有检查框;
	bool 平面滚动条;
	bool 单一选择;
	bool 始终显示选择项;
	int 现行选中项;
	StrW 结束编辑文本;

	定义_字体成员;

	图片组 _图片组;
	图片组 _状态图片组;
	图片组 _表头图片组;

	bool 创建(const 参数& cs, 容器类* 父窗口 = NULL, HWND 父句柄 = NULL);

	bool 销毁();

	void 边框_(超级列表框边框 v);

	void 文本颜色_(颜色 cr);

	void 文本背景色_(颜色 cr);

	void 背景颜色_(颜色 cr, bool 是否重画 = true);

	void 类型_(超级列表框类型 v);

	void 图标对齐方式_(超级列表框对齐方式 v);

	void 自动排列图标_(bool is);

	void 标题自动换行_(bool is);

	void 无表头_(bool is);

	void 表头可单击_(bool is);

	void 整行选择_(bool is);

	void 显示表格线_(bool is);

	void 表列可拖动_(bool is);

	void 热点跟踪_(bool is);

	void 手形指针_(bool is);

	void 标注非热点_(bool is);

	void 标注热点_(bool is);

	void 允许编辑_(bool is);

	void 排序方式_(超级列表框排序方式 v);

	void 是否有检查框_(bool is);

	void 平面滚动条_(bool is);

	void 单一选择_(bool is);

	void 始终显示选择项_(bool is);

	void 现行选中项_(int index);

	void 置图片组(图片组& imageList);

	void 置状态图片组(图片组& imageList);

	void 置表头图片组(图片组& imageList);

	StrW 取标题(int 表项索引, int 列索引 = 0);

	void 置标题(int 表项索引, int 列索引 = 0, StrW 标题 = L"");

	int 取图片(int 表项索引, int 列索引 = 0);

	void 置图片(int 表项索引, int 列索引 = 0, int 图片索引 = -1);

	int 取当前状态图片(int 表项索引);

	void 置状态图片(int 表项索引, int 状态图片索引 = -1);

	int 取缩进数目(int 表项索引);

	void 置缩进数目(int 表项索引, int 缩进数目 = 0);

	int 取表项数值(int 表项索引);

	void 置表项数值(int 表项索引, int 表项数值 = 0);

	Arraybe<int> 取被选择表项();

	int 取已选择表项数();

	bool 是否被选择(int 表项索引);

	void 选择表项(int 表项索引);

	void 取消表项选择(int 表项索引);

	int 取表项数();

	int 插入表项(int 插入位置 = -1, StrW 标题 = L"", int 图片索引 = -1, int 状态图片索引 = -1, int 缩进数目 = 0, int 表项数值 = 0);

	void 删除表项(int 表项索引);

	void 全部删除();

	int 查找表项(StrW 标题, int 起始查找位置 = 0, bool 是否精确匹配 = true, int 列索引 = 0);

	int 查找表项数值(int 起始查找位置 = 0, int 表项数值 = 0);

	void 保证显示(int 表项索引);

	void 排列图标();

	StrW 取列标题(int 列索引);

	int 取列对齐方式(int 列索引);

	int 取列宽(int 列索引);

	int 取列图片(int 列索引);

	bool 图片是否居右(int 列索引);

	void 置列标题(int 列索引, StrW 标题 = L"");

	void 置列宽(int 列索引, int 列宽 = -1);

	void 置列图片(int 列索引, int 图片索引 = -1);

	int 取列数();

	int 插入列(int 插入位置 = -1, StrW 标题 = L"", int 列宽 = 50, int 对齐方式 = 0, int 图片索引 = -1, bool 图片是否居右 = false);

	void 删除列(int 列索引);

	Bytes 导出列项();

	void 载入列项(const Bytes& dat);

	Bytes 导出表项();

	void 载入表项(const Bytes& dat);

	void 进入编辑(int 表项索引);

	virtual void 事件_当前表项被改变();
	virtual void 事件_表项被激活();
	virtual void 事件_表头被单击(int 被单击列索引);
	virtual void 事件_表项跟踪(int 表项索引);
	virtual void 事件_左键单击表项();
	virtual void 事件_右键单击表项();
	virtual bool 事件_开始编辑(int 表项索引);
	virtual bool 事件_结束编辑(int 表项索引);
	virtual void 事件_检查框状态被改变(int 表项索引, int 检查框状态);

	virtual LRESULT 挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	void _同步样式();

	DWORD _取基础风格();

	DWORD _取扩展风格();

	int _取现行选中项();
};
