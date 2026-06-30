#ifndef _BEWin32UIProj
#include <BEWin32UI/VSDllSDK.hpp>
#else
#include "../../VSDllSDK.hpp"
#endif
#include "../编辑框.h"

编辑框* UI_编辑框_创建(byte* fb, 容器类* 父窗口)
{
	if (fb==nullptr){
		delete 父窗口; //当参数1给出空指针时此方法变为销毁此投入的参数2实例对象
		return 0;
	}
	编辑框::参数 cs;
	cs.左边 = gFBf(fb, 0);
	cs.顶边 = gFBf(fb, 1);
	cs.宽度 = gFBf(fb, 2);
	cs.高度 = gFBf(fb, 3);
	cs.可视 = gFBb(fb, 4);
	cs.禁止 = gFBb(fb, 5);
	cs.可停留焦点 = gFBb(fb, 6);
	cs.停留顺序 = gFBi(fb, 7);
	cs.内容 = gFBs<W>(fb, 8);
	cs.边框 = gFBi(fb, 9);
	cs.字体 = gFBpo<字体>(fb, 10);
	cs.文本颜色 = gFBi(fb, 11);
	cs.背景颜色 = gFBi(fb, 12);
	cs.隐藏选择 = gFBb(fb, 13);
	cs.最大允许长度 = gFBi(fb, 14);
	cs.是否允许多行 = gFBb(fb, 15);
	cs.滚动条 = gFBi(fb, 16);
	cs.对齐方式 = gFBi(fb, 17);
	cs.输入方式 = gFBi(fb, 18);
	cs.密码遮盖字符 = gFBs<W>(fb, 19);
	cs.转换方式 = gFBi(fb, 20);
	cs.调节器方式 = gFBi(fb, 21);
	cs.调节器底限值 = gFBi(fb, 22);
	cs.调节器上限值 = gFBi(fb, 23);
	编辑框* ui = new 编辑框;
	ui->创建(cs, 父窗口);
	return ui;
}

编辑框::参数 编辑框::_df;
static UI属性元信息 META_编辑框[] = {
	//int 索引,LPWSTR 名称;DWORD 类型;union 默认值;LPWSTR 枚举值;LPWSTR 说明;BOOL 可空;BOOL 子项;
	_MK属性(0, L"左边", UI属性类型::数值, 编辑框::_df.左边, NULL, L"", false, false),
	_MK属性(1, L"顶边", UI属性类型::数值, 编辑框::_df.顶边, NULL, L"", false, false),
	_MK属性(2, L"宽度", UI属性类型::数值, 编辑框::_df.宽度, NULL, L"", false, false),
	_MK属性(3, L"高度", UI属性类型::数值, 编辑框::_df.高度, NULL, L"", false, false),
	_MK属性(4, L"可视", UI属性类型::布尔, 编辑框::_df.可视, NULL, L"", false, false),
	_MK属性(5, L"禁止", UI属性类型::布尔, 编辑框::_df.禁止, NULL, L"", false, false),
	_MK属性(6, L"可停留焦点", UI属性类型::布尔, 编辑框::_df.可停留焦点, NULL, L"", false, false),
	_MK属性(7, L"停留顺序", UI属性类型::整数, 编辑框::_df.停留顺序, NULL, L"", false, false),
	_MK属性(8, L"内容", UI属性类型::文本, 编辑框::_df.内容, NULL, L"", false, false),
	_MK属性(9, L"边框", UI属性类型::枚举, (int)编辑框::_df.边框, L"0.无边框|1.凹入式|2.凸出式|3.浅凹入式|4.镜框式|5.单线边框式", L"", false, false),
	_MK属性(10, L"字体", UI属性类型::字体, &编辑框::_df.字体, NULL, L"", false, false),
	_MK属性(11, L"文本颜色", UI属性类型::颜色, (int)编辑框::_df.文本颜色, NULL, L"", false, false),
	_MK属性(12, L"背景颜色", UI属性类型::颜色, (int)编辑框::_df.背景颜色, NULL, L"", false, false),
	_MK属性(13, L"隐藏选择", UI属性类型::布尔, 编辑框::_df.隐藏选择, NULL, L"", false, false),
	_MK属性(14, L"最大允许长度", UI属性类型::整数, 编辑框::_df.最大允许长度, NULL, L"", false, false),
	_MK属性(15, L"是否允许多行", UI属性类型::布尔, 编辑框::_df.是否允许多行, NULL, L"", false, false),
	_MK属性(16, L"滚动条", UI属性类型::枚举, (int)编辑框::_df.滚动条, L"0.无|1.横向滚动条|2.纵向滚动条|3.横向及纵向滚动条", L"", false, false),
	_MK属性(17, L"对齐方式", UI属性类型::枚举, (int)编辑框::_df.对齐方式, L"0.左对齐|1.居中|2.右对齐", L"", false, false),
	_MK属性(18, L"输入方式", UI属性类型::枚举, (int)编辑框::_df.输入方式, L"0.通常方式|1.只读方式|2.密码输入|3.整数文本输入|4.小数文本输入|5.输入字节|6.输入短整数|7.输入整数|8.输入长整数|9.输入小数|10.输入双精度小数|11.输入日期时间", L"", false, false),
	_MK属性(19, L"密码遮盖字符", UI属性类型::文本, 编辑框::_df.密码遮盖字符, NULL, L"", false, false),
	_MK属性(20, L"转换方式", UI属性类型::枚举, (int)编辑框::_df.转换方式, L"0.无|1.大写到小写|2.小写到大写", L"", false, false),
	_MK属性(21, L"调节器方式", UI属性类型::枚举, (int)编辑框::_df.调节器方式, L"0.无调节器|1.自动调节器|2.手动调节器", L"", false, false),
	_MK属性(22, L"调节器底限值", UI属性类型::整数, 编辑框::_df.调节器底限值, NULL, L"", false, false),
	_MK属性(23, L"调节器上限值", UI属性类型::整数, 编辑框::_df.调节器上限值, NULL, L"", false, false),
};

void UI_编辑框_置属性(编辑框* ui, int methodi, byte* fb)
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
		ui->内容_(gFBs<W>(fb, 0));
		break;
	case 9:
		ui->边框_(gFBi(fb, 0));
		break;
	case 10:
		//字体
		break;
	case 11:
		ui->文本颜色_(gFBi(fb, 0));
		break;
	case 12:
		ui->背景颜色_(gFBi(fb, 0));
		break;
	case 13:
		ui->隐藏选择_(gFBb(fb, 0));
		break;
	case 14:
		ui->最大允许长度_(gFBi(fb, 0));
		break;
	case 15:
		ui->是否允许多行_(gFBb(fb, 0));
		break;
	case 16:
		ui->滚动条_(gFBi(fb, 0));
		break;
	case 17:
		ui->对齐方式_(gFBi(fb, 0));
		break;
	case 18:
		ui->输入方式_(gFBi(fb, 0));
		break;
	case 19:
		ui->密码遮盖字符_(gFBs<W>(fb, 0));
		break;
	case 20:
		ui->转换方式_(gFBi(fb, 0));
		break;
	case 21:
		ui->调节器方式_(gFBi(fb, 0));
		break;
	case 22:
		ui->调节器底限值_(gFBi(fb, 0));
		break;
	case 23:
		ui->调节器上限值_(gFBi(fb, 0));
		break;
	}
}
