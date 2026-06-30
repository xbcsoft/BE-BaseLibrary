#include "标签.h"

class EXP 超级链接框 : public 标签
{
public:
	struct 参数 : 控件类::参数 {
		定义_子组件通用构造方法;
		StrW 标题 = L"超级链接框";
		StrW 地址; //支持http://, mailto://
		颜色 文本颜色 = RGB(0, 0, 255);
		颜色 访问后的颜色 = RGB(128, 0, 128);
		颜色 热点颜色 = RGB(0, 102, 204);
		颜色 背景颜色 = 颜色::透明;
		标签效果 效果 = 标签效果::通常;
		标签边框 边框 = 标签边框::无边框;
		字体 字体;
		bool 是否自动折行 = false;
		横向对齐 横向对齐方式 = 横向对齐::左对齐;
		纵向对齐 纵向对齐方式 = 纵向对齐::顶对齐;
	}static _df;

	bool 创建(const 参数& cs, 容器类* 父窗口 = NULL, HWND 父句柄 = NULL) {
		标签::参数 tag_cs;
		tag_cs.左边 = cs.左边; tag_cs.顶边 = cs.顶边; tag_cs.宽度 = cs.宽度; tag_cs.高度 = cs.高度;
		tag_cs.可视 = cs.可视; tag_cs.禁止 = cs.禁止; tag_cs.可停留焦点 = cs.可停留焦点; tag_cs.停留顺序 = cs.停留顺序;
		tag_cs.标题 = cs.标题;
		tag_cs.文本颜色 = cs.文本颜色;
		tag_cs.背景颜色 = cs.背景颜色;
		tag_cs.效果 = cs.效果;
		tag_cs.边框 = cs.边框;
		tag_cs.字体 = cs.字体;
		tag_cs.横向对齐方式 = cs.横向对齐方式;
		tag_cs.是否自动折行 = cs.是否自动折行;
		tag_cs.纵向对齐方式 = cs.纵向对齐方式;

		if (!标签::创建(tag_cs, 父窗口, 父句柄)) return false;

		地址 = cs.地址;
		访问后的颜色 = cs.访问后的颜色;
		热点颜色 = cs.热点颜色;
		初始颜色 = cs.文本颜色;

		return true;
	}

	void 地址_(const StrW& addr) { 地址 = addr; }

	void 访问后的颜色_(颜色 cr) {
		访问后的颜色 = cr;
		if (已访问 && !鼠标悬停) {
			文本颜色 = 访问后的颜色; 重画();
		}
	}

	void 热点颜色_(颜色 cr) {
		热点颜色 = cr;
		if (鼠标悬停) {
			文本颜色 = 热点颜色; 重画();
		}
	}

public:
	virtual LRESULT 挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
	void 打开链接() {
		if (地址==L"") return;

		// 直接调用 ShellExecuteW，由系统根据协议（http://, mailto:// 等）自动识别
		ShellExecuteW(NULL, L"open", 地址, NULL, NULL, SW_SHOWNORMAL);
		已访问 = true;

		if (!鼠标悬停) {
			文本颜色 = 访问后的颜色;
			重画();
		}
	}

public:
	StrW 地址;
	颜色 访问后的颜色;
	颜色 热点颜色;
	颜色 初始颜色;
	bool 鼠标悬停 = false;
	bool 已访问 = false;
};

LRESULT 超级链接框::挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_MOUSEMOVE: {
		if (!鼠标悬停) {
			鼠标悬停 = true;
			文本颜色 = 热点颜色;
			重画();

			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(tme);
			tme.dwFlags = TME_LEAVE;
			tme.hwndTrack = hwnd;
			TrackMouseEvent(&tme);
		}
		break;
	}
	case WM_MOUSELEAVE: {
		鼠标悬停 = false;
		文本颜色 = 已访问 ? 访问后的颜色 : 初始颜色;
		重画();
		break;
	}
	case WM_SETCURSOR: {
		HCURSOR hHand = LoadCursorW(NULL, (LPCWSTR)IDC_HAND);
		if (hHand) {
			SetCursor(hHand);
			return TRUE;
		}
		break;
	}
	case WM_LBUTTONUP: {
		打开链接();
		break;
	}
	}
	return 标签::挂接消息(hwnd, msg, wParam, lParam);
}
