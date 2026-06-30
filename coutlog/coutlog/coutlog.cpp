/**@ModuleTitle:白易远程控制台日志输出
*  @version:     1.0
*  @platform:    win32(x86|x64)
*  @compiler:    source
*  @language:    
*  @author:
*  @datetime:
*  @description:
*/

#include "stdafx.h"

#ifndef _BE_RELEASE


namespace {

// 线程私有变量
thread_local StrA _tlsco_host = "";
thread_local int _tlsco_port = 0;
thread_local int _tlsco_portT = 0;

// 全局静态变量
StrA _gco_host = "";
int _gco_port = 10086;
bool _g_coutlog_isStop = false;

// HTTP POST 核心逻辑
StrU8 _http_post(c_AutoStr params, const Bytes& body) {
	if (_g_coutlog_isStop) return "";

	StrU8 host = _tlsco_host;
	int port = _tlsco_port;

	if (host == "") {
		host = _gco_host != "" ? _gco_host : StrA("127.0.0.1");
	}
	if (port == 0) {
		port = _gco_port;
	} else if (port < 0) {
		port = -port;
		_tlsco_port = _tlsco_portT;
	}

	TCP客户端 client;
	if (!client.连接(host, port, 3000)) return "";

	StrU8 request;
	request += "POST /?";
	request += params.str;
	request += " HTTP/1.1\r\nHost: ";
	request += host;
	request += ":";
	request += ToStr(port);
	request += "\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: ";
	request += ToStr(body.size);
	request += "\r\n\r\n";
	request.bytes += body;

	if (!client.发送(request.bytes, 3000)) return "";

	Bytes resp_bytes;
	while (true) {
		Bytes chunk = client.接收();
		if (chunk.size == 0) break;
		resp_bytes.append(chunk);
	}
	if (resp_bytes.size == 0) return "";

	StrU8& resp = (StrU8&)resp_bytes;
	const char* p = strstr(resp, "\r\n\r\n");
	if (p) {
		const char* body_start = p + 4;
		size_t body_len = resp_bytes.size - (body_start - (char*)resp_bytes.buf);
		return StrU8(body_start, body_len);
	}

	return "";
}

} // namespace

// 设置调试目标 (IP:端口)
void coutlog_option(c_StrX host, bool isStop = false) {
	_g_coutlog_isStop = isStop;
	StrU8 s = host;
	const char* p = strchr(s, ':');
	if (p) {
		int colon = (int)(p - (const char*)s);
		_tlsco_host = StrU8(s, colon);
		_tlsco_port = ToInt(p + 1);
	} else {
		_tlsco_host = s;
		_tlsco_port = 10086;
	}
}

void coutlog_optionT(c_StrX host) {
	StrU8 s = host;
	const char* p = strchr(s, ':');
	if (!p) return;
	int colon = (int)(p - (const char*)s);
	_tlsco_host = StrU8(s, colon);
	_tlsco_portT = _tlsco_port;
	_tlsco_port = -ToInt(p + 1);
}

// 发送日志
StrU8 coutlog(c_AutoStr str, bool zusai = false) {
	StrU8 params = "ctl=1&zs=";
	params += zusai ? "1" : "0";
	return _http_post(params, str.str.bytes);
}

StrU8 coutlogR(c_Bytes str, bool zusai = false, c_AutoStr ext = "log") {
	StrU8 params = "ctl=2&zs=";
	params += zusai ? "1" : "0";
	params += "&ext=";
	params += (StrU8)ext;
	return _http_post(params, str);
}

bool coutlog_is_stop() {
	return _g_coutlog_isStop;
}

template <typename... Args>
void coutlogV(Args&&... args) {
	if (coutlog_is_stop()) return;
	AutoStr result;
	be::_sprint_helper(result, args...);
	coutlog(result);
}

void coutClear() {
	_http_post("ctl=0", BR(""));
}

#else

void coutlog_option(AutoS host, bool isStop = false) {}
void coutlog_optionT(AutoS host) {}
StrU8 coutlog(AutoS str, bool zusai = false) { return ""; }
StrU8 coutlogR(AutoS str, bool zusai = false, AutoS ext = "") { return ""; }
bool coutlog_is_stop() { return true; }
void coutlogV(...) {}
void coutClear() {}

#endif