#ifndef _BEWin32UIProj
#include <BEWin32UI/VSDllSDK.hpp>
#else
#include "../../VSDllSDK.hpp"
#endif
#include "../树型框.h"

树型框* UI_树型框_创建(byte* fb, 容器类* 父窗口)
{
	if (fb==nullptr){
		delete 父窗口; //当参数1给出空指针时此方法变为销毁此投入的参数2实例对象
		return 0;
	}
	树型框::参数 cs;
	cs.左边 = gFBi(fb, 0);
	cs.顶边 = gFBi(fb, 1);
	cs.宽度 = gFBi(fb, 2);
	cs.高度 = gFBi(fb, 3);
	cs.可视 = gFBb(fb, 4);
	cs.禁止 = gFBb(fb, 5);
	cs.可停留焦点 = gFBb(fb, 6);
	cs.停留顺序 = gFBi(fb, 7);
	cs.边框 = gFBi(fb, 8);
	cs.文本颜色 = gFBi(fb, 9);
	cs.背景颜色 = gFBi(fb, 10);
	cs.显示加减框 = gFBb(fb, 11);
	cs.显示连线 = gFBb(fb, 12);
	cs.显示根部线 = gFBb(fb, 13);
	cs.允许编辑 = gFBb(fb, 14);
	cs.始终显示选择项 = gFBb(fb, 15);
	cs.现行选中项 = gFBi(fb, 16);
	cs.行高 = gFBi(fb, 17);
	cs.字体 = gFBpo<字体>(fb, 18);
	cs.图片组 = gFBpo<Bytes>(fb, 19);
	cs.项目 = gFBpo<Bytes>(fb, 20);
	cs.是否有检查框 = gFBb(fb, 21);
	cs.检查框图片组 = gFBpo<Bytes>(fb, 22);
	cs.检查框状态数 = gFBi(fb, 23);
	树型框* ui = new 树型框;
	ui->创建(cs, 父窗口);
	return ui;
}

树型框::参数 树型框::_df;
static UI属性元信息 META_树型框[] = {
	//int 索引,LPWSTR 名称;DWORD 类型;union 默认值;LPWSTR 枚举值;LPWSTR 说明;BOOL 可空;BOOL 子项;
	_MK属性(0, L"左边", UI属性类型::整数, 树型框::_df.左边, NULL, L"", false, false),
	_MK属性(1, L"顶边", UI属性类型::整数, 树型框::_df.顶边, NULL, L"", false, false),
	_MK属性(2, L"宽度", UI属性类型::整数, 树型框::_df.宽度, NULL, L"", false, false),
	_MK属性(3, L"高度", UI属性类型::整数, 树型框::_df.高度, NULL, L"", false, false),
	_MK属性(4, L"可视", UI属性类型::布尔, 树型框::_df.可视, NULL, L"", false, false),
	_MK属性(5, L"禁止", UI属性类型::布尔, 树型框::_df.禁止, NULL, L"", false, false),
	_MK属性(6, L"可停留焦点", UI属性类型::布尔, 树型框::_df.可停留焦点, NULL, L"", false, false),
	_MK属性(7, L"停留顺序", UI属性类型::整数, 树型框::_df.停留顺序, NULL, L"", false, false),
	_MK属性(8, L"边框", UI属性类型::枚举, (int)树型框::_df.边框, L"0.无边框|1.凹入式|2.凸出式|3.浅凹入式|4.镜框式|5.单线边框式", L"", false, false),
	_MK属性(9, L"文本颜色", UI属性类型::颜色, (int)树型框::_df.文本颜色, NULL, L"", false, false),
	_MK属性(10, L"背景颜色", UI属性类型::颜色, (int)树型框::_df.背景颜色, NULL, L"", false, false),
	_MK属性(11, L"显示加减框", UI属性类型::布尔, 树型框::_df.显示加减框, NULL, L"", false, false),
	_MK属性(12, L"显示连线", UI属性类型::布尔, 树型框::_df.显示连线, NULL, L"", false, false),
	_MK属性(13, L"显示根部线", UI属性类型::布尔, 树型框::_df.显示根部线, NULL, L"", false, false),
	_MK属性(14, L"允许编辑", UI属性类型::布尔, 树型框::_df.允许编辑, NULL, L"", false, false),
	_MK属性(15, L"始终显示选择项", UI属性类型::布尔, 树型框::_df.始终显示选择项, NULL, L"", false, false),
	_MK属性(16, L"现行选中项", UI属性类型::整数, 树型框::_df.现行选中项, NULL, L"", false, false),
	_MK属性(17, L"行高", UI属性类型::整数, 树型框::_df.行高, NULL, L"", false, false),
	_MK属性(18, L"字体", UI属性类型::字体, &树型框::_df.字体, NULL, L"", false, false),
	_MK属性(19, L"图片组", UI属性类型::数据, &树型框::_df.图片组, NULL, L"", false, false),
	_MK属性(20, L"项目", UI属性类型::数据, &树型框::_df.项目, NULL, L"", false, false),
	_MK属性(21, L"是否有检查框", UI属性类型::布尔, 树型框::_df.是否有检查框, NULL, L"", false, false),
	_MK属性(22, L"检查框图片组", UI属性类型::数据, &树型框::_df.检查框图片组, NULL, L"", false, false),
	_MK属性(23, L"检查框状态数", UI属性类型::整数, 树型框::_df.检查框状态数, NULL, L"", false, false),
};

void UI_树型框_置属性(树型框* ui, int methodi, byte* fb)
{
	switch (methodi)
	{
	case 0:
		ui->移动(gFBi(fb, 0), 空, 空, 空);
		break;
	case 1:
		ui->移动(空, gFBi(fb, 0), 空, 空);
		break;
	case 2:
		ui->移动(空, 空, gFBi(fb, 0), 空);
		break;
	case 3:
		ui->移动(空, 空, 空, gFBi(fb, 0));
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
		ui->文本颜色_(gFBi(fb, 0));
		break;
	case 10:
		ui->背景颜色_(gFBi(fb, 0));
		break;
	case 11:
		ui->显示加减框_(gFBb(fb, 0));
		break;
	case 12:
		ui->显示连线_(gFBb(fb, 0));
		break;
	case 13:
		ui->显示根部线_(gFBb(fb, 0));
		break;
	case 14:
		ui->允许编辑_(gFBb(fb, 0));
		break;
	case 15:
		ui->始终显示选择项_(gFBb(fb, 0));
		break;
	case 16:
		ui->现行选中项_(gFBi(fb, 0));
		break;
	case 17:
		ui->行高_(gFBi(fb, 0));
		break;
	case 18:
		//字体
		break;
	case 19:
		//ui->置图片组(gFBpo<Bytes>(fb, 0));
		break;
	case 20:
		//项目
		break;
	case 21:
		ui->是否有检查框_(gFBb(fb, 0));
		break;
	case 22:
		//ui->置检查框图片组(gFBpo<Bytes>(fb, 0));
		break;
	case 23:
		ui->检查框状态数_(gFBi(fb, 0));
		break;
	}
}
