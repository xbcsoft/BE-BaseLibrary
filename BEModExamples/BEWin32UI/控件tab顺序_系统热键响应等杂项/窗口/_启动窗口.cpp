#include <BEMod.h>

struct __启动窗口 : 窗口
{
	void 事件_创建完毕()
	{
		//在窗口创建的时候已经设定cs.子控件焦点导航 = true;

		按钮1.停留顺序_ = 3;
		按钮2.停留顺序_ = 22;
		按钮3.停留顺序_ = 2;
		按钮4.停留顺序_ = 22;

		按钮5.可停留焦点(false); //手动让该控件直接不可用TAB切换

		按钮6.停留顺序_ = 6; //该控件在分组框1内，默认情况下分组框是允许TAB穿透的

		按钮7.停留顺序_ = 6; //该控件在分组框2内，现在让其TAB无法穿透
		窗口_移除扩展风格(分组框2.窗口句柄, WS_EX_CONTROLPARENT);

		/*
		Arraybe<控件类*> arr;
		arr.push(&按钮1);
		arr.push(&按钮2);
		arr.push(&按钮3);
		arr.push(&按钮4);
		调整控件组的Tab顺序(arr);
		*/
		调整控件组的Tab顺序(); //无参调用时内部会自动枚举子窗口

		this->快捷键_注册('6');
	}

	void 事件_快捷键(WORD cmdID)
	{
		BYTE 主热键;
		快捷键_ID反取(cmdID, 主热键);
		if (主热键=='6'){
			信息框(666);
		}
	}

#pragma region 组件成员
	struct :按钮 {
	}按钮1;
	struct :按钮 {
	}按钮2;
	struct :按钮 {
	}按钮3;
	struct :按钮 {
	}按钮4;
	struct :按钮 {
	}按钮5;
	struct :按钮 {
	}按钮6;
	struct _分组框1 : 分组框 {
		
	}分组框1;
	struct : 分组框 {

	}分组框2;
	struct :按钮 {
	}按钮7;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
} _启动窗口;