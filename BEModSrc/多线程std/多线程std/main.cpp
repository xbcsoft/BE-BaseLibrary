#include "stdafx.h"
#include "多线程std.h" //由MakeBEC.exe自动生成
//此main.cpp仅用作模块测试不参与头文件的解析生成
//若想排除特定cpp文件的解析可在工程属性页-生成事件
//改写MakeBEC.exe参数-excludesrc=多文件[需加空格]

void main()
{
	互斥锁 lock;
	int count = 0;

	线程 t1([&]() {
		for (int i = 0; i < 10000; i++) {
			自动套锁<> guard(lock);
			count++;
		}
	});

	线程 t2([&]() {
		for (int i = 0; i < 10000; i++) {
			自动套锁<> guard(lock);
			count++;
		}
	});

	t1.等待();
	t2.等待();

	printf("count = %d\n", count);

	线程::启动([]() {
		printf("这是一个分离运行的线程\n");
	});
}
