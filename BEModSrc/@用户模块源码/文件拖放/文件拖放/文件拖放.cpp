/**@ModuleTitle:文件拖放
*  @version:     1.0
*  @platform:    win32(x86|x64)
*  @compiler:    source
*  @author:
*  @datetime:
*  @description: 实现了窗口及控件的文件拖放注册与卸载。
*/
#include "stdafx.h"
#include <BECore/BECore.h>
#include <shellapi.h>

// 内部使用的属性名
#define PROP_OLDWNDPROC L"BE_DragDrop_OldWndProc"
#define PROP_CALLBACK   L"BE_DragDrop_Callback"
#define PROP_OLDEXSTYLE L"BE_DragDrop_OldExStyle"

// 以换行符"\r\n"进行分割
typedef void (CALLBACK* 文件拖放回调)(const wchar_t* 文件列表);

// 内部函数声明
namespace {
LRESULT CALLBACK _文件拖放_窗口回调(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
void _文件拖放_突破权限(HWND hwnd);
}

/**注册拖放功能到指定窗口或控件
 * @param 控件句柄 目标窗口或控件的句柄
 * @param 回调 接收到文件拖放后的回调函数
 * @return bool 是否成功
 */
bool 文件拖放_注册(HWND 控件句柄, 文件拖放回调 回调) {
	if (!IsWindow(控件句柄) || 回调 == nullptr) return false;

	// 检查是否已经注册
	if (GetPropW(控件句柄, PROP_OLDWNDPROC) != nullptr) return false;

	// 1. 设置窗口扩展风格
	LONG_PTR oldExStyle = GetWindowLongPtrW(控件句柄, GWL_EXSTYLE);
	SetWindowLongPtrW(控件句柄, GWL_EXSTYLE, oldExStyle | WS_EX_ACCEPTFILES);

	// 2. 备份信息
	SetPropW(控件句柄, PROP_OLDEXSTYLE, (HANDLE)oldExStyle);
	SetPropW(控件句柄, PROP_CALLBACK, (HANDLE)回调);

	// 3. 子类化
	WNDPROC oldWndProc = (WNDPROC)SetWindowLongPtrW(控件句柄, GWLP_WNDPROC, (LONG_PTR)_文件拖放_窗口回调);
	SetPropW(控件句柄, PROP_OLDWNDPROC, (HANDLE)oldWndProc);

	// 4. 突破权限限制
	_文件拖放_突破权限(控件句柄);
	return true;
}

/**
 * @brief 卸载指定窗口或控件的拖放功能
 * @param 控件句柄 目标窗口或控件的句柄
 * @return bool 是否成功
 */
bool 文件拖放_卸载(HWND 控件句柄) {
	if (!IsWindow(控件句柄)) return false;
	WNDPROC oldWndProc = (WNDPROC)GetPropW(控件句柄, PROP_OLDWNDPROC);
	if (oldWndProc == nullptr) return false;

	// 1. 还原窗口过程
	SetWindowLongPtrW(控件句柄, GWLP_WNDPROC, (LONG_PTR)oldWndProc);

	// 2. 还原扩展风格
	LONG_PTR oldExStyle = (LONG_PTR)GetPropW(控件句柄, PROP_OLDEXSTYLE);
	SetWindowLongPtrW(控件句柄, GWL_EXSTYLE, oldExStyle);

	// 3. 移除属性
	RemovePropW(控件句柄, PROP_OLDWNDPROC);
	RemovePropW(控件句柄, PROP_CALLBACK);
	RemovePropW(控件句柄, PROP_OLDEXSTYLE);
	return true;
}

namespace {

LRESULT CALLBACK _文件拖放_窗口回调(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
	WNDPROC oldWndProc = (WNDPROC)GetPropW(hwnd, PROP_OLDWNDPROC);
	文件拖放回调 callback = (文件拖放回调)GetPropW(hwnd, PROP_CALLBACK);

	if (iMsg == WM_DROPFILES) {
		HDROP hDrop = (HDROP)wParam;
		UINT count = DragQueryFileW(hDrop, 0xFFFFFFFF, nullptr, 0);

		StrW allFiles;
		for (UINT i = 0; i < count; ++i) {
			UINT size = DragQueryFileW(hDrop, i, nullptr, 0);
			if (size > 0) {
				StrW fileName;
				fileName.reset(size);
				DragQueryFileW(hDrop, i, (wchar_t*)fileName, size + 1);

				if (allFiles.len() > 0) allFiles += L"\r\n";
				allFiles += fileName;
			}
		}
		DragFinish(hDrop);

		if (callback && allFiles.len() > 0) {
			callback(allFiles);
		}
		return 0;
	}

	if (iMsg == WM_NCDESTROY) {
		文件拖放_卸载(hwnd);
		oldWndProc = (WNDPROC)GetWindowLongPtrW(hwnd, GWLP_WNDPROC);
	}

	if (oldWndProc) return CallWindowProcW(oldWndProc, hwnd, iMsg, wParam, lParam);
	return DefWindowProcW(hwnd, iMsg, wParam, lParam);
}

void _文件拖放_突破权限(HWND hwnd) {
	const UINT msgList[] = { WM_DROPFILES, WM_COPYDATA, 0x0049 };
	for (UINT msg : msgList) {
		ChangeWindowMessageFilter(msg, 1);
	}
}

} // namespace
