#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "stdafx.h"
#include "其他系统操作.h"
#pragma comment(lib, "ws2_32.lib")

namespace {
timeval _生成超时(int 毫秒)
{
	if (毫秒 < 0) 毫秒 = 0;
	timeval tv;
	tv.tv_sec = 毫秒 / 1000;
	tv.tv_usec = (毫秒 % 1000) * 1000;
	return tv;
}

bool _初始化WinSock()
{
	static bool ok = []() {
		WSADATA wsaData;
		return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
	}();
	return ok;
}
}

int 套接字_检查是否可读(SOCKET sock, int 超时值)
{
	if (sock == INVALID_SOCKET) return -1;

	fd_set fs_r;
	FD_ZERO(&fs_r);
	FD_SET(sock, &fs_r);
	timeval tv = _生成超时(超时值);

	int r = select(0, &fs_r, nullptr, nullptr, &tv);
	if (r == SOCKET_ERROR) return -1;
	if (r > 0) return r;

	return 0;
}

int 套接字_检查是否可写(SOCKET sock, int 超时值)
{
	if (sock == INVALID_SOCKET) return -1;
	if (超时值 <= 0) 超时值 = 1;

	fd_set fs_w;
	FD_ZERO(&fs_w);
	FD_SET(sock, &fs_w);
	timeval tv = _生成超时(超时值);

	int r = select(0, nullptr, &fs_w, nullptr, &tv);
	if (r == SOCKET_ERROR) return -1;

	int err = 0;
	int optLen = sizeof(err);
	if (getsockopt(sock, SOL_SOCKET, SO_ERROR, (char*)&err, &optLen) == SOCKET_ERROR) return -1;
	if (r > 0) return err == 0 ? r : 0;
	return 0;
}

StrA 转换为IP地址(const StrA& 欲转换主机名)
{
	if (!_初始化WinSock()) return "";
	if (欲转换主机名=="")return "";

	unsigned long addr = inet_addr(欲转换主机名);
	if (addr != INADDR_NONE) return 欲转换主机名;

	hostent* host = gethostbyname(欲转换主机名);
	if (host && host->h_addr_list[0]) {
		return inet_ntoa(*(in_addr*)host->h_addr_list[0]);
	}
	return "";
}


class EXP TCP客户端
{
public:
	SOCKET sock = INVALID_SOCKET;

	TCP客户端() { _初始化WinSock(); }
	~TCP客户端() { 断开(); }

	bool 连接(const char* 地址, int 端口, int 连接超时值 = 5000,
		bool* 失败因超时 = nullptr, bool 使用event = false)
	{
		if (失败因超时) *失败因超时 = false;
		if (!_初始化WinSock()) return false;

		StrA ip = 转换为IP地址(地址);
		if (ip == "") return false;

		sockaddr_in ip地址;
		memset(&ip地址, 0, sizeof(ip地址));
		ip地址.sin_family = AF_INET;
		ip地址.sin_port = htons((u_short)端口);
		ip地址.sin_addr.s_addr = inet_addr((const char*)ip);

		if (sock != INVALID_SOCKET) 断开();

		sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sock == INVALID_SOCKET) {
			_lastError = WSAGetLastError();
			return false;
		}

		u_long nonBlock = 1;
		ioctlsocket(sock, FIONBIO, &nonBlock);

		int r = connect(sock, (sockaddr*)&ip地址, sizeof(ip地址));
		if (r == 0) return true;

		_lastError = WSAGetLastError();
		if (_lastError != WSAEWOULDBLOCK && _lastError != WSAEINPROGRESS && _lastError != WSAEINVAL) {
			断开();
			return false;
		}

		if (连接超时值 <= 0) 连接超时值 = 5000;

		if (使用event) {
			_event = WSACreateEvent();
			if (_event == WSA_INVALID_EVENT) {
				_lastError = WSAGetLastError();
				断开();
				return false;
			}

			WSAEventSelect(sock, _event, FD_CONNECT);
			r = MsgWaitForMultipleObjects(1, &_event, FALSE, 连接超时值, QS_ALLINPUT);
			WSACloseEvent(_event);
			_event = WSA_INVALID_EVENT;

			if (r == WAIT_OBJECT_0) {
				int res = 套接字_检查是否可写(sock, 0);
				if (res > 0) return true;
				_lastError = (res == -1) ? WSAGetLastError() : 0;
				断开();
				return false;
			}
			if (r == WAIT_TIMEOUT && 失败因超时) *失败因超时 = true;
			断开();
			return false;
		}

		int res = 套接字_检查是否可写(sock, 1100);
		if (res == 0 && _lastError == WSAECONNREFUSED) {
			断开();
			return false;
		}
		if (res == 0) {
			res = 套接字_检查是否可写(sock, 连接超时值 - 1100);
		}
		if (res <= 0) {
			if (失败因超时) *失败因超时 = (res == 0);
			if (res == -1) _lastError = WSAGetLastError();
			断开();
			return false;
		}
		return true;
	}


	void 断开()
	{
		if (sock != INVALID_SOCKET) {
			closesocket(sock);
			sock = INVALID_SOCKET;
		}
		if (_event != WSA_INVALID_EVENT) {
			WSACloseEvent(_event);
			_event = WSA_INVALID_EVENT;
		}
	}

	bool 发送(const Bytes& 发送内容, int 等待时间 = 1000 * 3600,
		int* 因超时已拷贝的大小 = nullptr)
	{
		if (因超时已拷贝的大小) *因超时已拷贝的大小 = 0;
		if (sock == INVALID_SOCKET) {
			_lastError = WSAENOTSOCK;
			return false;
		}
		if (!发送内容.buf || 发送内容.size == 0) return true;
		if (等待时间 <= 0) 等待时间 = 1000 * 3600;

		char* p数据 = (char*)发送内容.buf;
		int 总的长度 = (int)发送内容.size;
		int 已拷贝 = 0;
		int t_start = (int)GetTickCount();
		int timed = 等待时间;

		while (已拷贝 < 总的长度) {
			int r = send(sock, p数据 + 已拷贝, 总的长度 - 已拷贝, 0);
			if (r == SOCKET_ERROR) {
				_lastError = WSAGetLastError();
				int res = 套接字_检查是否可写(sock, timed);
				if (res == -1 || _lastError != 0) {
					if (因超时已拷贝的大小) *因超时已拷贝的大小 = 0;
					断开();
					return false;
				}
				if (res == 0) {
					if (因超时已拷贝的大小) *因超时已拷贝的大小 = 已拷贝;
					return false;
				}
				timed = 等待时间 - ((int)GetTickCount() - t_start);
				if (timed <= 0) {
					if (因超时已拷贝的大小) *因超时已拷贝的大小 = 已拷贝;
					return false;
				}
				continue;
			}
			if (r <= 0) {
				断开();
				return false;
			}
			已拷贝 += r;
			if (因超时已拷贝的大小) *因超时已拷贝的大小 = 已拷贝;
		}
		return true;
	}

	Bytes 接收(int 超时值 = 1000 * 7200, bool* 是否超时 = nullptr)
	{
		if (是否超时) *是否超时 = false;
		if (超时值 <= 0) 超时值 = 1000 * 7200;

		int res = 套接字_检查是否可读(sock, 超时值);
		if (res == 0) {
			if (是否超时) *是否超时 = true;
			return Bytes();
		}
		if (res == -1) {
			_lastError = WSAGetLastError();
			断开();
			return Bytes();
		}

		u_long 将接收大小 = 0;
		if (ioctlsocket(sock, FIONREAD, &将接收大小) == SOCKET_ERROR || 将接收大小 == 0) {
			_lastError = WSAGetLastError();
			断开();
			return Bytes();
		}

		Bytes 接收buf((int)将接收大小);
		int r = 接收p(接收buf.buf, (int)将接收大小, 0);
		if (r <= 0) return Bytes();
		接收buf.size = r;
		return 接收buf;
	}


	int 接收p(void* 接收buf, int 将接收大小, int 超时值 = 1000 * 7200)
	{
		if (!接收buf || 将接收大小 <= 0) return -1;
		if (超时值 <= 0) 超时值 = 1000 * 7200;

		int res = 套接字_检查是否可读(sock, 超时值);
		if (res == 0) return 0;
		if (res == -1) {
			_lastError = WSAGetLastError();
			断开();
			return -1;
		}

		int r = recv(sock, (char*)接收buf, 将接收大小, 0);
		if (r <= 0) {
			_lastError = WSAGetLastError();
			断开();
			return -1;
		}
		return r;
	}

	//由于需要依赖 void 处理事件() 此处先声明，在BEWin32UI中定义
	int 接收ep(void* 接收buf, int 将接收大小, int 超时值 = 1000 * 7200);

private:
	int _lastError = 0;
	WSAEVENT _event = WSA_INVALID_EVENT;
};

// --- 全局程序集函数 ---

int TCP_断开连接(SOCKET hsock)
{
	return closesocket(hsock);
}

int TCP_取客户端口(SOCKET 客户句柄)
{
	sockaddr_in sa;
	int len = sizeof(sa);
	if (getpeername(客户句柄, (sockaddr*)&sa, &len) == 0) {
		return ntohs(sa.sin_port);
	}
	return 0;
}


//此服务器实现同步阻塞性能低下，仅用作参考及简单测试，切勿应用在生产环境
class EXP TCP服务器
{
public:
	typedef void(*事件_客户进来)(TCP服务器& 服务器, SOCKET 客户句柄, int 客户端口);
	typedef void(*事件_客户数据到来)(TCP服务器& 服务器, SOCKET 客户句柄, c_Bytes 接收数据);
	typedef void(*事件_客户断开)(TCP服务器& 服务器, SOCKET 客户句柄, int 客户端口);

	SOCKET hsock = INVALID_SOCKET;

	TCP服务器() { _初始化WinSock(); }
	~TCP服务器() { 关闭服务(); }

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
		int 接收缓冲区大小 = 20480)
	{
		if (!_启动(端口, 5, 绑定监听网卡IP)) return false;

		if (接收缓冲区大小 <= 0) 接收缓冲区大小 = 20480;

		_投参.hsockServ = hsock;
		_投参.客户进来 = 客户进来;
		_投参.客户数据到来 = 客户数据到来;
		_投参.客户断开 = 客户断开;
		_投参.接收缓冲区大小 = 接收缓冲区大小;

		if (以新线程启动) {
			启动线程([](void* p) -> DWORD {
				((TCP服务器*)p)->_TCP_启动();
				return 0;
			}, this, _hThread);
		} else {
			_TCP_启动();
		}
		return true;
	}

	StrA 取客户IP(SOCKET 客户句柄)
	{
		sockaddr_in sa;
		int len = sizeof(sa);
		if (getpeername(客户句柄, (sockaddr*)&sa, &len) == SOCKET_ERROR) return "";
		return inet_ntoa(sa.sin_addr);
	}

	int 取客户端口(SOCKET 客户句柄)
	{
		return TCP_取客户端口(客户句柄);
	}

	bool 发送数据(SOCKET 客户句柄, const Bytes& 发送内容)
	{
		if (客户句柄 == INVALID_SOCKET) return false;
		if (!发送内容.buf || 发送内容.size == 0) return true;
		int r = send(客户句柄, (const char*)发送内容.buf, (int)发送内容.size, 0);
		return r == (int)发送内容.size;
	}

	bool 断开客户(SOCKET 客户句柄)
	{
		if (客户句柄 != INVALID_SOCKET) {
			return closesocket(客户句柄) != SOCKET_ERROR;
		}
		return true;
	}

	void 关闭服务()
	{
		if (hsock != INVALID_SOCKET) {
			强制断开客户(hsock);
			hsock = INVALID_SOCKET;
		}
		if (_hThread) {
			WaitForSingleObject(_hThread, 500);
			CloseHandle(_hThread);
			_hThread = nullptr;
		}
	}

	bool 强制断开客户(SOCKET 客户句柄)
	{
		if (客户句柄 == INVALID_SOCKET) return false;

		linger so_linger;
		so_linger.l_onoff = 1;
		so_linger.l_linger = 0;
		
		int opt = 0;
		setsockopt(客户句柄, SOL_SOCKET, SO_DONTLINGER, (const char*)&opt, 4);
		setsockopt(客户句柄, SOL_SOCKET, SO_LINGER, (const char*)&so_linger, sizeof(so_linger));
		opt = 1;
		setsockopt(客户句柄, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, 4);

		return 断开客户(客户句柄);
	}

protected:
	struct {
		SOCKET hsockServ;
		事件_客户进来 客户进来;
		事件_客户数据到来 客户数据到来;
		事件_客户断开 客户断开;
		int 接收缓冲区大小;
	} _投参 = { 0 };
	HANDLE _hThread = nullptr;

	bool _启动(int 端口, int 等待个数, const char* 绑定监听网卡IP)
	{
		if (!_初始化WinSock()) return false;
		hsock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (hsock == INVALID_SOCKET) return false;

		sockaddr_in saServer;
		memset(&saServer, 0, sizeof(saServer));
		saServer.sin_family = AF_INET;
		saServer.sin_port = htons((u_short)端口);
		if (绑定监听网卡IP && 绑定监听网卡IP[0]) {
			saServer.sin_addr.s_addr = inet_addr(绑定监听网卡IP);
		} else {
			saServer.sin_addr.s_addr = INADDR_ANY;
		}

		if (bind(hsock, (sockaddr*)&saServer, sizeof(saServer)) == SOCKET_ERROR) {
			closesocket(hsock);
			hsock = INVALID_SOCKET;
			return false;
		}

		if (listen(hsock, 等待个数 <= 0 ? SOMAXCONN : 等待个数) == SOCKET_ERROR) {
			closesocket(hsock);
			hsock = INVALID_SOCKET;
			return false;
		}
		return true;
	}

	void _TCP_启动()
	{
		sockaddr_in addr;
		int addrLen = sizeof(addr);
		while (true) {
			SOCKET hclient = accept(_投参.hsockServ, (struct sockaddr*)&addr, &addrLen);
			if (hclient == INVALID_SOCKET) break;

			// 直接进入客户处理，处理完（断开）后再接受下一个
			_TCP_客户进入(hclient, ntohs(addr.sin_port));
		}
	}

	//这个虚函数留给HTTP服务器时可单独处理
	virtual void _TCP_客户进入(SOCKET sClient, int 客户端口)
	{
		if (_投参.客户进来) _投参.客户进来(*this, sClient, 客户端口);

		Bytes recvBuf(_投参.接收缓冲区大小);

		while (true) {
			int r = recv(sClient, (char*)recvBuf.buf, _投参.接收缓冲区大小, 0);
			if (r <= 0) break;
			recvBuf.size = r;
			if (_投参.客户数据到来) _投参.客户数据到来(*this, sClient, recvBuf);
		}

		if (_投参.客户断开) _投参.客户断开(*this, sClient, 客户端口);
		closesocket(sClient);
	}
};