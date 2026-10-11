#include "stdafx.h"

互斥锁 g_lock;         // 全局令牌（临界区）
int g_dwTickes = 10;   // 总共10张票

void 卖票线程(int myThreadId)
{
	while (true)
	{
		g_lock.加锁();

		if (g_dwTickes <= 0) {
			g_lock.解锁();
			break;
		}
		printf("当前还有%d张票\n", g_dwTickes--);
		printf("线程%d, 现卖出一张, 还有%d张\n", myThreadId, g_dwTickes);

		g_lock.解锁();
		Sleep(10);
	}
}

//如果觉得上边写的繁琐还可以用下面等价写法
void 卖票线程_(int myThreadId)
{
	while (true)
	{
		{
			自动套锁<> guard(g_lock); //作用域锁

			if (g_dwTickes <= 0)
				break;

			printf("当前还有%d张票\n", g_dwTickes--);
			printf("线程%d, 现卖出一张, 还有%d张\n",
				myThreadId, g_dwTickes);
		}

		Sleep(10);
	}
}

void main()
{
	线程 t1([]() { 卖票线程(1); });
	线程 t2([]() { 卖票线程_(2); });

	t1.等待();
	t2.等待();

	printf("卖票完毕！\n");
}