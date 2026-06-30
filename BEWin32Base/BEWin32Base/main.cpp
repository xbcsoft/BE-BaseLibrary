#include "stdafx.h"
#include "BEWin32Base.h" //由MakeBEC.exe自动生成
//此main.cpp仅用作模块测试不参与头文件的解析生成
//若想排除特定cpp文件的解析可在工程属性页-生成事件
//改写MakeBEC.exe参数-excludesrc=多文件[需加空格]

void 测试函数(int a, StrA s) {
	be::print("测试函数(带参数)执行: a =", a, "s =", s);
}

void main()
{
	//创建进程("cmd.exe", "/k echo 66", R"(D:\Develop\sdk\qt\5.14.2\msvc2017\bin)");

	return;

	//be::print(取特定目录(目录::系统桌面));
	StrA iniPath = "test_config.ini";
	// 写入几个配置项，分别测试直接投入 StrA 和 StrW
	StrA key1 = "key1_StrA";
	StrW val1 = L"val1_StrW";
	写配置项(iniPath, L"Section1", key1, val1);

	StrA key2 = "key2_StrA";
	StrA val2 = "val2_StrA";
	写配置项(iniPath, L"Section2", key2, val2);

	StrW key3 = L"key3_StrW";
	StrW val3 = L"val3_StrW";
	写配置项(iniPath, L"Section3", key3, val3);

	// 获取配置节名 (StrA版本)
	Arraybe<StrA> sections = 取配置节名(iniPath);
	be::print("--- 取配置节名 (StrA版) 测试 ---");
	be::print("配置节数量:", sections.count);
	for (auto& sec : sections) {
		be::print("节名 (StrA):", (const char*)sec);
		// 获取并遍历该节下的所有配置项名
		Arraybe<StrA> keys = 取配置项名(iniPath, sec);
		for (auto& key : keys) {
			be::print("  - 配置项名 (StrA):", (const char*)key);
		}
	}

	// 清理测试生成的文件
	删除文件(iniPath);

	return;

	be::print("--- 推迟调用子程序 测试开始 ---");

	// 1. 普通推迟调用 (2秒后)
	推迟调用子程序(2000, [](int x) {
		be::print("普通推迟(2s) Lambda 执行, 收到参数:", x);
	}, 空, 1001);


	// 2. 互斥 ID 测试 (3秒后)
	be::print("发起互斥测试...");
	int 互斥ID = 推迟调用子程序<true>(1000, 测试函数, 空, 456, "第一次调用(应成功)");
	be::print("得到系统返回 ID:", 互斥ID);
	
	// 再次用相同 ID 调用互斥，由于该 ID 已经在等待中，本次应被忽略
	推迟调用子程序<true>(1000, 测试函数, 互斥ID, 789, "第二次调用(应被互斥忽略)");


	// 3. 重置/重载测试 (刷新时间)
	be::print("发起重置测试 (即时打断并刷新时间)...");
	int 重置ID = 推迟调用子程序(1000, [](int x) {
		be::print("重置测试失败：你不应该看到参数为 1000 的这条输出！");
	}, 空, 1000);

	// 接着立即重置该 ID，将 1000ms 的任务改为 2000ms 后的新任务
	// 注意：由于系统特性，重置后的 ID 会发生变化
	int 新ID = 推迟调用子程序<false>(2000, [](int x) {
		be::print("重置测试成功！1s 的任务已被打断，现在是 2s 后执行，收到参数:", x);
	}, 重置ID, 2000);
	be::print("旧 ID:", 重置ID, "-> 刷新后的新 ID:", 新ID);

	be::print("消息循环运行中，请等待定时器触发... (按下 Ctrl+C 退出)");

	// Win32 定时器需要消息循环才能从队列中取出并分发 WM_TIMER 消息
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}