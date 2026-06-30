#include "stdafx.h"
#include "信息框增强版.h" //由MakeBEC.exe自动生成
//此main.cpp仅用作模块测试不参与头文件的解析生成
//若想排除特定cpp文件的解析可在工程属性页-生成事件
//改写MakeBEC.exe参数-excludesrc=多文件[需加空格]

void main()
{
	int r = 信息框_DIY({ "支持", "中立", "反对" },
		"你支持这个 Hook 功能吗？", MB::是否取消 | MB::提示, "按钮标题自定义测试", [](HWND hwnd) {
			be::print("信息框窗口创建并激活，句柄为:", (int)hwnd);
		});
	if (r == MB::ID是) {
		printf("支持\n");
	} else if (r == MB::ID否) {
		printf("中立\n");
	} else if (r == MB::ID取消) {
		printf("反对\n");
	}

	be::print("开始测试定时信息框，等待3秒后自动关闭...");
	int r2 = 信息框_定时("这个是定时信息框，将在3秒后关闭", 3000, "定时测试", MB::确认取消 | MB::警告);
	if (r2 == 32000) {
		be::print("信息框超时关闭！");
	} else {
		be::print("用户点击了按钮，返回值:", r2);
	}
}