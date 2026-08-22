#include <BEMod.h>

struct _窗口02_JS异步协程 : 窗口
{
	void 事件_创建完毕()
	{
		标题_(L"SciterUI - JS 异步协程并发演示");
		// 在测试时先启动python out/server.py（每个请求协程延迟1秒）

		//默认情况下fetch使用的是wininet只能最大并发4个HTTP请求
		//但若改为下面使用Sciter内置的http客户端的话则没有连接池（每个fetch都是一个新TCP连接）
		g_sapi->SciterSetOption(NULL, SCITER_USE_INTERNAL_HTTP_CLIENT, TRUE);

		//如有高并发需求需自行封装WinHTTP或其他高性能Web组件
	}

#pragma region 组件成员
	struct _st :SciterUI {
	} st;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
} 窗口02_JS异步协程;