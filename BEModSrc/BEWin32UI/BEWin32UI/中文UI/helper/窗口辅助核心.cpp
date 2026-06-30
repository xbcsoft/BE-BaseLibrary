#include "../stdafx.h"
#include "窗口常量.h"
#include <CommCtrl.h>

AutoStr __AutoStr__(const HWND& r) {
	return 到文本((int)r);
}

float GetCurrentDeviceDPI()
{
	float dpi;
	HDC hdc = GetDC(0);
	int d = GetDeviceCaps(hdc, DESKTOPHORZRES);
	int h = GetDeviceCaps(hdc, HORZRES);
	if (d!=h) {
		dpi = (float)d/h;
	} else {
		int logX = GetDeviceCaps(hdc, LOGPIXELSX);
		dpi = (float)logX / 96.0f;
	}
	ReleaseDC(0, hdc);
	return dpi;
}
void dpi_backup(float newDPI)
{
	_g_dpi_bak = g_dpi;
	g_dpi = newDPI;
}
void dpi_restore()
{
	g_dpi = _g_dpi_bak;
}
float dpi_get()
{
	return g_dpi;
}
int dpi(float v)
{
	return (v*g_dpi+ 0.5f);
}
RECT dpi(RECTF v)
{
	return { (long)dpi(v.left), (long)dpi(v.top),
		(long)dpi(v.right), (long)dpi(v.bottom) };
}
SIZE dpi(SIZEF v)
{
	return { (long)(dpi(v.cx) + 0.5f), (long)(dpi(v.cy) + 0.5f) };
}
float rdpi(int v)
{
	return v/g_dpi;
}
RECTF rdpi(RECT v, char floatRet)
{
	return { rdpi(v.left), rdpi(v.top),
		rdpi(v.right), rdpi(v.bottom) };
}

SIZEF rdpi(SIZE v)
{
	return { rdpi(v.cx), rdpi(v.cy) };
}

RECT SIZEToRECT(SIZE size, int left = 0, int top = 0)
{
	return { left, top, left + size.cx, top + size.cy };
}

void 全局初始化配置(HINSTANCE hInstance, bool 支持高DPI)
{
	g_hInst = hInstance;
	BYTE* base = (BYTE*)g_hInst;
	DWORD e_lfanew = *(DWORD*)(base + 0x3C);
	BYTE* nt = base + e_lfanew;
	BYTE* opt = nt + 24; // skip PE signature + IMAGE_FILE_HEADER
	DWORD size_of_image = *(DWORD*)(opt + 0x38);
	g_hInstEnd = (char*)g_hInst+size_of_image;

	if (支持高DPI) {
		g_dpi = GetCurrentDeviceDPI();
		SetProcessDPIAware();
	} else {
		g_dpi = 1;
	}
}

void 初始化Win32公共控件() {
	static bool 已初始化 = false;
	if (已初始化) return;
	INITCOMMONCONTROLSEX icc = { sizeof(icc), ICC_TAB_CLASSES };
	InitCommonControlsEx(&icc);
	已初始化 = true;
}

bool IsNotModuleAddr(void* p)
{
	char* addr = (char*)p;
	return !(addr >= (char*)g_hInst && addr < g_hInstEnd);
}

bool operator==(const ACCEL& a, const ACCEL& b)
{
	return a.fVirt == b.fVirt &&
		a.key   == b.key &&
		a.cmd   == b.cmd;
}


struct WindowAccel
{
	HWND hwnd = 0; HACCEL hAccel = 0;
	Arraybe<ACCEL> keys;
	void set(HWND h, const Arraybe<ACCEL>& keys);
	void update();
	void destroy();
	~WindowAccel();
};
void WindowAccel::set(HWND h, const Arraybe<ACCEL>& keys)
{
	hwnd = h; this->keys = keys;
	update();
}
void WindowAccel::update()
{
	destroy();
	hAccel = CreateAcceleratorTable(keys.pArr, keys.count);
}
void WindowAccel::destroy()
{
	if (hAccel) {
		DestroyAcceleratorTable(hAccel);
		hAccel = 0;
	}
}
WindowAccel::~WindowAccel()
{
	destroy();
}

/**即时处理白易特制的Windows线程消息队列直到为空
 */
void 处理事件();
int TCP客户端::接收ep(void* 接收buf, int 将接收大小, int 超时值)
{
	if (!接收buf || 将接收大小 <= 0) return -1;
	if (超时值 <= 0) 超时值 = 1000 * 7200;
	if (sock == INVALID_SOCKET) return -1;

	if (_event == WSA_INVALID_EVENT) {
		_event = WSACreateEvent();
		WSAEventSelect(sock, _event, FD_READ | FD_CLOSE);
	}

	while (true) {
		DWORD r = MsgWaitForMultipleObjects(1, &_event, FALSE, 超时值, QS_ALLINPUT);
		if (r == WAIT_OBJECT_0) {
			break;
		}
		if (r == WAIT_TIMEOUT) {
			return 0;
		}
		处理事件();
	}

	WSAResetEvent(_event);
	int r = recv(sock, (char*)接收buf, 将接收大小, 0);
	if (r <= 0) {
		_lastError = WSAGetLastError();
		断开();
		return -1;
	}
	return r;
}

/**相比于延时（为线程级阻塞），而延迟可同时处理消息让窗口不卡死且不占用CPU
 * @param 欲等待的毫秒
 */
void 延迟(int 欲等待的毫秒) {
	if (欲等待的毫秒 <= 0) { return; }
	HANDLE hTimer = CreateWaitableTimerA(NULL, FALSE, NULL);
	LARGE_INTEGER dueTime;
	dueTime.QuadPart = -10000LL * 欲等待的毫秒;
	SetWaitableTimer(hTimer, &dueTime, 0, NULL, NULL, FALSE);
	while (MsgWaitForMultipleObjects(1, &hTimer, FALSE, INFINITE, QS_ALLINPUT) != 0)
		处理事件();
	CloseHandle(hTimer);
}


/**此方法为同步判断，需配合在WM_KEYDOWN消息中即时使用
 * @param 控制键 VK_CONTROL(不区分左右),VK_LCONTROL,VK_RCONTROL(左/右Ctrl)
   - VK_SHIFT(不区分左右),VK_LSHIFT,VK_RSHIFT(左/极Ctrl)
   - VK_MENU(这个其实是Alt,不区分左右),VK_LMENU,VK_RMENU(左/右Alt)
 * @return
 */
bool 控制键_是否被按下(BYTE 控制键) {
	return GetKeyState(控制键) & 0x8000;
}

DWORD 窗口_添加扩展风格(HWND hwnd, DWORD 扩展风格) {
	DWORD cur = GetWindowLong(hwnd, GWL_EXSTYLE);
	SetWindowLong(hwnd, GWL_EXSTYLE, cur|(扩展风格));
	SetWindowPos(hwnd, 0, 0, 0, 0, 0, (SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOZORDER|SWP_FRAMECHANGED));
	return cur;
}

DWORD 窗口_移除扩展风格(HWND hwnd, DWORD 扩展风格) {
	DWORD cur = GetWindowLong(hwnd, GWL_EXSTYLE);
	SetWindowLong(hwnd, GWL_EXSTYLE, cur&(~扩展风格));
	SetWindowPos(hwnd, 0, 0, 0, 0, 0, (SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOZORDER|SWP_FRAMECHANGED));
	return cur;
}

DWORD 窗口_添加风格(HWND hwnd, DWORD 风格) {
	DWORD cur = GetWindowLong(hwnd, GWL_STYLE);
	return SetWindowLong(hwnd, GWL_STYLE, cur|风格);
}

DWORD 窗口_移除风格(HWND hwnd, DWORD 风格) {
	DWORD cur = GetWindowLong(hwnd, GWL_STYLE);
	return SetWindowLong(hwnd, GWL_STYLE, cur&(~风格));
}

struct _EnumChildContext {
	Arraybe<HWND>* arr;
	bool(*回调)(HWND);
};

static BOOL CALLBACK _EnumChildProc(HWND hwnd, LPARAM lParam) {
	_EnumChildContext* ctx = (_EnumChildContext*)lParam;
	if (ctx->回调) {
		if (!ctx->回调(hwnd)) return FALSE;
	}
	ctx->arr->push(hwnd);
	return TRUE;
}

Arraybe<HWND> 窗口_枚举子窗口句柄(HWND 父窗口句柄, bool 是否递归 = true,
	bool(*回调)(HWND) = nullptr)
{
	Arraybe<HWND> arr;
	if (是否递归) {
		_EnumChildContext ctx = { &arr, 回调 };
		EnumChildWindows(父窗口句柄, _EnumChildProc, (LPARAM)&ctx);
	} else {
		HWND child = NULL;
		while ((child = FindWindowExW(父窗口句柄, child, NULL, NULL)) != NULL) {
			if (回调) {
				if (!回调(child)) break;
			}
			arr.push(child);
		}
	}
	return arr;
}

class 窗口基类;

/**窗口_句柄取对象 只有经过白易子类化后的对象才能获取
 * @param hwnd
 * @return
 */
窗口基类* 窗口_句柄取对象(HWND hwnd)
{
	return (窗口基类*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
}
Arraybe<窗口基类*> 窗口_枚举子窗口对象(HWND 父窗口句柄, bool 是否递归 = true,
	bool(*回调)(窗口基类*) = nullptr)
{
	Arraybe<窗口基类*> obs;
	for (HWND h : 窗口_枚举子窗口句柄(父窗口句柄, 是否递归)) {
		窗口基类* p = 窗口_句柄取对象(h);
		if (p) {
			if (回调 && !回调(p)) break;
			obs.push(p);
		}
	}
	return obs;
}

void 窗口_置标题(HWND hwnd, const StrW& title) {
	if (hwnd) SetWindowTextW(hwnd, title);
}

StrW 窗口_取标题(HWND hwnd) {
	if (!hwnd) return L"";
	int len = GetWindowTextLengthW(hwnd);
	if (len <= 0) return L"";
	StrW r(len);
	GetWindowTextW(hwnd, r, len + 1);
	return r;
}

HWND 窗口_取祖宗句柄(HWND hwnd) {
	return GetAncestor(hwnd, GA_ROOT);
}

HWND 窗口_取鼠标所在句柄() {
	POINT pt;
	if (GetCursorPos(&pt)) {
		return WindowFromPoint(pt);
	}
	return NULL;
}