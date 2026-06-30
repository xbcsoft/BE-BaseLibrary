#pragma once
#include "stdafx.h"
#include <BEWin32Base/BEWin32Base.h>

typedef void(*MsgBoxCallback)(HWND);

extern "C" int WINAPI MessageBoxTimeoutW(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType, WORD wLanguageId, DWORD dwMilliseconds);

/**信息框_DIY
 * @param 按钮组标题
 * @param 提示信息
 * @param 按钮=0
 * @param 窗口标题=L"提示："
 * @param 创建回调<可空>
 * @return 
 */
int 信息框_DIY(c_Arraybe<StrX> 按钮组标题, c_StrX 提示信息, MB 按钮 = 0, c_StrX 窗口标题 = L"提示：",
	MsgBoxCallback 创建回调 = nullptr);

/**信息框_定时
 * @param 提示信息
 * @param 等待时间 毫秒
 * @param 窗口标题=L"提示："
 * @param 按钮=0
 * @param 句柄<可空>
 * @return 32000为超时，其他为按钮ID
 */
int 信息框_定时(c_StrX 提示信息, int 等待时间, c_StrX 窗口标题 = L"提示：", MB 按钮 = 0, 可空<HWND> 句柄 = 空);
