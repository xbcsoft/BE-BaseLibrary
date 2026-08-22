/**@ModuleTitle:
*  @version:     1.0
*  @platform:    win32(x86|x64)
*  @compiler:    source
*  @author:
*  @datetime:
*  @description:
*/
#include "stdafx.h"
#include "SciterDom.h"
#include "NTCps.h"
#include "SciterZip.h"

ISciterAPI* g_sapi = nullptr;
HMODULE g_sciterModule = nullptr;
POINT g_sciterMaximizedMargin = {};

// 修改版 Sciter DLL 中预留的可写光标仲裁标志（x86/x64 各自固定 RVA）。
#ifdef _WIN64
static constexpr SIZE_T SCITER_CURSOR_SUPPRESS_FLAG_RVA = 0x7CF7F8;
#else
static constexpr SIZE_T SCITER_CURSOR_SUPPRESS_FLAG_RVA = 0x5F57F0;
#endif

class EXP SciterUI
{
public:
	/**仅需在全局初始化一次
	 */
	static void 全局初始化() {

#ifdef _WIN64
		Bytes dll = DLL::sciter64;
		StrA s_dll = "be\\sciter64_5.0.3.15.dll";
#else
		Bytes dll = DLL::sciter;
		StrA s_dll = "be\\sciter_5.0.3.15.dll";
#endif
//#ifdef _WIN64
//		s_dll = R"(C:\Users\admin\AppData\Local\Temp\be\sciter64_5.0.3.15_fixed01.dll)";
//#else
//		s_dll = R"(C:\Users\admin\AppData\Local\Temp\be\sciter_5.0.3.15.dll)";
//#endif

		if (!g_sapi) {
			g_sciterModule = NTCps::加载DLL(dll, s_dll);
			//g_sciterModule = LoadLibraryA((const char*)s_dll);

			auto getSciterAPI = (SciterAPI_ptr)GetProcAddress(g_sciterModule, "SciterAPI");
			g_sapi = _SAPI(getSciterAPI());

#ifdef _DEBUG
			g_sapi->SciterSetOption(NULL, SCITER_SET_SCRIPT_RUNTIME_FEATURES,
				ALLOW_FILE_IO | ALLOW_SOCKET_IO | ALLOW_EVAL | ALLOW_SYSINFO);
			g_sapi->SciterSetOption(NULL, SCITER_SET_DEBUG_MODE, TRUE);
#endif

			// Sciter 5.0.3.15 会在非空文本选区的终点继续绘制 caret。
			// 约一个系统闪烁周期后 caret 消失，肉眼看起来就像选区在收尾时又闪了一次。
			static const BYTE masterFixCss[] =
				"[sciter-selection-caret-hidden]{text-selection-caret-color:transparent !important;}"
				"menu.context {min-width: 150px !important}"
				"menu.context>li{padding:4px 16px !important;foreground-image:none !important;flow:horizontal !important;}"
				"menu.context>li>span{margin-left:* !important;}";
			g_sapi->SciterAppendMasterCSS(masterFixCss, sizeof(masterFixCss) - 1);

			const int frameX = GetSystemMetrics(SM_CXSIZEFRAME) +
				GetSystemMetrics(SM_CXPADDEDBORDER);
			const int frameY = GetSystemMetrics(SM_CYSIZEFRAME) +
				GetSystemMetrics(SM_CXPADDEDBORDER);
			const int padX = static_cast<int>(rdpi(frameX) + 0.5f);
			const int padY = static_cast<int>(rdpi(frameY) + 0.5f);
			g_sciterMaximizedMargin.x = padX > 1 ? padX - dpi(1) : 0;
			g_sciterMaximizedMargin.y = padY > 1 ? padY - dpi(1) : 0;
		}
	}

	struct 参数
	{
		可空<StrU8> 内存_html; //UTF8编码的内存字符串数据
		可空<StrU8> 内存_url; //如果填了内存_html，则该参数默认为L"file:///index.html"
		可空<StrU8> 文件_html; //html文件的路径
		可空<Bytes> 内存_zip; //内存中ZIP文件的二进制数据
		bool 是否有启动动画 = false; //如果有启动动画则默认禁止DWM过渡，开启后将直接令窗口可视为假（之后由JS那边控制窗口可视）
		//如后续还需要DWM联动，则可 推迟调用子程序(一定毫秒数) 到动画结束后开启。
	}static _df;

	WNDPROC origWndProc = NULL;
	HWND hwnd = NULL;
	int _shadowMargin = 0;
	int _shadowMarginR = 0;
	bool _isTransparentFrame = false; // 是否声明了由 HTML 完整接管非客户区的自绘框架
	bool _isDragging = false; // 是否正在被拖动或改变大小
	static void _设置Sciter内部光标屏蔽(bool enabled)
	{
		if (g_sciterModule) {
			auto* flag = reinterpret_cast<volatile BYTE*>(
				reinterpret_cast<BYTE*>(g_sciterModule) + SCITER_CURSOR_SUPPRESS_FLAG_RVA);
			*flag = enabled ? 1 : 0;
		}
	}

	static bool _允许窗口边缘拉伸(HWND targetHwnd)
	{
		auto* targetWindow = reinterpret_cast<窗口*>(窗口_句柄取对象(targetHwnd));
		return !targetWindow || (int)targetWindow->边框 == 1;
	}

	void _设置BodyMargin(HELEMENT root, LPCWSTR marginStr)
	{
		HELEMENT bodyEl = NULL;
		g_sapi->SciterGetNthChild(root, 1, &bodyEl);
		g_sapi->SciterSetStyleAttribute(bodyEl, "margin", marginStr);
	}

	static HELEMENT _查找文本编辑框(HELEMENT target)
	{
		for (HELEMENT el = target; el;) {
			LPCSTR type = nullptr;
			if (g_sapi->SciterGetElementType(el, &type) == SCDOM_OK && type &&
				(_stricmp(type, "input") == 0 || _stricmp(type, "textarea") == 0 ||
					_stricmp(type, "plaintext") == 0 || _stricmp(type, "htmlarea") == 0))
				return el;

			HELEMENT parent = NULL;
			if (g_sapi->SciterGetParentElement(el, &parent) != SCDOM_OK || !parent)
				break;
			el = parent;
		}
		return NULL;
	}

	static void _同步文本选区光标(HELEMENT target, bool prepareForSelection)
	{
		HELEMENT editor = _查找文本编辑框(target);
		if (!editor) return;

		if (prepareForSelection) {
			g_sapi->SciterSetAttributeByName(
				editor, "sciter-selection-caret-hidden", L"");
			return;
		}

		static const wchar_t syncScript[] =
			L"var api = this.edit || this.textarea || this.plaintext;"
			L"var selected = api && typeof api.selectionText === 'string' && api.selectionText.length > 0;"
			L"if(selected) this.setAttribute('sciter-selection-caret-hidden','');"
			L"else this.removeAttribute('sciter-selection-caret-hidden');";
		SciterObj r;
		g_sapi->SciterEvalElementScript(
			editor, syncScript, (UINT)wcslen(syncScript), &r.val);
	}

	// -1: 尚未同步；0: 普通窗口；1: 最大化；2: 贴靠；3: 仅垂直铺满。
	// 最小化不是一种阴影布局，必须保留最小化前的状态，否则还原时会
	// 重复改写 DOM 样式并导致透明窗口表面重建。
	int _shadowLayoutState = -1;

	bool _应用阴影布局状态(int newLayoutState, BOOL forceRender)
	{
		if (_shadowMargin <= 0 ||
			newLayoutState == _shadowLayoutState)
			return false;

		HELEMENT root = NULL;
		if (g_sapi->SciterGetRootElement(hwnd, &root) != SCDOM_OK || !root)
			return false;
		wchar_t margin[64];
		if (newLayoutState == 1) {
			// 最大化窗口的原生边框位于工作区之外。这里使用系统边框宽度
			// 抵消那部分越界，而不是继续使用 HTML 的外围边距。
			// 因而屏幕可见区域中的 body 恰好从 (0,0) 开始，不会镂空，
			// 同时也不会因直接 margin:0 而让内容被原生边框裁掉。
			swprintf(margin, 64, L"%dpx %dpx",
				g_sciterMaximizedMargin.y, g_sciterMaximizedMargin.x);
			_设置BodyMargin(root, margin);
		} else if (newLayoutState == 2) {
			_设置BodyMargin(root, L"0px");
		}else if (newLayoutState == 3) {
			// 双击顶部/底部缩放边框只会把窗口垂直铺满。上下阴影应消失，
			// 但左右阴影仍须保留，否则可见内容会凭空增宽 2 * shadowMargin。
			swprintf(margin, 48, L"0px %dpx", _shadowMarginR);
			_设置BodyMargin(root, margin);
		} else {
			swprintf(margin, 32, L"%dpx", _shadowMarginR);
			_设置BodyMargin(root, margin);
		}

		_shadowLayoutState = newLayoutState;
		return true;
	}

	~SciterUI()
	{
		_zip.关闭();
	}

	virtual UINT 事件_资源加载(LPSCN_LOAD_DATA pld) {
		return LOAD_OK;
	}

	/**
	 * @brief 阴影窗口拉伸/命中测试事件 (WM_NCHITTEST)
	 * 供派生类/用户高度定制无边框阴影窗口边缘拉伸与区域命中结果。
	 *
	 * @param x, y               鼠标在窗口内部的相对坐标 (0 <= x <= w, 0 <= y <= h)
	 * @param w, h               窗口当前的实际宽度与高度
	 * @param inLeft, inRight    默认计算出的左/右窄边热区命中状态
	 * @param inTop, inBottom    默认计算出的上/下窄边热区命中状态
	 * @param inCornerLeft       默认计算出的左侧角区 (左上/左下) 命中状态
	 * @param inCornerRight      默认计算出的右侧角区 (右上/右下) 命中状态
	 * @param inCornerTop        默认计算出的顶部角区 (左上/右上) 命中状态
	 * @param inCornerBottom     默认计算出的底部角区 (左下/右下) 命中状态
	 *
	 * @return 可空<char>        返回 nil 表示不进行自定义，沿用系统默认判定；
	 *                           返回具体的 Win32 HitTest 码 (如 HTTOPLEFT=13, HTRIGHT=11, HTCLIENT=1 等)
	 *                           则直接作为 WM_NCHITTEST 的响应返回值。
	 */
	virtual 可空<char> 事件_窗口外围被拉伸(
		int x, int y, int w, int h,
		bool inLeft, bool inRight, bool inTop, bool inBottom,
		bool inCornerLeft, bool inCornerRight, bool inCornerTop, bool inCornerBottom) {
		return nil;
	}


	// Sciter原版的鼠标滚轮存在滚动中途产生僵硬问题，我们这里接管滚动让其丝滑运行
	static BOOL SC_CALLBACK _StaticWindowMouseEventHandler(LPVOID tag, HELEMENT he, UINT evtg, LPVOID prms) {
		if (!tag) return FALSE;
		if (evtg == HANDLE_BEHAVIOR_EVENT) {
			auto* p = (BEHAVIOR_EVENT_PARAMS*)prms;
			// 剪切、删除、撤销等操作会折叠选区，但不会经过鼠标抬起事件。
			// 在值变更完成后重新检查选区，及时移除透明 caret 属性。
			if ((p->cmd & SINKING) == 0 && (p->cmd & ~SINKING) == VALUE_CHANGED)
				_同步文本选区光标(p->heTarget, false);
			return FALSE;
		}
		if (evtg == HANDLE_KEY) {
			auto* p = (KEY_PARAMS*)prms;
			if ((p->cmd & 0xFF) == KEY_DOWN && p->key_code == 'A' &&
				(p->alt_state & CONTROL_KEY_PRESSED) != 0) {
				// 捕获阶段先隐藏；冒泡阶段全选已经完成，再按实际选区同步。
				_同步文本选区光标(p->target, (p->cmd & SINKING) != 0);
			}
			return FALSE;
		}
		if (evtg != HANDLE_MOUSE) return FALSE;
		auto* p = (MOUSE_PARAMS*)prms;
		auto* self = (SciterUI*)tag;
		const UINT mouseCommand = p->cmd & 0xFF;
		// 窗口事件处理器会最先收到 SINKING（捕获）阶段。只在这一阶段
		// 截断拉伸热区内的事件，使目标 DOM/原生滚动条没有机会改写光标。
		// 从客户区进入非客户区时首先可能出现 MOUSE_LEAVE，因此不能只拦 MOVE。
		if ((p->cmd & SINKING) != 0 &&
			(mouseCommand == MOUSE_ENTER || mouseCommand == MOUSE_LEAVE ||
				mouseCommand == MOUSE_MOVE || mouseCommand == MOUSE_IDLE)) {
			UINT forcedCursorId = CURSOR_ARROW;
			if (self->_强制自定义拉伸光标(&forcedCursorId)) {
				p->cursor_type = forcedCursorId;
				return TRUE;
			}
		}

		// 不要在捕获阶段改写 textarea 属性。Sciter 5 在处理按下位置前若收到
		// DOM 属性变更，会重建多行编辑器并把内部滚动位置归零，后续点击坐标
		// 也会按回顶后的内容解释。等引擎完成选择后再同步属性，仍赶在绘制前生效。
		if ((p->cmd & SINKING) == 0 &&
			(mouseCommand == MOUSE_UP || mouseCommand == MOUSE_DCLICK ||
			(mouseCommand == MOUSE_MOVE && (p->button_state & MAIN_MOUSE_BUTTON) != 0))) {
			_同步文本选区光标(p->target, false);
			return FALSE;
		}

		// 滚轮完全交给补丁 DLL；应用层不查找容器、不累计目标、不拦截事件。
		return FALSE;
	}

	//在 Sciter 引擎里，向它提供资源数据有两条平行且互斥的通道：
	//	通道 A：直接写入成员字段（延迟拷贝）
	//	做法：赋值 pld->outData = 缓冲区指针; 。
	//	过程：回调函数返回 LOAD_OK->Sciter 引擎拿到控制权->引擎读取该指针并复制数据。
	//	弊端：必须保证指针在函数返回后依然有效（所以之前需要使用 static）。

	//	通道 B：调用 SciterDataReady（即时拷贝，我们现在使用的方法）
	//	做法：调用 SciterDataReady(hwnd, uri, 数据, 长度)。
	//	过程：当场立即拷贝。在 SciterDataReady 函数被执行的瞬间，数据就已经被塞进 Sciter 引擎的统一缓存区里了。
	//	优势：函数返回前数据就已经安全到达引擎，局部变量死活无所谓了。
	int 资源返回(LPSCN_LOAD_DATA pld, c_Bytes 数据, int 返回码 = LOAD_OK)
	{
		SciterDataReady(hwnd, pld->uri, 数据.buf, 数据.size);
		pld->outData = nullptr; pld->outDataSize = 0;
		return 返回码;
	}

	int 资源返回(LPSCN_LOAD_DATA pld, c_StrA 数据, int 返回码 = LOAD_OK)
	{
		SciterDataReady(hwnd, pld->uri, 数据.bytes, 数据.len());
		pld->outData = nullptr; pld->outDataSize = 0;
		return 返回码;
	}

	//  宏	作用对象	含义
	//	WS_CLIPSIBLINGS	同级子窗口之间；
		/*当父窗口有多个子窗口（siblings），且它们在位置上有重叠时：
		  不设 WS_CLIPSIBLINGS：子窗口 A 调用 WM_PAINT 时，GDI 绘图会"穿透"到兄弟子窗口 B 的区域，导致 B 的内容被覆盖/污染。
		  设了 WS_CLIPSIBLINGS：Windows 在给 A 发 WM_PAINT 前，会把 B 的矩形区域从 A 的 DC clip region 中剪掉，A 的绘制绝对不会画到 B 的区域里。
		*/
	//	WS_CLIPCHILDREN	父窗口与其子窗口	我自己(是父)画的时候裁掉我的子窗口所在区域（这样父的部分不用重复绘制）
	void _支持Win32子控件()
	{
		窗口_添加风格(hwnd, WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	}

	void 创建(const 参数& cs, 窗口* 父窗口)
	{
		if (origWndProc != NULL) return; // 防重入保护
		hwnd = 父窗口->窗口句柄;
		父窗口->标记_置整数((size_t)this);

		// 如果有内存zip，直接通过内存二进制数据流式加载打开
		if (cs.内存_zip != nil) {
			_zip.打开((Bytes&)cs.内存_zip.val);
		}

		// 1. 进行主动子类化，接管窗口消息路由，使用白易内置 of 窗口_句柄取对象 实现超低开销的对象访问
		origWndProc = (WNDPROC)SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)(WNDPROC)[](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) -> LRESULT {
			auto* pThis = (SciterUI*)reinterpret_cast<窗口*>(窗口_句柄取对象(hwnd))->标记_取整数();
			BOOL handled = FALSE; LRESULT lr = 0;

			// 按当前屏幕坐标重新询问用户定义的拉伸热区。
			// WM_SETCURSOR 不能只依赖上一次 WM_NCHITTEST 的缓存，
			// 否则快速移出热区时会短暂残留错误光标。
			auto queryCustomResizeHit = [&](POINT screenPoint) -> 可空<char> {
				if (pThis->_shadowMargin <= 0 || IsZoomed(hwnd)) return nil;
				if (!pThis->_允许窗口边缘拉伸(hwnd)) return nil;
				RECT rc = {};
				if (!GetWindowRect(hwnd, &rc)) return nil;
				const int x = screenPoint.x - rc.left;
				const int y = screenPoint.y - rc.top;
				const int w = rc.right - rc.left;
				const int h = rc.bottom - rc.top;
				const int activeMargin = dpi(pThis->_shadowMargin - 1);
				const int cornerMargin = dpi(pThis->_shadowMargin < 10 ? 10 : pThis->_shadowMargin);

				const bool inLeft = x >= 0 && x < activeMargin;
				const bool inRight = x >= w - activeMargin && x < w;
				const bool inTop = y >= 0 && y < activeMargin;
				const bool inBottom = y >= h - activeMargin && y < h;
				const bool inCornerLeft = x >= 0 && x < cornerMargin;
				const bool inCornerRight = x >= w - cornerMargin && x < w;
				const bool inCornerTop = y >= 0 && y < cornerMargin;
				const bool inCornerBottom = y >= h - cornerMargin && y < h;

				return pThis->事件_窗口外围被拉伸(
					x, y, w, h,
					inLeft, inRight, inTop, inBottom,
					inCornerLeft, inCornerRight, inCornerTop, inCornerBottom);
			};

			auto setCustomResizeCursor = [](char hit) -> bool {
				LPCWSTR cursorId = nullptr;
				switch (hit) {
				case HTLEFT:
				case HTRIGHT: cursorId = IDC_SIZEWE; break;
				case HTTOP:
				case HTBOTTOM: cursorId = IDC_SIZENS; break;
				case HTTOPLEFT:
				case HTBOTTOMRIGHT: cursorId = IDC_SIZENWSE; break;
				case HTTOPRIGHT:
				case HTBOTTOMLEFT: cursorId = IDC_SIZENESW; break;
				default:
					_设置Sciter内部光标屏蔽(false);
					return false;
				}
				_设置Sciter内部光标屏蔽(true);
				SetCursor(LoadCursorW(nullptr, cursorId));
				return true;
			};

			switch (msg) {
			case WM_ENTERSIZEMOVE: {
				pThis->_isDragging = true; break;
			}
			case WM_EXITSIZEMOVE: {
				pThis->_isDragging = false; break;
			}
			case WM_WINDOWPOSCHANGING: {
				auto* wp = (WINDOWPOS*)lParam;
				if ((wp->flags & SWP_NOSIZE) && pThis->_isDragging) {
					// 仅移动窗口（且处于拖动状态）时，完全绕过 Sciter 引擎的计算，防止引擎在内部进行排版计算导致拖动阻力
					return CallWindowProcW(pThis->origWndProc, hwnd, msg, wParam, lParam);
				}
				break;
			}
			case WM_NCPAINT: // 拦截非客户区重绘，防止拖动窗口时 DWM 尝试使用 GDI 绘制边框导致严重的掉帧和卡顿
			case WM_NCCALCSIZE:
			{
				if (pThis->_isTransparentFrame) {
					// 拦截 WM_NCCALCSIZE 防止倒扣标题栏导致缩水，同时不论 wParam 真假都返回 0
					// 消除原生非客户区尺寸，完美保留 Win32 原生行为同时由我们自绘
					return 0;
				}
			}
			case WM_NCACTIVATE: {
				if (pThis->_isTransparentFrame) { //
					// 拦截原生标题栏激活状态绘制，并返回 1 表示允许状态变更，但不交由 DefWindowProc 绘制
					return 1;
				}
				break;
			}
			case WM_SIZE: {
				if (pThis->_shadowMargin > 0) {
					// 拖动拉伸的每一步也会收到 SIZE_RESTORED。旧实现每一步都
					// 清空并恢复 box-shadow，造成透明表面反复重建和底部残影。
					// 这里只在“普通 <-> 最大化”真正切换时更新一次布局。
					int newLayoutState = pThis->_shadowLayoutState;
					if (wParam == SIZE_MAXIMIZED) {
						newLayoutState = 1;
					} else if (wParam == SIZE_RESTORED) {
						newLayoutState = 0;
						RECT rc;
						GetWindowRect(hwnd, &rc);
						HMONITOR hMon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
						MONITORINFO mi = { sizeof(mi) };
						if (GetMonitorInfoW(hMon, &mi)) {
							int w = rc.right - rc.left;
							int h = rc.bottom - rc.top;
							int workW = mi.rcWork.right - mi.rcWork.left;
							int workH = mi.rcWork.bottom - mi.rcWork.top;

							const bool fillsHeight = h >= workH - 2;
							const bool fillsWidth = w >= workW - 2;
							const bool halfWidth = abs(w - workW / 2) < 10;
							const bool quarter = halfWidth && abs(h - workH / 2) < 10;

							// 双击顶部/底部缩放边框产生的是“仅垂直铺满”：高度占满，
							// 宽度仍是原窗口宽度。它不能与左右半屏贴靠共用 padding:0。
							if (fillsHeight && !fillsWidth && !halfWidth) {
								newLayoutState = 3;
							} else if (fillsHeight || fillsWidth || quarter) {
								newLayoutState = 2; // 贴边状态 (Snapped)
							}
						}
					}

					pThis->_应用阴影布局状态(newLayoutState, FALSE);
				}
				break;
			}
			case WM_ACTIVATE: {
				// Sciter 5 嵌入现有 Win32 窗口时不会自动维护
					// :window-active。把原生激活状态显式同步为 DOM 属性，
					// 供 HTML 使用 html[window-active] 选择器。
				HELEMENT root = NULL;
				if (g_sapi->SciterGetRootElement(hwnd, &root) == SCDOM_OK && root) {
					const bool active = LOWORD(wParam) != WA_INACTIVE;
					g_sapi->SciterSetAttributeByName(
						root, "window-active", active ? L"" : nullptr);
				}
				break;
			}
			case WM_SYSCOMMAND: {
				WORD cmd = (WORD)(wParam & 0xFFF0);
				const bool restoringFromMaximized =
					cmd == SC_RESTORE && IsZoomed(hwnd) && !IsIconic(hwnd);
				auto* pWin = reinterpret_cast<窗口*>(窗口_句柄取对象(hwnd));
				if (pWin) {
					if (cmd == SC_MINIMIZE && !pWin->最小化按钮) {
						return 0; // C++ 侧拦截：如果 最小化按钮 == false，直接禁止最小化！
					}
					if (cmd == SC_MAXIMIZE && !pWin->最大化按钮) {
						return 0; // C++ 侧拦截：如果 最大化按钮 == false，直接禁止最大化！
					}
					if (cmd == SC_RESTORE && IsZoomed(hwnd) && !pWin->最大化按钮) {
						return 0; // 仅在从最大化还原且无最大化按钮时拦截；从最小化还原 (IsIconic) 永远放行！
					}
				}

				// 必须在 DefWindowProc/Sciter 启动 DWM 窗口动画之前提交目标
				// padding。否则动画第一帧仍是普通窗口的阴影镂空画面，
				// WM_SIZE 到达后才会发生肉眼可见的二次纠正。
				if (cmd == SC_MAXIMIZE) {
					pThis->_应用阴影布局状态(1, TRUE);
				} else if (restoringFromMaximized) {
					pThis->_应用阴影布局状态(0, TRUE);
				}
				break;
			}
			case WM_NCLBUTTONDBLCLK: {
				if (wParam == HTCAPTION) {
					auto* pWin = reinterpret_cast<窗口*>(窗口_句柄取对象(hwnd));
					if (pWin && !pWin->最大化按钮) {
						return 0; // 标题栏双击：如果没有最大化按钮，禁止双击标题栏最大化！
					}
				}
				break;
			}
			case WM_NCLBUTTONDOWN: {
				switch ((int)wParam) {
				case HTLEFT:
				case HTRIGHT:
				case HTTOP:
				case HTBOTTOM:
				case HTTOPLEFT:
				case HTTOPRIGHT:
				case HTBOTTOMLEFT:
				case HTBOTTOMRIGHT:
					// 拉伸区完全归原生窗口过程处理，避免 Sciter 吞掉
					// 非客户区按下消息而只显示缩放光标却无法拖动。
					return CallWindowProcW(pThis->origWndProc, hwnd, msg, wParam, lParam);
				}
				if (pThis->_shadowMargin > 0 &&
					IsZoomed(hwnd) && wParam == HTCLOSE) {
					PostMessageW(hwnd, WM_CLOSE, 0, 0);
					return 0;
				}
				break;
			}
			case WM_SETCURSOR: {
				if (!pThis->_允许窗口边缘拉伸(hwnd)) {
					switch (LOWORD(lParam)) {
					case HTLEFT: case HTRIGHT: case HTTOP: case HTBOTTOM:
					case HTTOPLEFT: case HTTOPRIGHT: case HTBOTTOMLEFT: case HTBOTTOMRIGHT:
						_设置Sciter内部光标屏蔽(true);
						SetCursor(LoadCursorW(nullptr, IDC_ARROW));
						return TRUE;
					}
				}
				POINT screenPoint = {};
				if (GetCursorPos(&screenPoint)) {
					可空<char> customHit = queryCustomResizeHit(screenPoint);
					if (customHit != nil && setCustomResizeCursor((char)customHit)) {
						return TRUE;
					}
				}
				_设置Sciter内部光标屏蔽(false);
				break;
			}
			case WM_NCHITTEST: {
				// 每次命中测试先释放屏蔽；只有确认返回拉伸命中码时才重新置位。
				_设置Sciter内部光标屏蔽(false);

				if (!pThis->_允许窗口边缘拉伸(hwnd)) {
					lr = SciterProcND(hwnd, msg, wParam, lParam, &handled);
					if (handled) {
						switch (lr) {
						case HTLEFT: case HTRIGHT: case HTTOP: case HTBOTTOM:
						case HTTOPLEFT: case HTTOPRIGHT: case HTBOTTOMLEFT: case HTBOTTOMRIGHT:
							return HTCLIENT;
						default:
							return lr;
						}
					}
					return HTCLIENT;
				}

				if (pThis->_shadowMargin > 0) {
					if (IsZoomed(hwnd)) {
						POINT screenPoint = {
							(short)LOWORD(lParam),
							(short)HIWORD(lParam)
						};
						MONITORINFO monitorInfo = { sizeof(monitorInfo) };
						HMONITOR monitor =
							MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);

						// 最大化时屏幕最顶部 (y == rcWork.top) 恰好位于 DOM 边界上，
						// 向内容区探测 1 个物理像素直接请 Sciter 引擎识别。
						// 若右上角命中关闭按钮则返回 HTCLOSE，其余交给下方映射为 HTCAPTION。
						if (GetMonitorInfoW(monitor, &monitorInfo) &&
							screenPoint.y == monitorInfo.rcWork.top) {
							LPARAM probeLparam = MAKELPARAM(screenPoint.x, monitorInfo.rcWork.top + 1);
							BOOL probeHandled = FALSE;
							LRESULT probeLr = SciterProcND(hwnd, msg, wParam, probeLparam, &probeHandled);
							if (probeHandled && probeLr == HTCLOSE) {
								return HTCLOSE;
							}
						}

						// 最大化时全屏为 HTCLIENT，彻底杜绝边缘误拉伸
						lr = SciterProcND(hwnd, msg, wParam, lParam, &handled);
						if (handled) {
							switch (lr) {
								// 最大化窗口不能从顶部继续拉伸，但原生窗口会把
								// 最顶端视作标题栏：可向下拖动还原，也可双击还原。
							case HTCAPTION:
							case HTTOP:
							case HTTOPLEFT:
							case HTTOPRIGHT:
								return HTCAPTION;
							case HTRIGHT:
							case HTLEFT:
							case HTBOTTOM:
							case HTBOTTOMLEFT:
							case HTBOTTOMRIGHT:
								return HTCLIENT;
							}
						}
						return HTCLIENT;
					}

					POINT pt = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
					RECT rc; GetWindowRect(hwnd, &rc);
					int x = pt.x - rc.left;
					int y = pt.y - rc.top;
					int w = rc.right - rc.left;
					int h = rc.bottom - rc.top;

					int activeMargin = dpi(pThis->_shadowMargin -1);
					int cornerMargin = dpi(pThis->_shadowMargin < 10 ? 10 : pThis->_shadowMargin);

					// 仅在外围阴影区及微调区内触发 Win32 原生拉伸，物理分辨率缩放后依然保留充足安全空间给滚动条
					bool inLeft = (x >= 0 && x < activeMargin);
					bool inRight = (x >= w - activeMargin && x < w);
					bool inTop = (y >= 0 && y < activeMargin);
					bool inBottom = (y >= h - activeMargin && y < h);

					// 四角使用独立的大热区，不要求鼠标同时落入两条较窄的
					// 边缘热区。这样右下角斜向拉伸容易触发，又不会扩大整条
					// 右边缘去抢占内容区或滚动条。
					bool inCornerLeft = (x >= 0 && x < cornerMargin);
					bool inCornerRight = (x >= w - cornerMargin && x < w);
					bool inCornerTop = (y >= 0 && y < cornerMargin);
					bool inCornerBottom = (y >= h - cornerMargin && y < h);

					// 优先触发回调事件，供用户高度定制阴影窗口拉伸/命中逻辑
					可空<char> customRes = pThis->事件_窗口外围被拉伸(
						x, y, w, h,
						inLeft, inRight, inTop, inBottom,
						inCornerLeft, inCornerRight, inCornerTop, inCornerBottom);
					if (customRes != nil) {
						// 命中测试阶段立即切换光标，消除从 DOM/滚动条进入
						// 自定义拉伸热区时等待 WM_SETCURSOR 的单帧闪烁。
						setCustomResizeCursor((char)customRes);
						return (LRESULT)(char)customRes;
					}

					if (inCornerLeft && inCornerTop) {
						setCustomResizeCursor(HTTOPLEFT); return HTTOPLEFT;
					}
					if (inCornerRight && inCornerTop) {
						setCustomResizeCursor(HTTOPRIGHT); return HTTOPRIGHT;
					}
					if (inCornerLeft && inCornerBottom) {
						setCustomResizeCursor(HTBOTTOMLEFT); return HTBOTTOMLEFT;
					}
					if (inCornerRight && inCornerBottom) {
						setCustomResizeCursor(HTBOTTOMRIGHT); return HTBOTTOMRIGHT;
					}

					// 四边仍维持较窄的命中范围。
					if (inLeft) { setCustomResizeCursor(HTLEFT); return HTLEFT; }
					if (inRight) { setCustomResizeCursor(HTRIGHT); return HTRIGHT; }
					if (inTop) { setCustomResizeCursor(HTTOP); return HTTOP; }
					if (inBottom) { setCustomResizeCursor(HTBOTTOM); return HTBOTTOM; }

					// 处于内部 body 内容区域（包含滚动条），完全交由 Sciter 处理，彻底消除游标反复闪烁与干扰
					lr = SciterProcND(hwnd, msg, wParam, lParam, &handled);
					if (handled) return lr;

					return HTCLIENT;
				}
				break;
			}
			case WM_SETTINGCHANGE: {
				LPCWSTR section = (LPCWSTR)lParam;
				if (section && wcscmp(section, L"ImmersiveColorSet") == 0) {
					bool isDark = (读注册表整数(L"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize\\AppsUseLightTheme") == 0);

					HELEMENT root = NULL;
					if (g_sapi && g_sapi->SciterGetRootElement(hwnd, &root) == SCDOM_OK && root) {
						wchar_t code[128] = {};
						swprintf(code, 128, L"if(typeof window.onSystemThemeChanged==='function') window.onSystemThemeChanged(%s);", isDark ? L"true" : L"false");
						VALUE dummy;
						g_sapi->ValueInit(&dummy);
						g_sapi->SciterEvalElementScript(root, code, (UINT)wcslen(code), &dummy);
						g_sapi->ValueClear(&dummy);
					}
				}
				break;
			}
			default:
				break;
			}
			lr = SciterProcND(hwnd, msg, wParam, lParam, &handled);
			if (handled) return lr;
			lr = CallWindowProcW(pThis->origWndProc, hwnd, msg, wParam, lParam);
			return lr;
		});

		// 2. 主动向 SciterProcND 发送 WM_CREATE 进行引擎的绑定初始化
		BOOL handled = FALSE;
		SciterProcND(hwnd, WM_CREATE, 0, 0, &handled);

		// C++ 侧接管原生 DOM 鼠标滚轮事件，完全由 C++ 精确掌控滚动与累加
		g_sapi->SciterWindowAttachEventHandler(hwnd, _StaticWindowMouseEventHandler, this,
			HANDLE_MOUSE | HANDLE_KEY | HANDLE_BEHAVIOR_EVENT);

		// 3. 注册 SciterUI 回调处理函数，用于资源请求拦截等
		SciterSetCallback(hwnd, [](LPSCITER_CALLBACK_NOTIFICATION pns, LPVOID callbackParam) -> UINT {
			auto* pThis = (SciterUI*)callbackParam;
			if (pns->code == SC_SET_CURSOR) {
				UINT forcedCursorId = CURSOR_ARROW;
				if (pThis->_强制自定义拉伸光标(&forcedCursorId)) {
					// 宿主拉伸热区活动：宿主已经提交拉伸光标，DLL 包装器会
					// 仅跳过 Sciter 紧随其后的这次 SetCursor。
					_设置Sciter内部光标屏蔽(true);
					return 1;
				}

				// 普通 DOM 区域完全交还 Sciter，保留 I-beam、hand、URL 自定义
				// 光标及引擎内部的完整状态切换。
				_设置Sciter内部光标屏蔽(false);
				return 0;
			}
			if (pns->code == SC_LOAD_DATA) {
				auto* pld = (LPSCN_LOAD_DATA)pns;
				StrW uri(pld->uri);
				dbg_print("SciterUI SC_LOAD_DATA: ", uri);

				Bytes fileData;

				// 优先从 ZIP 内存中提取资源并填充至 outData
				if (pThis->_zip.是否已打开()) {
					StrW relPathW = uri;
					if (文本_对比左边<W>(uri, L"file:///")) {
						relPathW = (const charW*)uri + 8;
					}
					StrU8 relPath = WtoU8(relPathW);
					relPath = 子文本替换(relPath, "\\", "/");

					fileData = pThis->_zip.读取成员(relPath);
					if (fileData) {
						pld->outData = fileData.buf;
						pld->outDataSize = (UINT)fileData.size;
						dbg_print("Loaded from ZIP: ", relPath);
					}
				}

				// 后期再委托给虚函数事件进行改写/覆盖/处理
				UINT res = pThis->事件_资源加载(pld);
				if (res == LOAD_OK && pld->outData) {
					SciterDataReady(pThis->hwnd, pld->uri, pld->outData, pld->outDataSize);
					return LOAD_OK;
				}
				return res;
			}
			return LOAD_OK;
		}, this);

		// 4. 载入 HTML 渲染内容
		if (cs.文件_html != nil) {
			StrW path = U8toW(cs.文件_html);
			wchar_t absPath[MAX_PATH];
			if (GetFullPathNameW(path, MAX_PATH, absPath, nullptr) > 0) {
				SciterLoadFile(hwnd, absPath);
			} else {
				SciterLoadFile(hwnd, path);
			}
		} else if (cs.内存_html != nil) {
			const StrU8& htmlVal = cs.内存_html;
			StrW url = L"file:///index.html";
			if (cs.内存_url != nil) {
				url = U8toW(cs.内存_url);
			}
			SciterLoadHtml(hwnd, htmlVal.bytes, htmlVal.len(), url);
		} else if (_zip.是否已打开()) {
			Bytes htmlVal = _zip.读取成员("index.html");
			if (htmlVal) {
				SciterLoadHtml(hwnd, htmlVal.buf, htmlVal.size, L"file:///index.html");
			}
		}

		// body 的单值 margin 就是外围阴影及拉伸热区宽度。
		HELEMENT root = NULL;
		if (g_sapi->SciterGetRootElement(hwnd, &root) == SCDOM_OK) {
			// transparent 与 solid-with-shadow 都由 HTML 完整绘制标题栏。
			// 后者不启用 WS_EX_LAYERED，可让滚动内容走普通 DWM 合成路径。
			g_sapi->SciterGetAttributeByNameCB(
				root, "window-frame",
				[](LPCWSTR text, UINT length, LPVOID param) {
				if (!text || !length || !param) return;
				auto* self = reinterpret_cast<SciterUI*>(param);
				if (wcsstr(text, L"transparent")/* || wcsstr(text, L"extended")*/) {
					self->_isTransparentFrame = true;
				}
			}, this);

			HELEMENT bodyEl = NULL;
			g_sapi->SciterGetNthChild(root, 1, &bodyEl);
			g_sapi->SciterGetStyleAttributeCB(bodyEl, "margin-left",
				[](LPCWSTR text, UINT, LPVOID param) {
				*reinterpret_cast<int*>(param) = _wtoi(text);
			}, &_shadowMargin);
			_shadowMarginR = _shadowMargin;
		}

		//_支持Win32子控件();
		if (cs.是否有启动动画) {
			父窗口->可视_(false); //由JS那边控制动画开始时才设置可视
			支持DWM动效(false);
		} else {
			支持DWM动效(true);
		}
	}

	void 支持DWM动效(bool is)
	{
		if (is) {
			窗口_添加风格(hwnd, WS_CAPTION | WS_THICKFRAME);
		} else {
			窗口_移除风格(hwnd, WS_CAPTION | WS_THICKFRAME);
		}
		SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	}

	SciterDom 取文档模型()
	{
		HELEMENT root = NULL;
		g_sapi->SciterGetRootElement(hwnd, &root);
		SciterDom r; r.ctx = root; r._st = this;
		return r;
	}

	// 按当前鼠标位置查询用户定义的原生拉伸区，并强制对应光标。
	bool _强制自定义拉伸光标(UINT* sciterCursorId)
	{
		_设置Sciter内部光标屏蔽(false);
		if (!hwnd || _shadowMargin <= 0 || IsZoomed(hwnd)) return false;
		if (!_允许窗口边缘拉伸(hwnd)) return false;

		POINT pt = {};
		RECT rc = {};
		if (!GetCursorPos(&pt) || !GetWindowRect(hwnd, &rc)) return false;

		const int x = pt.x - rc.left;
		const int y = pt.y - rc.top;
		const int w = rc.right - rc.left;
		const int h = rc.bottom - rc.top;
		const int activeMargin = dpi(_shadowMargin - 1);
		const int cornerMargin = dpi(_shadowMargin < 10 ? 10 : _shadowMargin);

		可空<char> hit = 事件_窗口外围被拉伸(
			x, y, w, h,
			x >= 0 && x < activeMargin,
			x >= w - activeMargin && x < w,
			y >= 0 && y < activeMargin,
			y >= h - activeMargin && y < h,
			x >= 0 && x < cornerMargin,
			x >= w - cornerMargin && x < w,
			y >= 0 && y < cornerMargin,
			y >= h - cornerMargin && y < h);
		if (hit == nil) return false;

		LPCWSTR cursorId = nullptr;
		switch ((char)hit) {
		case HTLEFT:
		case HTRIGHT:
			cursorId = IDC_SIZEWE;
			if (sciterCursorId) *sciterCursorId = CURSOR_SIZEWE;
			break;
		case HTTOP:
		case HTBOTTOM:
			cursorId = IDC_SIZENS;
			if (sciterCursorId) *sciterCursorId = CURSOR_SIZENS;
			break;
		case HTTOPLEFT:
		case HTBOTTOMRIGHT:
			cursorId = IDC_SIZENWSE;
			if (sciterCursorId) *sciterCursorId = CURSOR_SIZENWSE;
			break;
		case HTTOPRIGHT:
		case HTBOTTOMLEFT:
			cursorId = IDC_SIZENESW;
			if (sciterCursorId) *sciterCursorId = CURSOR_SIZENESW;
			break;
		default:
			return false;
		}

		_设置Sciter内部光标屏蔽(true);
		SetCursor(LoadCursorW(nullptr, cursorId));
		return true;
	}

private:
	SciterZip _zip;
};

void SciterDom::浏览(c_StrU8 路径)
{
	执行JS脚本("Window.this.load('"+子文本替换(路径, "\\", "/")+"');");
	if (_st)*this = _st->取文档模型();
}