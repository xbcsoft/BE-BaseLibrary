#include "stdafx.h"

事件信号 g_event;

void main()
{
	线程 t([]() {
		printf("等待事件信号...\n");
		g_event.等待();
		printf("已收到事件信号！\n");
	});

	Sleep(1000);
	printf("发送事件信号。\n");
	g_event.置事件();

	t.等待();
}