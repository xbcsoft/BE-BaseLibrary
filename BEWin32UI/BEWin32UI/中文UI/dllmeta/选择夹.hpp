#ifndef _BEWin32UIProj
#include <BEWin32UI/VSDllSDK.hpp>
#else
#include "../../VSDllSDK.hpp"
#endif
#include "../选择夹.h"

选择夹* UI_选择夹_创建(byte* fb, 容器类* 父窗口)
{
	if (fb==nullptr){
		delete 父窗口; //当参数1给出空指针时此方法变为销毁此投入的参数2实例对象
		return 0;
	}
	选择夹::参数 cs;
	cs.左边 = gFBf(fb, 0);
	cs.顶边 = gFBf(fb, 1);
	cs.宽度 = gFBf(fb, 2);
	cs.高度 = gFBf(fb, 3);
	cs.可视 = gFBb(fb, 4);
	cs.禁止 = gFBb(fb, 5);
	cs.字体 = gFBpo<字体>(fb, 6);
	cs.背景颜色 = gFBi(fb, 7);
	cs.方向 = gFBi(fb, 8);
	选择夹* ui = new 选择夹;
	ui->创建(cs, 父窗口);
	return ui;
}

选择夹::参数 选择夹::_df;
static UI属性元信息 META_选择夹[] = {
	//int 索引,LPWSTR 名称;DWORD 类型;union 默认值;LPWSTR 枚举值;LPWSTR 说明;BOOL 可空;BOOL 子项;
	_MK属性(0, L"左边", UI属性类型::数值, 选择夹::_df.左边, NULL, L"", false, false),
	_MK属性(1, L"顶边", UI属性类型::数值, 选择夹::_df.顶边, NULL, L"", false, false),
	_MK属性(2, L"宽度", UI属性类型::数值, 选择夹::_df.宽度, NULL, L"", false, false),
	_MK属性(3, L"高度", UI属性类型::数值, 选择夹::_df.高度, NULL, L"", false, false),
	_MK属性(4, L"可视", UI属性类型::布尔, 选择夹::_df.可视, NULL, L"", false, false),
	_MK属性(5, L"禁止", UI属性类型::布尔, 选择夹::_df.禁止, NULL, L"", false, false),
	_MK属性(6, L"字体", UI属性类型::字体, &选择夹::_df.字体, NULL, L"", false, false),
	_MK属性(7, L"背景颜色", UI属性类型::颜色, (int)选择夹::_df.背景颜色, NULL, L"", false, false),
	_MK属性(8, L"方向", UI属性类型::枚举, (int)选择夹::_df.方向, L"0.上|TCS_BOTTOM.下", L"", false, false),
};

void UI_选择夹_置属性(选择夹* ui, int methodi, byte* fb)
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
		//字体
		break;
	case 7:
		ui->背景颜色_(gFBi(fb, 0));
		break;
	case 8:
		//方向
		break;
	}
}
