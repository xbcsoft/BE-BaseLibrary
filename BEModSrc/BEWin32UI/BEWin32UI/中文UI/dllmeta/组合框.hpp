#ifndef _BEWin32UIProj
#include <BEWin32UI/VSDllSDK.hpp>
#else
#include "../../VSDllSDK.hpp"
#endif
#include "../组合框.h"

组合框* UI_组合框_创建(byte* fb, 容器类* 父窗口)
{
	if (fb==nullptr){
		delete 父窗口; //当参数1给出空指针时此方法变为销毁此投入的参数2实例对象
		return 0;
	}
	组合框::参数 cs;
	cs.左边 = gFBf(fb, 0);
	cs.顶边 = gFBf(fb, 1);
	cs.宽度 = gFBf(fb, 2);
	cs.高度 = gFBf(fb, 3);
	cs.可视 = gFBb(fb, 4);
	cs.禁止 = gFBb(fb, 5);
	cs.可停留焦点 = gFBb(fb, 6);
	cs.停留顺序 = gFBi(fb, 7);
	cs.类型 = gFBi(fb, 8);
	cs.内容 = gFBs<W>(fb, 9);
	cs.最大文本长度 = gFBi(fb, 10);
	cs.自动排序 = gFBb(fb, 11);
	cs.行间距 = gFBi(fb, 12);
	cs.文本颜色 = gFBi(fb, 13);
	cs.背景颜色 = gFBi(fb, 14);
	cs.字体 = gFBpo<字体>(fb, 15);
	cs.现行选中项 = gFBi(fb, 16);
	组合框* ui = new 组合框;
	ui->创建(cs, 父窗口);
	return ui;
}

组合框::参数 组合框::_df;
static UI属性元信息 META_组合框[] = {
	//int 索引,LPWSTR 名称;DWORD 类型;union 默认值;LPWSTR 枚举值;LPWSTR 说明;BOOL 可空;BOOL 子项;
	_MK属性(0, L"左边", UI属性类型::数值, 组合框::_df.左边, NULL, L"", false, false),
	_MK属性(1, L"顶边", UI属性类型::数值, 组合框::_df.顶边, NULL, L"", false, false),
	_MK属性(2, L"宽度", UI属性类型::数值, 组合框::_df.宽度, NULL, L"", false, false),
	_MK属性(3, L"高度", UI属性类型::数值, 组合框::_df.高度, NULL, L"", false, false),
	_MK属性(4, L"可视", UI属性类型::布尔, 组合框::_df.可视, NULL, L"", false, false),
	_MK属性(5, L"禁止", UI属性类型::布尔, 组合框::_df.禁止, NULL, L"", false, false),
	_MK属性(6, L"可停留焦点", UI属性类型::布尔, 组合框::_df.可停留焦点, NULL, L"", false, false),
	_MK属性(7, L"停留顺序", UI属性类型::整数, 组合框::_df.停留顺序, NULL, L"", false, false),
	_MK属性(8, L"类型", UI属性类型::枚举, (int)组合框::_df.类型, L"0.可编辑列表式|1.可编辑下拉式|2.不可编辑下拉式", L"", false, false),
	_MK属性(9, L"内容", UI属性类型::文本, 组合框::_df.内容, NULL, L"", false, false),
	_MK属性(10, L"最大文本长度", UI属性类型::整数, 组合框::_df.最大文本长度, NULL, L"", false, false),
	_MK属性(11, L"自动排序", UI属性类型::布尔, 组合框::_df.自动排序, NULL, L"", false, false),
	_MK属性(12, L"行间距", UI属性类型::整数, 组合框::_df.行间距, NULL, L"", false, false),
	_MK属性(13, L"文本颜色", UI属性类型::颜色, (int)组合框::_df.文本颜色, NULL, L"", false, false),
	_MK属性(14, L"背景颜色", UI属性类型::颜色, (int)组合框::_df.背景颜色, NULL, L"", false, false),
	_MK属性(15, L"字体", UI属性类型::字体, &组合框::_df.字体, NULL, L"", false, false),
	_MK属性(16, L"现行选中项", UI属性类型::整数, 组合框::_df.现行选中项, NULL, L"", false, false),
};

void UI_组合框_置属性(组合框* ui, int methodi, byte* fb)
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
		//类型
		break;
	case 9:
		ui->内容_(gFBs<W>(fb, 0));
		break;
	case 10:
		//最大文本长度
		break;
	case 11:
		//自动排序
		break;
	case 12:
		ui->行间距_(gFBi(fb, 0));
		break;
	case 13:
		ui->文本颜色_(gFBi(fb, 0));
		break;
	case 14:
		ui->背景颜色_(gFBi(fb, 0));
		break;
	case 15:
		//字体
		break;
	case 16:
		ui->现行选中项_(gFBi(fb, 0));
		break;
	}
}
