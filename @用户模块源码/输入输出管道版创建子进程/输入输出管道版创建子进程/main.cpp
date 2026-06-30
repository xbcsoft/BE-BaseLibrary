#include "stdafx.h"
#include "输入输出管道版创建子进程.hpp"

// 实时输出回调函数
void CallbackOutput(const StrA& text) {
	// StrA 可以隐式转换为 const char*，但在 printf 中建议强转为 (char*) 以避开变参陷阱
	printf("[Realtime]: %s", (char*)text);
}

// 进程创建成功后的回调
void OnProcessCreated(PROCESS_INFORMATION& pi, HANDLE hStdinWrite) {
	printf("[Created Callback]: PID = %d\n", pi.dwProcessId);
}

int main() {
	printf("=== Subprocess Overlapped Pipe Test ===\n");

	StrA accumulatedOutput;

	// 调用创建子进程，在模板参数中指定不开启处理消息循环 (false)
	bool ok = 创建子进程_读写管道<false>(
		"sub.exe",          // 可执行文件
		nil,                // 命令行参数
		nil,                // 工作目录
		accumulatedOutput,  // 用于累加最终输出
		OnProcessCreated,   // 进程创建完毕回调
		CallbackOutput      // 实时输出回调
	);

	if (ok) {
		printf("\n=== Process Finished ===\n");
		printf("Final Accumulated Output:\n%s\n", (char*)accumulatedOutput);
	} else {
		printf("\nFailed to run subprocess.\n");
	}

	return 0;
}