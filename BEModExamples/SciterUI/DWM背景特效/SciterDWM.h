#pragma once
#include "stdafx.h"
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

// ============================================================================
// 系统版本检测
// ============================================================================
enum WinVersion {
	Win_Older,    // Win10 及更早
	Win11_21H2,   // Build 22000，支持 DWMWA_MICA_EFFECT
	Win11_22H2,   // Build 22621+，支持 DWMWA_SYSTEMBACKDROP_TYPE
};

WinVersion 检测Windows版本();

enum class SciterDWMType {
	Aero = 0,
	Acrylic = 1
};

/**务必说个问题：从表现效果来看Win11压根就没有Aero效果
而Win11还能调用“应用Accent模糊(hwnd, ACCENT_ENABLE_BLURBEHIND)”
	而它的实现效果其实跟Win10的亚克力效果是一样的（不信你拿Win10-1709+测一下）
而Win11的亚克力特效实则又是另一种版本的高浓模糊，反正我个人还是喜欢Win10版的亚克力
不过亚克力的效果总有在DWM动画时有穿帮为背景透明度的效果，所以还是建议把DWM关掉
 * @param st
 * @param type
 * @param 亚克力系统一致性=false 如果为true，则在Win11上统一使用Aero参数（但实际表现出来的本身就是亚克力效果）
 */
void SciterDWM背景特效(SciterUI& st, SciterDWMType type, bool 亚克力系统一致性 = false);
