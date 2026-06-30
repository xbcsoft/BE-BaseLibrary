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

namespace {

// 移入移出事件数据结构
struct 移入移出事件结构 {
	HWND 句柄;
	PHoverCallback 移入事件;
	PHoverCallback 移出事件;
	WNDPROC 消息钩子句柄; // 原窗口过程
	bool 是否移入;
};

// 使用白易哈希表存储事件，键为窗口句柄，值为事件结构
// HashTbe 提供了极速的 O(1) 查找性能
HashTbe<HWND, 移入移出事件结构> 移入移出事件表;

// 窗口过程函数
LRESULT CALLBACK 子程序_窗口函数(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
	// 使用哈希表快速定位当前句柄对应的事件信息
	if (auto* item = 移入移出事件表.find(hWnd)) {
		if (Msg == WM_MOUSELEAVE) {
			item->是否移入 = false;
			if (item->移出事件) item->移出事件(hWnd);
		} else if (Msg == WM_MOUSEMOVE) {
			if (!item->是否移入) {
				TRACKMOUSEEVENT 跟踪事件 = { sizeof(TRACKMOUSEEVENT) };
				跟踪事件.dwFlags = TME_LEAVE;
				跟踪事件.hwndTrack = hWnd;
				跟踪事件.dwHoverTime = HOVER_DEFAULT;
				::TrackMouseEvent(&跟踪事件);

				item->是否移入 = true;
				if (item->移入事件) item->移入事件(hWnd);
			}
		}
		return ::CallWindowProc(item->消息钩子句柄, hWnd, Msg, wParam, lParam);
	}
	return ::DefWindowProc(hWnd, Msg, wParam, lParam);
}

} // namespace


/**
 * 控件_添加鼠标移入移出事件
 * @param 句柄 目标窗口或控件句柄
 * @param 移入事件 鼠标移入时的回调
 * @param 移出事件 鼠标移出时的回调
 */
void 控件_添加鼠标移入移出事件(HWND 句柄, PHoverCallback 移入事件, PHoverCallback 移出事件) {
	if (!IsWindow(句柄)) return;

	移入移出事件结构 zTmp;
	zTmp.句柄 = 句柄;
	zTmp.移入事件 = 移入事件;
	zTmp.移出事件 = 移出事件;
	zTmp.是否移入 = false;

	// 执行窗口改类（子类化）
	zTmp.消息钩子句柄 = (WNDPROC)SetWindowLongPtr(句柄, GWLP_WNDPROC, (LONG_PTR)子程序_窗口函数);

	// 将事件信息存入哈希表
	移入移出事件表.set(句柄, zTmp);
}