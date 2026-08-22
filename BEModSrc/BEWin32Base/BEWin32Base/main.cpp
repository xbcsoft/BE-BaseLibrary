#include "stdafx.h"
#include "BEWin32Base.h" //由MakeBEC.exe自动生成

void main()
{
	StrA file = 文件对话框_打开(L"浏览html", L"网页文件(*.html;*.htm)|*.html;*.htm|所有文件(*.*)|*.*", 0, 取运行目录(), true);
	printf(file);
}