#pragma once
/**@ModuleTitle: SciterHTML自绘库
*  @version:     1.0
*  @platform:    win32(x86|x64)
*  @compiler:    source
*  @author:
*  @datetime:
*  @description: 封装自拥有独立HTML引擎的Sciter轻量化窗口UI自绘库
*/
#include "stdafx.h"
#include "SciterDom.h"
#include "NTCps.h"
#include "SciterZip.h"

extern ISciterAPI* g_sapi;
extern HMODULE g_sciterModule;
extern POINT g_sciterMaximizedMargin;

// 修改版 Sciter DLL 中预留的可写光标仲裁标志（x86/x64 各自固定 RVA）。
#ifdef _WIN64
static constexpr SIZE_T SCITER_CURSOR_SUPPRESS_FLAG_RVA = 0x7CF7F8;
#else
static constexpr SIZE_T SCITER_CURSOR_SUPPRESS_FLAG_RVA = 0x5F57F0;
#endif

class SciterUI
{
public:
	/**仅需在全局初始化一次
	 */
	static void 全局初始化();

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
	static void _设置Sciter内部光标屏蔽(bool enabled);

	static bool _允许窗口边缘拉伸(HWND targetHwnd);

	void _设置BodyMargin(HELEMENT root, LPCWSTR marginStr);

	static HELEMENT _查找文本编辑框(HELEMENT target);

	static void _同步文本选区光标(HELEMENT target, bool prepareForSelection);

	// -1: 尚未同步；0: 普通窗口；1: 最大化；2: 贴靠；3: 仅垂直铺满。
	// 最小化不是一种阴影布局，必须保留最小化前的状态，否则还原时会
	// 重复改写 DOM 样式并导致透明窗口表面重建。
	int _shadowLayoutState = -1;

	bool _应用阴影布局状态(int newLayoutState, BOOL forceRender);

	~SciterUI();

	virtual UINT 事件_资源加载(LPSCN_LOAD_DATA pld);

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
		bool inCornerLeft, bool inCornerRight, bool inCornerTop, bool inCornerBottom);


	// Sciter原版的鼠标滚轮存在滚动中途产生僵硬问题，我们这里接管滚动让其丝滑运行
	static BOOL SC_CALLBACK _StaticWindowMouseEventHandler(LPVOID tag, HELEMENT he, UINT evtg, LPVOID prms);

	//在 Sciter 引擎里，向它提供资源数据有两条平行且互斥的通道：
	//	通道 A：直接写入成员字段（延迟拷贝）
	//	做法：赋值 pld->outData = 缓冲区指针; 。
	//	过程：回调函数返回 LOAD_OK->Sciter 引擎拿到控制权->引擎读取该指针并复制数据。
	//	弊端：必须保证指针在函数返回后依然有效（所以之前需要使用 static）。

	//	通道 B：调用 SciterDataReady（即时拷贝，我们现在使用的方法）
	//	做法：调用 SciterDataReady(hwnd, uri, 数据, 长度)。
	//	过程：当场立即拷贝。在 SciterDataReady 函数被执行的瞬间，数据就已经被塞进 Sciter 引擎的统一缓存区里了。
	//	优势：函数返回前数据就已经安全到达引擎，局部变量死活无所谓了。
	int 资源返回(LPSCN_LOAD_DATA pld, c_Bytes 数据, int 返回码 = LOAD_OK);

	int 资源返回(LPSCN_LOAD_DATA pld, c_StrA 数据, int 返回码 = LOAD_OK);

	//  宏	作用对象	含义
	//	WS_CLIPSIBLINGS	同级子窗口之间；
		/*当父窗口有多个子窗口（siblings），且它们在位置上有重叠时：
		  不设 WS_CLIPSIBLINGS：子窗口 A 调用 WM_PAINT 时，GDI 绘图会"穿透"到兄弟子窗口 B 的区域，导致 B 的内容被覆盖/污染。
		  设了 WS_CLIPSIBLINGS：Windows 在给 A 发 WM_PAINT 前，会把 B 的矩形区域从 A 的 DC clip region 中剪掉，A 的绘制绝对不会画到 B 的区域里。
		*/
	//	WS_CLIPCHILDREN	父窗口与其子窗口	我自己(是父)画的时候裁掉我的子窗口所在区域（这样父的部分不用重复绘制）
	void _支持Win32子控件();

	void 创建(const 参数& cs, 窗口* 父窗口);

	void 支持DWM动效(bool is);

	SciterDom 取文档模型();

	// 按当前鼠标位置查询用户定义的原生拉伸区，并强制对应光标。
	bool _强制自定义拉伸光标(UINT* sciterCursorId);

private:
	SciterZip _zip;
};
