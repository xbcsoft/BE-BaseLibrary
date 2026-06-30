#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "stdafx.h"
#include "其他系统操作.h"
#pragma comment(lib, "ws2_32.lib")

int 套接字_检查是否可读(SOCKET sock, int 超时值);

int 套接字_检查是否可写(SOCKET sock, int 超时值);

StrA 转换为IP地址(const StrA& 欲转换主机名);


class TCP客户端
{
public:
	SOCKET sock = INVALID_SOCKET;

	TCP客户端();
	~TCP客户端();

	bool 连接(const char* 地址, int 端口, int 连接超时值 = 5000,
		bool* 失败因超时 = nullptr, bool 使用event = false);


	void 断开();

	bool 发送(const Bytes& 发送内容, int 等待时间 = 1000 * 3600,
		int* 因超时已拷贝的大小 = nullptr);

	Bytes 接收(int 超时值 = 1000 * 7200, bool* 是否超时 = nullptr);


	int 接收p(void* 接收buf, int 将接收大小, int 超时值 = 1000 * 7200);

	//由于需要依赖 void 处理事件() 此处先声明，在BEWin32UI中定义
	int 接收ep(void* 接收buf, int 将接收大小, int 超时值 = 1000 * 7200);

private:
	int _lastError = 0;
	WSAEVENT _event = WSA_INVALID_EVENT;
};

// --- 全局程序集函数 ---

int TCP_断开连接(SOCKET hsock);

int TCP_取客户端口(SOCKET 客户句柄);


//此服务器实现同步阻塞性能低下，仅用作参考及简单测试，切勿应用在生产环境
class TCP服务器
{
public:
	typedef void(*事件_客户进来)(TCP服务器& 服务器, SOCKET 客户句柄, int 客户端口);
	typedef void(*事件_客户数据到来)(TCP服务器& 服务器, SOCKET 客户句柄, c_Bytes 接收数据);
	typedef void(*事件_客户断开)(TCP服务器& 服务器, SOCKET 客户句柄, int 客户端口);

	SOCKET hsock = INVALID_SOCKET;

	TCP服务器();
	~TCP服务器();

	/**此服务器实现同步阻塞性能低下，仅用作参考及简单测试，切勿应用在生产环境
	 * @param 端口
	 * @param 客户端口=nullptr
	 * @param len=nullptr
	 * @param 客户端口=nullptr
	 * @param 是否处理HTTP帧=false
	 * @param 绑定监听网卡IP=""
	 * @param 接收缓冲区大小=20480
	 * @return
	 */
	bool 创建(int 端口,
		事件_客户进来 客户进来 = nullptr,
		事件_客户数据到来 客户数据到来 = nullptr,
		事件_客户断开 客户断开 = nullptr,
		bool 以新线程启动 = false,
		const char* 绑定监听网卡IP = "",
		int 接收缓冲区大小 = 20480);

	StrA 取客户IP(SOCKET 客户句柄);

	int 取客户端口(SOCKET 客户句柄);

	bool 发送数据(SOCKET 客户句柄, const Bytes& 发送内容);

	bool 断开客户(SOCKET 客户句柄);

	void 关闭服务();

	bool 强制断开客户(SOCKET 客户句柄);

protected:
	struct {
		SOCKET hsockServ;
		事件_客户进来 客户进来;
		事件_客户数据到来 客户数据到来;
		事件_客户断开 客户断开;
		int 接收缓冲区大小;
	} _投参 = { 0 };
	HANDLE _hThread = nullptr;

	bool _启动(int 端口, int 等待个数, const char* 绑定监听网卡IP);

	void _TCP_启动();

	//这个虚函数留给HTTP服务器时可单独处理
	virtual void _TCP_客户进入(SOCKET sClient, int 客户端口);
};
