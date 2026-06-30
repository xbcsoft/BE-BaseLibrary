#include "stdafx.h"
#include <BEWin32Base/BEWin32Base.h>

typedef void(*MsgBoxCallback)(HWND);

extern "C" int WINAPI MessageBoxTimeoutW(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType, WORD wLanguageId, DWORD dwMilliseconds);

namespace {
HHOOK m_hMsgBoxHook;
StrW m_按钮组标题[8];
MsgBoxCallback g_MsgBoxCreateCallback;

LRESULT CALLBACK HookProc(int code, WPARAM wp, LPARAM lp) {
	if (code == HCBT_ACTIVATE) { // 5
		HWND hwnd = (HWND)wp;
		for (int i = 1; i <= 7; ++i) {
			if (m_按钮组标题[i].len() > 0) {
				SetDlgItemTextW(hwnd, i, m_按钮组标题[i]);
			}
		}
		if (g_MsgBoxCreateCallback) {
			g_MsgBoxCreateCallback(hwnd);
		}
		UnhookWindowsHookEx(m_hMsgBoxHook);
		m_hMsgBoxHook = NULL;
		g_MsgBoxCreateCallback = nullptr;
	}
	return CallNextHookEx(NULL, code, wp, lp);
}
}

/**信息框_DIY
 * @param 按钮组标题
 * @param 提示信息
 * @param 按钮=0
 * @param 窗口标题=L"提示："
 * @param 创建回调<可空>
 * @return 
 */
int 信息框_DIY(c_Arraybe<StrX> 按钮组标题, c_StrX 提示信息, MB 按钮 = 0, c_StrX 窗口标题 = L"提示：",
	MsgBoxCallback 创建回调 = nullptr)
{
	// 初始化/重置按钮组标题
	for (int i = 0; i < 8; ++i) {
		m_按钮组标题[i] = L"";
	}

	g_MsgBoxCreateCallback = 创建回调;

	auto getTitle = [&](int index) -> StrW {
		if (index < 按钮组标题.count) {
			return 按钮组标题[index];
		}
		return L"";
	};

	// 确认钮 (MB_OK = 0)
	if ((按钮 & MB::确认) == MB::确认) {
		m_按钮组标题[1] = getTitle(0);
	}
	// 确认取消钮 (MB_OKCANCEL = 1)
	if ((按钮 & MB::确认取消) == MB::确认取消) {
		m_按钮组标题[1] = getTitle(0);
		m_按钮组标题[2] = getTitle(1);
	}
	// 是钮 (IDYES = 6)
	if ((按钮 & MB::ID是) == MB::ID是) {
		m_按钮组标题[6] = getTitle(0);
	}
	// 是否钮 (MB_YESNO = 4)
	if ((按钮 & MB::是否) == MB::是否) {
		m_按钮组标题[6] = getTitle(0);
		m_按钮组标题[7] = getTitle(1);
	}
	// 是否取消 (MB_YESNOCANCEL = 3)
	if ((按钮 & MB::是否取消) == MB::是否取消) {
		m_按钮组标题[6] = getTitle(0);
		m_按钮组标题[7] = getTitle(1);
		m_按钮组标题[2] = getTitle(2);
	}
	// 重试取消 (MB_RETRYCANCEL = 5)
	if ((按钮 & MB::重试取消) == MB::重试取消) {
		m_按钮组标题[4] = getTitle(0);
		m_按钮组标题[2] = getTitle(1);
	}
	// 放弃重试忽略 (MB_ABORTRETRYIGNORE = 2)
	if ((按钮 & MB::放弃重试忽略) == MB::放弃重试忽略) {
		m_按钮组标题[3] = getTitle(0);
		m_按钮组标题[4] = getTitle(1);
		m_按钮组标题[5] = getTitle(2);
	}

	m_hMsgBoxHook = SetWindowsHookExW(WH_CBT, HookProc, NULL, GetCurrentThreadId());
	return 信息框(提示信息, 窗口标题, 按钮);
}

/**信息框_定时
 * @param 提示信息
 * @param 等待时间 毫秒
 * @param 窗口标题=L"提示："
 * @param 按钮=0
 * @param 句柄<可空>
 * @return 32000为超时，其他为按钮ID
 */
int 信息框_定时(c_StrX 提示信息, int 等待时间, c_StrX 窗口标题 = L"提示：", MB 按钮 = 0, 可空<HWND> 句柄 = 空)
{
	HWND hWnd = (句柄 != 空) ? (HWND)句柄 : GetActiveWindow();
	return MessageBoxTimeoutW(hWnd, 提示信息, 窗口标题, 按钮, 0, 等待时间);
}