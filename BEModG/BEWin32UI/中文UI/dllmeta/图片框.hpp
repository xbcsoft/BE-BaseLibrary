#ifndef _BEWin32UIProj
#include <BEWin32UI/VSDllSDK.hpp>
#else
#include "../../VSDllSDK.hpp"
#endif
#include "../图片框.h"

图片框* UI_图片框_创建(byte* fb, 容器类* 父窗口)
{
	if (fb==nullptr){
		UI_销毁控件(父窗口); //当参数1给出空指针时此方法变为销毁此投入的参数2实例对象
		return 0;
	}
	图片框::参数 cs;
	cs.左边 = gFBf(fb, 0);
	cs.顶边 = gFBf(fb, 1);
	cs.宽度 = gFBf(fb, 2);
	cs.高度 = gFBf(fb, 3);
	cs.可视 = gFBb(fb, 4);
	cs.禁止 = gFBb(fb, 5);
	cs.图片 = gFB(fb, 6);
	cs.是否贮存图片 = gFBb(fb, 7);
	cs.边框 = gFBi(fb, 8);
	cs.背景颜色 = gFBi(fb, 9);
	cs.显示方式 = gFBi(fb, 10);
	图片框* ui = new 图片框;
	ui->创建(cs, 父窗口);
	return ui;
}

图片框::参数 图片框::_df;
static UI属性元信息 META_图片框[] = {
	//int 索引,LPWSTR 名称;DWORD 类型;union 默认值;LPWSTR 枚举值;LPWSTR 说明;BOOL 可空;BOOL 子项;
	_MK属性(0, L"左边", UI属性类型::数值, 图片框::_df.左边, NULL, L"", false, false),
	_MK属性(1, L"顶边", UI属性类型::数值, 图片框::_df.顶边, NULL, L"", false, false),
	_MK属性(2, L"宽度", UI属性类型::数值, 图片框::_df.宽度, NULL, L"", false, false),
	_MK属性(3, L"高度", UI属性类型::数值, 图片框::_df.高度, NULL, L"", false, false),
	_MK属性(4, L"可视", UI属性类型::布尔, 图片框::_df.可视, NULL, L"", false, false),
	_MK属性(5, L"禁止", UI属性类型::布尔, 图片框::_df.禁止, NULL, L"", false, false),
	_MK属性(6, L"图片", UI属性类型::数据, &图片框::_df.图片, NULL, L"", false, false),
	_MK属性(7, L"是否贮存图片", UI属性类型::布尔, 图片框::_df.是否贮存图片, NULL, L"", false, false),
	_MK属性(8, L"边框", UI属性类型::枚举, (int)图片框::_df.边框, L"0.无边框|1.凹入式|2.凸出式|3.浅凹入式|4.镜框式|5.单线边框式|6.渐变镜框式", L"", false, false),
	_MK属性(9, L"背景颜色", UI属性类型::颜色, (int)图片框::_df.背景颜色, NULL, L"", false, false),
	_MK属性(10, L"显示方式", UI属性类型::枚举, (int)图片框::_df.显示方式, L"0.图片居左上|1.缩放图片|2.居中显示", L"", false, false),
};

void UI_图片框_置属性(图片框* ui, int methodi, byte* fb)
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
		ui->置图片(gFB(fb, 0));
		break;
	case 7:
		//是否贮存图片
		break;
	case 8:
		ui->边框_(gFBi(fb, 0));
		break;
	case 9:
		ui->背景颜色_(gFBi(fb, 0));
		break;
	case 10:
		ui->显示方式_(gFBi(fb, 0));
		break;
	}
}
