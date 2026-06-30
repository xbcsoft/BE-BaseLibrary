#include "多夹接口.h"
#include "子容器.hpp"

定义_枚举型(子夹头方向, char,
	上 = 0, 下 = TCS_BOTTOM
);

class EXP 选择夹 :public 多夹接口
{
public:
	struct 参数 : 容器类::参数 {
		定义_子组件通用构造方法;
		字体 字体;
		颜色 背景颜色;
		子夹头方向 方向 = 子夹头方向::上;
	}static _df;

	int 现行子夹 = 0;
	颜色 背景颜色;
	定义_字体成员;

	bool 创建(const 参数& cs, 容器类* 父窗口 = NULL, HWND 父句柄 = NULL) {
		初始化Win32公共控件();
		if (!容器类::_子初始(cs, 父窗口, 父句柄))return false;
		背景颜色 = cs.背景颜色;

		窗口句柄 = CreateWindowExW(WS_EX_CONTROLPARENT, L"SysTabControl32",
			L"", _窗口风格 | cs.方向, dpi(左边), dpi(顶边), dpi(宽度), dpi(高度),
			父窗口句柄, NULL, g_hInst, NULL);
		if (!窗口句柄) return false;

		置字体(cs.字体);
		选择夹::_子类化();
		return true;
	}

	bool 销毁() {
		if (!窗口句柄)return false;
		_子夹集合.clear(); //这当中会一个个调用_子夹中的析构函数则相关资源也会随之释放
		DestroyWindow(窗口句柄);
		容器类::销毁();
		return true;
	}

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
		RECTF r = 取子夹头矩形();

		子容器<>::参数 cs(1, r.bottom, 宽度 - 3, 高度 - r.bottom - 1);
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

	void 现行子夹_(int 索引) {
		if (_判断索引非法(索引))return;
		_子夹集合[现行子夹]->可视_(false);
		_子夹集合[索引]->可视_(true);
		现行子夹 = 索引;
		TabCtrl_SetCurSel(窗口句柄, 索引);
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
		char buf[256];
		item.cchTextMax = 256;
		item.pszText = (TCHAR*)buf;
		SendMessageW(窗口句柄, TCM_GETITEM, 索引, (LPARAM)&item);
		return StrW(item.pszText);
	}

	void 子夹添加组件(int 索引, 窗口基类& a) {
		if (_判断索引非法(索引))return;
		a.窗口置父(_子夹集合[索引]);
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
		_背景画刷 = CreateSolidBrush(cr);
		现行子夹_(现行子夹);
	}
protected:
	图片组 _图片组;
	Arraybe<子容器<>*> _子夹集合;


private:
	void _调整子夹尺寸() {
		RECTF r = 取子夹头矩形();
		for (int i = 0; i < _子夹集合.count; i++) {
			_子夹集合[i]->移动(1, r.bottom, 宽度 - 3, 高度 - r.bottom - 1);
		}
	}

	bool _判断索引非法(int 子夹索引) {
		return  (子夹索引 < 0 || 子夹索引 >= _子夹集合.count);
	}
};

LRESULT 选择夹::挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_ERASEBKGND: {
		RECT rc = { 0, 0, (int)dpi(宽度), (int)dpi(高度) };
		FillRect((HDC)wParam, &rc, _背景画刷);
		return 1;
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
