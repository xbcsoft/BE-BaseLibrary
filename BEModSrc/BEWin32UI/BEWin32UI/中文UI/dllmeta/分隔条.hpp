#ifndef _BEWin32UIProj
#include <BEWin32UI/VSDllSDK.hpp>
#else
#include "../../VSDllSDK.hpp"
#endif
#include "../分隔条.h"

分隔条* UI_分隔条_创建(byte* fb, 容器类* 父窗口)
{
	if (fb==nullptr){
		UI_销毁控件(父窗口); //当参数1给出空指针时此方法变为销毁此投入的参数2实例对象
		return 0;
	}
	分隔条::参数 cs;
	cs.左边 = gFBf(fb, 0);
	cs.顶边 = gFBf(fb, 1);
	cs.宽度 = gFBf(fb, 2);
	cs.高度 = gFBf(fb, 3);
	cs.可视 = gFBb(fb, 4);
	cs.禁止 = gFBb(fb, 5);
	cs.可停留焦点 = gFBb(fb, 6);
	cs.停留顺序 = gFBi(fb, 7);
	cs.方向 = gFBi(fb, 8);
	cs.线条颜色 = gFBi(fb, 9);
	cs.点燃颜色 = gFBi(fb, 10);
	cs.线条粗细 = gFBd(fb, 11);
	cs.可拖动 = gFBb(fb, 12);
	cs.自动移动自身 = gFBb(fb, 13);
	分隔条* ui = new 分隔条;
	ui->创建(cs, 父窗口);
	return ui;
}

分隔条::参数 分隔条::_df;
static UI属性元信息 META_分隔条[] = {
	//int 索引,LPWSTR 名称;DWORD 类型;union 默认值;LPWSTR 枚举值;LPWSTR 说明;BOOL 可空;BOOL 子项;
	_MK属性(0, L"左边", UI属性类型::数值, 分隔条::_df.左边, NULL, L"", false, false),
	_MK属性(1, L"顶边", UI属性类型::数值, 分隔条::_df.顶边, NULL, L"", false, false),
	_MK属性(2, L"宽度", UI属性类型::数值, 分隔条::_df.宽度, NULL, L"", false, false),
	_MK属性(3, L"高度", UI属性类型::数值, 分隔条::_df.高度, NULL, L"", false, false),
	_MK属性(4, L"可视", UI属性类型::布尔, 分隔条::_df.可视, NULL, L"", false, false),
	_MK属性(5, L"禁止", UI属性类型::布尔, 分隔条::_df.禁止, NULL, L"", false, false),
	_MK属性(6, L"可停留焦点", UI属性类型::布尔, 分隔条::_df.可停留焦点, NULL, L"", false, false),
	_MK属性(7, L"停留顺序", UI属性类型::整数, 分隔条::_df.停留顺序, NULL, L"", false, false),
	_MK属性(8, L"方向", UI属性类型::枚举, (int)分隔条::_df.方向, L"0.水平|1.垂直", L"", false, false),
	_MK属性(9, L"线条颜色", UI属性类型::颜色, (int)分隔条::_df.线条颜色, NULL, L"", false, false),
	_MK属性(10, L"点燃颜色", UI属性类型::颜色, (int)分隔条::_df.点燃颜色, NULL, L"", false, false),
	_MK属性(11, L"线条粗细", UI属性类型::数值, 分隔条::_df.线条粗细, NULL, L"", false, false),
	_MK属性(12, L"可拖动", UI属性类型::布尔, 分隔条::_df.可拖动, NULL, L"", false, false),
	_MK属性(13, L"自动移动自身", UI属性类型::布尔, 分隔条::_df.自动移动自身, NULL, L"", false, false),
};

void UI_分隔条_置属性(分隔条* ui, int methodi, byte* fb)
{
	switch (methodi)
	{
	case 0:
		ui->移动(gFBf(fb, 0), 空, 空, 空);
		break;
	case 1:
		ui->移动(空, gFBf(fb, 0), 空, 空);
		break;
	case 2:
		ui->移动(空, 空, gFBf(fb, 0), 空);
		break;
	case 3:
		ui->移动(空, 空, 空, gFBf(fb, 0));
		break;
	case 4:
		//可视
		break;
	case 5:
		//禁止
		break;
	case 6:
		//可停留焦点
		break;
	case 7:
		//停留顺序
		break;
	case 8:
		ui->方向_(gFBi(fb, 0));
		break;
	case 9:
		ui->线条颜色_(gFBi(fb, 0));
		break;
	case 10:
		ui->点燃颜色_(gFBi(fb, 0));
		break;
	case 11:
		ui->线条粗细_(gFBd(fb, 0));
		break;
	case 12:
		ui->可拖动_(gFBb(fb, 0));
		break;
	case 13:
		ui->自动移动自身_(gFBb(fb, 0));
		break;
	}
}
