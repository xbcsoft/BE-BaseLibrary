#include "stdafx.h"

日期时间型 到时间(const SYSTEMTIME& st, char 跨时区小时 = 期::全局时区) {
	日期时间型 dt;
	dt.年 = st.wYear;
	dt.月 = (char)st.wMonth;
	dt.日 = (char)st.wDay;
	dt.时 = (char)st.wHour;
	dt.分 = (char)st.wMinute;
	dt.秒 = (char)st.wSecond;
	dt.毫秒 = (short)st.wMilliseconds;
	dt.时区偏移 = 跨时区小时;
	return dt;
}

日期时间型 到时间(const FILETIME& ft, char 跨时区小时 = 期::全局时区) {
	ULARGE_INTEGER ui;
	ui.LowPart = ft.dwLowDateTime;
	ui.HighPart = ft.dwHighDateTime;
	const uint64 epoch100ns = 116444736000000000ULL;
	if (ui.QuadPart < epoch100ns) return 日期时间型(0, 跨时区小时);

	int64 ts = (int64)((ui.QuadPart - epoch100ns) / 10000000ULL);
	日期时间型 dt(ts, 跨时区小时);
	// 处理毫秒（FILETIME 是 100ns 为单位）
	dt.毫秒 = (short)((ui.QuadPart % 10000000ULL) / 10000ULL);
	return dt;
}

SYSTEMTIME 到SYSTEMTIME(const 日期时间型& dt) {
	SYSTEMTIME st;
	st.wYear = (WORD)dt.年;
	st.wMonth = (WORD)dt.月;
	st.wDay = (WORD)dt.日;
	st.wHour = (WORD)dt.时;
	st.wMinute = (WORD)dt.分;
	st.wSecond = (WORD)dt.秒;
	st.wMilliseconds = (WORD)dt.毫秒;

	// 填充 wDayOfWeek（借用系统函数进行自动计算）
	FILETIME ft;
	if (SystemTimeToFileTime(&st, &ft)) {
		FileTimeToSystemTime(&ft, &st);
	} else {
		st.wDayOfWeek = 0;
	}
	return st;
}

FILETIME 到FILETIME(const 日期时间型& dt) {
	int64 ts = dt.取时间戳();
	ULARGE_INTEGER ui;
	const uint64 epoch100ns = 116444736000000000ULL;
	// 注意这里加上了毫秒偏移
	ui.QuadPart = (uint64)ts * 10000000ULL + epoch100ns + (uint64)dt.毫秒 * 10000ULL;
	FILETIME ft;
	ft.dwLowDateTime = ui.LowPart;
	ft.dwHighDateTime = ui.HighPart;
	return ft;
}
