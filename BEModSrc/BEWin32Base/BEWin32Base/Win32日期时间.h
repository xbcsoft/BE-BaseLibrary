#pragma once
#include "stdafx.h"

日期时间型 到时间(const SYSTEMTIME& st, char 跨时区小时 = 期::全局时区);

日期时间型 到时间(const FILETIME& ft, char 跨时区小时 = 期::全局时区);

SYSTEMTIME 到SYSTEMTIME(const 日期时间型& dt);

FILETIME 到FILETIME(const 日期时间型& dt);
