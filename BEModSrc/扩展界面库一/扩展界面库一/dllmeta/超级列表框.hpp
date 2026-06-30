#ifndef _BEWin32UIProj
#include <BEWin32UI/VSDllSDK.hpp>
#else
#include "../../VSDllSDK.hpp"
#endif
#include "../超级列表框.h"

超级列表框* UI_超级列表框_创建(byte* fb, 容器类* 父窗口)
{
	if (fb==nullptr){
		delete 父窗口; //当参数1给出空指针时此方法变为销毁此投入的参数2实例对象
		return 0;
	}
	超级列表框::参数 cs;
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
	cs.文本背景色 = gFBi(fb, 10);
	cs.背景颜色 = gFBi(fb, 11);
	cs.字体 = gFBpo<字体>(fb, 12);
	cs.类型 = gFBi(fb, 13);
	cs.图标对齐方式 = gFBi(fb, 14);
	cs.自动排列图标 = gFBb(fb, 15);
	cs.标题自动换行 = gFBb(fb, 16);
	cs.无表头 = gFBb(fb, 17);
	cs.表头可单击 = gFBb(fb, 18);
	cs.整行选择 = gFBb(fb, 19);
	cs.显示表格线 = gFBb(fb, 20);
	cs.表列可拖动 = gFBb(fb, 21);
	cs.热点跟踪 = gFBb(fb, 22);
	cs.手形指针 = gFBb(fb, 23);
	cs.标注非热点 = gFBb(fb, 24);
	cs.标注热点 = gFBb(fb, 25);
	cs.允许编辑 = gFBb(fb, 26);
	cs.排序方式 = gFBi(fb, 27);
	cs.是否有检查框 = gFBb(fb, 28);
	cs.平面滚动条 = gFBb(fb, 29);
	cs.单一选择 = gFBb(fb, 30);
	cs.始终显示选择项 = gFBb(fb, 31);
	cs.现行选中项 = gFBi(fb, 32);
	cs.报表列 = gFBpo<Bytes>(fb, 33);
	cs.表项 = gFBpo<Bytes>(fb, 34);
	超级列表框* ui = new 超级列表框;
	ui->创建(cs, 父窗口);
	return ui;
}

超级列表框::参数 超级列表框::_df;
static UI属性元信息 META_超级列表框[] = {
	//int 索引,LPWSTR 名称;DWORD 类型;union 默认值;LPWSTR 枚举值;LPWSTR 说明;BOOL 可空;BOOL 子项;
	_MK属性(0, L"左边", UI属性类型::整数, 超级列表框::_df.左边, NULL, L"", false, false),
	_MK属性(1, L"顶边", UI属性类型::整数, 超级列表框::_df.顶边, NULL, L"", false, false),
	_MK属性(2, L"宽度", UI属性类型::整数, 超级列表框::_df.宽度, NULL, L"", false, false),
	_MK属性(3, L"高度", UI属性类型::整数, 超级列表框::_df.高度, NULL, L"", false, false),
	_MK属性(4, L"可视", UI属性类型::布尔, 超级列表框::_df.可视, NULL, L"", false, false),
	_MK属性(5, L"禁止", UI属性类型::布尔, 超级列表框::_df.禁止, NULL, L"", false, false),
	_MK属性(6, L"可停留焦点", UI属性类型::布尔, 超级列表框::_df.可停留焦点, NULL, L"", false, false),
	_MK属性(7, L"停留顺序", UI属性类型::整数, 超级列表框::_df.停留顺序, NULL, L"", false, false),
	_MK属性(8, L"边框", UI属性类型::枚举, (int)超级列表框::_df.边框, L"0.无边框|1.凹入式|2.凸出式|3.浅凹入式|4.镜框式|5.单线边框式", L"", false, false),
	_MK属性(9, L"文本颜色", UI属性类型::颜色, (int)超级列表框::_df.文本颜色, NULL, L"", false, false),
	_MK属性(10, L"文本背景色", UI属性类型::颜色, (int)超级列表框::_df.文本背景色, NULL, L"", false, false),
	_MK属性(11, L"背景颜色", UI属性类型::颜色, (int)超级列表框::_df.背景颜色, NULL, L"", false, false),
	_MK属性(12, L"字体", UI属性类型::字体, &超级列表框::_df.字体, NULL, L"", false, false),
	_MK属性(13, L"类型", UI属性类型::枚举, (int)超级列表框::_df.类型, L"0.大图标列表框|1.小图标列表框|2.普通列表框|3.报表列表框", L"", false, false),
	_MK属性(14, L"图标对齐方式", UI属性类型::枚举, (int)超级列表框::_df.图标对齐方式, L"0.顶部对齐|1.左边对齐", L"", false, false),
	_MK属性(15, L"自动排列图标", UI属性类型::布尔, 超级列表框::_df.自动排列图标, NULL, L"", false, false),
	_MK属性(16, L"标题自动换行", UI属性类型::布尔, 超级列表框::_df.标题自动换行, NULL, L"", false, false),
	_MK属性(17, L"无表头", UI属性类型::布尔, 超级列表框::_df.无表头, NULL, L"", false, false),
	_MK属性(18, L"表头可单击", UI属性类型::布尔, 超级列表框::_df.表头可单击, NULL, L"", false, false),
	_MK属性(19, L"整行选择", UI属性类型::布尔, 超级列表框::_df.整行选择, NULL, L"", false, false),
	_MK属性(20, L"显示表格线", UI属性类型::布尔, 超级列表框::_df.显示表格线, NULL, L"", false, false),
	_MK属性(21, L"表列可拖动", UI属性类型::布尔, 超级列表框::_df.表列可拖动, NULL, L"", false, false),
	_MK属性(22, L"热点跟踪", UI属性类型::布尔, 超级列表框::_df.热点跟踪, NULL, L"", false, false),
	_MK属性(23, L"手形指针", UI属性类型::布尔, 超级列表框::_df.手形指针, NULL, L"", false, false),
	_MK属性(24, L"标注非热点", UI属性类型::布尔, 超级列表框::_df.标注非热点, NULL, L"", false, false),
	_MK属性(25, L"标注热点", UI属性类型::布尔, 超级列表框::_df.标注热点, NULL, L"", false, false),
	_MK属性(26, L"允许编辑", UI属性类型::布尔, 超级列表框::_df.允许编辑, NULL, L"", false, false),
	_MK属性(27, L"排序方式", UI属性类型::枚举, (int)超级列表框::_df.排序方式, L"0.不排序|1.正向排序|2.逆向排序", L"", false, false),
	_MK属性(28, L"是否有检查框", UI属性类型::布尔, 超级列表框::_df.是否有检查框, NULL, L"", false, false),
	_MK属性(29, L"平面滚动条", UI属性类型::布尔, 超级列表框::_df.平面滚动条, NULL, L"", false, false),
	_MK属性(30, L"单一选择", UI属性类型::布尔, 超级列表框::_df.单一选择, NULL, L"", false, false),
	_MK属性(31, L"始终显示选择项", UI属性类型::布尔, 超级列表框::_df.始终显示选择项, NULL, L"", false, false),
	_MK属性(32, L"现行选中项", UI属性类型::整数, 超级列表框::_df.现行选中项, NULL, L"", false, false),
	_MK属性(33, L"报表列", UI属性类型::数据, &超级列表框::_df.报表列, NULL, L"", false, false),
	_MK属性(34, L"表项", UI属性类型::数据, &超级列表框::_df.表项, NULL, L"", false, false),
};

void UI_超级列表框_置属性(超级列表框* ui, int methodi, byte* fb)
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
		ui->文本背景色_(gFBi(fb, 0));
		break;
	case 11:
		ui->背景颜色_(gFBi(fb, 0));
		break;
	case 12:
		//字体
		break;
	case 13:
		ui->类型_(gFBi(fb, 0));
		break;
	case 14:
		ui->图标对齐方式_(gFBi(fb, 0));
		break;
	case 15:
		ui->自动排列图标_(gFBb(fb, 0));
		break;
	case 16:
		ui->标题自动换行_(gFBb(fb, 0));
		break;
	case 17:
		ui->无表头_(gFBb(fb, 0));
		break;
	case 18:
		ui->表头可单击_(gFBb(fb, 0));
		break;
	case 19:
		ui->整行选择_(gFBb(fb, 0));
		break;
	case 20:
		ui->显示表格线_(gFBb(fb, 0));
		break;
	case 21:
		ui->表列可拖动_(gFBb(fb, 0));
		break;
	case 22:
		ui->热点跟踪_(gFBb(fb, 0));
		break;
	case 23:
		ui->手形指针_(gFBb(fb, 0));
		break;
	case 24:
		ui->标注非热点_(gFBb(fb, 0));
		break;
	case 25:
		ui->标注热点_(gFBb(fb, 0));
		break;
	case 26:
		ui->允许编辑_(gFBb(fb, 0));
		break;
	case 27:
		ui->排序方式_(gFBi(fb, 0));
		break;
	case 28:
		ui->是否有检查框_(gFBb(fb, 0));
		break;
	case 29:
		ui->平面滚动条_(gFBb(fb, 0));
		break;
	case 30:
		ui->单一选择_(gFBb(fb, 0));
		break;
	case 31:
		ui->始终显示选择项_(gFBb(fb, 0));
		break;
	case 32:
		ui->现行选中项_(gFBi(fb, 0));
		break;
	case 33:
		//报表列
		break;
	case 34:
		//表项
		break;
	}
}
