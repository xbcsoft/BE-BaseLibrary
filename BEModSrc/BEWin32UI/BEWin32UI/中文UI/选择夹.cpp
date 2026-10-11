#include "多夹接口.h"
#include "子容器.hpp"
#include "窗口.h"

定义_枚举型(子夹头方向, char,
	上 = 0, 下 = TCS_BOTTOM
);

class EXP 选择夹 :public 多夹接口
{
public:
	~选择夹() override {
		销毁(true);
	}

	struct 参数 : 容器类::参数 {
		字体 字体;
		子夹头方向 方向 = 子夹头方向::上;
		bool 隐藏自身 = false;
		颜色 背景颜色 = 颜色::白色;
		Bytes 子夹管理;
	}static _df;

	int 现行子夹 = 0;
	bool 隐藏自身 = false;
	颜色 背景颜色;
	Bytes 子夹管理;
	子夹头方向 方向 = 子夹头方向::上;
	定义_字体成员;

	bool 创建(const 参数& cs, 容器类* 父窗口 = NULL, HWND 父句柄 = NULL) {
		初始化Win32公共控件();
		if (!容器类::_子初始(cs, 父窗口, 父句柄)) return false;
		隐藏自身 = cs.隐藏自身;
		背景颜色 = cs.背景颜色;
		方向 = cs.方向;
		if (背景颜色 != 颜色::透明) {
			_背景画刷 = CreateSolidBrush(背景颜色);
		}
		子夹管理 = cs.子夹管理;

		DWORD style = _窗口风格 | 方向;
		窗口句柄 = CreateWindowExW(WS_EX_CONTROLPARENT, L"SysTabControl32", L"", style,
			(int)dpi(左边), (int)dpi(顶边), (int)dpi(宽度), (int)dpi(高度),
			父窗口句柄, NULL, g_hInst, NULL);
		if (!窗口句柄) return false;

		选择夹::_子类化();
		置字体(cs.字体);
		if (子夹管理) {
			const charW* p = (const charW*)子夹管理.buf;
			const charW* end = p + 子夹管理.size / sizeof(charW);
			while (p < end) {
				插入子夹(p);
				while (p < end && *p++);
			}
		}
		return true;
	}

	bool 销毁(bool 同步 = false) {
		if (!窗口句柄) return false;
		if (!同步) {
			PostMessageW(窗口句柄, WM_APP, 0, 0);
			return true;
		}
		for (int i = 0; i < _子夹集合.count; i++) {
			if (_子夹集合[i]) {
				数组_删除元素值成员(_组件列表, _子夹集合[i]);
				_子夹集合[i]->销毁(true);
				delete _子夹集合[i];
			}
		}
		_子夹集合.clear();
		return 容器类::销毁(true);
	}

	void 子夹管理_(c_Bytes 数组) { 子夹管理 = 数组; }

	RECTF 取子夹头矩形(int 索引 = 0) {
		RECT r;
		SendMessageW(窗口句柄, TCM_GETITEMRECT, 索引, (LPARAM)&r);
		return rdpi(r, true);
	}

	SIZEF 置子夹头尺寸(SIZEF sz) {
		SIZE physicalSz = dpi(sz);
		int a = (int)SendMessageW(窗口句柄, TCM_SETITEMSIZE, 0, MAKELONG(physicalSz.cx, physicalSz.cy));
		SIZE retPhysical = { LOWORD(a), HIWORD(a) };
		return rdpi(retPhysical);
	}

	int 插入子夹(StrW 标题, 可空<int> 子夹索引 = nil) {
		return 插入子夹(标题, 子夹索引, -1, 0);
	}

	int 插入子夹(StrW 标题, 可空<int> 子夹索引, int 图片索引, int 子夹数值 = 0) {
		TCITEM item = { 0 };
		item.mask = TCIF_TEXT | TCIF_IMAGE | TCIF_PARAM;
		item.pszText = 标题;
		item.iImage = 图片索引;
		item.lParam = 子夹数值;
		int 插入索引 = (子夹索引 == 空) ? _子夹集合.count : 子夹索引;
		if (TabCtrl_InsertItem(窗口句柄, 插入索引, &item) < 0) {
			return -1;
		}

		子容器<>* zrq = new 子容器<>;
		子容器<>::参数 cs;
		if (隐藏自身) {
			cs.左边 = 0; cs.顶边 = 0; cs.宽度 = 宽度; cs.高度 = 高度;
		} else {
			RECTF r = 取子夹头矩形();
			bool 在下方 = (r.top > 高度 / 2);
			if (在下方) {
				cs.左边 = 1; cs.顶边 = 1; cs.宽度 = 宽度 - 3; cs.高度 = r.top - 1;
			} else {
				cs.左边 = 1; cs.顶边 = r.bottom; cs.宽度 = 宽度 - 3; cs.高度 = 高度 - r.bottom - 1;
			}
		}
		cs.可视 = false;

		if (!zrq->创建(cs, this, 窗口句柄))return -1;

		_子夹集合.push(zrq);

		if (_子夹集合.count == 1) {
			TabCtrl_SetCurSel(窗口句柄, 0);
			zrq->可视_(true);
		}
		_调整子夹尺寸();
		return 插入索引;
	}

	子容器<>* 取子夹容器(int 索引) {
		if (_判断索引非法(索引))return NULL;
		return _子夹集合[索引];
	}

	int 现行子夹_(int 索引) override {
		if (索引 == -1) return 现行子夹;
		if (_判断索引非法(索引)) return -1;

		int 旧索引 = 现行子夹;
		if (!_判断索引非法(旧索引)) {
			_子夹集合[旧索引]->可视_(false);
		}
		_子夹集合[索引]->可视_(true);
		现行子夹 = 索引;
		TabCtrl_SetCurSel(窗口句柄, 索引);
		return 旧索引;
	}

	bool _判定客户鼠标拖动(int x, int y) override {
		if (隐藏自身 || !窗口句柄 || TabCtrl_GetItemCount(窗口句柄) <= 0)
			return false;
		RECT client = {};
		GetClientRect(窗口句柄, &client);
		if (x < client.left || x >= client.right
			|| y < client.top || y >= client.bottom) return false;
		// SysTabControl32 的子夹头位于固定方向的客户区边缘；使用 DPI
		// 后的设计时命中带，避免主题下 TCM_HITTEST/GETITEMRECT 对被
		// 子类化、嵌入窗口返回空矩形。
		int headerBand = dpi(24);
		return 方向 == 子夹头方向::下
			? y >= client.bottom - headerBand : y < headerBand;
	}

	void 隐藏自身_(bool is) {
		if (隐藏自身 == is) return;
		隐藏自身 = is;
		_调整子夹尺寸();
		重画();
	}

	void 方向_(子夹头方向 d) {
		if (方向 == d) return;
		方向 = d;
		if (窗口句柄) {
			LONG_PTR style = GetWindowLongPtrW(窗口句柄, GWL_STYLE);
			style &= ~TCS_BOTTOM;
			style |= d;
			SetWindowLongPtrW(窗口句柄, GWL_STYLE, style);
			SetWindowPos(窗口句柄, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
			_调整子夹尺寸();
		}
	}

	void 置子夹标题(int 索引, StrW 标题) {
		if (_判断索引非法(索引))return;
		TCITEM item = { 0 };
		item.mask = TCIF_TEXT;
		item.pszText = 标题;
		TabCtrl_SetItem(窗口句柄, 索引, &item);
	}

	void 置子夹图片(int 索引, int 图片索引) {
		if (_判断索引非法(索引))return;
		TCITEM item = { 0 };
		item.mask = TCIF_IMAGE;
		item.lParam = 图片索引;
		TabCtrl_SetItem(窗口句柄, 索引, &item);
	}

	void 置子夹数值(int 索引, int a) {
		if (_判断索引非法(索引))return;
		TCITEM item = { 0 };
		item.mask = TCIF_PARAM;
		item.lParam = a;
		TabCtrl_SetItem(窗口句柄, 索引, &item);
	}

	int 取子夹图片(int 索引) {
		if (_判断索引非法(索引))return -1;
		TCITEM item = { 0 };
		item.mask = TCIF_IMAGE;
		SendMessageW(窗口句柄, TCM_GETITEM, 索引, (LPARAM)&item);
		return item.iImage;
	}

	StrW 取子夹标题(int 索引) {
		if (_判断索引非法(索引))return StrW();
		TCITEM item = { 0 };
		item.mask = TCIF_TEXT;
		wchar_t buf[256];
		item.cchTextMax = 256;
		item.pszText = buf;
		SendMessageW(窗口句柄, TCM_GETITEM, 索引, (LPARAM)&item);
		return StrW(item.pszText);
	}

	void 子夹添加组件(int 索引, 窗口基类& a) {
		if (_判断索引非法(索引))return;
		// 控件被放入子夹后，生命周期归属也必须同步迁移到对应子容器。
		// 否则 父窗口 已指向子容器，但指针仍残留在选择夹::_组件列表；
		// 设计器先销毁该控件再销毁选择夹时，选择夹会再次访问已释放对象。
		a.窗口置父(_子夹集合[索引], true);
		a.可视_(true);
	}

	void 置图片组(图片组& imageList) {
		_图片组.从图片组复制(imageList);
		SendMessageW(窗口句柄, TCM_SETIMAGELIST, 0, (LPARAM)(_图片组.h));
	}

	LRESULT 挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	//默认选择夹背景只能是白色（这个是系统现代主题在WM_PAINT决定的无法更改）
	//而且如果要强行改，除了自绘外就只能贴透明容器了然后借助白易透明背景机制让父窗口用自己的画刷
	//理论上选择夹自身是没办法做到全透明的...所以指定透明色将无效
	颜色 _取背景颜色(颜色 ** out = 0) {
		if (out)*out = &背景颜色;
		return 背景颜色;
	}
	void 背景颜色_(颜色 cr, bool 重画 = true) {
		背景颜色 = cr;
		if (背景颜色 == 颜色::透明) {
			_背景画刷 = NULL;
		} else {
			_背景画刷 = CreateSolidBrush(cr);
		}
		现行子夹_(现行子夹);
		if (重画) this->重画();
	}
protected:
	图片组 _图片组;
	Arraybe<子容器<>*> _子夹集合;


private:

	void _调整子夹尺寸() {
		RECTF 全大小 = { 0, 0, 宽度, 高度 };
		if (隐藏自身) {
			for (int i = 0; i < _子夹集合.count; i++) {
				_子夹集合[i]->移动(0.0f, 0.0f, 宽度, 高度);
				_子夹集合[i]->重画(全大小, true);
			}
		} else {
			RECTF r = 取子夹头矩形();
			bool 在下方 = (r.top > 高度 / 2);
			for (int i = 0; i < _子夹集合.count; i++) {
				if (在下方) {
					_子夹集合[i]->移动(0.0f, 1.0f, 宽度, r.top);
				} else {
					_子夹集合[i]->移动(0.0f, r.bottom, 宽度, 高度 - r.bottom);
				}
				_子夹集合[i]->重画(全大小, true);
			}
		}
		重画(全大小, true);
	}

	bool _判断索引非法(int 子夹索引) {
		return  (子夹索引 < 0 || 子夹索引 >= _子夹集合.count);
	}
};

LRESULT 选择夹::挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_ERASEBKGND: {
		RECT rc = { 0, 0, dpi(宽度), dpi(高度) };
		if (背景颜色 == 颜色::透明) {
			UI_请求父窗绘制背景(窗口句柄, (HDC)wParam, rc);
			return 1;
		}
		if (!_背景画刷) _背景画刷 = CreateSolidBrush(背景颜色);
		FillRect((HDC)wParam, &rc, _背景画刷);
		return 1;
	}
	case WM_PAINT: {
		if (隐藏自身) {
			PAINTSTRUCT ps;
			BeginPaint(窗口句柄, &ps);
			EndPaint(窗口句柄, &ps);
			return 0;
		}
		break;
	}
	case WM_INITDIALOG: //让选择夹不具有停留焦点（否则你切换子夹的时候会有焦点）
		SetFocus(窗口句柄); //指定别的控件（这个确实要置自己才能改变）
		return FALSE; // 告诉系统别自动聚焦该控件
	case WM_SIZE: {
		_调整子夹尺寸();
		break;
	}
	case BE_REFLECT + WM_NOTIFY: {
		NMHDR* hdr = (NMHDR*)lParam;
		if (hdr->hwndFrom == 窗口句柄 && hdr->code == TCN_SELCHANGE) {
			现行子夹_(TabCtrl_GetCurSel(窗口句柄));
			重画();
			return 0;
		}
		break;
	}
	}
	return 容器类::挂接消息(hwnd, msg, wParam, lParam);
}
