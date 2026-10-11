#pragma once
#include "BEWin32UI.h"
#include <CommDlg.h>

EXPC Bytes* CBytes_New(int size = 0) { return new Bytes(size); }
EXPC Bytes* CBytes_New2(void* p, int size = 0) { return new Bytes(p, size); }
EXPC byte* CBytes_GetPointer(Bytes* b) { return b->buf; }
EXPC byte* CBytes_GetPointer2(Bytes* b, int& size) { size = b->size; return b->buf; }
EXPC int CBytes_GetSize(Bytes* b) { return b->size; }
EXPC void CBytes_Free(Bytes* b) { delete b; }
EXPC void CBytes_Reset(Bytes* b, int len) { b->reset(len); }
EXPC void CBytes_CopyFrom(Bytes* b, void* p, int len) { b->copyFrom(p, len); }
EXPC void CBytes_CopyFrom2(Bytes* b, int startI, void* p, int len) { b->copyFrom(startI, p, len); }
EXPC 字体* CFont_New(charW* 名称, int 大小, bool 下划线, bool 删除线, bool 倾斜, bool 加粗, int 角度) {
	字体* f = new 字体;
	f->字体名称 = 名称;
	f->字体大小 = 大小;
	f->下划线 = 下划线;
	f->删除线 = 删除线;
	f->倾斜 = 倾斜;
	f->加粗 = 加粗;
	f->角度 = 角度;
	return f;
}
EXPC void CFont_Free(字体* f) { delete f; }
// 设计器不得依赖 C++ 对象成员偏移读取 HWND；窗口类层次调整后对象布局会变化。
// 实组件统一通过本模块导出的 ABI 取其原生窗口句柄，虚组件不会调用此接口。
EXPC HWND UI_Hwnd(窗口基类* ui) { return ui ? ui->窗口句柄 : NULL; }

// 每个扩展 DLL 都静态链接一份 BEWin32UI，也各自拥有一份 字体::_hdc。
// 设计器可能直接创建扩展控件而不先创建该模块的窗口，因此在 SDK
// 静态初始化阶段给当前模块准备字体换算所需的屏幕 DC。
// 同时保留宿主窗口计数基线，避免临时编辑窗口销毁时发送 WM_QUIT。
static int _VSDllSDK初始化 = []() {
	字体::_hdc = GetDC(NULL);
	return _g_wndCount = 1;
}();

// VS 设计器 DLL 载入 SDK 后为本模块保留宿主窗口计数基线，
// 避免临时编辑窗口销毁时因计数归零向宿主 UI 线程发送 WM_QUIT。
static 
#include <FB/FB.cpp>

void UI_销毁控件(组件基类* 子组件)
{
	容器类* parent = 子组件->父容器;
	if (parent) {
		数组_删除元素值成员(parent->_组件列表, 子组件);
	}
	子组件->销毁(true);
	delete 子组件;
}

namespace UI一维数组编辑窗口实现 {

class 一维数组编辑窗口;

struct 数组列表框 : 列表框 {
	一维数组编辑窗口* 主窗口 = NULL;
	void 事件_列表项被选择() override;
	void 事件_双击选择() override;
};

struct 数组项目编辑框 : 编辑框 {
	一维数组编辑窗口* 主窗口 = NULL;
	void 事件_内容被改变() override;
};

struct 数组操作按钮 : 按钮 {
	enum 操作类型 { 确定, 取消, 前插, 后插, 删除 } 操作;
	一维数组编辑窗口* 主窗口 = NULL;
	void 事件_被单击() override;
};

class 一维数组编辑窗口 : public 窗口 {
public:
	数组列表框 项目列表;
	数组项目编辑框 标题编辑框;
	标签 修改提示标签;
	数组操作按钮 确定按钮, 取消按钮, 前插按钮, 后插按钮, 删除按钮;
	Arraybe<StrW> 初始项目;
	Bytes 编辑结果;
	int 当前索引 = -1;
	bool 正在同步 = false;
	bool 已确认 = false;
	bool 整数数组模式 = false;
	StrW 项目称呼 = L"";
	void 解析当前数据(const Bytes* 当前数据) {
		初始项目.clear();
		if (!当前数据 || !当前数据->buf || 当前数据->size <= 0) return;
		if (整数数组模式) {
			const int* p = (const int*)当前数据->buf;
			int n = (int)(当前数据->size / sizeof(int));
			for (int i = 0; i < n; i++) {
				wchar_t 文本[32] = { 0 };
				swprintf_s(文本, L"%d", p[i]);
				初始项目.push(StrW(文本));
			}
			return;
		}
		if (当前数据->size >= sizeof(charW)) {
			const charW* p = (const charW*)当前数据->buf;
			int 字符数 = (int)(当前数据->size / sizeof(charW));
			int 起点 = 0;
			for (int i = 0; i < 字符数; i++) {
				if (p[i] != 0) continue;
				初始项目.push(StrW(p + 起点, i - 起点));
				起点 = i + 1;
			}
			if (起点 < 字符数) 初始项目.push(StrW(p + 起点, 字符数 - 起点));
		}
	}

	Bytes 序列化项目() {
		int n = 项目列表.取项目数();
		if (整数数组模式) {
			Bytes r((size_t)n * sizeof(int));
			for (int i = 0; i < n; i++) {
				StrW 文本 = (i == 当前索引) ? 标题编辑框._取内容()
					: 项目列表.取项目文本(i);
				int 数值 = _wtoi((const wchar_t*)文本);
				r.copyFrom((size_t)i * sizeof(int), &数值, sizeof(数值));
			}
			return r;
		}
		size_t 总字节数 = 0;
		for (int i = 0; i < n; i++) {
			StrW 文本 = (i == 当前索引) ? 标题编辑框._取内容()
				: 项目列表.取项目文本(i);
			总字节数 += 文本._size() + sizeof(charW);
		}
		Bytes r(总字节数);
		size_t 写入位置 = 0;
		charW 零 = 0;
		for (int i = 0; i < n; i++) {
			// 确定按钮可能在编辑框尚未失去焦点时被按下；当前项直接
			// 读取编辑框，保证最后一次键入一定进入返回字节集。
			StrW 文本 = (i == 当前索引) ? 标题编辑框._取内容()
				: 项目列表.取项目文本(i);
			if (文本._size()) {
				r.copyFrom(写入位置, 文本._buf(), 文本._size());
				写入位置 += 文本._size();
			}
			r.copyFrom(写入位置, &零, sizeof(零));
			写入位置 += sizeof(零);
		}
		return r;
	}

	void 保存当前标题() {
		if (正在同步 || 当前索引 < 0 || 当前索引 >= 项目列表.取项目数()) return;
		正在同步 = true;
		项目列表.置项目文本(当前索引, 标题编辑框._取内容());
		正在同步 = false;
	}

	void 标题内容被改变() {
		保存当前标题();
	}

	void 选择项目(int 索引) {
		正在同步 = true;
		int n = 项目列表.取项目数();
		if (索引 < 0 || 索引 >= n) {
			当前索引 = -1;
			项目列表.现行选中项_(-1);
			标题编辑框.内容_(L"");
			标题编辑框.禁止_(true);
			StrW 提示 = L"当前没有可编辑的";
			提示 += 项目称呼;
			提示 += L"：";
			修改提示标签.标题_(提示);
		} else {
			当前索引 = 索引;
			项目列表.现行选中项_(索引);
			标题编辑框.禁止_(false);
			标题编辑框.内容_(项目列表.取项目文本(索引));
			wchar_t 提示[64] = { 0 };
			swprintf_s(提示, L"在此修改第 %d 项的内容：", 索引 + 1);
			修改提示标签.标题_(提示);
		}
		正在同步 = false;
	}

	void 列表选择被改变() {
		if (正在同步) return;
		// 先保存用户刚点击的新索引。列表框::置项目文本内部会删除并
		// 重插旧项目，从而短暂改变原生 ListBox 的当前选择。
		int 新索引 = 项目列表._取现行选中项();
		保存当前标题();
		选择项目(新索引);
	}

	void 执行操作(数组操作按钮::操作类型 操作) {
		if (操作 == 数组操作按钮::确定) {
			// 按钮可在编辑框仍保持焦点时触发；先把 HWND 中的最新文字
			// 明确写回列表，再进行最终序列化。
			保存当前标题();
			编辑结果 = 序列化项目();
			已确认 = true;
			销毁();
			return;
		}
		if (操作 == 数组操作按钮::取消) {
			已确认 = false;
			销毁();
			return;
		}

		保存当前标题();
		int n = 项目列表.取项目数();
		if (操作 == 数组操作按钮::删除) {
			if (当前索引 < 0 || 当前索引 >= n) return;
			int old = 当前索引;
			项目列表.删除项目(old);
			选择项目(old < 项目列表.取项目数() ? old : 项目列表.取项目数() - 1);
			return;
		}

		int 插入位置;
		if (当前索引 < 0) 插入位置 = n;
		else if (操作 == 数组操作按钮::前插) 插入位置 = 当前索引;
		else 插入位置 = 当前索引 + 1;
		int 新索引 = 项目列表.插入项目(插入位置, L"");
		选择项目(新索引);
		标题编辑框.获取焦点();
	}

	bool 事件_被关闭() override {
		已确认 = false;
		return true;
	}

	void 事件_创建完毕() override {
		选择项目(项目列表.取项目数() ? 0 : -1);
		if (当前索引 >= 0) {
			标题编辑框.获取焦点();
			标题编辑框.置被选择字符数(-1, 0);
		}
	}

	void 事件_快捷键(WORD cmdID) override {
		if (cmdID == 快捷键_取ID(VK_RETURN)) 执行操作(数组操作按钮::确定);
	}

	bool 打开(const Bytes* 当前数据, bool 整数模式 = false,
		const charW* 窗口标题 = L"一维数组管理", const charW* 称呼 = L"项目") {
		整数数组模式 = 整数模式;
		项目称呼 = 称呼;
		解析当前数据(当前数据);

		窗口::参数 cs;
		cs.标题 = 窗口标题;
		cs.类名 = L"BEOneDimArrayEditorWindow";
		cs.宽度 = 430; cs.高度 = 310;
		cs.边框 = 窗口边框::普通固定边框;
		cs.控制按钮 = true; cs.最小化按钮 = false; cs.最大化按钮 = false;
		cs.Esc键关闭 = true; cs.子控件焦点导航 = true;
		cs.图标ID = 0; cs.可视 = false;
		cs.总在最前 = true;
		cs.在任务栏显示 = false;
		if (!窗口::创建(cs, NULL, false)) {
			return false;
		}
		// 窗口基类只会在首次创建全局隐藏窗口时顺带设置 owner；设计器
		// 进程此时通常早已有 _g_interWnd，因此这里显式建立 owner 关系，
		// 保证编辑器即使后创建也永远不进入任务栏。
		if (_g_interWnd && _g_interWnd->窗口句柄) {
			SetWindowLongPtrW(窗口句柄, GWLP_HWNDPARENT,
				(LONG_PTR)_g_interWnd->窗口句柄);
		}
		LONG_PTR 编辑窗口扩展风格 = GetWindowLongPtrW(窗口句柄, GWL_EXSTYLE);
		编辑窗口扩展风格 &= ~WS_EX_APPWINDOW;
		SetWindowLongPtrW(窗口句柄, GWL_EXSTYLE, 编辑窗口扩展风格);

		列表框::参数 lc;
		lc.左边 = 12; lc.顶边 = 12; lc.宽度 = 230; lc.高度 = 190;
		lc.可停留焦点 = true;
		项目列表.主窗口 = this;
		项目列表.创建(lc, this);
		// 编辑窗口对象每次都是一次全新的模态会话；显式恢复列表框的
		// enabled 状态，避免上一次销毁过程留下的 Win32 禁用状态影响复用。
		项目列表.禁止_(false);
		for (int i = 0; i < 初始项目.count; i++) 项目列表.加入项目(初始项目[i]);

		创建按钮(确定按钮, 数组操作按钮::确定, L"确定(&O)", 255, 12);
		创建按钮(取消按钮, 数组操作按钮::取消, L"取消(&C)", 255, 46);
		创建按钮(前插按钮, 数组操作按钮::前插, L"向前插入空项目(&P)", 255, 108);
		创建按钮(后插按钮, 数组操作按钮::后插, L"向后插入空项目(&N)", 255, 142);
		创建按钮(删除按钮, 数组操作按钮::删除, L"删除当前项目(&D)", 255, 176);
		窗口_添加风格(确定按钮.窗口句柄, BS_DEFPUSHBUTTON);

		标签::参数 tc;
		tc.左边 = 12; tc.顶边 = 214; tc.宽度 = 388; tc.高度 = 20;
		StrW 初始提示 = L"当前没有可编辑的";
		初始提示 += 项目称呼;
		初始提示 += L"：";
		tc.标题 = 初始提示;
		修改提示标签.创建(tc, this);

		编辑框::参数 ec;
		ec.左边 = 12; ec.顶边 = 238; ec.宽度 = 388; ec.高度 = 22;
		ec.可停留焦点 = true;
		标题编辑框.主窗口 = this;
		标题编辑框.创建(ec, this);

		快捷键_注册(VK_RETURN);
		_组件列表.clear();
		可视 = true;
		// 自定义属性编辑器不进入任务栏，并始终保持在 VS 之上。
		ShowWindow(窗口句柄, SW_SHOWNORMAL);
		SetForegroundWindow(窗口句柄);
		窗口::完毕(true);
		return 已确认;
	}

private:
	void 创建按钮(数组操作按钮& btn, 数组操作按钮::操作类型 op,
		const charW* 标题, float 左, float 顶) {
		btn.主窗口 = this; btn.操作 = op;
		按钮::参数 bc;
		bc.左边 = 左; bc.顶边 = 顶; bc.宽度 = 145; bc.高度 = 26;
		bc.标题 = 标题; bc.可停留焦点 = true;
		btn.创建(bc, this);
	}
};

inline void 数组列表框::事件_列表项被选择() {
	if (主窗口) 主窗口->列表选择被改变();
}
inline void 数组列表框::事件_双击选择() {
	if (主窗口) {
		主窗口->列表选择被改变();
		主窗口->标题编辑框.获取焦点();
		主窗口->标题编辑框.置被选择字符数(-1, 0);
	}
}
inline void 数组项目编辑框::事件_内容被改变() {
	if (主窗口) 主窗口->标题内容被改变();
}
inline void 数组操作按钮::事件_被单击() {
	if (主窗口) 主窗口->执行操作(操作);
}

} // namespace UI一维数组编辑窗口实现

Bytes* UI_一维数组_编辑窗口(const Bytes* 当前数据, bool 整数数组模式,
	const charW* 窗口标题, const charW* 项目称呼)
{
	UI一维数组编辑窗口实现::一维数组编辑窗口 w;
	if (!w.打开(当前数据, 整数数组模式, 窗口标题, 项目称呼)) {
		return NULL;
	}
	return new Bytes(w.编辑结果);
}


namespace UI图片组原生编辑器实现 {

enum {
	ID_图标列表 = 1001, ID_宽度 = 1002, ID_高度 = 1003,
	ID_加入 = 1010, ID_插入, ID_删除, ID_清除, ID_导出, ID_导入, ID_合并,
	ID_确定 = IDOK, ID_取消 = IDCANCEL, ID_不选择 = 1020
};

class 图片组窗口 : public 窗口 {
public:
	HWND 图标列表 = NULL, 宽度框 = NULL, 高度框 = NULL;
	HandleFONT API字体;
	HIMAGELIST DPI显示图片组 = NULL;
	图片组 图组;
	bool 已确认 = false, 选择模式 = false, 尺寸锁定 = false;
	int 选择结果 = -1;
	Bytes 编辑结果;

	~图片组窗口() {
		if (图标列表 && IsWindow(图标列表))
			ListView_SetImageList(图标列表, NULL, LVSIL_NORMAL);
		if (DPI显示图片组) {
			ImageList_Destroy(DPI显示图片组);
			DPI显示图片组 = NULL;
		}
	}

	void 初始化API字体() {
		字体 f;
		API字体 = f.到字体句柄();
	}
	void 置API字体(HWND h) {
		if (h) SendMessageW(h, WM_SETFONT,
			(WPARAM)(API字体 ? (HFONT)API字体 : (HFONT)GetStockObject(DEFAULT_GUI_FONT)), TRUE);
	}
	HWND 创建API控件(const wchar_t* cls, const wchar_t* text, DWORD style,
		int x, int y, int w, int h, int id, DWORD ex = 0) {
		HWND c = CreateWindowExW(ex, cls, text, WS_CHILD | WS_VISIBLE | style,
			dpi(x), dpi(y), dpi(w), dpi(h), 窗口句柄, (HMENU)(INT_PTR)id, g_hInst, NULL);
		置API字体(c);
		return c;
	}
	static int 取编辑整数(HWND h, int 默认值) {
		wchar_t buf[32] = { 0 };
		GetWindowTextW(h, buf, 32);
		return buf[0] ? _wtoi(buf) : 默认值;
	}
	static void 置编辑整数(HWND h, int v) {
		wchar_t buf[32] = { 0 }; swprintf_s(buf, L"%d", v); SetWindowTextW(h, buf);
	}
	void 锁定尺寸() {
		尺寸锁定 = true;
		if (宽度框) EnableWindow(宽度框, FALSE);
		if (高度框) EnableWindow(高度框, FALSE);
	}
	void 载入(const Bytes* dat) {
		if (!dat || !dat->buf || dat->size <= 0) return;
		图组.h = 图组.导入图片组字节集(*dat);
		if (图组.h) {
			SIZE sz = 图组.取图标尺寸();
			置编辑整数(宽度框, sz.cx); 置编辑整数(高度框, sz.cy);
			锁定尺寸();
		}
	}
	void 重建DPI显示图片组() {
		if (!图标列表) return;
		ListView_SetImageList(图标列表, NULL, LVSIL_NORMAL);
		if (DPI显示图片组) {
			ImageList_Destroy(DPI显示图片组);
			DPI显示图片组 = NULL;
		}
		if (!图组.h) return;
		SIZE 原尺寸 = 图组.取图标尺寸();
		int 显示宽度 = dpi((float)原尺寸.cx);
		int 显示高度 = dpi((float)原尺寸.cy);
		if (显示宽度 <= 0) 显示宽度 = 1;
		if (显示高度 <= 0) 显示高度 = 1;
		int n = 图组.取图片数();
		DPI显示图片组 = ImageList_Create(显示宽度, 显示高度,
			ILC_COLOR32 | ILC_MASK, n, 2);
		if (!DPI显示图片组) return;
		for (int i = 0; i < n; ++i) {
			HICON 原图标 = 图组.取图标句柄_拷贝(i, ILD_NORMAL);
			if (!原图标) continue;
			HICON 显示图标 = (HICON)CopyImage(原图标, IMAGE_ICON,
				显示宽度, 显示高度, 0);
			ImageList_AddIcon(DPI显示图片组, 显示图标 ? 显示图标 : 原图标);
			if (显示图标) DestroyIcon(显示图标);
			DestroyIcon(原图标);
		}
		ListView_SetImageList(图标列表, DPI显示图片组, LVSIL_NORMAL);
	}
	void 刷新列表(int sel = -1) {
		ListView_DeleteAllItems(图标列表);
		重建DPI显示图片组();
		int n = 图组.h ? 图组.取图片数() : 0;
		for (int i = 0; i < n; ++i) {
			wchar_t t[32] = { 0 }; swprintf_s(t, L"%d", i);
			LVITEMW item = { 0 };
			item.mask = LVIF_TEXT | LVIF_IMAGE;
			item.iItem = i; item.iImage = i; item.pszText = t;
			ListView_InsertItem(图标列表, &item);
		}
		if (n > 0) {
			if (sel < 0 || sel >= n) sel = 0;
			ListView_SetItemState(图标列表, sel, LVIS_SELECTED | LVIS_FOCUSED,
				LVIS_SELECTED | LVIS_FOCUSED);
			ListView_EnsureVisible(图标列表, sel, FALSE);
		}
	}
	int 当前索引() const { return ListView_GetNextItem(图标列表, -1, LVNI_SELECTED); }
	static bool 选择文件(HWND owner, bool 保存, const wchar_t* filter, wchar_t* path, DWORD count) {
		OPENFILENAMEW o = { sizeof(o) };
		o.hwndOwner = owner; o.lpstrFilter = filter; o.lpstrFile = path; o.nMaxFile = count;
		o.Flags = OFN_EXPLORER | OFN_HIDEREADONLY | (保存 ? OFN_OVERWRITEPROMPT : OFN_FILEMUSTEXIST);
		return 保存 ? GetSaveFileNameW(&o) != FALSE : GetOpenFileNameW(&o) != FALSE;
	}
	static bool 写文件(const wchar_t* path, const Bytes& data) {
		HANDLE f = CreateFileW(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (f == INVALID_HANDLE_VALUE) return false;
		DWORD n = 0; BOOL ok = data.size == 0 || WriteFile(f, data.buf, data.size, &n, NULL);
		CloseHandle(f); return ok && n == (DWORD)data.size;
	}
	static Bytes 读文件(const wchar_t* path) {
		HANDLE f = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (f == INVALID_HANDLE_VALUE) return Bytes();
		DWORD size = GetFileSize(f, NULL), n = 0; Bytes r(size);
		if (size && !ReadFile(f, r.buf, size, &n, NULL)) r.reset();
		CloseHandle(f); if (n != size) r.reset(); return r;
	}
	static int 图片格式(const wchar_t* path) {
		const wchar_t* e = wcsrchr(path, L'.');
		return e && (_wcsicmp(e, L".ico") == 0 || _wcsicmp(e, L".cur") == 0) ? IMAGE_ICON : IMAGE_BITMAP;
	}
	bool 确保创建() {
		if (图组.h) return true;
		int w = 取编辑整数(宽度框, 16), h = 取编辑整数(高度框, 16);
		if (w <= 0 || h <= 0) return false;
		if (!图组.创建(w, h, ILC_COLOR32 | ILC_MASK, 0, 2)) return false;
		锁定尺寸(); return true;
	}
	void 加图片(bool 插入) {
		wchar_t path[MAX_PATH] = { 0 };
		if (!选择文件(窗口句柄, false, L"图片文件\0*.bmp;*.jpg;*.jpeg;*.gif;*.ico\0所有文件\0*.*\0", path, MAX_PATH)) return;
		if (!确保创建()) return;
		int old = 图组.取图片数(), pos = 插入 ? 当前索引() : old;
		if (pos < 0 || pos > old) pos = old;
		if (!插入 || pos == old) {
			int r = 图组.添加图片文件(path, 图片格式(path), false, 0, NULL);
			if (r >= 0) 刷新列表(r);
			return;
		}
		SIZE sz = 图组.取图标尺寸(); 图片组 新组;
		if (!新组.创建(sz.cx, sz.cy, ILC_COLOR32 | ILC_MASK, 0, 2)) return;
		for (int i = 0; i < pos; ++i) { HICON h = 图组.取图标句柄_拷贝(i, ILD_NORMAL); 新组.添加图标(h); DestroyIcon(h); }
		if (新组.添加图片文件(path, 图片格式(path), false, 0, NULL) < 0) return;
		for (int i = pos; i < old; ++i) { HICON h = 图组.取图标句柄_拷贝(i, ILD_NORMAL); 新组.添加图标(h); DestroyIcon(h); }
		ListView_SetImageList(图标列表, NULL, LVSIL_NORMAL);
		图组.销毁(); 图组.h = 新组.h; 新组.h = NULL; 刷新列表(pos);
	}
	void 删除当前() {
		int i = 当前索引(); if (!图组.h || i < 0) return;
		图组.删除图片(i); 刷新列表(i);
	}
	void 清空() {
		if (!图组.h) return;
		while (图组.取图片数() > 0) 图组.删除图片(图组.取图片数() - 1);
		刷新列表();
	}
	void 导出() {
		if (!图组.h) return;
		wchar_t path[MAX_PATH] = L"图片组.beimg";
		if (!选择文件(窗口句柄, true, L"白易图片组\0*.beimg\0所有文件\0*.*\0", path, MAX_PATH)) return;
		写文件(path, 图组.导出图片组到字节集());
	}
	void 导入(bool 合并) {
		wchar_t path[MAX_PATH] = { 0 };
		if (!选择文件(窗口句柄, false, L"白易图片组\0*.beimg\0所有文件\0*.*\0", path, MAX_PATH)) return;
		Bytes data = 读文件(path); if (!data) return;
		图片组 t; t.h = t.导入图片组字节集(data); if (!t.h) return;
		if (!合并 || !图组.h) {
			ListView_SetImageList(图标列表, NULL, LVSIL_NORMAL);
			图组.销毁(); 图组.h = t.h; t.h = NULL;
			SIZE sz = 图组.取图标尺寸(); 置编辑整数(宽度框, sz.cx); 置编辑整数(高度框, sz.cy);
			锁定尺寸(); 刷新列表(); return;
		}
		SIZE a = 图组.取图标尺寸(), b = t.取图标尺寸();
		if (a.cx != b.cx || a.cy != b.cy) {
			MessageBoxW(窗口句柄, L"合并的图片组尺寸必须一致。", L"图片组", MB_OK | MB_ICONINFORMATION); return;
		}
		for (int i = 0; i < t.取图片数(); ++i) { HICON h = t.取图标句柄_拷贝(i, ILD_NORMAL); 图组.添加图标(h); DestroyIcon(h); }
		刷新列表();
	}
	void 结束(bool ok) {
		if (ok) {
			if (选择模式) 选择结果 = 当前索引();
			else 编辑结果 = 图组.h ? 图组.导出图片组到字节集() : Bytes();
			已确认 = true;
		}
		销毁();
	}
	void 命令(int id) {
		switch (id) {
		case ID_加入: 加图片(false); break;
		case ID_插入: 加图片(true); break;
		case ID_删除: 删除当前(); break;
		case ID_清除: 清空(); break;
		case ID_导出: 导出(); break;
		case ID_导入: 导入(false); break;
		case ID_合并: 导入(true); break;
		case ID_不选择: 选择结果 = -1; 已确认 = true; 销毁(); break;
		case ID_确定: 结束(true); break;
		case ID_取消: 销毁(); break;
		}
	}
	bool 事件_被关闭() override { return true; }
	void 事件_快捷键(WORD cmdID) override {
		if (cmdID == 快捷键_取ID(VK_RETURN)) 命令(ID_确定);
	}
	LRESULT 挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override {
		switch (msg) {
		case WM_COMMAND:
			if (HIWORD(wParam) == BN_CLICKED) 命令(LOWORD(wParam));
			break;
		case WM_NOTIFY: {
			NMHDR* n = (NMHDR*)lParam;
			if (n && n->idFrom == ID_图标列表 && n->code == NM_DBLCLK && 选择模式) 结束(true);
			break;
		}
		}
		return 窗口::挂接消息(hwnd, msg, wParam, lParam);
	}
	bool 打开(const Bytes* dat, const charW* 标题, bool picker, int 当前 = -1, bool 模态打开 = false) {
		选择模式 = picker;
		初始化Win32公共控件();
		HWND 模态父窗口 = GetActiveWindow();
		if (模态父窗口 && GetAncestor(模态父窗口, GA_ROOT) != 模态父窗口)
			模态父窗口 = GetAncestor(模态父窗口, GA_ROOT);
		窗口::参数 cs;
		cs.标题 = 标题;
		cs.类名 = picker ? L"BEImagePickerWindow" : L"BEImageListWindow";
		cs.宽度 = picker ? 500 : 500;
		cs.高度 = picker ? 310 : 340;
		cs.边框 = 窗口边框::普通固定边框;
		cs.控制按钮 = true;
		cs.最小化按钮 = false;
		cs.最大化按钮 = false;
		cs.Esc键关闭 = true;
		cs.子控件焦点导航 = true;
		cs.图标ID = 0;
		cs.可视 = false;
		cs.总在最前 = true;
		cs.在任务栏显示 = false;
		if (!窗口::创建(cs, NULL, false)) return false;
		if (!模态父窗口 && _g_interWnd && _g_interWnd->窗口句柄)
			模态父窗口 = _g_interWnd->窗口句柄;
		if (模态父窗口 && 模态父窗口 != 窗口句柄)
			SetWindowLongPtrW(窗口句柄, GWLP_HWNDPARENT, (LONG_PTR)模态父窗口);
		SetWindowLongPtrW(窗口句柄, GWL_EXSTYLE,
			GetWindowLongPtrW(窗口句柄, GWL_EXSTYLE) & ~WS_EX_APPWINDOW);
		初始化API字体();

		DWORD ls = LVS_ICON | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_AUTOARRANGE | WS_TABSTOP;
		图标列表 = 创建API控件(WC_LISTVIEWW, L"", ls, 12, 12,
			460, picker ? 205 : 178, ID_图标列表, WS_EX_CLIENTEDGE);
		ListView_SetBkColor(图标列表, RGB(225, 244, 247));
		ListView_SetTextBkColor(图标列表, RGB(225, 244, 247));
		ListView_SetTextColor(图标列表, RGB(0, 0, 0));
		if (!picker) {
			创建API控件(L"STATIC", L"单一图片宽度：", SS_LEFT, 12, 198, 100, 22, 0);
			宽度框 = 创建API控件(L"EDIT", L"16", ES_AUTOHSCROLL | WS_TABSTOP, 112, 196, 64, 24, ID_宽度, WS_EX_CLIENTEDGE);
			创建API控件(L"STATIC", L"高度：", SS_LEFT, 192, 198, 42, 22, 0);
			高度框 = 创建API控件(L"EDIT", L"16", ES_AUTOHSCROLL | WS_TABSTOP, 236, 196, 64, 24, ID_高度, WS_EX_CLIENTEDGE);
			const wchar_t* bt[] = { L"加入",L"插入",L"删除",L"清除",L"导出",L"导入",L"合并",L"确定(&O)",L"取消(&C)" };
			int ids[] = { ID_加入,ID_插入,ID_删除,ID_清除,ID_导出,ID_导入,ID_合并,ID_确定,ID_取消 };
			for (int i = 0; i < 9; ++i)
				创建API控件(L"BUTTON", bt[i], (i == 7 ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON) | WS_TABSTOP,
					12 + i * 51, 228, 47, 27, ids[i]);
			创建API控件(L"STATIC",
				L"图片组内所有图片共用同一尺寸。只有属性原始数据为空时宽度、高度可编辑；一旦创建、导入或已有图片组，尺寸立即锁定。",
				SS_LEFT, 12, 266, 460, 42, 0);
			载入(dat); 刷新列表();
		} else {
			if (dat && dat->buf && dat->size > 0) 图组.h = 图组.导入图片组字节集(*dat);
			刷新列表(当前);
			创建API控件(L"BUTTON", L"不选择图片(&N)", BS_PUSHBUTTON | WS_TABSTOP, 12, 229, 125, 28, ID_不选择);
			创建API控件(L"BUTTON", L"确定(&O)", BS_DEFPUSHBUTTON | WS_TABSTOP, 304, 229, 80, 28, ID_确定);
			创建API控件(L"BUTTON", L"取消(&C)", BS_PUSHBUTTON | WS_TABSTOP, 392, 229, 80, 28, ID_取消);
		}
		快捷键_注册(VK_RETURN);
		// 常规从插件打开时只建立 owner 关系，不禁用拥有者；
		// 超级列表框等嵌套编辑器可要求真正模态，避免用户越过当前编辑层级操作父窗口。
		if (模态打开 && 模态父窗口 && IsWindow(模态父窗口))
			EnableWindow(模态父窗口, FALSE);
		可视 = true;
		SetForegroundWindow(窗口句柄);
		窗口::完毕(true);
		if (模态父窗口 && IsWindow(模态父窗口)) {
			if (模态打开) EnableWindow(模态父窗口, TRUE);
			SetActiveWindow(模态父窗口);
		}
		return 已确认;
	}
};

} // namespace UI图片组原生编辑器实现

Bytes* UI_图片组_编辑窗口(const Bytes* 当前数据, const charW* 窗口标题 = L"图片组设置")
{
	UI图片组原生编辑器实现::图片组窗口 w;
	if (!w.打开(当前数据, 窗口标题, false)) return NULL;
	return new Bytes(w.编辑结果);
}

bool UI_图片组_选择图片(const Bytes* 图片组数据, int 当前索引, int& 选择结果,
	const charW* 窗口标题 = L"图片浏览选择", bool 模态打开 = false)
{
	UI图片组原生编辑器实现::图片组窗口 w;
	if (!w.打开(图片组数据, 窗口标题, true, 当前索引, 模态打开)) return false;
	选择结果 = w.选择结果;
	return true;
}


//_________________________________________________________________________

void _UI_多夹_插入子夹(多夹接口* ui, LPWSTR 标题, int 索引) {
	ui->插入子夹(标题, 索引);
}
void _UI_多夹_子夹添加组件(多夹接口* ui, int 索引, 窗口基类* a) {
	ui->子夹添加组件(索引, *a);
}
Bytes* _UI_多夹_取子夹标题(多夹接口* ui, int 索引) {
	StrW r = ui->取子夹标题(索引);
	return CBytes_New2(r, r._size());
}
void _UI_多夹_置子夹标题(多夹接口* ui, int 索引, LPWSTR 标题) {
	ui->置子夹标题(索引, 标题);
}
int _UI_多夹_现行子夹(多夹接口* ui, int 索引) {
	return ui->现行子夹_(索引);
}
bool _UI_多夹_判定客户鼠标拖动(多夹接口* ui, int x, int y) {
	return ui->_判定客户鼠标拖动(x, y);
}

#pragma pack(1)
struct UI属性类型 {
	enum {
		枚举 = 0, 布尔 = 1, 颜色 = 2, 整数 = 3,
		数值 = 4, 数据 = 5, 文本 = 6, 字体 = 7,
		自定数据 = 10
	};
};

// fn全局属性面板 的“即将改变到属性值”在特殊调用中承载事件码。
// 使用强类型常量并提供 LPWSTR 转换，调用处无需显式指针强转。
class UI事件类型 {
	ULONG_PTR _值;
	constexpr explicit UI事件类型(ULONG_PTR 值) : _值(值) {}
public:
	operator LPWSTR() const { return (LPWSTR)_值; }
	static const UI事件类型 自定数据;
	static const UI事件类型 响应属性;
};
inline const UI事件类型 UI事件类型::自定数据(10);
inline const UI事件类型 UI事件类型::响应属性(11);

// “自定义”属性的持久化约定。XML 仅保存 _R窗口名::对象名_属性名；属性编辑
// 回调产生的原始 Bytes 存入项目 BEMod\_R.lib 中同名的 const byte 符号。
// 128 是 UTF-8 逻辑符号叶名称的上限，不是字节集容量；字节集不能填充，
// 否则 DLL 收到的长度会与真实数据不一致。
struct UI自定义属性资源 {
	static const char* 库文件名() { return "_R.lib"; }
	static const char* 命名空间前缀() { return "_R"; }
	enum { 符号名UTF8最大字节数 = 128 };
};
struct UI属性元信息 {
	int 序号;
	LPWSTR 名称;
	DWORD 类型;
	union 默认
	{ //真值类型只有以下四种，其他一律采用指针看待
		int 整数;
		bool 布尔;
		double 数值;
		LPWSTR 文本;
		constexpr 默认(int 整数) :整数(整数) {}
		constexpr 默认(uint 整数) : 整数(整数) {}
		constexpr 默认(DWORD 整数) : 整数(整数) {}
		constexpr 默认(bool 布尔) : 布尔(布尔) {}
		constexpr 默认(double 数值) : 数值(数值) {}
		constexpr 默认(LPWSTR 文本) : 文本(文本) {}
		constexpr 默认(void* 指针) : 指针(指针) {}
		constexpr 默认(const StrW& 文本) : 文本((LPWSTR)文本.bytes.buf) {}
		template<class T> constexpr 默认(NilOpt<T> v) {}
		void* 指针;
	}默认值;
	LPWSTR 枚举值; //L"0.左对齐|1.居中|2.右对齐" //用"|"进行分割
	LPWSTR 说明;
	BOOL 可空;
	BOOL 子项;
};
constexpr UI属性元信息 _MK属性(int 序号, LPWSTR 名称, DWORD 类型, UI属性元信息::默认 默认值,
	LPWSTR 枚举值, LPWSTR 说明, BOOL 可空, BOOL 子项) {
	return { 序号, 名称, 类型, 默认值, 枚举值, 说明, 可空, 子项 };
}

struct UI类型 {
	enum {
		控件 = 0, 容器 = 1, 多夹 = 2,
		虚拟控件 = 3, 虚拟容器 = 4
	};
};

struct UI库组件信息 {
	LPWSTR 名称;
	int 组件类型; //默认为 UI类型::控件=0
	LPWSTR 特化分组名; //如果为NULL则使用默认分组名
	int 图标大小;
	const byte* 图标;
	int 属性个数;
	void* 属性元信息;
	void* fn创建;
	void* fn置属性;
	void* 保留[10];
};

struct UI多夹类型 {
	void* fn插入子夹 = _UI_多夹_插入子夹;
	void* fn子夹添加组件 = _UI_多夹_子夹添加组件;
	void* fn取子夹标题 = _UI_多夹_取子夹标题;
	void* fn置子夹标题 = _UI_多夹_置子夹标题;
	void* fn现行子夹 = _UI_多夹_现行子夹;
	void* fn判定客户鼠标拖动 = _UI_多夹_判定客户鼠标拖动;
}_fn多夹;

typedef Bytes*(*BEIDEFN)(int fnID, byte* fbDat);
namespace IDE方法 {
enum {
	启用禁用属性 = 1, //void fb(LPWSTR 属性名,bool 启用禁用)
	//普通上下文取到属性面板文本；自定义上下文取到 _R.lib 的原始 Bytes。
	取属性值 = 2, //r = Bytes* fb(LPWSTR 属性名) !注：返回值需手动delete r;
	// 自定义上下文中，取属性值仍带属性名但返回 _R.lib 的真实 Bytes；
	// 置属性值的第二项也改为 Bytes。普通上下文仍按属性面板文本处理。
	置属性值 = 3 //普通: FB(LPWSTR 属性名,LPWSTR 值)；自定义: FB(LPWSTR 属性名,Bytes 值)
};
};
typedef bool(*属性面板回调)(LPCWSTR 组件名称,
	LPCWSTR 属性名, LPCWSTR 当前属性值, LPCWSTR 即将改变到属性值, BEIDEFN ide方法);

struct _UI库信息 {
	DWORD SDK主版本 = 1;
	DWORD SDK次版本 = 0;
	LPSTR 模块名 = NULL;
	LPSTR 分组名 = NULL;
	LPSTR 作者 = NULL;
	LPSTR 日期 = NULL;
	LPSTR 备注 = NULL;
	UI多夹类型* fn多夹 = &_fn多夹;
	属性面板回调 fn全局属性面板 = NULL;
	void* 保留[10]{};

	//后续继承的【UI库信息】必须紧接着成员：
	//int 组件个数;
	//UI库组件信息 interfaceUI[x];
};
#pragma pack()
