#pragma once
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
char 取系统本地时区();


/**日期时间型（总结一句：从时间戳解析是入/再转时间戳是出，入加出减）
 */
struct 日期时间型
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
	日期时间型(int64 时间戳秒, char 跨时区小时 = 期::全局时区);

	/**取时间戳
	 * @param 跨时区小时<可空> 默认为该实例内置的时区偏移
	 * @return
	 */
	int64 取时间戳(可空<char> 跨时区小时 = 空) const;

};

AutoStr __AutoStr__(const 日期时间型& dt);

// ==================== 日期字面量支持 (2026_y/3/11/ 12/30/10) ====================
struct _年 { uint v; };
struct _年月 { uint y; uint m; };
struct _日期时 { 日期时间型 dt; operator 日期时间型(); };
struct _日期时分 { 日期时间型 dt; operator 日期时间型(); };

_年 operator "" _y(unsigned long long v);
_年月 operator/(const _年& y, int m);
日期时间型 operator/(const _年月& ym, int d);
_日期时 operator/(const 日期时间型& dt, int h);
_日期时分 operator/(const _日期时& dth, int m);
日期时间型 operator/(const _日期时分& dtm, int s);
日期时间型 operator+(const 日期时间型& dt, int offset);

/**到时间
 * @param 投入文本
 * @param 格式文本="y-m-d h:i:s" 注意这里仅支持ANSI字符集,y必须4位字符,其余必须都是2位
 * @return
 */
日期时间型 到时间(c_StrX 投入文本, c_StrX 格式文本 = "y-m-d h:i:s");

/**默认为用当前全局时区取现行时间
 */
日期时间型 取现行时间(char 跨时区小时 = 期::全局时区);

日期时间型 增减时间(日期时间型 时间, byte 被增加部分, int 增加值);

StrX 到格式化JS时间(日期时间型 时间);
