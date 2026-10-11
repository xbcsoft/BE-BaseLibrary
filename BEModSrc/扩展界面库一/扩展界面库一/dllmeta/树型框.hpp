#ifndef _BEWin32UIProj
#include <BEWin32UI/VSDllSDK.hpp>
#else
#include "../../VSDllSDK.hpp"
#endif
#include "../树型框.h"

树型框* UI_树型框_创建(byte* fb, 容器类* 父窗口)
{
	if (fb==nullptr){
		UI_销毁控件(父窗口); //当参数1给出空指针时此方法变为销毁此投入的参数2实例对象
		return 0;
	}
	树型框::参数 cs;
	cs.左边 = gFBf(fb, 0);
	cs.顶边 = gFBf(fb, 1);
	cs.宽度 = gFBf(fb, 2);
	cs.高度 = gFBf(fb, 3);
	cs.可视 = gFBb(fb, 4);
	cs.禁止 = gFBb(fb, 5);
	cs.可停留焦点 = gFBb(fb, 6);
	cs.停留顺序 = gFBi(fb, 7);
	cs.边框 = gFBi(fb, 8);
	cs.文本颜色 = gFBi(fb, 9);
	cs.背景颜色 = gFBi(fb, 10);
	cs.显示加减框 = gFBb(fb, 11);
	cs.显示连线 = gFBb(fb, 12);
	cs.显示根部线 = gFBb(fb, 13);
	cs.允许编辑 = gFBb(fb, 14);
	cs.始终显示选择项 = gFBb(fb, 15);
	cs.现行选中项 = gFBi(fb, 16);
	cs.行高 = gFBi(fb, 17);
	cs.字体 = gFBpo<字体>(fb, 18);
	cs.图片组 = gFB(fb, 19);
	cs.项目 = gFB(fb, 20);
	cs.是否有检查框 = gFBb(fb, 21);
	cs.检查框图片组 = gFB(fb, 22);
	cs.检查框状态数 = gFBi(fb, 23);
	树型框* ui = new 树型框;
	ui->创建(cs, 父窗口);
	return ui;
}

树型框::参数 树型框::_df;
static UI属性元信息 META_树型框[] = {
	//int 索引,LPWSTR 名称;DWORD 类型;union 默认值;LPWSTR 枚举值;LPWSTR 说明;BOOL 可空;BOOL 子项;
	_MK属性(0, L"左边", UI属性类型::数值, 树型框::_df.左边, NULL, L"", false, false),
	_MK属性(1, L"顶边", UI属性类型::数值, 树型框::_df.顶边, NULL, L"", false, false),
	_MK属性(2, L"宽度", UI属性类型::数值, 树型框::_df.宽度, NULL, L"", false, false),
	_MK属性(3, L"高度", UI属性类型::数值, 树型框::_df.高度, NULL, L"", false, false),
	_MK属性(4, L"可视", UI属性类型::布尔, 树型框::_df.可视, NULL, L"", false, false),
	_MK属性(5, L"禁止", UI属性类型::布尔, 树型框::_df.禁止, NULL, L"", false, false),
	_MK属性(6, L"可停留焦点", UI属性类型::布尔, 树型框::_df.可停留焦点, NULL, L"", false, false),
	_MK属性(7, L"停留顺序", UI属性类型::整数, 树型框::_df.停留顺序, NULL, L"", false, false),
	_MK属性(8, L"边框", UI属性类型::枚举, (int)树型框::_df.边框, L"0.无边框|1.凹入式|2.凸出式|3.浅凹入式|4.镜框式|5.单线边框式", L"", false, false),
	_MK属性(9, L"文本颜色", UI属性类型::颜色, (int)树型框::_df.文本颜色, NULL, L"", false, false),
	_MK属性(10, L"背景颜色", UI属性类型::颜色, (int)树型框::_df.背景颜色, NULL, L"", false, false),
	_MK属性(11, L"显示加减框", UI属性类型::布尔, 树型框::_df.显示加减框, NULL, L"", false, false),
	_MK属性(12, L"显示连线", UI属性类型::布尔, 树型框::_df.显示连线, NULL, L"", false, false),
	_MK属性(13, L"显示根部线", UI属性类型::布尔, 树型框::_df.显示根部线, NULL, L"", false, false),
	_MK属性(14, L"允许编辑", UI属性类型::布尔, 树型框::_df.允许编辑, NULL, L"", false, false),
	_MK属性(15, L"始终显示选择项", UI属性类型::布尔, 树型框::_df.始终显示选择项, NULL, L"", false, false),
	_MK属性(16, L"现行选中项", UI属性类型::整数, 树型框::_df.现行选中项, NULL, L"", false, false),
	_MK属性(17, L"行高", UI属性类型::整数, 树型框::_df.行高, NULL, L"", false, false),
	_MK属性(18, L"字体", UI属性类型::字体, &树型框::_df.字体, NULL, L"", false, false),
	_MK属性(19, L"图片组", UI属性类型::自定数据, &树型框::_df.图片组, NULL, L"", false, false),
	_MK属性(20, L"项目", UI属性类型::自定数据, &树型框::_df.项目, NULL, L"", false, false),
	_MK属性(21, L"是否有检查框", UI属性类型::布尔, 树型框::_df.是否有检查框, NULL, L"", false, false),
	_MK属性(22, L"检查框图片组", UI属性类型::自定数据, &树型框::_df.检查框图片组, NULL, L"", false, false),
	_MK属性(23, L"检查框状态数", UI属性类型::整数, 树型框::_df.检查框状态数, NULL, L"", false, false),
};

namespace UI树型框二维编辑器实现 {

class 编辑窗口 {
public:
	bool 已确认 = false;
	Bytes 编辑结果;
	BEIDEFN IDE回调 = NULL;
	HandleFONT API字体;
	HWND 窗口 = NULL, 列表 = NULL;
	HWND 标题框 = NULL, 图片框 = NULL, 选中图片框 = NULL, 数值框 = NULL, 加粗框 = NULL;
	int 当前 = -1;
	Arraybe<StrW> 文本;
	Arraybe<int> 图片, 选中图片, 数值, 状态, 缩进;
	Arraybe<bool> 加粗值, 展开值;

	static StrW 取文本(HWND h) { wchar_t buf[2048] = { 0 }; GetWindowTextW(h, buf, 2048); return StrW(buf); }
	static int 取整数(HWND h, int def = 0) { wchar_t buf[64] = { 0 }; GetWindowTextW(h, buf, 64); return buf[0] ? _wtoi(buf) : def; }
	static void 置整数(HWND h, int v) { wchar_t buf[32]; swprintf_s(buf, L"%d", v); SetWindowTextW(h, buf); }
	void 初始化API字体() { 字体 f; API字体 = f.到字体句柄(); }
	void 置API字体(HWND h) { if (h) SendMessageW(h, WM_SETFONT, (WPARAM)(API字体 ? (HFONT)API字体 : (HFONT)GetStockObject(DEFAULT_GUI_FONT)), TRUE); }
	HWND 创建API控件(const wchar_t* cls, const wchar_t* text, DWORD style, int x, int y, int w, int h, int id, DWORD ex = 0) {
		HWND c = CreateWindowExW(ex, cls, text, WS_CHILD | WS_VISIBLE | style,
			dpi(x), dpi(y), dpi(w), dpi(h), 窗口, (HMENU)(INT_PTR)id, g_hInst, NULL);
		置API字体(c); return c;
	}

	void 解析(const Bytes* dat) {
		if (!dat || !dat->buf || dat->size <= 0) return;
		deFB(*dat, 文本, 图片, 选中图片, 数值, 状态, 加粗值, 展开值, 缩进);
	}
	void 刷新列表() {
		SendMessageW(列表, LB_RESETCONTENT, 0, 0);
		for (int i = 0; i < 文本.count; ++i) {
			StrW s;
			for (int n = 1; n < 缩进[i]; ++n) s += L"    ";
			s += 文本[i];
			int p = (int)SendMessageW(列表, LB_ADDSTRING, 0, (LPARAM)(const wchar_t*)s);
			SendMessageW(列表, LB_SETITEMDATA, p, i);
		}
	}
	void 保存当前() {
		if (当前 < 0 || 当前 >= 文本.count) return;
		文本[当前] = 取文本(标题框);
		图片[当前] = 取整数(图片框, 0);
		选中图片[当前] = 取整数(选中图片框, -1);
		数值[当前] = 取整数(数值框, 0);
		加粗值[当前] = SendMessageW(加粗框, BM_GETCHECK, 0, 0) == BST_CHECKED;
	}
	void 刷新当前标题() {
		if (当前 < 0 || 当前 >= 文本.count) return;
		文本[当前] = 取文本(标题框);
		int sel = (int)SendMessageW(列表, LB_GETCURSEL, 0, 0);
		if (sel == LB_ERR) return;
		StrW s;
		for (int n = 1; n < 缩进[当前]; ++n) s += L"    ";
		s += 文本[当前];
		SendMessageW(列表, LB_DELETESTRING, sel, 0);
		int p = (int)SendMessageW(列表, LB_INSERTSTRING, sel, (LPARAM)(const wchar_t*)s);
		SendMessageW(列表, LB_SETITEMDATA, p, 当前);
		SendMessageW(列表, LB_SETCURSEL, p, 0);
	}
	void 载入选中() {
		int sel = (int)SendMessageW(列表, LB_GETCURSEL, 0, 0); if (sel == LB_ERR) return;
		当前 = (int)SendMessageW(列表, LB_GETITEMDATA, sel, 0);
		SetWindowTextW(标题框, 文本[当前]); 置整数(图片框, 图片[当前]); 置整数(选中图片框, 选中图片[当前]);
		置整数(数值框, 数值[当前]);
		SendMessageW(加粗框, BM_SETCHECK, 加粗值[当前] ? BST_CHECKED : BST_UNCHECKED, 0);
	}
	void 选择图片(bool 选中态) {
		if (!IDE回调) return;
		Bytes* 图片组数据 = IDE回调(IDE方法::取属性值, FB(L"图片组"));
		HWND 目标 = 选中态 ? 选中图片框 : 图片框;
		int v = 取整数(目标, 选中态 ? -1 : 0);
		if (图片组数据 && UI_图片组_选择图片(图片组数据, v, v, L"图片浏览选择", true)) 置整数(目标, v);
		delete 图片组数据;
	}
	void 插入(bool 子项) {
		保存当前();
		int level = 1, pos = 文本.count;
		if (子项 && 当前 >= 0 && 当前 < 文本.count) {
			level = 缩进[当前] + 1; pos = 当前 + 1;
			while (pos < 缩进.count && 缩进[pos] > 缩进[当前]) ++pos;
		}
		文本.insert(pos, StrW(子项 ? L"子项目" : L"新项目")); 图片.insert(pos, 0); 选中图片.insert(pos, -1);
		数值.insert(pos, 0); 状态.insert(pos, 0); 加粗值.insert(pos, false); 展开值.insert(pos, false); 缩进.insert(pos, level);
		刷新列表(); SendMessageW(列表, LB_SETCURSEL, pos, 0); 载入选中();
		SetFocus(标题框); SendMessageW(标题框, EM_SETSEL, 0, -1);
	}
	void 删除当前() {
		if (当前 < 0 || 当前 >= 文本.count) return;
		保存当前(); int end = 当前 + 1; while (end < 缩进.count && 缩进[end] > 缩进[当前]) ++end;
		int n = end - 当前;
		文本.del(当前, n); 图片.del(当前, n); 选中图片.del(当前, n); 数值.del(当前, n); 状态.del(当前, n); 加粗值.del(当前, n); 展开值.del(当前, n); 缩进.del(当前, n);
		当前 = -1; 刷新列表();
	}
	Bytes 序列化() { 保存当前(); return FB(文本, 图片, 选中图片, 数值, 状态, 加粗值, 展开值, 缩进); }

	static LRESULT CALLBACK WndProc(HWND h, UINT m, WPARAM w, LPARAM l) {
		编辑窗口* p = (编辑窗口*)GetWindowLongPtrW(h, GWLP_USERDATA);
		if (m == WM_NCCREATE) { p = (编辑窗口*)((CREATESTRUCTW*)l)->lpCreateParams; SetWindowLongPtrW(h, GWLP_USERDATA, (LONG_PTR)p); p->窗口 = h; }
		if (!p) return DefWindowProcW(h, m, w, l);
		if (m == WM_COMMAND) {
			int id = LOWORD(w), code = HIWORD(w);
			if ((HWND)l == p->标题框 && code == EN_CHANGE) { p->刷新当前标题(); return 0; }
			if (id == 100 && code == LBN_SELCHANGE) { p->保存当前(); p->载入选中(); SetFocus(p->标题框); SendMessageW(p->标题框, EM_SETSEL, 0, -1); return 0; }
			if (id == 201) { p->插入(false); return 0; }
			if (id == 202) { p->插入(true); return 0; }
			if (id == 203) { p->删除当前(); return 0; }
			if (id == 204) { p->选择图片(false); return 0; }
			if (id == 205) { p->选择图片(true); return 0; }
			if (id == IDOK) { p->编辑结果 = p->序列化(); p->已确认 = true; DestroyWindow(h); return 0; }
			if (id == IDCANCEL) { DestroyWindow(h); return 0; }
		}
		if (m == WM_CLOSE) { DestroyWindow(h); return 0; }
		return DefWindowProcW(h, m, w, l);
	}
	Bytes* 打开(const Bytes* dat, BEIDEFN ide回调 = NULL) {
		IDE回调 = ide回调;
		解析(dat);
		static bool cls = false;
		if (!cls) { WNDCLASSW wc = {}; wc.lpfnWndProc = WndProc; wc.hInstance = g_hInst; wc.hCursor = LoadCursorW(NULL, IDC_ARROW); wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1); wc.lpszClassName = L"BETree2DEditor"; RegisterClassW(&wc); cls = true; }
		HWND owner = GetActiveWindow();
		int wndW = dpi(560.0f), wndH = dpi(368.0f);
		RECT rcCenter = { 0 };
		if (owner && IsWindow(owner)) GetWindowRect(owner, &rcCenter);
		else SystemParametersInfoW(SPI_GETWORKAREA, 0, &rcCenter, 0);
		int wndX = rcCenter.left + ((rcCenter.right - rcCenter.left) - wndW) / 2;
		int wndY = rcCenter.top + ((rcCenter.bottom - rcCenter.top) - wndH) / 2;
		窗口 = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, L"BETree2DEditor", L"树型框项目设置", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
			wndX, wndY, wndW, wndH, owner, NULL, g_hInst, this);
		初始化API字体();
		列表 = 创建API控件(L"LISTBOX", L"", LBS_NOTIFY | WS_VSCROLL | WS_TABSTOP, 11, 11, 280, 272, 100, WS_EX_CLIENTEDGE);
		创建API控件(L"STATIC", L"标题:", SS_LEFT, 306, 16, 53, 18, 0); 标题框 = 创建API控件(L"EDIT", L"", ES_AUTOHSCROLL | WS_TABSTOP, 363, 14, 166, 20, 101, WS_EX_CLIENTEDGE);
		创建API控件(L"STATIC", L"图片索引:", SS_LEFT, 306, 46, 56, 18, 0); 图片框 = 创建API控件(L"EDIT", L"0", ES_AUTOHSCROLL | WS_TABSTOP, 446, 45, 52, 20, 102, WS_EX_CLIENTEDGE);
		创建API控件(L"BUTTON", L"...", BS_PUSHBUTTON | WS_TABSTOP, 503, 44, 26, 20, 204);
		创建API控件(L"STATIC", L"选中后图片索引:", SS_LEFT, 306, 77, 116, 18, 0); 选中图片框 = 创建API控件(L"EDIT", L"-1", ES_AUTOHSCROLL | WS_TABSTOP, 446, 75, 52, 20, 103, WS_EX_CLIENTEDGE);
		创建API控件(L"BUTTON", L"...", BS_PUSHBUTTON | WS_TABSTOP, 503, 74, 26, 20, 205);
		创建API控件(L"STATIC", L"项目数值:", SS_LEFT, 306, 107, 72, 18, 0); 数值框 = 创建API控件(L"EDIT", L"0", ES_AUTOHSCROLL | WS_TABSTOP, 446, 106, 52, 20, 104, WS_EX_CLIENTEDGE);
		加粗框 = 创建API控件(L"BUTTON", L"加粗", BS_AUTOCHECKBOX | WS_TABSTOP, 306, 141, 72, 20, 105);
		创建API控件(L"STATIC", L"注：图片索引从0开始，用作指定“图片组”属性中的某张图片。选中后图片索引用于指定当项目被选中后显示的图片。项目数值指定一个与当前项目相关联的值，以后可在程序中读出使用。",
			SS_LEFT | SS_EDITCONTROL, 306, 178, 223, 85, 0, WS_EX_CLIENTEDGE);
		创建API控件(L"BUTTON", L"加项目(&A)", BS_PUSHBUTTON | WS_TABSTOP, 11, 292, 88, 22, 201);
		创建API控件(L"BUTTON", L"加子项目(&S)", BS_PUSHBUTTON | WS_TABSTOP, 106, 292, 88, 22, 202);
		创建API控件(L"BUTTON", L"删除项目(&D)", BS_PUSHBUTTON | WS_TABSTOP, 200, 292, 88, 22, 203);
		创建API控件(L"BUTTON", L"确认(&O)", BS_DEFPUSHBUTTON | WS_TABSTOP, 363, 292, 75, 22, IDOK);
		创建API控件(L"BUTTON", L"取消(&C)", BS_PUSHBUTTON | WS_TABSTOP, 454, 292, 75, 22, IDCANCEL);
		刷新列表(); if (SendMessageW(列表, LB_GETCOUNT, 0, 0) > 0) { SendMessageW(列表, LB_SETCURSEL, 0, 0); 载入选中(); }
		ShowWindow(窗口, SW_SHOW); SetWindowPos(窗口, HWND_TOPMOST, wndX, wndY, wndW, wndH, SWP_SHOWWINDOW); SetForegroundWindow(窗口);
		MSG msg; while (IsWindow(窗口) && GetMessageW(&msg, NULL, 0, 0) > 0) { TranslateMessage(&msg); DispatchMessageW(&msg); }
		if (owner && IsWindow(owner)) SetForegroundWindow(owner);
		return 已确认 ? new Bytes(编辑结果) : NULL;
	}
};
inline Bytes* 编辑项目(const Bytes* 当前数据, BEIDEFN ide方法) { 编辑窗口 w; return w.打开(当前数据, ide方法); }

} // namespace UI树型框二维编辑器实现

void UI_树型框_置属性(树型框* ui, int methodi, byte* fb)
{
	switch (methodi)
	{
	case 0:
		ui->移动(gFBf(fb, 0), 空, 空, 空);
		break;
	case 1:
		ui->移动(空, gFBf(fb, 0), 空, 空);
		break;
	case 2:
		ui->移动(空, 空, gFBf(fb, 0), 空);
		break;
	case 3:
		ui->移动(空, 空, 空, gFBf(fb, 0));
		break;
	case 4:
		//可视
		break;
	case 5:
		//禁止
		break;
	case 6:
		//可停留焦点
		break;
	case 7:
		//停留顺序
		break;
	case 8:
		ui->边框_(gFBi(fb, 0));
		break;
	case 9:
		ui->文本颜色_(gFBi(fb, 0));
		break;
	case 10:
		ui->背景颜色_(gFBi(fb, 0));
		break;
	case 11:
		ui->显示加减框_(gFBb(fb, 0));
		break;
	case 12:
		ui->显示连线_(gFBb(fb, 0));
		break;
	case 13:
		ui->显示根部线_(gFBb(fb, 0));
		break;
	case 14:
		ui->允许编辑_(gFBb(fb, 0));
		break;
	case 15:
		ui->始终显示选择项_(gFBb(fb, 0));
		break;
	case 16:
		ui->现行选中项_(gFBi(fb, 0));
		break;
	case 17:
		ui->行高_(gFBi(fb, 0));
		break;
	case 18:
		ui->置字体(gFBpo<字体>(fb, 0));
		break;
	case 19: {
		Bytes data = gFB(fb, 0);
		图片组 imageList;
		imageList.h = imageList.导入图片组字节集(data);
		if (imageList.h) ui->置图片组(imageList);
		break;
	}
	case 20:
		ui->项目_(gFB(fb, 0));
		break;
	case 21:
		ui->是否有检查框_(gFBb(fb, 0));
		break;
	case 22: {
		Bytes data = gFB(fb, 0);
		图片组 imageList;
		imageList.h = imageList.导入图片组字节集(data);
		if (imageList.h) ui->置检查框图片组(imageList);
		break;
	}
	case 23:
		ui->检查框状态数_(gFBi(fb, 0));
		break;
	}
}
