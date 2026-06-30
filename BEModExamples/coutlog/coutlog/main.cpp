#include "stdafx.h"

void 简单测试()
{
	coutlog("hello world"); //这个就是默认10086
	coutlog_option("127.0.0.1:10087"); //设置后本线程内都生效
	coutClear();

	StrA a = coutlog("Hello from BE coutlog!", 1);
	信息框(a);

	coutlog_optionT("127.0.0.1:10088"); //临时设置一次（仅对下次cout有效）
	// 测试变参 coutlogV
	coutlogV("Variadic Test", 101, 3.14, StrW(L"宽字符测试"));

	coutlog("再输出时已经变回10087");

	printf("Done. Check your log receiver!\n");
}

void 多线程测试()
{
	创建线程([](LPVOID lpThreadParameter)->DWORD {
		coutlog_option("127.0.0.1:10086", false);
		for (int i = 0; i < 10; i++)
		{
			coutlog("哈哈10086");
		}
		return 0;
	});

	创建线程([](LPVOID lpThreadParameter)->DWORD {
		coutlog_option("127.0.0.1:10087", false);
		for (int i = 0; i < 10; i++)
		{
			coutlog("哈哈10087");
		}
		return 0;
	});

	创建线程([](LPVOID lpThreadParameter)->DWORD {
		coutlog_option("127.0.0.1:10088", false);
		for (int i = 0; i < 10; i++)
		{
			coutlog("哈哈10088");
		}
		return 0;
	});
}

void main()
{
	简单测试();
	//多线程测试();

	getchar();
}