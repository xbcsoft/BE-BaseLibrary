#pragma once
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

// 设置调试目标 (IP:端口)
void coutlog_option(c_StrX host, bool isStop = false);

void coutlog_optionT(c_StrX host);

// 发送日志
StrU8 coutlog(c_AutoStr str, bool zusai = false);

StrU8 coutlogR(c_Bytes str, bool zusai = false, c_AutoStr ext = "log");

bool coutlog_is_stop();

template <typename... Args>
void coutlogV(Args&&... args) {
	if (coutlog_is_stop()) return;
	AutoStr result;
	be::_sprint_helper(result, args...);
	coutlog(result);
}

void coutClear();

#else

void coutlog_option(AutoS host, bool isStop = false);
void coutlog_optionT(AutoS host);
StrU8 coutlog(AutoS str, bool zusai = false);
StrU8 coutlogR(AutoS str, bool zusai = false, AutoS ext = "");
bool coutlog_is_stop();
void coutlogV(...);
void coutClear();

#endif
