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

	static StrA 格式化日志(c_StrX data);

	void 写(c_StrX data);

	~文件日志();
};


extern 文件日志 _dbglog;
namespace be {
/**以参1为bool标记做的日志，在程序启动之前会被清空，
   并在整个程序生命周期内可持续向尾写入
 * @param bool any
 * @param args
 */
template <typename... Args>
void log(bool, Args&&... args) {
	AutoStr result;
	_sprint_helper(result, args...);
	_dbglog.写(result.str);
}

/**自定义路径日志，总是向文件末尾追加写入
 * @param c_StrW path 自定义路径
 * @param args
 */
template <typename... Args>
void log(c_StrX path, Args&&... args) {
	AutoStr result;
	_sprint_helper(result, args...);
	StrA line = 文件日志::格式化日志(result.str);
	StrW dir = 路径_去文件名(path);
	if (dir && !目录是否存在(dir)) {
		创建目录Ex(dir);
	}
	文件读写 f(false);
	if (f.打开文件(path, 2, 3)) {
		f.移动读写位置(2, 0);
		f.写出字节集(line.bytes);
		f.关闭文件();
	}
}
}


#if DBG
#define dbg_log(...) be::log(0, __VA_ARGS__)
#define dbg_flog(file, ...) be::log(file, __VA_ARGS__)
#else
#define dbg_log(...)
#define dbg_flog(...)
#endif
