#include "stdafx.h"
#include "文件拖放.h"

void CALLBACK MyDragCallback(const wchar_t* files) {
    printf("收到拖放文件:\n%ls\n", files);
}

void main()
{
    // 获取控制台窗口句柄进行测试 (注意：控制台拖放可能受限，通常用于 UI 窗口)
	HWND hwnd = GetConsoleWindow();
	if (文件拖放_注册(hwnd, MyDragCallback)) {
        printf("注册拖放成功，请尝试拖入文件...\n");
        // 进入简单的消息循环以维持运行
        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    } else {
        printf("注册拖放失败。\n");
	}
}