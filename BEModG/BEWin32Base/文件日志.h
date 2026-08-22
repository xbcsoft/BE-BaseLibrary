#pragma once
#include "stdafx.h"
#include "环境存取.h"
#include "文件读写.h"
#include "磁盘目录操作.h"
#include "Win32日期时间.h"

class 文件日志
{
private:
	StrW _dir;
	StrW _name;
	int _cycle = 7;
	HANDLE _hf = INVALID_HANDLE_VALUE;
	StrW _fm;

	void 关闭();

	int64 _dTime(c_StrX file__lastTime);

public:
	文件日志() = default;

	文件日志(c_StrX name, int cycle = 7, c_StrX dir = L"");

	void 初始化(c_StrX name, int cycle = 7, c_StrX dir = L"");

	void 写(c_StrX data);

	~文件日志();
};


extern 文件日志 _dbglog;
namespace be {
template <typename... Args>
void log(Args&&... args) {
	AutoStr result;
	_sprint_helper(result, args...);
	_dbglog.写(result.str);
}
}


#if DBG
#define dbg_log(...) be::log(__VA_ARGS__)
#else
#define dbg_log(...)
#endif
