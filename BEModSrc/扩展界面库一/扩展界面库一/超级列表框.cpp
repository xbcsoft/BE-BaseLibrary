#include "stdafx.h"
#include <CommCtrl.h>

定义_枚举型(超级列表框边框, char,
	无边框 = 0, 凹入式 = 1, 凸出式 = 2, 浅凹入式 = 3, 镜框式 = 4, 单线边框式 = 5
);

定义_枚举型(超级列表框类型, char,
	大图标列表框 = 0, 小图标列表框 = 1, 普通列表框 = 2, 报表列表框 = 3
);

定义_枚举型(超级列表框对齐方式, char,
	顶部对齐 = 0, 左边对齐 = 1
);

定义_枚举型(超级列表框排序方式, char,
	不排序 = 0, 正向排序 = 1, 逆向排序 = 2
);

class EXP 超级列表框 : public 控件类
{
public:
	struct 参数 : 控件类::参数 {
		定义_子组件通用构造方法;
		超级列表框边框 边框 = 超级列表框边框::凹入式;
		颜色 文本颜色 = 颜色::黑色;
		颜色 文本背景色 = 颜色::透明;
		颜色 背景颜色 = 颜色::白色;
		字体 字体;
		超级列表框类型 类型 = 超级列表框类型::报表列表框;
		超级列表框对齐方式 图标对齐方式 = 超级列表框对齐方式::顶部对齐;
		bool 自动排列图标 = true;
		bool 标题自动换行 = true;
		bool 无表头 = false;
		bool 表头可单击 = true;
		bool 整行选择 = false;
		bool 显示表格线 = false;
		bool 表列可拖动 = false;
		bool 热点跟踪 = false;
		bool 手形指针 = false;
		bool 标注非热点 = false;
		bool 标注热点 = false;
		bool 允许编辑 = false;
		超级列表框排序方式 排序方式 = 超级列表框排序方式::不排序;
		bool 是否有检查框 = false;
		bool 平面滚动条 = false;
		bool 单一选择 = false;
		bool 始终显示选择项 = false;
		int 现行选中项 = -1;
		Bytes 报表列;
		Bytes 表项;
	} static _df;

	超级列表框边框 边框;
	颜色 文本颜色;
	颜色 文本背景色;
	颜色 背景颜色;
	超级列表框类型 类型;
	超级列表框对齐方式 图标对齐方式;
	bool 自动排列图标;
	bool 标题自动换行;
	bool 无表头;
	bool 表头可单击;
	bool 整行选择;
	bool 显示表格线;
	bool 表列可拖动;
	bool 热点跟踪;
	bool 手形指针;
	bool 标注非热点;
	bool 标注热点;
	bool 允许编辑;
	超级列表框排序方式 排序方式;
	bool 是否有检查框;
	bool 平面滚动条;
	bool 单一选择;
	bool 始终显示选择项;
	int 现行选中项;
	StrW 结束编辑文本;

	定义_字体成员;

	图片组 _图片组;
	图片组 _状态图片组;
	图片组 _表头图片组;

	bool 创建(const 参数& cs, 容器类* 父窗口 = NULL, HWND 父句柄 = NULL) {
		初始化Win32公共控件();
		控件类::_子初始(cs, 父窗口, 父句柄);

		边框 = cs.边框;
		文本颜色 = cs.文本颜色;
		文本背景色 = cs.文本背景色;
		背景颜色 = cs.背景颜色;
		类型 = cs.类型;
		图标对齐方式 = cs.图标对齐方式;
		自动排列图标 = cs.自动排列图标;
		标题自动换行 = cs.标题自动换行;
		无表头 = cs.无表头;
		表头可单击 = cs.表头可单击;
		整行选择 = cs.整行选择;
		显示表格线 = cs.显示表格线;
		表列可拖动 = cs.表列可拖动;
		热点跟踪 = cs.热点跟踪;
		手形指针 = cs.手形指针;
		标注非热点 = cs.标注非热点;
		标注热点 = cs.标注热点;
		允许编辑 = cs.允许编辑;
		排序方式 = cs.排序方式;
		是否有检查框 = cs.是否有检查框;
		平面滚动条 = cs.平面滚动条;
		单一选择 = cs.单一选择;
		始终显示选择项 = cs.始终显示选择项;
		现行选中项 = cs.现行选中项;

		DWORD style = _取基础风格();
		DWORD exStyle = _取扩展风格();

		窗口句柄 = CreateWindowExW(exStyle, WC_LISTVIEWW, L"", style,
			dpi(左边), dpi(顶边), dpi(宽度), dpi(高度), 父窗口句柄,
			(HMENU)this, g_hInst, NULL);
		if (!窗口句柄) return false;

		_同步样式();

		if (文本颜色.val != 颜色::黑色) 文本颜色_(文本颜色);
		文本背景色_(文本背景色);
		背景颜色_(背景颜色, false);

		置字体(cs.字体);

		if (现行选中项 >= 0) {
			现行选中项_(现行选中项);
		}

		控件类::_子类化();

		if (cs.报表列) 载入列项(cs.报表列);
		if (cs.表项) 载入表项(cs.表项);

		return true;
	}

	bool 销毁() {
		if (!窗口句柄) return false;
		_图片组.销毁();
		_状态图片组.销毁();
		_表头图片组.销毁();
		DestroyWindow(窗口句柄);
		控件类::销毁();
		return true;
	}

	void 边框_(超级列表框边框 v) {
		边框 = v;
		_同步样式();
	}

	void 文本颜色_(颜色 cr) {
		文本颜色 = cr;
		SendMessageW(窗口句柄, LVM_SETTEXTCOLOR, 0, (LPARAM)cr.val);
	}

	void 文本背景色_(颜色 cr) {
		文本背景色 = cr;
		SendMessageW(窗口句柄, LVM_SETTEXTBKCOLOR, 0, (LPARAM)cr.val);
	}

	void 背景颜色_(颜色 cr, bool 是否重画 = true) {
		背景颜色 = cr;
		SendMessageW(窗口句柄, LVM_SETBKCOLOR, 0, (LPARAM)cr.val);
		if (是否重画) 重画(nil, true);
	}

	void 类型_(超级列表框类型 v) {
		类型 = v;
		_同步样式();
	}

	void 图标对齐方式_(超级列表框对齐方式 v) {
		图标对齐方式 = v;
		_同步样式();
	}

	void 自动排列图标_(bool is) {
		自动排列图标 = is;
		_同步样式();
	}

	void 标题自动换行_(bool is) {
		标题自动换行 = is;
		_同步样式();
	}

	void 无表头_(bool is) {
		无表头 = is;
		_同步样式();
	}

	void 表头可单击_(bool is) {
		表头可单击 = is;
		_同步样式();
	}

	void 整行选择_(bool is) {
		整行选择 = is;
		_同步样式();
	}

	void 显示表格线_(bool is) {
		显示表格线 = is;
		_同步样式();
	}

	void 表列可拖动_(bool is) {
		表列可拖动 = is;
		_同步样式();
	}

	void 热点跟踪_(bool is) {
		热点跟踪 = is;
		_同步样式();
	}

	void 手形指针_(bool is) {
		手形指针 = is;
		_同步样式();
	}

	void 标注非热点_(bool is) {
		标注非热点 = is;
		_同步样式();
	}

	void 标注热点_(bool is) {
		标注热点 = is;
		_同步样式();
	}

	void 允许编辑_(bool is) {
		允许编辑 = is;
		_同步样式();
	}

	void 排序方式_(超级列表框排序方式 v) {
		排序方式 = v;
		_同步样式();
	}

	void 是否有检查框_(bool is) {
		是否有检查框 = is;
		_同步样式();
	}

	void 平面滚动条_(bool is) {
		平面滚动条 = is;
		_同步样式();
	}

	void 单一选择_(bool is) {
		单一选择 = is;
		_同步样式();
	}

	void 始终显示选择项_(bool is) {
		始终显示选择项 = is;
		_同步样式();
	}

	void 现行选中项_(int index) {
		if (index == -1) {
			int count = 取表项数();
			for (int i = 0; i < count; ++i) {
				取消表项选择(i);
			}
			现行选中项 = -1;
		} else {
			if (单一选择) {
				int count = 取表项数();
				for (int i = 0; i < count; ++i) {
					if (i != index) 取消表项选择(i);
				}
			}
			选择表项(index);
			现行选中项 = index;
		}
	}

	void 置图片组(图片组& imageList) {
		_图片组.从图片组复制(imageList);
		SendMessageW(窗口句柄, LVM_SETIMAGELIST, LVSIL_NORMAL, (LPARAM)_图片组.h);
		SendMessageW(窗口句柄, LVM_SETIMAGELIST, LVSIL_SMALL, (LPARAM)_图片组.h);
	}

	void 置状态图片组(图片组& imageList) {
		_状态图片组.从图片组复制(imageList);
		SendMessageW(窗口句柄, LVM_SETIMAGELIST, LVSIL_STATE, (LPARAM)_状态图片组.h);
	}

	void 置表头图片组(图片组& imageList) {
		_表头图片组.从图片组复制(imageList);
		HWND hwndHeader = (HWND)SendMessageW(窗口句柄, LVM_GETHEADER, 0, 0);
		if (hwndHeader) {
			SendMessageW(hwndHeader, HDM_SETIMAGELIST, 0, (LPARAM)_表头图片组.h);
		}
	}

	StrW 取标题(int 表项索引, int 列索引 = 0) {
		LVITEMW item = { 0 };
		item.mask = LVIF_TEXT;
		item.iItem = 表项索引;
		item.iSubItem = 列索引;
		wchar_t buf[2048] = { 0 };
		item.pszText = buf;
		item.cchTextMax = 2048;
		if (SendMessageW(窗口句柄, LVM_GETITEMW, 0, (LPARAM)&item)) {
			return StrW(buf);
		}
		return L"";
	}

	void 置标题(int 表项索引, int 列索引 = 0, StrW 标题 = L"") {
		LVITEMW item = { 0 };
		item.mask = LVIF_TEXT;
		item.iItem = 表项索引;
		item.iSubItem = 列索引;
		item.pszText = (wchar_t*)(const wchar_t*)标题;
		SendMessageW(窗口句柄, LVM_SETITEMW, 0, (LPARAM)&item);
	}

	int 取图片(int 表项索引, int 列索引 = 0) {
		LVITEMW item = { 0 };
		item.mask = LVIF_IMAGE;
		item.iItem = 表项索引;
		item.iSubItem = 列索引;
		if (SendMessageW(窗口句柄, LVM_GETITEMW, 0, (LPARAM)&item)) {
			return item.iImage;
		}
		return -1;
	}

	void 置图片(int 表项索引, int 列索引 = 0, int 图片索引 = -1) {
		LVITEMW item = { 0 };
		item.mask = LVIF_IMAGE;
		item.iItem = 表项索引;
		item.iSubItem = 列索引;
		item.iImage = 图片索引;
		SendMessageW(窗口句柄, LVM_SETITEMW, 0, (LPARAM)&item);
	}

	int 取当前状态图片(int 表项索引) {
		LVITEMW item = { 0 };
		item.mask = LVIF_STATE;
		item.stateMask = LVIS_STATEIMAGEMASK;
		item.iItem = 表项索引;
		item.iSubItem = 0;
		if (SendMessageW(窗口句柄, LVM_GETITEMW, 0, (LPARAM)&item)) {
			int state = (item.state & LVIS_STATEIMAGEMASK) >> 12;
			if (state == 0) return -1;
			if (是否有检查框 && !_状态图片组.h) {
				return (state == 2) ? 1 : 0;
			}
			return state - 1;
		}
		return -1;
	}

	void 置状态图片(int 表项索引, int 状态图片索引 = -1) {
		LVITEMW item = { 0 };
		item.mask = LVIF_STATE;
		item.stateMask = LVIS_STATEIMAGEMASK;
		item.iItem = 表项索引;
		item.iSubItem = 0;
		int state = 0;
		if (状态图片索引 == -1) {
			state = 0;
		} else {
			if (是否有检查框 && !_状态图片组.h) {
				state = (状态图片索引 == 1) ? 2 : 1;
			} else {
				state = 状态图片索引 + 1;
			}
		}
		item.state = INDEXTOSTATEIMAGEMASK(state);
		SendMessageW(窗口句柄, LVM_SETITEMW, 0, (LPARAM)&item);
	}

	int 取缩进数目(int 表项索引) {
		LVITEMW item = { 0 };
		item.mask = LVIF_INDENT;
		item.iItem = 表项索引;
		item.iSubItem = 0;
		if (SendMessageW(窗口句柄, LVM_GETITEMW, 0, (LPARAM)&item)) {
			return item.iIndent;
		}
		return 0;
	}

	void 置缩进数目(int 表项索引, int 缩进数目 = 0) {
		LVITEMW item = { 0 };
		item.mask = LVIF_INDENT;
		item.iItem = 表项索引;
		item.iSubItem = 0;
		item.iIndent = 缩进数目;
		SendMessageW(窗口句柄, LVM_SETITEMW, 0, (LPARAM)&item);
	}

	int 取表项数值(int 表项索引) {
		LVITEMW item = { 0 };
		item.mask = LVIF_PARAM;
		item.iItem = 表项索引;
		item.iSubItem = 0;
		if (SendMessageW(窗口句柄, LVM_GETITEMW, 0, (LPARAM)&item)) {
			return (int)item.lParam;
		}
		return 0;
	}

	void 置表项数值(int 表项索引, int 表项数值 = 0) {
		LVITEMW item = { 0 };
		item.mask = LVIF_PARAM;
		item.iItem = 表项索引;
		item.iSubItem = 0;
		item.lParam = 表项数值;
		SendMessageW(窗口句柄, LVM_SETITEMW, 0, (LPARAM)&item);
	}

	Arraybe<int> 取被选择表项() {
		Arraybe<int> res;
		int item = -1;
		while ((item = (int)SendMessageW(窗口句柄, LVM_GETNEXTITEM, item, LVNI_SELECTED)) != -1) {
			res.push(item);
		}
		return res;
	}

	int 取已选择表项数() {
		return (int)SendMessageW(窗口句柄, LVM_GETSELECTEDCOUNT, 0, 0);
	}

	bool 是否被选择(int 表项索引) {
		return (SendMessageW(窗口句柄, LVM_GETITEMSTATE, 表项索引, LVIS_SELECTED) & LVIS_SELECTED) != 0;
	}

	void 选择表项(int 表项索引) {
		LVITEMW item = { 0 };
		item.mask = LVIF_STATE;
		item.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
		item.state = LVIS_SELECTED | LVIS_FOCUSED;
		SendMessageW(窗口句柄, LVM_SETITEMSTATE, 表项索引, (LPARAM)&item);
	}

	void 取消表项选择(int 表项索引) {
		LVITEMW item = { 0 };
		item.mask = LVIF_STATE;
		item.stateMask = LVIS_SELECTED;
		item.state = 0;
		SendMessageW(窗口句柄, LVM_SETITEMSTATE, 表项索引, (LPARAM)&item);
	}

	int 取表项数() {
		return (int)SendMessageW(窗口句柄, LVM_GETITEMCOUNT, 0, 0);
	}

	int 插入表项(int 插入位置 = -1, StrW 标题 = L"", int 图片索引 = -1, int 状态图片索引 = -1, int 缩进数目 = 0, int 表项数值 = 0) {
		LVITEMW item = { 0 };
		item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM | LVIF_INDENT;

		int state = 0;
		if (状态图片索引 != -1) {
			if (是否有检查框 && !_状态图片组.h) {
				state = (状态图片索引 == 1) ? 2 : 1;
			} else {
				state = 状态图片索引 + 1;
			}
		} else if (是否有检查框 && !_状态图片组.h) {
			state = 1;
		}

		if (state > 0) {
			item.mask |= LVIF_STATE;
			item.stateMask = LVIS_STATEIMAGEMASK;
			item.state = INDEXTOSTATEIMAGEMASK(state);
		}

		int count = 取表项数();
		if (插入位置 == -1 || 插入位置 > count) {
			item.iItem = count;
		} else {
			item.iItem = 插入位置;
		}

		if (类型 == 超级列表框类型::大图标列表框 || 类型 == 超级列表框类型::小图标列表框) {
			item.iItem = count;
		}

		item.iSubItem = 0;
		item.pszText = (wchar_t*)(const wchar_t*)标题;
		item.iImage = 图片索引;
		item.iIndent = 缩进数目;
		item.lParam = 表项数值;

		return (int)SendMessageW(窗口句柄, LVM_INSERTITEMW, 0, (LPARAM)&item);
	}

	void 删除表项(int 表项索引) {
		SendMessageW(窗口句柄, LVM_DELETEITEM, 表项索引, 0);
	}

	void 全部删除() {
		SendMessageW(窗口句柄, LVM_DELETEALLITEMS, 0, 0);
	}

	int 查找表项(StrW 标题, int 起始查找位置 = 0, bool 是否精确匹配 = true, int 列索引 = 0) {
		int count = 取表项数();
		for (int i = 起始查找位置; i < count; ++i) {
			if (列索引 == -1) {
				int cols = 取列数();
				for (int col = 0; col < cols; ++col) {
					StrW txt = 取标题(i, col);
					if (是否精确匹配) {
						if (txt == 标题) return i;
					} else {
						const wchar_t* p1 = txt;
						const wchar_t* p2 = 标题;
						if (p1 && p2 && wcsncmp(p1, p2, wcslen(p2)) == 0) return i;
					}
				}
			} else {
				StrW txt = 取标题(i, 列索引);
				if (是否精确匹配) {
					if (txt == 标题) return i;
				} else {
					const wchar_t* p1 = txt;
					const wchar_t* p2 = 标题;
					if (p1 && p2 && wcsncmp(p1, p2, wcslen(p2)) == 0) return i;
				}
			}
		}
		return -1;
	}

	int 查找表项数值(int 起始查找位置 = 0, int 表项数值 = 0) {
		int count = 取表项数();
		for (int i = 起始查找位置; i < count; ++i) {
			if (取表项数值(i) == 表项数值) {
				return i;
			}
		}
		return -1;
	}

	void 保证显示(int 表项索引) {
		SendMessageW(窗口句柄, LVM_ENSUREVISIBLE, 表项索引, FALSE);
	}

	void 排列图标() {
		SendMessageW(窗口句柄, LVM_ARRANGE, LVA_DEFAULT, 0);
	}

	StrW 取列标题(int 列索引) {
		LVCOLUMNW col = { 0 };
		col.mask = LVCF_TEXT;
		wchar_t buf[256] = { 0 };
		col.pszText = buf;
		col.cchTextMax = 256;
		if (SendMessageW(窗口句柄, LVM_GETCOLUMNW, 列索引, (LPARAM)&col)) {
			return StrW(buf);
		}
		return L"";
	}

	int 取列对齐方式(int 列索引) {
		LVCOLUMNW col = { 0 };
		col.mask = LVCF_FMT;
		if (SendMessageW(窗口句柄, LVM_GETCOLUMNW, 列索引, (LPARAM)&col)) {
			int fmt = col.fmt & LVCFMT_JUSTIFYMASK;
			if (fmt == LVCFMT_RIGHT) return 2;
			if (fmt == LVCFMT_CENTER) return 1;
			return 0;
		}
		return 0;
	}

	int 取列宽(int 列索引) {
		return rdpi((int)SendMessageW(窗口句柄, LVM_GETCOLUMNWIDTH, 列索引, 0));
	}

	int 取列图片(int 列索引) {
		LVCOLUMNW col = { 0 };
		col.mask = LVCF_IMAGE;
		if (SendMessageW(窗口句柄, LVM_GETCOLUMNW, 列索引, (LPARAM)&col)) {
			if (col.fmt & LVCFMT_IMAGE) {
				return col.iImage;
			}
		}
		return -1;
	}

	bool 图片是否居右(int 列索引) {
		LVCOLUMNW col = { 0 };
		col.mask = LVCF_FMT;
		if (SendMessageW(窗口句柄, LVM_GETCOLUMNW, 列索引, (LPARAM)&col)) {
			return (col.fmt & LVCFMT_BITMAP_ON_RIGHT) != 0;
		}
		return false;
	}

	void 置列标题(int 列索引, StrW 标题 = L"") {
		LVCOLUMNW col = { 0 };
		col.mask = LVCF_TEXT;
		col.pszText = (wchar_t*)(const wchar_t*)标题;
		SendMessageW(窗口句柄, LVM_SETCOLUMNW, 列索引, (LPARAM)&col);
	}

	void 置列宽(int 列索引, int 列宽 = -1) {
		int w = 列宽;
		if (列宽 == -1) w = LVSCW_AUTOSIZE;
		else if (列宽 == -2) w = LVSCW_AUTOSIZE_USEHEADER;
		else w = dpi(列宽);
		SendMessageW(窗口句柄, LVM_SETCOLUMNWIDTH, 列索引, w);
	}

	void 置列图片(int 列索引, int 图片索引 = -1) {
		LVCOLUMNW col = { 0 };
		col.mask = LVCF_FMT | LVCF_IMAGE;
		SendMessageW(窗口句柄, LVM_GETCOLUMNW, 列索引, (LPARAM)&col);
		if (图片索引 == -1) {
			col.fmt &= ~LVCFMT_IMAGE;
			col.iImage = -1;
		} else {
			col.fmt |= LVCFMT_IMAGE;
			col.iImage = 图片索引;
		}
		SendMessageW(窗口句柄, LVM_SETCOLUMNW, 列索引, (LPARAM)&col);
	}

	int 取列数() {
		HWND hwndHeader = (HWND)SendMessageW(窗口句柄, LVM_GETHEADER, 0, 0);
		if (hwndHeader) {
			return (int)SendMessageW(hwndHeader, HDM_GETITEMCOUNT, 0, 0);
		}
		return 0;
	}

	int 插入列(int 插入位置 = -1, StrW 标题 = L"", int 列宽 = 50, int 对齐方式 = 0, int 图片索引 = -1, bool 图片是否居右 = false) {
		LVCOLUMNW col = { 0 };
		col.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
		col.pszText = (wchar_t*)(const wchar_t*)标题;
		col.cx = dpi(列宽);

		int fmt = 0;
		if (对齐方式 == 1) fmt |= LVCFMT_CENTER;
		else if (对齐方式 == 2) fmt |= LVCFMT_RIGHT;
		else fmt |= LVCFMT_LEFT;

		if (图片索引 != -1) {
			col.mask |= LVCF_IMAGE;
			fmt |= LVCFMT_IMAGE;
			col.iImage = 图片索引;
			if (图片是否居右) {
				fmt |= LVCFMT_BITMAP_ON_RIGHT;
			}
		}
		col.fmt = fmt;

		int count = 取列数();
		int pos = 插入位置;
		if (插入位置 == -1 || 插入位置 > count) {
			pos = count;
		}

		return (int)SendMessageW(窗口句柄, LVM_INSERTCOLUMNW, pos, (LPARAM)&col);
	}

	void 删除列(int 列索引) {
		SendMessageW(窗口句柄, LVM_DELETECOLUMN, 列索引, 0);
	}

	Bytes 导出列项() {
		int cols = 取列数();
		Arraybe<StrW> titles;
		Arraybe<int> widths;
		Arraybe<int> aligns;
		Arraybe<int> images;
		Arraybe<bool> isRights;
		for (int i = 0; i < cols; ++i) {
			titles.push(取列标题(i));
			widths.push(取列宽(i));
			aligns.push(取列对齐方式(i));
			images.push(取列图片(i));
			isRights.push(图片是否居右(i));
		}
		return FB(titles, widths, aligns, images, isRights);
	}

	void 载入列项(const Bytes& dat) {
		if (!dat.buf) return;
		Arraybe<StrW> titles;
		Arraybe<int> widths;
		Arraybe<int> aligns;
		Arraybe<int> images;
		Arraybe<bool> isRights;
		deFB(dat, titles, widths, aligns, images, isRights);
		
		int old_cols = 取列数();
		for (int i = old_cols - 1; i >= 0; i--) {
			删除列(i);
		}

		for (int i = 0; i < titles.count; ++i) {
			插入列(i, titles[i], widths[i], aligns[i], images[i], isRights[i]);
		}
	}

	Bytes 导出表项() {
		int rows = 取表项数();
		int cols = 取列数();
		if (cols == 0) cols = 1;

		Arraybe<Arraybe<StrW>> allTexts;
		Arraybe<Arraybe<int>> allImages;
		Arraybe<int> states;
		Arraybe<int> indents;
		Arraybe<int> values;

		for (int i = 0; i < rows; ++i) {
			Arraybe<StrW> rowTexts;
			Arraybe<int> rowImages;
			for (int j = 0; j < cols; ++j) {
				rowTexts.push(取标题(i, j));
				rowImages.push(取图片(i, j));
			}
			allTexts.push(rowTexts);
			allImages.push(rowImages);
			
			states.push(取当前状态图片(i));
			indents.push(取缩进数目(i));
			values.push(取表项数值(i));
		}
		return FB(allTexts, allImages, states, indents, values);
	}

	void 载入表项(const Bytes& dat) {
		if (!dat.buf) return;
		Arraybe<Arraybe<StrW>> allTexts;
		Arraybe<Arraybe<int>> allImages;
		Arraybe<int> states;
		Arraybe<int> indents;
		Arraybe<int> values;
		deFB(dat, allTexts, allImages, states, indents, values);
		
		全部删除();
		
		for (int i = 0; i < allTexts.count; ++i) {
			int img = allImages[i].count > 0 ? allImages[i][0] : -1;
			int state = states[i];
			int indent = indents[i];
			int val = values[i];
			StrW title = allTexts[i].count > 0 ? allTexts[i][0] : StrW();
			
			插入表项(i, title, img, state, indent, val);
			
			for (int j = 1; j < allTexts[i].count; ++j) {
				置标题(i, j, allTexts[i][j]);
				if (allImages[i][j] != -1) {
					置图片(i, j, allImages[i][j]);
				}
			}
		}
	}

	void 进入编辑(int 表项索引) {
		if (允许编辑) {
			SendMessageW(窗口句柄, LVM_EDITLABELW, 表项索引, 0);
		}
	}

	virtual void 事件_当前表项被改变() {}
	virtual void 事件_表项被激活() {}
	virtual void 事件_表头被单击(int 被单击列索引) {}
	virtual void 事件_表项跟踪(int 表项索引) {}
	virtual void 事件_左键单击表项() {}
	virtual void 事件_右键单击表项() {}
	virtual bool 事件_开始编辑(int 表项索引) { return true; }
	virtual bool 事件_结束编辑(int 表项索引) { return true; }
	virtual void 事件_检查框状态被改变(int 表项索引, int 检查框状态) {}

	virtual LRESULT 挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	void _同步样式() {
		if (!窗口句柄) return;

		LONG_PTR style = GetWindowLongPtrW(窗口句柄, GWL_STYLE);
		style &= ~(LVS_TYPEMASK | LVS_ALIGNMASK | LVS_AUTOARRANGE | LVS_NOLABELWRAP | 
				   LVS_NOCOLUMNHEADER | LVS_NOSORTHEADER | LVS_EDITLABELS | 
				   LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_SORTASCENDING | LVS_SORTDESCENDING | WS_BORDER);
		style |= _取基础风格();
		SetWindowLongPtrW(窗口句柄, GWL_STYLE, style);

		DWORD lvsExStyle = 0;
		if (整行选择) lvsExStyle |= LVS_EX_FULLROWSELECT;
		if (显示表格线) lvsExStyle |= LVS_EX_GRIDLINES;
		if (表列可拖动) lvsExStyle |= LVS_EX_HEADERDRAGDROP;
		if (是否有检查框) lvsExStyle |= LVS_EX_CHECKBOXES;
		if (平面滚动条) lvsExStyle |= LVS_EX_FLATSB;
		if (热点跟踪) lvsExStyle |= LVS_EX_TRACKSELECT;
		if (标注热点) lvsExStyle |= LVS_EX_UNDERLINEHOT;
		if (标注非热点) lvsExStyle |= LVS_EX_UNDERLINECOLD;
		if (手形指针) lvsExStyle |= LVS_EX_TRACKSELECT;

		SendMessageW(窗口句柄, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, (LPARAM)lvsExStyle);

		LONG_PTR exstyle = GetWindowLongPtrW(窗口句柄, GWL_EXSTYLE);
		exstyle &= ~(WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE | WS_EX_STATICEDGE | WS_EX_DLGMODALFRAME);
		exstyle |= _取扩展风格();
		SetWindowLongPtrW(窗口句柄, GWL_EXSTYLE, exstyle);

		SetWindowPos(窗口句柄, NULL, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
	}

	DWORD _取基础风格() {
		DWORD style = _窗口风格;
		if (类型 == 超级列表框类型::大图标列表框) style |= LVS_ICON;
		else if (类型 == 超级列表框类型::小图标列表框) style |= LVS_SMALLICON;
		else if (类型 == 超级列表框类型::普通列表框) style |= LVS_LIST;
		else if (类型 == 超级列表框类型::报表列表框) style |= LVS_REPORT;

		if (图标对齐方式 == 超级列表框对齐方式::左边对齐) style |= LVS_ALIGNLEFT;
		else style |= LVS_ALIGNTOP;

		if (自动排列图标) style |= LVS_AUTOARRANGE;
		if (!标题自动换行) style |= LVS_NOLABELWRAP;
		if (无表头) style |= LVS_NOCOLUMNHEADER;
		if (!表头可单击) style |= LVS_NOSORTHEADER;
		if (允许编辑) style |= LVS_EDITLABELS;
		if (单一选择) style |= LVS_SINGLESEL;
		if (始终显示选择项) style |= LVS_SHOWSELALWAYS;

		if (排序方式 == 超级列表框排序方式::正向排序) style |= LVS_SORTASCENDING;
		else if (排序方式 == 超级列表框排序方式::逆向排序) style |= LVS_SORTDESCENDING;

		if (边框 == 超级列表框边框::单线边框式) style |= WS_BORDER;

		return style;
	}

	DWORD _取扩展风格() {
		switch (边框) {
		case 超级列表框边框::凹入式:
			return WS_EX_CLIENTEDGE;
		case 超级列表框边框::凸出式:
			return WS_EX_WINDOWEDGE | WS_EX_DLGMODALFRAME;
		case 超级列表框边框::浅凹入式:
			return WS_EX_STATICEDGE;
		case 超级列表框边框::镜框式:
			return WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE;
		}
		return 0;
	}

	int _取现行选中项() {
		return (int)SendMessageW(窗口句柄, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
	}
};

LRESULT 超级列表框::挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_SETCURSOR: {
		if (手形指针 && 窗口句柄) {
			LVHITTESTINFO ht = { 0 };
			GetCursorPos(&ht.pt);
			ScreenToClient(窗口句柄, &ht.pt);
			ListView_HitTest(窗口句柄, &ht);
			if (ht.iItem != -1) {
				SetCursor(LoadCursor(NULL, IDC_HAND));
				return TRUE;
			}
		}
		break;
	}

	case WM_NOTIFY: {
		NMHDR* hdr = (NMHDR*)lParam;
		HWND hwndHeader = (HWND)SendMessageW(窗口句柄, LVM_GETHEADER, 0, 0);
		if (hwndHeader && hdr->hwndFrom == hwndHeader) {
			if (hdr->code == HDN_ITEMCLICKW || hdr->code == HDN_ITEMCLICKA) {
				NMHEADERW* pnmh = (NMHEADERW*)lParam;
				事件_表头被单击(pnmh->iItem);
			}
		}
		break;
	}
	case BE_REFLECT + WM_NOTIFY: {
		NMHDR* hdr = (NMHDR*)lParam;
		if (hdr->hwndFrom == 窗口句柄) {
			switch (hdr->code) {
			case LVN_ITEMCHANGED: {
				NMLISTVIEW* pnmv = (NMLISTVIEW*)lParam;
				if (pnmv->uChanged & LVIF_STATE) {
					if ((pnmv->uNewState ^ pnmv->uOldState) & LVIS_SELECTED) {
						现行选中项 = _取现行选中项();
						事件_当前表项被改变();
					}
					if (是否有检查框 && !_状态图片组.h) {
						if ((pnmv->uNewState & LVIS_STATEIMAGEMASK) != (pnmv->uOldState & LVIS_STATEIMAGEMASK)) {
							int newState = (pnmv->uNewState & LVIS_STATEIMAGEMASK) >> 12;
							int state = (newState == 2) ? 1 : 0;
							事件_检查框状态被改变(pnmv->iItem, state);
						}
					}
				}
				break;
			}
			case NM_CLICK: {
				现行选中项 = _取现行选中项();
				事件_左键单击表项();

				if (是否有检查框 && _状态图片组.h) {
					LVHITTESTINFO ht = { 0 };
					GetCursorPos(&ht.pt);
					ScreenToClient(窗口句柄, &ht.pt);
					ListView_HitTest(窗口句柄, &ht);
					if (ht.flags & LVHT_ONITEMSTATEICON) {
						int num = _状态图片组.取图片数();
						if (num > 0) {
							int cur = 取当前状态图片(ht.iItem);
							int next = (cur + 1) % num;
							置状态图片(ht.iItem, next);
							事件_检查框状态被改变(ht.iItem, next);
						}
					}
				}
				break;
			}
			case NM_RCLICK: {
				现行选中项 = _取现行选中项();
				事件_右键单击表项();
				break;
			}
			case NM_DBLCLK: {
				现行选中项 = _取现行选中项();
				事件_表项被激活();
				break;
			}
			case LVN_KEYDOWN: {
				NMLVKEYDOWN* pnmkd = (NMLVKEYDOWN*)lParam;
				if (pnmkd->wVKey == VK_RETURN) {
					现行选中项 = _取现行选中项();
					事件_表项被激活();
				} else if (pnmkd->wVKey == VK_SPACE && 是否有检查框 && _状态图片组.h) {
					int curSel = _取现行选中项();
					if (curSel >= 0) {
						int num = _状态图片组.取图片数();
						if (num > 0) {
							int cur = 取当前状态图片(curSel);
							int next = (cur + 1) % num;
							置状态图片(curSel, next);
							事件_检查框状态被改变(curSel, next);
						}
					}
				}
				break;
			}
			case LVN_HOTTRACK: {
				NMLISTVIEW* pnmv = (NMLISTVIEW*)lParam;
				事件_表项跟踪(pnmv->iItem);
				break;
			}
			case LVN_BEGINLABELEDITW:
			case LVN_BEGINLABELEDITA: {
				NMLVDISPINFOW* pdi = (NMLVDISPINFOW*)lParam;
				bool allow = 事件_开始编辑(pdi->item.iItem);
				return allow ? FALSE : TRUE;
			}
			case LVN_ENDLABELEDITW: {
				NMLVDISPINFOW* pdi = (NMLVDISPINFOW*)lParam;
				if (pdi->item.pszText != NULL) {
					结束编辑文本 = pdi->item.pszText;
					bool accept = 事件_结束编辑(pdi->item.iItem);
					return accept ? TRUE : FALSE;
				}
				break;
			}
			case LVN_ENDLABELEDITA: {
				NMLVDISPINFOA* pdi = (NMLVDISPINFOA*)lParam;
				if (pdi->item.pszText != NULL) {
					结束编辑文本 = AtoW(pdi->item.pszText);
					bool accept = 事件_结束编辑(pdi->item.iItem);
					return accept ? TRUE : FALSE;
				}
				break;
			}
			}
		}
		break;
	}
	}
	return 窗口基类::挂接消息(hwnd, msg, wParam, lParam);
}