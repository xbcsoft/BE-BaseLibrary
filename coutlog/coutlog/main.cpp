#include "stdafx.h"
#include "coutlog.h"

void main()
{
	coutlog_option("127.0.0.1:10086");
	coutClear();

	StrA a = coutlog("Hello from BE coutlog!", 1);
	信息框(a);
	
	// 测试变参 coutlogV
	coutlogV("Variadic Test", 101, 3.14, StrW(L"宽字符测试"));
	
	printf("Done. Check your log receiver!\n");
}