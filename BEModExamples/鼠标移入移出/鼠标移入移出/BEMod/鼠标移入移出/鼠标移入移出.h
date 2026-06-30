#pragma once
/**@ModuleTitle: 鼠标移入移出
*  @version:     1.0
*  @platform:    win32(x86|x64)
*  @compiler:    source
*  @author:      Antigravity
*  @datetime:    2026-05-13
*  @description: 提供控件的鼠标移入与移出事件支持，仿易语言实现。
*/
#include "stdafx.h"
#include <BECore/中文核心.hpp>

// 定义回调函数指针类型
typedef void(*PHoverCallback)(HWND hWnd);


/**
 * 控件_添加鼠标移入移出事件
 * @param 句柄 目标窗口或控件句柄
 * @param 移入事件 鼠标移入时的回调
 * @param 移出事件 鼠标移出时的回调
 */
void 控件_添加鼠标移入移出事件(HWND 句柄, PHoverCallback 移入事件, PHoverCallback 移出事件);
