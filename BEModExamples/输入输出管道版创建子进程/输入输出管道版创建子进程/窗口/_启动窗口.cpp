#include <BEMod.h>
#include "子窗口.h"

struct __启动窗口 : 窗口
{
	void _打开计算窗口按钮_被单击()
	{
		// 由于通过载入(this, false)建立了父子属主关系，当主窗口关闭时，Windows内部会顺带销毁子窗口。
		// 销毁时子窗口关闭 stdin 写端，促使子进程因接收到 EOF 正常退出。
		// 管道等待循环检测到进程退出后跳出，最终让整个主程序安全、无残留的退出。
		子窗口.载入(this, false);
	}

#pragma region 组件成员
	struct : 按钮 {
		void 事件_被单击() {
			_启动窗口._打开计算窗口按钮_被单击();
		}
	} 打开计算窗口按钮;

	编辑框 结果编辑框;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
} _启动窗口;