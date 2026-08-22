#include "stdafx.h"
#include "环境存取.h"
#include "文件读写.h"
#include "磁盘目录操作.h"
#include "Win32日期时间.h"

class EXP 文件日志
{
private:
	StrW _dir;
	StrW _name;
	int _cycle = 7;
	HANDLE _hf = INVALID_HANDLE_VALUE;
	StrW _fm;

	void 关闭() {
		if (_hf != INVALID_HANDLE_VALUE) {
			CloseHandle(_hf);
			_hf = INVALID_HANDLE_VALUE;
		}
	}

	int64 _dTime(c_StrX file__lastTime) {
		if (!文件是否存在(file__lastTime)) {
			return (int64)_cycle * 3600 * 24 + 1;
		}
		日期时间型 last = 取文件时间(file__lastTime, 0);
		日期时间型 midnight = last;
		midnight.时 = 0;
		midnight.分 = 0;
		midnight.秒 = 0;
		midnight.毫秒 = 0;
		return 取现行时间().取时间戳() - midnight.取时间戳();
	}

public:
	文件日志() = default;

	文件日志(c_StrX name, int cycle = 7, c_StrX dir = L"") {
		初始化(name, cycle, dir);
	}

	void 初始化(c_StrX name, int cycle = 7, c_StrX dir = L"") {
		_name = name;
		_cycle = cycle;
		if (dir && ((const wchar_t*)dir)[0] != L'\0') {
			_dir = dir;
		} else {
			_dir = 取当前目录() + L"\\logs";
		}

		if (_cycle == -1) {
			StrW fm = _dir + L"\\" + _name;
			if (!目录是否存在(_dir)) {
				创建目录Ex(_dir);
			}
			HANDLE h = CreateFileW(fm, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (h != INVALID_HANDLE_VALUE) {
				CloseHandle(h);
			}
		}
	}

	void 写(c_StrX data) {
		StrW fm;
		if (_cycle > 0) {
			StrW file__lastTime = _dir + L"\\_" + _name + L"_lastTime";
			StrW _t;

			if (_dTime(file__lastTime) > (int64)_cycle * 3600 * 24) {
				日期时间型 now = 取现行时间();
				wchar_t t_buf[128];
				swprintf_s(t_buf, L"%04d-%02d-%02d+%d", (int)now.年, (int)now.月, (int)now.日, _cycle);
				_t = t_buf;

				// Write to file__lastTime using WtoU8
				StrA t_ansi = WtoU8(_t);
				写到文件Ex(file__lastTime, t_ansi.bytes);
			} else {
				Bytes content_bytes = 读入文件(file__lastTime);
				if (content_bytes.size > 0) {
					StrA content_str((const char*)content_bytes.buf, content_bytes.size);
					_t = U8toW(content_str);
				} else {
					// Fallback if read failed
					日期时间型 now = 取现行时间();
					wchar_t t_buf[128];
					swprintf_s(t_buf, L"%04d-%02d-%02d+%d", (int)now.年, (int)now.月, (int)now.日, _cycle);
					_t = t_buf;
				}
			}

			StrW log_dir = _dir + L"\\" + _t;
			if (!目录是否存在(log_dir)) {
				创建目录Ex(log_dir);
			}
			fm = log_dir + L"\\" + _name;
		} else {
			if (!目录是否存在(_dir)) {
				创建目录Ex(_dir);
			}
			fm = _dir + L"\\" + _name;
		}

		if (fm != _fm) {
			关闭();
			_hf = CreateFileW(fm, FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			_fm = fm;
		}

		if (_hf != INVALID_HANDLE_VALUE) {
			日期时间型 now = 取现行时间();
			wchar_t time_buf[64];
			swprintf_s(time_buf, L"%04d-%02d-%02d %02d:%02d:%02d ",
				(int)now.年, (int)now.月, (int)now.日,
				(int)now.时, (int)now.分, (int)now.秒);
			StrW log_line = StrW(time_buf) + StrW(data) + L"\r\n";

			// Convert to UTF-8 using WtoU8
			StrA log_line_u8 = WtoU8(log_line);
			DWORD dwWritten = 0;
			WriteFile(_hf, (const char*)log_line_u8, (DWORD)log_line_u8.len(), &dwWritten, NULL);
		}
	}

	~文件日志() {
		关闭();
	}
};

#pragma region BE_IGNORE
文件日志 _dbglog("dbg.log", -1);
#pragma endregion
extern 文件日志 _dbglog;

namespace be
{
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