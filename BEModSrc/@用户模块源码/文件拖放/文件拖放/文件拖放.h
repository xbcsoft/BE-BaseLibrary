#pragma once
/**@ModuleTitle:文件拖放
*  @version:     1.0
*  @platform:    win32(x86|x64)
*  @compiler:    source
*  @author:
*  @datetime:
*  @description: 实现了窗口及控件的文件拖放注册与卸载。
*/
#include "stdafx.h"
#include <shellapi.h>

// 内部使用的属性名
#define PROP_OLDWNDPROC L"BE_DragDrop_OldWndProc"
#define PROP_CALLBACK   L"BE_DragDrop_Callback"
#define PROP_OLDEXSTYLE L"BE_DragDrop_OldExStyle"

// 以换行符"\r\n"进行分割
typedef void (CALLBACK* 文件拖放回调)(const wchar_t* 文件列表);

// 内部函数声明

/**注册拖放功能到指定窗口或控件
 * @param 控件句柄 目标窗口或控件的句柄
 * @param 回调 接收到文件拖放后的回调函数
 * @return bool 是否成功
 */
bool 文件拖放_注册(HWND 控件句柄, 文件拖放回调 回调);

/**
 * @brief 卸载指定窗口或控件的拖放功能
 * @param 控件句柄 目标窗口或控件的句柄
 * @return bool 是否成功
 */
bool 文件拖放_卸载(HWND 控件句柄);
