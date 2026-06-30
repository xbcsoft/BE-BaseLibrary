#ifndef _BEWin32UIProj
#include <BEWin32UI/VSDllSDK.hpp>
#else
#include "../../VSDllSDK.hpp"
#endif
#include "../标签.h"

标签* UI_标签_创建(byte* fb, 容器类* 父窗口)
{
	if (fb==nullptr){
		delete 父窗口; //当参数1给出空指针时此方法变为销毁此投入的参数2实例对象
		return 0;
	}
	标签::参数 cs;
	cs.左边 = gFBf(fb, 0);
	cs.顶边 = gFBf(fb, 1);
	cs.宽度 = gFBf(fb, 2);
	cs.高度 = gFBf(fb, 3);
	cs.可视 = gFBb(fb, 4);
	cs.禁止 = gFBb(fb, 5);
	cs.可停留焦点 = gFBb(fb, 6);
	cs.停留顺序 = gFBi(fb, 7);
	cs.标题 = gFBs<W>(fb, 8);
	cs.效果 = gFBi(fb, 9);
	cs.边框 = gFBi(fb, 10);
	cs.字体 = gFBpo<字体>(fb, 11);
	cs.文本颜色 = gFBi(fb, 12);
	cs.背景颜色 = gFBi(fb, 13);
	cs.是否自动折行 = gFBb(fb, 14);
	cs.横向对齐方式 = gFBi(fb, 15);
	cs.纵向对齐方式 = gFBi(fb, 16);
	标签* ui = new 标签;
	ui->创建(cs, 父窗口);
	return ui;
}

标签::参数 标签::_df;
static UI属性元信息 META_标签[] = {
	//int 索引,LPWSTR 名称;DWORD 类型;union 默认值;LPWSTR 枚举值;LPWSTR 说明;BOOL 可空;BOOL 子项;
	_MK属性(0, L"左边", UI属性类型::数值, 标签::_df.左边, NULL, L"", false, false),
	_MK属性(1, L"顶边", UI属性类型::数值, 标签::_df.顶边, NULL, L"", false, false),
	_MK属性(2, L"宽度", UI属性类型::数值, 标签::_df.宽度, NULL, L"", false, false),
	_MK属性(3, L"高度", UI属性类型::数值, 标签::_df.高度, NULL, L"", false, false),
	_MK属性(4, L"可视", UI属性类型::布尔, 标签::_df.可视, NULL, L"", false, false),
	_MK属性(5, L"禁止", UI属性类型::布尔, 标签::_df.禁止, NULL, L"", false, false),
	_MK属性(6, L"可停留焦点", UI属性类型::布尔, 标签::_df.可停留焦点, NULL, L"", false, false),
	_MK属性(7, L"停留顺序", UI属性类型::整数, 标签::_df.停留顺序, NULL, L"", false, false),
	_MK属性(8, L"标题", UI属性类型::文本, 标签::_df.标题, NULL, L"", false, false),
	_MK属性(9, L"效果", UI属性类型::枚举, (int)标签::_df.效果, L"0.通常|1.凹入|2.凸出|3.阴影", L"", false, false),
	_MK属性(10, L"边框", UI属性类型::枚举, (int)标签::_df.边框, L"0.无边框|1.凹入式|2.凸出式|3.浅凹入式|4.镜框式|5.单线边框式|6.渐变镜框式", L"", false, false),
	_MK属性(11, L"字体", UI属性类型::字体, &标签::_df.字体, NULL, L"", false, false),
	_MK属性(12, L"文本颜色", UI属性类型::颜色, (int)标签::_df.文本颜色, NULL, L"", false, false),
	_MK属性(13, L"背景颜色", UI属性类型::颜色, (int)标签::_df.背景颜色, NULL, L"", false, false),
	_MK属性(14, L"是否自动折行", UI属性类型::布尔, 标签::_df.是否自动折行, NULL, L"", false, false),
	_MK属性(15, L"横向对齐方式", UI属性类型::枚举, (int)标签::_df.横向对齐方式, L"0.左对齐|1.居中|2.右对齐", L"", false, false),
	_MK属性(16, L"纵向对齐方式", UI属性类型::枚举, (int)标签::_df.纵向对齐方式, L"0.顶对齐|1.居中|2.底对齐", L"", false, false),
};

void UI_标签_置属性(标签* ui, int methodi, byte* fb)
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
		ui->标题_(gFBs<W>(fb, 0));
		break;
	case 9:
		ui->效果_(gFBi(fb, 0));
		break;
	case 10:
		ui->边框_(gFBi(fb, 0));
		break;
	case 11:
		//字体
		break;
	case 12:
		ui->文本颜色_(gFBi(fb, 0));
		break;
	case 13:
		ui->背景颜色_(gFBi(fb, 0));
		break;
	case 14:
		ui->是否自动折行_(gFBb(fb, 0));
		break;
	case 15:
		ui->横向对齐方式_(gFBi(fb, 0));
		break;
	case 16:
		ui->纵向对齐方式_(gFBi(fb, 0));
		break;
	}
}
