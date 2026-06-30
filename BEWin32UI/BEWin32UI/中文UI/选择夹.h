#pragma once
#include "多夹接口.h"
#include "子容器.hpp"

定义_枚举型(子夹头方向, char,
	上 = 0, 下 = TCS_BOTTOM
);

class 选择夹 :public 多夹接口
{
public:
	struct 参数 : 容器类::参数 {
		定义_子组件通用构造方法;
		字体 字体;
		颜色 背景颜色;
		子夹头方向 方向 = 子夹头方向::上;
	}static _df;

	int 现行子夹 = 0;
	颜色 背景颜色;
	定义_字体成员;

	bool 创建(const 参数& cs, 容器类* 父窗口 = NULL, HWND 父句柄 = NULL);

	bool 销毁();

	RECTF 取子夹头矩形(int 索引 = 0);

	SIZEF 置子夹头尺寸(SIZEF sz);

	int 插入子夹(StrW 标题, 可空<int> 子夹索引 = nil);

	int 插入子夹(StrW 标题, 可空<int> 子夹索引, int 图片索引, int 子夹数值 = 0);

	子容器<>* 取子夹容器(int 索引);

	void 现行子夹_(int 索引);

	void 置子夹标题(int 索引, StrW 标题);

	void 置子夹图片(int 索引, int 图片索引);

	void 置子夹数值(int 索引, int a);

	int 取子夹图片(int 索引);

	StrW 取子夹标题(int 索引);

	void 子夹添加组件(int 索引, 窗口基类& a);

	void 置图片组(图片组& imageList);

	LRESULT 挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	//默认选择夹背景只能是白色（这个是系统现代主题在WM_PAINT决定的无法更改）
	//而且如果要强行改，除了自绘外就只能贴透明容器了然后借助白易透明背景机制让父窗口用自己的画刷
	//理论上选择夹自身是没办法做到全透明的...所以指定透明色将无效
	颜色 _取背景颜色(颜色 ** out = 0);
	void 背景颜色_(颜色 cr, bool 重画 = true);
protected:
	图片组 _图片组;
	Arraybe<子容器<>*> _子夹集合;


private:
	void _调整子夹尺寸();

	bool _判断索引非法(int 子夹索引);
};
