#include "stdafx.h"
#include "HTTPNative.h" //由MakeBEC.exe自动生成
//此main.cpp仅用作模块测试不参与头文件的解析生成
//若想排除特定cpp文件的解析可在工程属性页-生成事件
//改写MakeBEC.exe参数-excludesrc=多文件[需加空格]

void 客户进来(HTTP原始报服务器& 服务器, SOCKET 客户句柄, int 客户端口)
{
	be::print("join", 客户句柄, 客户端口);
}

void 客户数据到来(HTTP原始报服务器& 服务器, SOCKET 客户句柄, c_Bytes 数据)
{
	// 打印收到的原始报文（已由服务器处理好粘包和GZIP）
	be::print("recv packet_text:", BSA<true>(数据));

	// 回复一个简单的网页
	服务器.HTTP发送(客户句柄, nil, BR("<html><body><h1>Hello from BE_HTTP_Server!</h1>"
		"<p>Response generated successfully.</p></body></html>"));
}

void 客户断开(HTTP原始报服务器& 服务器, SOCKET 客户句柄, int 客户端口)
{
	be::print("leave", 客户句柄, 客户端口);
}

void main()
{
	HTTP原始报服务器 http;
	be::print("Server starting at port 8877...");
	http.创建(8877, 客户进来, 客户数据到来, 客户断开);
}