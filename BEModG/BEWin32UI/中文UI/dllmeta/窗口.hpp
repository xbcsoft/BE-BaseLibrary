#ifndef _BEWin32UIProj
#include <BEWin32UI/VSDllSDK.hpp>
#else
#include "../../VSDllSDK.hpp"
#endif
#include "../窗口.h"

窗口* UI_窗口_创建(byte* fb, 窗口* 父窗口)
{
	if (fb==nullptr){
		delete 父窗口; //当参数1给出空指针时此方法变为销毁此投入的参数2实例对象
		return 0;
	}
	窗口::参数 cs;
	cs.左边 = gFBf(fb, 0);
	cs.顶边 = gFBf(fb, 1);
	cs.宽度 = gFBf(fb, 2);
	cs.高度 = gFBf(fb, 3);
	cs.可视.SET(gFBb(gFB_p(fb, 4), 0), gFBb(gFB_p(fb, 4), 1));
	cs.禁止 = gFBb(fb, 5);
	cs.类名 = gFBs<W>(fb, 6);
	cs.标题 = gFBs<W>(fb, 7);
	cs.图标.SET(gFBpo<Bytes>(gFB_p(fb, 8), 0), gFBb(gFB_p(fb, 8), 1));
	cs.图标ID.SET(gFBi(gFB_p(fb, 9), 0), gFBb(gFB_p(fb, 9), 1));
	cs.总在最前 = gFBb(fb, 10);
	cs.边框 = gFBi(fb, 11);
	cs.位置.SET(gFBi(gFB_p(fb, 12), 0), gFBb(gFB_p(fb, 12), 1));
	cs.背景颜色 = gFBi(fb, 13);
	cs.控制按钮 = gFBb(fb, 14);
	cs.最小化按钮 = gFBb(fb, 15);
	cs.最大化按钮 = gFBb(fb, 16);
	cs.Esc键关闭 = gFBb(fb, 17);
	cs.随意移动 = gFBb(fb, 18);
	cs.在任务栏显示 = gFBb(fb, 19);
	cs.保持标题栏点燃 = gFBb(fb, 20);
	cs.子控件焦点导航 = gFBb(fb, 21);
	窗口* ui = new 窗口;
	ui->创建(cs, 父窗口);
	return ui;
}

窗口::参数 窗口::_df;
static UI属性元信息 META_窗口[] = {
	//int 索引,LPWSTR 名称;DWORD 类型;union 默认值;LPWSTR 枚举值;LPWSTR 说明;BOOL 可空;BOOL 子项;
	_MK属性(0, L"左边", UI属性类型::数值, 窗口::_df.左边, NULL, L"", false, false),
	_MK属性(1, L"顶边", UI属性类型::数值, 窗口::_df.顶边, NULL, L"", false, false),
	_MK属性(2, L"宽度", UI属性类型::数值, 窗口::_df.宽度, NULL, L"", false, false),
	_MK属性(3, L"高度", UI属性类型::数值, 窗口::_df.高度, NULL, L"", false, false),
	_MK属性(4, L"可视", UI属性类型::布尔, NULL, NULL, L"该参数为空时由WinMain函数的nCmdShow决定", true, false),
	_MK属性(5, L"禁止", UI属性类型::布尔, 窗口::_df.禁止, NULL, L"", false, false),
	_MK属性(6, L"类名", UI属性类型::文本, 窗口::_df.类名, NULL, L"", false, false),
	_MK属性(7, L"标题", UI属性类型::文本, 窗口::_df.标题, NULL, L"", false, false),
	_MK属性(8, L"图标", UI属性类型::数据, NULL, NULL, L"设置窗口标题栏图标，与图标ID二选一", true, false),
	_MK属性(9, L"图标ID", UI属性类型::整数, NULL, NULL, L"设置窗口标题栏图标，与图标二选一", true, false),
	_MK属性(10, L"总在最前", UI属性类型::布尔, 窗口::_df.总在最前, NULL, L"", false, false),
	_MK属性(11, L"边框", UI属性类型::枚举, (int)窗口::_df.边框, L"0.无边框|1.普通可调边框|2.普通固定边框|3.窄标题可调边框|4.窄标题固定边框", L"", false, false),
	_MK属性(12, L"位置", UI属性类型::枚举, NULL, L"0.通常|1.居中|2.最小化|4.最大化", L"", true, false),
	_MK属性(13, L"背景颜色", UI属性类型::颜色, (int)窗口::_df.背景颜色, NULL, L"", false, false),
	_MK属性(14, L"控制按钮", UI属性类型::布尔, 窗口::_df.控制按钮, NULL, L"", false, false),
	_MK属性(15, L"最小化按钮", UI属性类型::布尔, 窗口::_df.最小化按钮, NULL, L"启用控制按钮时可使用最小化按钮", false, true),
	_MK属性(16, L"最大化按钮", UI属性类型::布尔, 窗口::_df.最大化按钮, NULL, L"启用控制按钮时可使用最大化按钮", false, true),
	_MK属性(17, L"Esc键关闭", UI属性类型::布尔, 窗口::_df.Esc键关闭, NULL, L"", false, false),
	_MK属性(18, L"随意移动", UI属性类型::布尔, 窗口::_df.随意移动, NULL, L"", false, false),
	_MK属性(19, L"在任务栏显示", UI属性类型::布尔, 窗口::_df.在任务栏显示, NULL, L"", false, false),
	_MK属性(20, L"保持标题栏点燃", UI属性类型::布尔, 窗口::_df.保持标题栏点燃, NULL, L"", false, false),
	_MK属性(21, L"子控件焦点导航", UI属性类型::布尔, 窗口::_df.子控件焦点导航, NULL, L"", false, false),
};

void UI_窗口_置属性(窗口* ui, int methodi, byte* fb)
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
		//类名
		break;
	case 7:
		ui->标题_(gFBs<W>(fb, 0));
		break;
	case 8:
		ui->置图标(gFBpo<Bytes>(fb, 0));
		break;
	case 9:
		//图标ID
		break;
	case 10:
		ui->总在最前_(gFBb(fb, 0));
		break;
	case 11:
		ui->边框_(gFBi(fb, 0));
		break;
	case 12:
		ui->位置_(gFBi(fb, 0));
		break;
	case 13:
		ui->背景颜色_(gFBi(fb, 0));
		break;
	case 14:
		//控制按钮
		break;
	case 15:
		//最小化按钮
		break;
	case 16:
		//最大化按钮
		break;
	case 17:
		//Esc键关闭
		break;
	case 18:
		//随意移动
		break;
	case 19:
		ui->在任务栏显示_(gFBb(fb, 0));
		break;
	case 20:
		//保持标题栏点燃
		break;
	case 21:
		//子控件焦点导航
		break;
	}
}
