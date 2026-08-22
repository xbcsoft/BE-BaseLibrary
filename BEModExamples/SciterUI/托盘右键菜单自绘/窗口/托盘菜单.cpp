#include <BEMod.h>

struct __托盘菜单 : 窗口
{
	SciterUI st;
	SciterDom dom;

	void 事件_创建本体()
	{
		SciterUI::参数 cs;
		cs.文件_html = "menu.html";
		st.创建(cs, this);
		dom = st.取文档模型();
		_g_wndCount--; //弹出层窗口不计入全局生命周期计数

		窗口_添加扩展风格(窗口句柄, WS_EX_TOOLWINDOW);
	}

	LRESULT 挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override
	{
		if (msg == WM_ACTIVATE && LOWORD(wParam) == WA_INACTIVE) {
			可视_(false);
		}
		return 窗口::挂接消息(hwnd, msg, wParam, lParam);
	}

	virtual void 弹出(可空<int> screenX = 空, 可空<int> screenY = 空)
	{
		// 默认获取当前鼠标屏幕坐标
		POINT pt; GetCursorPos(&pt);
		int sx = screenX.OR(pt.x);
		int sy = screenY.OR(pt.y);

		// 获取鼠标所在显示器的物理全屏区域（支持多显示器）
		POINT curPt = { sx, sy };
		HMONITOR hMon = MonitorFromPoint(curPt, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi = { sizeof(mi) };
		GetMonitorInfoW(hMon, &mi);
		RECT rcMon = mi.rcMonitor;

		int menuW = dpi(160);
		int menuH = dpi(165);

		// 1. 水平 X 轴：默认朝右展开，超出屏幕则向左翻转
		int x = sx;
		if (x + menuW > rcMon.right) {
			x = sx - menuW;
		}
		if (x < rcMon.left) {
			x = rcMon.left;
		}

		// 2. 垂直 Y 轴：下半区向上紧贴鼠标，上半区向下紧贴鼠标
		int screenMidY = (rcMon.top + rcMon.bottom) / 2;
		int y = (sy >= screenMidY) ? (sy - menuH) : sy;

		if (y + menuH > rcMon.bottom) y = rcMon.bottom - menuH;
		if (y < rcMon.top) y = rcMon.top;

		SetWindowPos(窗口句柄, HWND_TOPMOST, x, y, menuW, menuH, SWP_SHOWWINDOW);
		SetForegroundWindow(窗口句柄);
	}
} _托盘菜单;