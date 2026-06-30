#include <BEMod.h>
#include "_启动窗口.h"

struct _子窗口 : 窗口
{
	static void OnOutputReceived(const StrA& output) {
		_启动窗口.结果编辑框.内容_(U8toW(output));
	}

	void 事件_首次激活() {
		// 调用模板函数并使能消息循环以保持界面激活，同时传出 stdin 写端句柄以接管其生存期
		创建子进程_读写管道<true>(
			"md5app.exe",
			nil,
			nil,
			nil,
			nil,
			OnOutputReceived,
			&子窗口._hStdinWrite
		);
	}

	void 事件_被销毁() {
		// 关闭接管保持的 stdin 写句柄，触发 Go 程序接收 EOF 并退出，进而让模板函数跳出循环返回
		if (_hStdinWrite) {
			CloseHandle(_hStdinWrite);
			_hStdinWrite = NULL;
		}
	}

	void _计算按钮_被单击() {
		if (!_hStdinWrite) {
			信息框(L"子进程未运行！", L"错误");
			return;
		}
		管道_写入(_hStdinWrite, 输入编辑框.内容);
	}

#pragma region 组件成员
	编辑框 输入编辑框;

	struct : 按钮 {
		void 事件_被单击() {
			子窗口._计算按钮_被单击(); // 转发单击事件
		}
	} 计算按钮;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);

	HANDLE _hStdinWrite = NULL;
} 子窗口;
