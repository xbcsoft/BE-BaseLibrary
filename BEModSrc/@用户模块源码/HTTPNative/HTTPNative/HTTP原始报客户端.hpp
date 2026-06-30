#pragma once
#include "stdafx.h"

class HTTP原始报客户端 : public TCP客户端
{
private:
	void _从header解析服务器信息(const StrA& header, StrA& ip, int& port) {
		const char* p = header;
		if (!p) return;
		
		const char* pHost = strstr(p, "Host: ");
		if (!pHost) pHost = strstr(p, "host: ");
		
		if (pHost) {
			pHost += 6;
			const char* pEnd = strstr(pHost, "\r\n");
			if (pEnd) {
				StrA hostStr(pHost, pEnd - pHost);
				const char* colon = strchr(hostStr, ':');
				if (colon) {
					ip = StrA(hostStr, colon - hostStr);
					port = atoi(colon + 1);
				} else {
					ip = hostStr;
					port = 80;
				}
			}
		}
	}

	int _header取状态码(const StrA& header) {
		if (header.len() < 12) return 0;
		const char* p = header;
		if (memcmp(p, "HTTP/", 5) == 0) {
			const char* space = nullptr;
			for (size_t i = 0; i < header.len(); ++i) {
				if (p[i] == ' ') {
					space = p + i;
					break;
				}
			}
			if (space) {
				return atoi(space + 1);
			}
		}
		return 0;
	}

	Bytes _HTTP处理chunked(Bytes& initialBuf, int headerEndPos, int t_start, int 超时值) {
		Bytes body;
		Bytes buffer;
		if (initialBuf.size > (size_t)headerEndPos) {
			buffer.append(initialBuf.buf + headerEndPos, initialBuf.size - headerEndPos);
		}

		while (true) {
			int rnrn = -1;
			for (int i = 0; i <= (int)buffer.size - 2; ++i) {
				if (buffer.buf[i] == '\r' && buffer.buf[i + 1] == '\n') {
					rnrn = i;
					break;
				}
			}

			if (rnrn == -1) {
				int currentTimeout = 超时值 - (GetTickCount() - t_start);
				if (currentTimeout <= 0) break;
				Bytes more = 接收(currentTimeout);
				if (more.size == 0) return body;
				buffer += more;
				continue;
			}

			char tmp[32] = { 0 };
			int lenToCopy = rnrn;
			if (lenToCopy > 31) lenToCopy = 31;
			memcpy(tmp, buffer.buf, lenToCopy);
			int chunkSize = strtol(tmp, nullptr, 16);

			if (chunkSize == 0) break; 

			int needSize = rnrn + 2 + chunkSize + 2; 

			while ((int)buffer.size < needSize) {
				int currentTimeout = 超时值 - (GetTickCount() - t_start);
				if (currentTimeout <= 0) return body;
				Bytes more = 接收(currentTimeout);
				if (more.size == 0) return body;
				buffer += more;
			}

			body += Bytes(buffer.buf + rnrn + 2, BRef, chunkSize);

			int remaining = buffer.size - needSize;
			if (remaining > 0) {
				memmove(buffer.buf, buffer.buf + needSize, remaining);
			}
			buffer.size = remaining;
		}
		return body;
	}

public:
	bool HTTP连接(const StrA& header, int 超时 = 0) {
		StrA ip;
		int port = 80;
		_从header解析服务器信息(header, ip, port);
		if (ip.len() == 0) return false;
		return 连接(ip, port, 超时);
	}

	bool HTTP发送(const StrA& header, const Bytes& body = Bytes(), int 等待时间 = 5000) {
		int clPos = -1;
		const char* p = header;
		if (!p) return false;

		for (int i = 0; i <= (int)header.len() - 16; ++i) {
			if (memcmp(p + i, "Content-Length: ", 16) == 0) {
				clPos = i;
				break;
			}
		}

		StrA newHeader;
		if (clPos != -1) {
			int rnrn = -1;
			for (int i = clPos; i <= (int)header.len() - 2; ++i) {
				if (p[i] == '\r' && p[i + 1] == '\n') {
					rnrn = i;
					break;
				}
			}
			if (rnrn != -1) {
				newHeader = _BSA(p, clPos + 16) + ToStr((int)body.size) + _BSA(p + rnrn, header.len() - rnrn);
			} else {
				newHeader = header;
			}
		} else {
			const char* pEnd = strstr(p, "\r\n\r\n");
			if (pEnd) {
				int pos = (int)(pEnd - p);
				newHeader = _BSA(p, pos) + sprintF("\r\nContent-Length: %d", (int)body.size) + _BSA(p + pos, header.len() - pos);
			} else {
				newHeader = header + sprintF("\r\nContent-Length: %d\r\n\r\n", (int)body.size);
			}
		}

		Bytes fullPack(newHeader, newHeader.len(), body.buf, body.size);
		return 发送(fullPack, 等待时间);
	}

	Bytes HTTP接收(StrA* outHeader = nullptr, int* outStatus = nullptr, int 超时值 = 0) {
		int t_start = GetTickCount();
		if (超时值 <= 0) 超时值 = 60 * 1000;

		Bytes a = 接收(超时值);
		int currentTimeout = 超时值 - (GetTickCount() - t_start);
		
		if (a.size < 16) {
			if (currentTimeout > 0) a += 接收(currentTimeout);
		}
		int a_size = (int)a.size;

		int k = -1;
		for (int i = 0; i <= a_size - 16; ++i) {
			if (memcmp(a.buf + i, "Content-Length: ", 16) == 0) {
				k = i;
				break;
			}
		}

		StrA header;
		Bytes body;
		int 状态码 = 0;

		if (k != -1) {
			int k2 = -1;
			for (int i = k + 16; i <= a_size - 2; ++i) {
				if (a.buf[i] == '\r' && a.buf[i + 1] == '\n') {
					k2 = i;
					break;
				}
			}
			if (k2 == -1) return Bytes();

			char tmp[32] = { 0 };
			int numLen = k2 - (k + 16);
			if (numLen > 31) numLen = 31;
			memcpy(tmp, a.buf + k + 16, numLen);
			int contentLen = atoi(tmp);

			int k_rnrn = -1;
			for (int i = k2; i <= a_size - 4; ++i) {
				if (memcmp(a.buf + i, "\r\n\r\n", 4) == 0) {
					k_rnrn = i;
					break;
				}
			}
			if (k_rnrn == -1) return Bytes();

			header = _BSA(a.buf, k_rnrn);
			body = Bytes(a.buf + k_rnrn + 4, BRef, a_size - (k_rnrn + 4));
			状态码 = _header取状态码(header);

			while ((int)body.size < contentLen) {
				currentTimeout = 超时值 - (GetTickCount() - t_start);
				if (currentTimeout <= 0) break;
				Bytes more = 接收(currentTimeout);
				if (more.size == 0) break;
				body += more;
			}

			if (outHeader) *outHeader = header;
			if (outStatus) *outStatus = 状态码;
			return _HTTPNative_GZIP判断(header, Bytes(body.buf, BRef, contentLen));
		} else {
			int k_rnrn = -1;
			for (int i = 0; i <= a_size - 4; ++i) {
				if (memcmp(a.buf + i, "\r\n\r\n", 4) == 0) {
					k_rnrn = i;
					break;
				}
			}
			if (k_rnrn == -1) return Bytes();

			header = _BSA(a.buf, k_rnrn);
			状态码 = _header取状态码(header);

			if (a_size > k_rnrn + 4) {
				body = _HTTP处理chunked(a, k_rnrn + 4, t_start, 超时值);
				if (outHeader) *outHeader = header;
				if (outStatus) *outStatus = 状态码;
				return _HTTPNative_GZIP判断(header, body);
			} else {
				if (outHeader) *outHeader = header;
				if (outStatus) *outStatus = 状态码;
				return Bytes();
			}
		}
	}
};
