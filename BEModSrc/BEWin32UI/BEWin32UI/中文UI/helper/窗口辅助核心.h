#pragma once
#include "../stdafx.h"
#include "窗口常量.h"
#include <CommCtrl.h>

AutoStr __AutoStr__(const HWND& r);

float GetCurrentDeviceDPI();
void dpi_backup(float newDPI);
void dpi_restore();
float dpi_get();
int dpi(float v);
RECT dpi(RECTF v);
SIZE dpi(SIZEF v);
float rdpi(int v);
RECTF rdpi(RECT v, char floatRet);

SIZEF rdpi(SIZE v);

RECT SIZEToRECT(SIZE size, int left = 0, int top = 0);

void 全局初始化配置(HINSTANCE hInstance, bool 支持高DPI);

void 初始化Win32公共控件();

bool IsNotModuleAddr(void* p);

bool operator==(const ACCEL& a, const ACCEL& b);


struct WindowAccel
{
	HWND hwnd = 0; HACCEL hAccel = 0;
	Arraybe<ACCEL> keys;
	void set(HWND h, const Arraybe<ACCEL>& keys);
	void update();
	void destroy();
	~WindowAccel();
};

/**即时处理白易特制的Windows线程消息队列直到为空
 */
void 处理事件();

/**相比于延时（为线程级阻塞），而延迟可同时处理消息让窗口不卡死且不占用CPU
 * @param 欲等待的毫秒
 */
void 延迟(int 欲等待的毫秒);


/**此方法为同步判断，需配合在WM_KEYDOWN消息中即时使用
 * @param 控制键 VK_CONTROL(不区分左右),VK_LCONTROL,VK_RCONTROL(左/右Ctrl)
   - VK_SHIFT(不区分左右),VK_LSHIFT,VK_RSHIFT(左/极Ctrl)
   - VK_MENU(这个其实是Alt,不区分左右),VK_LMENU,VK_RMENU(左/右Alt)
 * @return
 */
bool 控制键_是否被按下(BYTE 控制键);

DWORD 窗口_添加扩展风格(HWND hwnd, DWORD 扩展风格);

DWORD 窗口_移除扩展风格(HWND hwnd, DWORD 扩展风格);

DWORD 窗口_添加风格(HWND hwnd, DWORD 风格);

DWORD 窗口_移除风格(HWND hwnd, DWORD 风格);

struct _EnumChildContext {
	Arraybe<HWND>* arr;
	bool(*回调)(HWND);
};

static BOOL CALLBACK _EnumChildProc(HWND hwnd, LPARAM lParam);

Arraybe<HWND> 窗口_枚举子窗口句柄(HWND 父窗口句柄, bool 是否递归 = true,
	bool(*回调)(HWND) = nullptr);

class 窗口基类;

/**窗口_句柄取对象 只有经过白易子类化后的对象才能获取
 * @param hwnd
 * @return
 */
窗口基类* 窗口_句柄取对象(HWND hwnd);
Arraybe<窗口基类*> 窗口_枚举子窗口对象(HWND 父窗口句柄, bool 是否递归 = true,
	bool(*回调)(窗口基类*) = nullptr);

void 窗口_置标题(HWND hwnd, const StrW& title);

StrW 窗口_取标题(HWND hwnd);

HWND 窗口_取祖宗句柄(HWND hwnd);

HWND 窗口_取鼠标所在句柄();
