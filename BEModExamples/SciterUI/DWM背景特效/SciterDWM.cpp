#include "stdafx.h"
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

namespace {
// ============================================================================
// Win10 旧方案（SetWindowCompositionAttribute，未公开API）
// ============================================================================
enum ACCENT_STATE {
	ACCENT_DISABLED = 0,
	ACCENT_ENABLE_GRADIENT = 1,
	ACCENT_ENABLE_TRANSPARENTGRADIENT = 2,
	ACCENT_ENABLE_BLURBEHIND = 3,           // Win7毛玻璃
	ACCENT_ENABLE_ACRYLICBLURBEHIND = 4,   // Win10亚克力
	ACCENT_ENABLE_HOSTBACKDROP = 5,
	ACCENT_INVALID_STATE = 6
};

// ============================================================================
// Win11 官方方案及 DWM 常量定义
// ============================================================================
// DWMWA_TRANSITIONS_FORCEDISABLED = 3，强制禁用窗口动画
#ifndef DWMWA_TRANSITIONS_FORCEDISABLED
#define DWMWA_TRANSITIONS_FORCEDISABLED 3
#define DWMWA_WINDOW_CORNER_PREFERENCE 33  /*Win11 原生圆角*/
#define DWMWA_CAPTION_COLOR 35  /*Win11 禁用标题栏主题色*/
#define DWMWA_COLOR_NONE 0xFFFFFFFE
#define DWMWA_SYSTEMBACKDROP_TYPE 38
#endif

enum DWM_WINDOW_CORNER_PREFERENCE {
	DWMWCP_DEFAULT = 0,    // 系统默认
	DWMWCP_DONOTROUND = 1, // 直角（方形）
	DWMWCP_ROUND = 2,      // 标准大圆角（约 8~12px）
	DWMWCP_ROUNDSMALL = 3  // 小圆角（约 4px）
};

// DWM_SYSTEMBACKDROP_TYPE 枚举值
enum DWM_SYSTEMBACKDROP_TYPE {
	DWMSBT_AUTO = 0,              // 系统自动决定
	DWMSBT_NONE = 1,              // 无背景效果
	DWMSBT_MAINWINDOW = 2,       // Mica 材质
	DWMSBT_TRANSIENTWINDOW = 3, // Acrylic 亚克力
	DWMSBT_TABBEDWINDOW = 4     // Tabbed Mica
};

struct WINDOWCOMPOSITIONATTRIBDATA {
	DWORD Attrib; // WCA_ACCENT_POLICY = 19
	PVOID pvData;
	SIZE_T cbData;
};

typedef BOOL(WINAPI* pfnSetWindowCompositionAttribute)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);
struct ACCENT_POLICY {
	ACCENT_STATE AccentState;
	DWORD AccentFlags;
	DWORD GradientColor; // AABBGGRR
	DWORD AnimationId;
};

// ========================================================================
// Win10 方案：使用 SetWindowCompositionAttribute (Acrylic 亚克力)
// ========================================================================
bool 应用Accent模糊(HWND hwnd, ACCENT_STATE state, DWORD gradientColor = 0x01FFFFFF)
{
	HMODULE hUser = GetModuleHandleW(L"user32.dll");
	if (!hUser) return false;
	auto setComposition = (pfnSetWindowCompositionAttribute)GetProcAddress(hUser, "SetWindowCompositionAttribute");
	if (!setComposition) return false;

	ACCENT_POLICY policy = {};
	policy.AccentState = state;
	policy.AccentFlags = (state == ACCENT_ENABLE_ACRYLICBLURBEHIND) ? 2 : 0;
	policy.GradientColor = gradientColor;

	WINDOWCOMPOSITIONATTRIBDATA data = { 0 };
	data.Attrib = 19; // WCA_ACCENT_POLICY
	data.pvData = &policy;
	data.cbData = sizeof(policy);

	return setComposition(hwnd, &data) != FALSE;
}

// ========================================================================
// Win11(22H2+) 方案：使用官方 DwmSetWindowAttribute API
// ========================================================================
bool 应用Win11特效(HWND hwnd, DWM_SYSTEMBACKDROP_TYPE backdropType = DWMSBT_TRANSIENTWINDOW)
{
	// 1. 扩展窗口客户区
	MARGINS margins = { -1 };
	DwmExtendFrameIntoClientArea(hwnd, &margins);

	// 2. Win11 硬件级原生圆角
	DWM_WINDOW_CORNER_PREFERENCE corner = DWMWCP_ROUND;
	DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &corner, sizeof(corner));

	// 3. 禁用系统主题色（粉红色）对标题栏强制着色
	COLORREF captionColor = DWMWA_COLOR_NONE;
	DwmSetWindowAttribute(hwnd, DWMWA_CAPTION_COLOR, &captionColor, sizeof(captionColor));

	int type = (int)backdropType;
	HRESULT hr = DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &type, sizeof(type));

	return SUCCEEDED(hr);
}
}

// ============================================================================
// 系统版本检测
// ============================================================================
enum WinVersion {
	Win_Older,    // Win10 及更早
	Win11_21H2,   // Build 22000，支持 DWMWA_MICA_EFFECT
	Win11_22H2,   // Build 22621+，支持 DWMWA_SYSTEMBACKDROP_TYPE
};

WinVersion 检测Windows版本()
{
	int build = 取系统版本();
	if (build >= 22621) return Win11_22H2;
	if (build >= 22000) return Win11_21H2;
	return Win_Older;
}

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
void SciterDWM背景特效(SciterUI& st, SciterDWMType type, bool 亚克力系统一致性 = false)
{
	WinVersion ver = 检测Windows版本();
	HWND hwnd = st.hwnd;
	st._shadowMarginR = 0; //必须让原始边框为0，在DWM模式下不能有自绘外阴影，而使用系统原生阴影
	HELEMENT root = NULL; g_sapi->SciterGetRootElement(hwnd, &root);
	st._设置BodyMargin(root, L"0");

	if (ver==Win11_21H2 || type==SciterDWMType::Aero || (ver >= Win11_22H2 && 亚克力系统一致性)) {
		if (ver>=Win11_22H2)st._isTransparentFrame = true; //22H2以上Win11必透明边框否则无法显示Aero
		//注：Win11_21H2不支持亚克力特效，只能退化到Aero
		应用Accent模糊(hwnd, ACCENT_ENABLE_BLURBEHIND);
		return;
	}

	if (ver >= Win11_22H2)
	{
		应用Win11特效(hwnd, DWMSBT_TRANSIENTWINDOW);
		return;
	}

	// Win10：应用 Accent Acrylic 亚克力模糊
	应用Accent模糊(hwnd, ACCENT_ENABLE_ACRYLICBLURBEHIND);
}
