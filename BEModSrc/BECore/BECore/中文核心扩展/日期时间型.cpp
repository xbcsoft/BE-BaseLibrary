#include "../中文核心.hpp"
#include "文本操作.h"
#include <time.h>

struct 期 {
	enum {
		年 = 1,
		季 = 2,
		月 = 3,
		周 = 4,
		日 = 5,
		时 = 6,
		分 = 7,
		秒 = 8
	};
	static char 全局时区;
};

#ifdef _WIN32
typedef struct _TIME_ZONE_INFORMATION TIME_ZONE_INFORMATION;
extern "C" __declspec(dllimport) unsigned long __stdcall
GetTimeZoneInformation(TIME_ZONE_INFORMATION*);
#endif

/**如当前系统时区是东八区，则取出正8
 */
char 取系统本地时区() {
#ifdef _WIN32
	int tz[43]{}; // 172 bytes
	unsigned int id = GetTimeZoneInformation((TIME_ZONE_INFORMATION*)tz);
	int bias = tz[0];
	if (id == 2) bias += tz[42];      // id == TIME_ZONE_ID_DAYLIGHT
	else if (id == 1) bias += tz[21]; // id == TIME_ZONE_ID_STANDARD
	return (char)(-bias / 60);
#else
	time_t now = time(nullptr);
	struct tm local {};
	localtime_r(&now, &local);
	long offset = local.tm_gmtoff; // seconds east of UTC
	return (char)(offset / 3600);
#endif
}
char 期::全局时区 = 取系统本地时区();


/**日期时间型（总结一句：从时间戳解析是入/再转时间戳是出，入加出减）
 */
struct EXP 日期时间型
{
	uint 年 = 1970;
	char 月 = 1;
	char 日 = 1;
	char 时区偏移 = 0;
	char 时 = 0;
	char 分 = 0;
	char 秒 = 0;
	short 毫秒 = 0;

	日期时间型() = default;
	日期时间型(const 日期时间型&) = default;
	日期时间型& operator=(const 日期时间型&) = default;

	/**根据时间戳构造，并自动设置实例的时区偏移
	 * @param 时间戳秒
	 * @param 跨时区小时=期::全局时区
	 */
	日期时间型(int64 时间戳秒, char 跨时区小时 = 期::全局时区) {
		this->时区偏移 = 跨时区小时;
		time_t tt = (time_t)(时间戳秒 + (int64)跨时区小时 * 3600);
		struct tm t {};
#ifdef _WIN32
		gmtime_s(&t, &tt);
#else
		gmtime_r(&tt, &t);
#endif
		年 = t.tm_year + 1900;
		月 = (char)(t.tm_mon + 1);
		日 = (char)t.tm_mday;
		时 = (char)t.tm_hour;
		分 = (char)t.tm_min;
		秒 = (char)t.tm_sec;
	}

	/**取时间戳
	 * @param 跨时区小时<可空> 默认为该实例内置的时区偏移
	 * @return
	 */
	int64 取时间戳(可空<char> 跨时区小时 = 空) const {
		struct tm t {};
		t.tm_year = 年 - 1900;
		t.tm_mon = 月 - 1;
		t.tm_mday = 日;
		t.tm_hour = 时;
		t.tm_min = 分;
		t.tm_sec = 秒;
		t.tm_isdst = -1;

#ifdef _WIN32
		time_t utc = _mkgmtime(&t);
#else
		time_t utc = timegm(&t);
#endif

		if (utc == (time_t)-1) return 0;
		char offset = (跨时区小时 == 空) ? 时区偏移 : (char)跨时区小时;
		return (int64)utc - (int64)offset * 3600;
	}

};

AutoStr __AutoStr__(const 日期时间型& dt) {
	if (dt.毫秒 == 0) {
		return sprintF("%04u-%02u-%02u %02u:%02u:%02u", dt.年, dt.月, dt.日, dt.时, dt.分, dt.秒);
	} else {
		return sprintF("%04u-%02u-%02u %02u:%02u:%02u.%03d", dt.年, dt.月, dt.日, dt.时, dt.分, dt.秒, (int)dt.毫秒);
	}
}

// ==================== 日期字面量支持 (2026_y/3/11/ 12/30/10) ====================
struct _年 { uint v; };
struct _年月 { uint y; uint m; };
struct _日期时 { 日期时间型 dt; operator 日期时间型(); };
struct _日期时分 { 日期时间型 dt; operator 日期时间型(); };

_年 operator "" _y(unsigned long long v) { return { (uint)v }; }
_年月 operator/(const _年& y, int m) { return { y.v, (uint)m }; }
日期时间型 operator/(const _年月& ym, int d) {
	日期时间型 dt;
	dt.年 = ym.y;
	dt.月 = (char)ym.m;
	dt.日 = (char)d;
	dt.时区偏移 = 期::全局时区;
	return dt;
}
_日期时 operator/(const 日期时间型& dt, int h) {
	日期时间型 res = dt;
	res.时 = (char)h;
	return { res };
}
_日期时分 operator/(const _日期时& dth, int m) {
	日期时间型 res = dth.dt;
	res.分 = (char)m;
	return { res };
}
日期时间型 operator/(const _日期时分& dtm, int s) {
	日期时间型 res = dtm.dt;
	res.秒 = (char)s;
	return res;
}
日期时间型 operator+(const 日期时间型& dt, int offset) {
	日期时间型 res = dt;
	res.时区偏移 = (char)offset;
	return res;
}
_日期时::operator 日期时间型() { return dt; }
_日期时分::operator 日期时间型() { return dt; }

namespace {

// ==================== 解析时间文本 → struct tm ====================
// 输入和格式均为 const char*，W 版调用前先 WtoA
bool _解析时间(const char* s, size_t slen, const char* f, struct tm& outTm) {
	size_t i = 0;
	int y = 1970, m = 1, d = 1, h = 0, mi = 0, sec = 0;

	//lambda对外部变量在内部所采用到的进行全引用版捕获
	auto 读数字 = [&](int 位数, int& out) {
		if (i + (size_t)位数 > slen) return false;
		int val = 0;
		for (int k = 0; k < 位数; k++) {
			char c = s[i + k];
			if (c < '0' || c > '9') return false;
			val = val * 10 + (c - '0');
		}
		out = val;
		i += (size_t)位数;
		return true;
	};

	for (; *f; f++) {
		switch (*f) {
		case 'y': if (!读数字(4, y))   return false; continue;
		case 'm': if (!读数字(2, m))   return false; continue;
		case 'd': if (!读数字(2, d))   return false; continue;
		case 'h': if (!读数字(2, h))   return false; continue;
		case 'i': if (!读数字(2, mi))  return false; continue;
		case 's': if (!读数字(2, sec)) return false; continue;
		default:
			if (i >= slen || s[i] != *f) return false;
			i++;
		}
	}
	outTm = {};
	outTm.tm_year = y - 1900;
	outTm.tm_mon = m - 1;
	outTm.tm_mday = d;
	outTm.tm_hour = h;
	outTm.tm_min = mi;
	outTm.tm_sec = sec;
	outTm.tm_isdst = -1;
	return true;
}

}

/**到时间
 * @param 投入文本
 * @param 格式文本="y-m-d h:i:s" 注意这里仅支持ANSI字符集,y必须4位字符,其余必须都是2位
 * @return
 */
日期时间型 到时间(c_StrX 投入文本, c_StrX 格式文本 = "y-m-d h:i:s") {
	const char* input = 投入文本;
	size_t inputLen = 投入文本.str.len();
	const char* format = 格式文本;

	日期时间型 ret{};
	if (inputLen == 0) return ret;

	struct tm t {};
	if (format == nullptr || !_解析时间(input, inputLen, format, t))
		return ret;

#ifdef _WIN32
	time_t utc = _mkgmtime(&t);
#else
	time_t utc = timegm(&t);
#endif
	if (utc == (time_t)-1) return ret;

	int64 ts = (int64)utc - (int64)期::全局时区 * 3600;
	return 日期时间型(ts, 期::全局时区);
}

/**默认为用当前全局时区取现行时间
 */
日期时间型 取现行时间(char 跨时区小时 = 期::全局时区) {
	return 日期时间型(time(nullptr), 跨时区小时);
}

日期时间型 增减时间(日期时间型 时间, byte 被增加部分, int 增加值) {
	if (增加值 == 0) return 时间;

	struct tm t {};
	t.tm_year = 时间.年 - 1900;
	t.tm_mon = 时间.月 - 1;
	t.tm_mday = 时间.日;
	t.tm_hour = 时间.时;
	t.tm_min = 时间.分;
	t.tm_sec = 时间.秒;
	t.tm_isdst = -1;

	switch (被增加部分) {
	case 期::年: t.tm_year += 增加值; break;
	case 期::季: t.tm_mon += 增加值 * 3; break;
	case 期::月: t.tm_mon += 增加值; break;
	case 期::周: t.tm_mday += 增加值 * 7; break;
	case 期::日: t.tm_mday += 增加值; break;
	case 期::时: t.tm_hour += 增加值; break;
	case 期::分: t.tm_min += 增加值; break;
	case 期::秒: t.tm_sec += 增加值; break;
	}

	mktime(&t); // 这个调用会自动规范化溢出的字段

	日期时间型 res = 时间;
	res.年 = t.tm_year + 1900;
	res.月 = (char)(t.tm_mon + 1);
	res.日 = (char)t.tm_mday;
	res.时 = (char)t.tm_hour;
	res.分 = (char)t.tm_min;
	res.秒 = (char)t.tm_sec;
	return res;
}

StrX 到格式化JS时间(日期时间型 时间) {
	struct tm t {};
	t.tm_year = 时间.年 - 1900;
	t.tm_mon = 时间.月 - 1;
	t.tm_mday = 时间.日;
	t.tm_hour = 时间.时;
	t.tm_min = 时间.分;
	t.tm_sec = 时间.秒;
	t.tm_isdst = -1;

#ifdef _WIN32
	time_t utc = _mkgmtime(&t);
#else
	time_t utc = timegm(&t);
#endif

	if (utc == (time_t)-1) return "";

	static const char* week[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
	static const char* month[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

	if (时间.时区偏移 == 0) {
		return sprintF("%s, %02u %s %04u %02u:%02u:%02u GMT",
			week[t.tm_wday], (uint)时间.日, month[t.tm_mon], (uint)时间.年,
			(uint)时间.时, (uint)时间.分, (uint)时间.秒);
	}

	char sign = 时间.时区偏移 > 0 ? '+' : '-';
	int absOffset = 时间.时区偏移 > 0 ? 时间.时区偏移 : -时间.时区偏移;

	return sprintF("%s, %02u %s %04u %02u:%02u:%02u GMT%c%02d00",
		week[t.tm_wday], (uint)时间.日, month[t.tm_mon], (uint)时间.年,
		(uint)时间.时, (uint)时间.分, (uint)时间.秒,
		sign, absOffset);
}
