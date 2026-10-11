#ifndef _BEWin32UIProj
#include <BEWin32UI/VSDllSDK.hpp>
#else
#include "../../VSDllSDK.hpp"
#endif
#include "../列表框.h"

列表框* UI_列表框_创建(byte* fb, 容器类* 父窗口)
{
	if (fb==nullptr){
		UI_销毁控件(父窗口); //当参数1给出空指针时此方法变为销毁此投入的参数2实例对象
		return 0;
	}
	列表框::参数 cs;
	cs.左边 = gFBf(fb, 0);
	cs.顶边 = gFBf(fb, 1);
	cs.宽度 = gFBf(fb, 2);
	cs.高度 = gFBf(fb, 3);
	cs.可视 = gFBb(fb, 4);
	cs.禁止 = gFBb(fb, 5);
	cs.可停留焦点 = gFBb(fb, 6);
	cs.停留顺序 = gFBi(fb, 7);
	cs.边框 = gFBi(fb, 8);
	cs.自动排序 = gFBb(fb, 9);
	cs.多列 = gFBb(fb, 10);
	cs.行间距 = gFBi(fb, 11);
	cs.文本颜色 = gFBi(fb, 12);
	cs.背景颜色 = gFBi(fb, 13);
	cs.字体 = gFBpo<字体>(fb, 14);
	cs.允许选择多项 = gFBb(fb, 15);
	cs.现行选中项 = gFBi(fb, 16);
	cs.列表项目 = gFB(fb, 17);
	cs.项目数值 = gFB(fb, 18);
	列表框* ui = new 列表框;
	ui->创建(cs, 父窗口);
	return ui;
}

列表框::参数 列表框::_df;
static UI属性元信息 META_列表框[] = {
	//int 索引,LPWSTR 名称;DWORD 类型;union 默认值;LPWSTR 枚举值;LPWSTR 说明;BOOL 可空;BOOL 子项;
	_MK属性(0, L"左边", UI属性类型::数值, 列表框::_df.左边, NULL, L"", false, false),
	_MK属性(1, L"顶边", UI属性类型::数值, 列表框::_df.顶边, NULL, L"", false, false),
	_MK属性(2, L"宽度", UI属性类型::数值, 列表框::_df.宽度, NULL, L"", false, false),
	_MK属性(3, L"高度", UI属性类型::数值, 列表框::_df.高度, NULL, L"", false, false),
	_MK属性(4, L"可视", UI属性类型::布尔, 列表框::_df.可视, NULL, L"", false, false),
	_MK属性(5, L"禁止", UI属性类型::布尔, 列表框::_df.禁止, NULL, L"", false, false),
	_MK属性(6, L"可停留焦点", UI属性类型::布尔, 列表框::_df.可停留焦点, NULL, L"", false, false),
	_MK属性(7, L"停留顺序", UI属性类型::整数, 列表框::_df.停留顺序, NULL, L"", false, false),
	_MK属性(8, L"边框", UI属性类型::枚举, (int)列表框::_df.边框, L"0.无边框|1.凹入式|2.凸出式|3.浅凹入式|4.镜框式|5.单线边框式", L"", false, false),
	_MK属性(9, L"自动排序", UI属性类型::布尔, 列表框::_df.自动排序, NULL, L"", false, false),
	_MK属性(10, L"多列", UI属性类型::布尔, 列表框::_df.多列, NULL, L"", false, false),
	_MK属性(11, L"行间距", UI属性类型::整数, 列表框::_df.行间距, NULL, L"", false, false),
	_MK属性(12, L"文本颜色", UI属性类型::颜色, (int)列表框::_df.文本颜色, NULL, L"", false, false),
	_MK属性(13, L"背景颜色", UI属性类型::颜色, (int)列表框::_df.背景颜色, NULL, L"", false, false),
	_MK属性(14, L"字体", UI属性类型::字体, &列表框::_df.字体, NULL, L"", false, false),
	_MK属性(15, L"允许选择多项", UI属性类型::布尔, 列表框::_df.允许选择多项, NULL, L"", false, false),
	_MK属性(16, L"现行选中项", UI属性类型::整数, 列表框::_df.现行选中项, NULL, L"", false, false),
	_MK属性(17, L"列表项目", UI属性类型::自定数据, &列表框::_df.列表项目, NULL, L"", false, false),
	_MK属性(18, L"项目数值", UI属性类型::自定数据, &列表框::_df.项目数值, NULL, L"", false, false),
};

void UI_列表框_置属性(列表框* ui, int methodi, byte* fb)
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
		ui->边框_(gFBi(fb, 0));
		break;
	case 9:
		ui->自动排序_(gFBb(fb, 0));
		break;
	case 10:
		ui->多列_(gFBb(fb, 0));
		break;
	case 11:
		ui->行间距_(gFBi(fb, 0));
		break;
	case 12:
		ui->文本颜色_(gFBi(fb, 0));
		break;
	case 13:
		ui->背景颜色_(gFBi(fb, 0));
		break;
	case 14:
		ui->置字体(gFBpo<字体>(fb, 0));
		break;
	case 15:
		ui->允许选择多项_(gFBb(fb, 0));
		break;
	case 16:
		ui->现行选中项_(gFBi(fb, 0));
		break;
	case 17:
		ui->列表项目_(gFB(fb, 0));
		break;
	case 18:
		ui->项目数值_(gFB(fb, 0));
		break;
	}
}
