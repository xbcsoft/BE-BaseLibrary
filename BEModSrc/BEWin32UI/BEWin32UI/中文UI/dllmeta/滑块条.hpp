#ifndef _BEWin32UIProj
#include <BEWin32UI/VSDllSDK.hpp>
#else
#include "../../VSDllSDK.hpp"
#endif
#include "../滑块条.h"

滑块条* UI_滑块条_创建(byte* fb, 容器类* 父窗口)
{
	if (fb==nullptr){
		delete 父窗口; //当参数1给出空指针时此方法变为销毁此投入的参数2实例对象
		return 0;
	}
	滑块条::参数 cs;
	cs.左边 = gFBf(fb, 0);
	cs.顶边 = gFBf(fb, 1);
	cs.宽度 = gFBf(fb, 2);
	cs.高度 = gFBf(fb, 3);
	cs.可视 = gFBb(fb, 4);
	cs.禁止 = gFBb(fb, 5);
	cs.可停留焦点 = gFBb(fb, 6);
	cs.停留顺序 = gFBi(fb, 7);
	cs.方向 = gFBi(fb, 8);
	cs.刻度类型 = gFBi(fb, 9);
	cs.单位刻度值 = gFBi(fb, 10);
	cs.允许选择 = gFBb(fb, 11);
	cs.首选择位置 = gFBi(fb, 12);
	cs.选择长度 = gFBi(fb, 13);
	cs.页改变值 = gFBi(fb, 14);
	cs.行改变值 = gFBi(fb, 15);
	cs.最小位置 = gFBi(fb, 16);
	cs.最大位置 = gFBi(fb, 17);
	cs.位置 = gFBi(fb, 18);
	滑块条* ui = new 滑块条;
	ui->创建(cs, 父窗口);
	return ui;
}

滑块条::参数 滑块条::_df;
static UI属性元信息 META_滑块条[] = {
	//int 索引,LPWSTR 名称;DWORD 类型;union 默认值;LPWSTR 枚举值;LPWSTR 说明;BOOL 可空;BOOL 子项;
	_MK属性(0, L"左边", UI属性类型::数值, 滑块条::_df.左边, NULL, L"", false, false),
	_MK属性(1, L"顶边", UI属性类型::数值, 滑块条::_df.顶边, NULL, L"", false, false),
	_MK属性(2, L"宽度", UI属性类型::数值, 滑块条::_df.宽度, NULL, L"", false, false),
	_MK属性(3, L"高度", UI属性类型::数值, 滑块条::_df.高度, NULL, L"", false, false),
	_MK属性(4, L"可视", UI属性类型::布尔, 滑块条::_df.可视, NULL, L"", false, false),
	_MK属性(5, L"禁止", UI属性类型::布尔, 滑块条::_df.禁止, NULL, L"", false, false),
	_MK属性(6, L"可停留焦点", UI属性类型::布尔, 滑块条::_df.可停留焦点, NULL, L"", false, false),
	_MK属性(7, L"停留顺序", UI属性类型::整数, 滑块条::_df.停留顺序, NULL, L"", false, false),
	_MK属性(8, L"方向", UI属性类型::枚举, (int)滑块条::_df.方向, L"TBS_HORZ.横向|TBS_VERT.纵向", L"", false, false),
	_MK属性(9, L"刻度类型", UI属性类型::枚举, (int)滑块条::_df.刻度类型, L"TBS_BOTTOM.下_右|TBS_TOP.上_左|TBS_BOTH.两端|TBS_NOTICKS.无刻度", L"", false, false),
	_MK属性(10, L"单位刻度值", UI属性类型::整数, 滑块条::_df.单位刻度值, NULL, L"", false, false),
	_MK属性(11, L"允许选择", UI属性类型::布尔, 滑块条::_df.允许选择, NULL, L"", false, false),
	_MK属性(12, L"首选择位置", UI属性类型::整数, 滑块条::_df.首选择位置, NULL, L"", false, false),
	_MK属性(13, L"选择长度", UI属性类型::整数, 滑块条::_df.选择长度, NULL, L"", false, false),
	_MK属性(14, L"页改变值", UI属性类型::整数, 滑块条::_df.页改变值, NULL, L"", false, false),
	_MK属性(15, L"行改变值", UI属性类型::整数, 滑块条::_df.行改变值, NULL, L"", false, false),
	_MK属性(16, L"最小位置", UI属性类型::整数, 滑块条::_df.最小位置, NULL, L"", false, false),
	_MK属性(17, L"最大位置", UI属性类型::整数, 滑块条::_df.最大位置, NULL, L"", false, false),
	_MK属性(18, L"位置", UI属性类型::整数, 滑块条::_df.位置, NULL, L"", false, false),
};

void UI_滑块条_置属性(滑块条* ui, int methodi, byte* fb)
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
		ui->刻度类型_(gFBi(fb, 0));
		break;
	case 10:
		ui->单位刻度值_(gFBi(fb, 0));
		break;
	case 11:
		ui->允许选择_(gFBb(fb, 0));
		break;
	case 12:
		ui->首选择位置_(gFBi(fb, 0));
		break;
	case 13:
		ui->选择长度_(gFBi(fb, 0));
		break;
	case 14:
		ui->页改变值_(gFBi(fb, 0));
		break;
	case 15:
		ui->行改变值_(gFBi(fb, 0));
		break;
	case 16:
		ui->最小位置_(gFBi(fb, 0));
		break;
	case 17:
		ui->最大位置_(gFBi(fb, 0));
		break;
	case 18:
		ui->位置_(gFBi(fb, 0));
		break;
	}
}
