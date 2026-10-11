#ifndef _BEWin32UIProj
#include <BEWin32UI/VSDllSDK.hpp>
#else
#include "../../VSDllSDK.hpp"
#endif
#include "../外形框.h"

外形框* UI_外形框_创建(byte* fb, 容器类* 父窗口)
{
	if (fb==nullptr){
		UI_销毁控件(父窗口); //当参数1给出空指针时此方法变为销毁此投入的参数2实例对象
		return 0;
	}
	外形框::参数 cs;
	cs.左边 = gFBf(fb, 0);
	cs.顶边 = gFBf(fb, 1);
	cs.宽度 = gFBf(fb, 2);
	cs.高度 = gFBf(fb, 3);
	cs.可视 = gFBb(fb, 4);
	cs.禁止 = gFBb(fb, 5);
	cs.外形 = gFBi(fb, 6);
	cs.线型 = gFBi(fb, 7);
	cs.线宽 = gFBi(fb, 8);
	cs.线条颜色 = gFBi(fb, 9);
	cs.背景颜色 = gFBi(fb, 10);
	cs.圆角半径 = gFBi(fb, 11);
	外形框* ui = new 外形框;
	ui->创建(cs, 父窗口);
	return ui;
}

外形框::参数 外形框::_df;
static UI属性元信息 META_外形框[] = {
	//int 索引,LPWSTR 名称;DWORD 类型;union 默认值;LPWSTR 枚举值;LPWSTR 说明;BOOL 可空;BOOL 子项;
	_MK属性(0, L"左边", UI属性类型::数值, 外形框::_df.左边, NULL, L"", false, false),
	_MK属性(1, L"顶边", UI属性类型::数值, 外形框::_df.顶边, NULL, L"", false, false),
	_MK属性(2, L"宽度", UI属性类型::数值, 外形框::_df.宽度, NULL, L"", false, false),
	_MK属性(3, L"高度", UI属性类型::数值, 外形框::_df.高度, NULL, L"", false, false),
	_MK属性(4, L"可视", UI属性类型::布尔, 外形框::_df.可视, NULL, L"", false, false),
	_MK属性(5, L"禁止", UI属性类型::布尔, 外形框::_df.禁止, NULL, L"", false, false),
	_MK属性(6, L"外形", UI属性类型::枚举, (int)外形框::_df.外形, L"0.无边框|1.矩形|2.圆角矩形|3.椭圆|4.凹入式|5.凸出式", L"", false, false),
	_MK属性(7, L"线型", UI属性类型::枚举, (int)外形框::_df.线型, L"-1.无|0.实线|1.划线|2.点线|3.点划线|4.双点划线", L"", false, false),
	_MK属性(8, L"线宽", UI属性类型::整数, 外形框::_df.线宽, NULL, L"", false, false),
	_MK属性(9, L"线条颜色", UI属性类型::颜色, (int)外形框::_df.线条颜色, NULL, L"", false, false),
	_MK属性(10, L"背景颜色", UI属性类型::颜色, (int)外形框::_df.背景颜色, NULL, L"", false, false),
	_MK属性(11, L"圆角半径", UI属性类型::整数, 外形框::_df.圆角半径, NULL, L"", false, false),
};

void UI_外形框_置属性(外形框* ui, int methodi, byte* fb)
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
		ui->外形_(gFBi(fb, 0));
		break;
	case 7:
		ui->线型_(gFBi(fb, 0));
		break;
	case 8:
		ui->线宽_(gFBi(fb, 0));
		break;
	case 9:
		ui->线条颜色_(gFBi(fb, 0));
		break;
	case 10:
		ui->背景颜色_(gFBi(fb, 0));
		break;
	case 11:
		ui->圆角半径_(gFBi(fb, 0));
		break;
	}
}
