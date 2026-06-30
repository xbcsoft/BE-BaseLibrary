#include "stdafx.h"

void main()
{
	be::print(读注册表文本(LR"(HKEY_CLASSES_ROOT\.jpg\@)"));
	be::print(读注册表字节集(LR"(HKEY_CLASSES_ROOT\comfile\EditFlags)"));

	注册表 a;
	a.打开项(HKEY_CURRENT_USER, LR"(Software\Microsoft\Windows)");
	a.枚举子项([](StrW& x) {
		be::print(x);
		return true;
	});

	//如需在32位程序上操作64位的注册表需要禁用Wow64重定向
	Wow64重定向_禁用();
	//...操作注册表
	Wow64重定向_启用();
}