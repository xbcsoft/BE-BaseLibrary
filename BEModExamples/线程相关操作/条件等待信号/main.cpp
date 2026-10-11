#include "stdafx.h"

互斥锁 g_lock;
条件锁 g_event;
bool g_ready = false;

void 等待事件线程()
{
	printf("等待线程：正在等待条件等待信号...\n");

	g_lock.加锁();

	g_event.等待(g_lock, []() {return g_ready; }); //等到g_ready为true时才返回
	printf("等待线程：收到条件等待信号，开始继续执行。\n");

	g_lock.解锁();
}

void 发送事件线程()
{
	//假装模拟先执行一些初始化工作
	Sleep(500);

	g_lock.加锁();
	g_ready = true; //可以处理一些互斥过程
	g_lock.解锁();

	printf("发送线程：发送条件等待信号。\n");
	g_event.唤醒一个();
}

void main()
{
	线程 t1([]() { 等待事件线程(); });
	线程 t2([]() { 发送事件线程(); });

	t1.等待();
	t2.等待();

	printf("全部执行完毕！\n");
}


//传统 C++ 条件变量（condition variable）的设计，本质上就是和互斥锁配套使用的。
//等待的核心动作不是单纯“睡眠”，而是一个关键的原子化过程：
/*
	当前线程持有互斥锁
	↓
	检查共享状态
	↓
	wait()
	↓
	释放互斥锁 + 进入等待
	↓
	其他线程取得互斥锁并修改共享状态
	↓
	notify_one / notify_all
	↓
	等待线程醒来
	↓
	重新取得互斥锁
	↓
	wait() 返回
*/
//但直到 C++20 开始有“不需要互斥锁”的等待方式
//目前C++17 推荐使用 事件信号Win32.hpp