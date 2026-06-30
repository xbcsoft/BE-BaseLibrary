#pragma once
#include "stdafx.h"
#include <CommCtrl.h>

定义_枚举型(树型框边框, char,
	无边框 = 0, 凹入式 = 1, 凸出式 = 2, 浅凹入式 = 3, 镜框式 = 4, 单线边框式 = 5
);

class EXP 树型框 : public 控件类
{
public:
	struct 参数 : 控件类::参数 {
		定义_子组件通用构造方法;
		树型框边框 边框 = 树型框边框::凹入式;
		颜色 文本颜色 = 颜色::黑色;
		颜色 背景颜色 = 颜色::白色;
		bool 显示加减框 = true;
		bool 显示连线 = true;
		bool 显示根部线 = true;
		bool 允许编辑 = false;
		bool 始终显示选择项 = false;
		int 现行选中项 = -1;
		int 行高 = -1;
		字体 字体;
		Bytes 图片组;
		Bytes 项目;
		bool 是否有检查框 = false;
		Bytes 检查框图片组;
		int 检查框状态数 = 2;
	} static _df;

	树型框边框 边框;
	颜色 文本颜色;
	颜色 背景颜色;
	bool 显示加减框;
	bool 显示连线;
	bool 显示根部线;
	bool 允许编辑;
	bool 始终显示选择项;
	int 现行选中项;
	int 行高;
	bool 是否有检查框;
	int 检查框状态数;
	StrW 结束编辑文本;

	定义_字体成员;

	图片组 _图片组;
	图片组 _检查框图片组;

	bool 创建(const 参数& cs, 容器类* 父窗口 = NULL, HWND 父句柄 = NULL) {
		初始化Win32公共控件();
		控件类::_子初始(cs, 父窗口, 父句柄);

		边框 = cs.边框;
		文本颜色 = cs.文本颜色;
		背景颜色 = cs.背景颜色;
		显示加减框 = cs.显示加减框;
		显示连线 = cs.显示连线;
		显示根部线 = cs.显示根部线;
		允许编辑 = cs.允许编辑;
		始终显示选择项 = cs.始终显示选择项;
		现行选中项 = cs.现行选中项;
		行高 = cs.行高;
		是否有检查框 = cs.是否有检查框;
		检查框状态数 = cs.检查框状态数;

		DWORD style = _取基础风格();
		DWORD exStyle = _取扩展风格();

		窗口句柄 = CreateWindowExW(exStyle, WC_TREEVIEWW, L"", style,
			dpi(左边), dpi(顶边), dpi(宽度), dpi(高度), 父窗口句柄,
			(HMENU)this, g_hInst, NULL);
		if (!窗口句柄) return false;

		_同步样式();

		if (文本颜色.val != 颜色::黑色) 文本颜色_(文本颜色);
		背景颜色_(背景颜色, false);
		if (行高 != -1) 行高_(行高);

		置字体(cs.字体);

		控件类::_子类化();

		if (cs.项目.size) 载入项目(cs.项目);

		if (现行选中项 >= 0) {
			现行选中项_(现行选中项);
		}

		return true;
	}

	bool 销毁() {
		if (!窗口句柄) return false;
		_图片组.销毁();
		_检查框图片组.销毁();
		DestroyWindow(窗口句柄);
		控件类::销毁();
		return true;
	}

	void 边框_(树型框边框 v) {
		边框 = v;
		_同步样式();
	}

	void 文本颜色_(颜色 cr) {
		文本颜色 = cr;
		SendMessageW(窗口句柄, TVM_SETTEXTCOLOR, 0, (LPARAM)cr.val);
	}

	void 背景颜色_(颜色 cr, bool 是否重画 = true) {
		背景颜色 = cr;
		SendMessageW(窗口句柄, TVM_SETBKCOLOR, 0, (LPARAM)cr.val);
		if (是否重画) 重画(nil, true);
	}

	void 显示加减框_(bool is) {
		显示加减框 = is;
		_同步样式();
	}

	void 显示连线_(bool is) {
		显示连线 = is;
		_同步样式();
	}

	void 显示根部线_(bool is) {
		显示根部线 = is;
		_同步样式();
	}

	void 允许编辑_(bool is) {
		允许编辑 = is;
		_同步样式();
	}

	void 始终显示选择项_(bool is) {
		始终显示选择项 = is;
		_同步样式();
	}

	void 现行选中项_(int index) {
		if (index == -1) {
			SendMessageW(窗口句柄, TVM_SELECTITEM, TVGN_CARET, (LPARAM)NULL);
			现行选中项 = -1;
		} else {
			HTREEITEM hItem = _取项目句柄(index);
			if (hItem) {
				SendMessageW(窗口句柄, TVM_SELECTITEM, TVGN_CARET, (LPARAM)hItem);
				现行选中项 = index;
			}
		}
	}

	void 行高_(int h) {
		行高 = h;
		if (h != -1) {
			SendMessageW(窗口句柄, TVM_SETITEMHEIGHT, h, 0);
		}
	}

	void 是否有检查框_(bool is) {
		是否有检查框 = is;
		_同步样式();
	}

	void 检查框状态数_(int num) {
		检查框状态数 = num;
	}

	void 置图片组(图片组& imageList) {
		_图片组.从图片组复制(imageList);
		SendMessageW(窗口句柄, TVM_SETIMAGELIST, TVSIL_NORMAL, (LPARAM)_图片组.h);
	}

	void 置检查框图片组(图片组& imageList) {
		_检查框图片组.从图片组复制(imageList);
		SendMessageW(窗口句柄, TVM_SETIMAGELIST, TVSIL_STATE, (LPARAM)_检查框图片组.h);
	}

	int 取项目数() {
		return (int)SendMessageW(窗口句柄, TVM_GETCOUNT, 0, 0);
	}

	int 取缩进层次(int 项目索引) {
		HTREEITEM hItem = _取项目句柄(项目索引);
		if (!hItem) return 0;
		int level = 1;
		HTREEITEM hParent = (HTREEITEM)SendMessageW(窗口句柄, TVM_GETNEXTITEM, TVGN_PARENT, (LPARAM)hItem);
		while (hParent) {
			level++;
			hParent = (HTREEITEM)SendMessageW(窗口句柄, TVM_GETNEXTITEM, TVGN_PARENT, (LPARAM)hParent);
		}
		return level;
	}

	StrW 取项目文本(int 项目索引) {
		HTREEITEM hItem = _取项目句柄(项目索引);
		if (!hItem) return L"";
		TVITEMW tvi = { 0 };
		tvi.mask = TVIF_TEXT | TVIF_HANDLE;
		tvi.hItem = hItem;
		wchar_t buf[2048] = { 0 };
		tvi.pszText = buf;
		tvi.cchTextMax = 2048;
		if (SendMessageW(窗口句柄, TVM_GETITEMW, 0, (LPARAM)&tvi)) {
			return StrW(buf);
		}
		return L"";
	}

	int 取图片(int 项目索引) {
		HTREEITEM hItem = _取项目句柄(项目索引);
		if (!hItem) return -1;
		TVITEMW tvi = { 0 };
		tvi.mask = TVIF_IMAGE | TVIF_HANDLE;
		tvi.hItem = hItem;
		if (SendMessageW(窗口句柄, TVM_GETITEMW, 0, (LPARAM)&tvi)) {
			return tvi.iImage;
		}
		return -1;
	}

	int 取选中图片(int 项目索引) {
		HTREEITEM hItem = _取项目句柄(项目索引);
		if (!hItem) return -1;
		TVITEMW tvi = { 0 };
		tvi.mask = TVIF_SELECTEDIMAGE | TVIF_HANDLE;
		tvi.hItem = hItem;
		if (SendMessageW(窗口句柄, TVM_GETITEMW, 0, (LPARAM)&tvi)) {
			return tvi.iSelectedImage;
		}
		return -1;
	}

	int 取项目数值(int 项目索引) {
		HTREEITEM hItem = _取项目句柄(项目索引);
		if (!hItem) return 0;
		TVITEMW tvi = { 0 };
		tvi.mask = TVIF_PARAM | TVIF_HANDLE;
		tvi.hItem = hItem;
		if (SendMessageW(窗口句柄, TVM_GETITEMW, 0, (LPARAM)&tvi)) {
			return (int)tvi.lParam;
		}
		return 0;
	}

	bool 置项目文本(int 项目索引, StrW 欲置入的项目文本) {
		HTREEITEM hItem = _取项目句柄(项目索引);
		if (!hItem) return false;
		TVITEMW tvi = { 0 };
		tvi.mask = TVIF_TEXT | TVIF_HANDLE;
		tvi.hItem = hItem;
		tvi.pszText = (wchar_t*)(const wchar_t*)欲置入的项目文本;
		return SendMessageW(窗口句柄, TVM_SETITEMW, 0, (LPARAM)&tvi) != 0;
	}

	bool 置图片(int 项目索引, int 图片索引) {
		HTREEITEM hItem = _取项目句柄(项目索引);
		if (!hItem) return false;
		TVITEMW tvi = { 0 };
		tvi.mask = TVIF_IMAGE | TVIF_HANDLE;
		tvi.hItem = hItem;
		tvi.iImage = 图片索引;
		return SendMessageW(窗口句柄, TVM_SETITEMW, 0, (LPARAM)&tvi) != 0;
	}

	bool 置选中图片(int 项目索引, int 图片索引) {
		HTREEITEM hItem = _取项目句柄(项目索引);
		if (!hItem) return false;
		TVITEMW tvi = { 0 };
		tvi.mask = TVIF_SELECTEDIMAGE | TVIF_HANDLE;
		tvi.hItem = hItem;
		tvi.iSelectedImage = 图片索引;
		return SendMessageW(窗口句柄, TVM_SETITEMW, 0, (LPARAM)&tvi) != 0;
	}

	bool 置项目数值(int 项目索引, int 欲置入的项目数值 = 0) {
		HTREEITEM hItem = _取项目句柄(项目索引);
		if (!hItem) return false;
		TVITEMW tvi = { 0 };
		tvi.mask = TVIF_PARAM | TVIF_HANDLE;
		tvi.hItem = hItem;
		tvi.lParam = 欲置入的项目数值;
		return SendMessageW(窗口句柄, TVM_SETITEMW, 0, (LPARAM)&tvi) != 0;
	}

	void 加粗(int 项目索引) {
		HTREEITEM hItem = _取项目句柄(项目索引);
		if (!hItem) return;
		TVITEMW tvi = { 0 };
		tvi.mask = TVIF_STATE | TVIF_HANDLE;
		tvi.hItem = hItem;
		tvi.stateMask = TVIS_BOLD;
		tvi.state = TVIS_BOLD;
		SendMessageW(窗口句柄, TVM_SETITEMW, 0, (LPARAM)&tvi);
	}

	void 取消加粗(int 项目索引) {
		HTREEITEM hItem = _取项目句柄(项目索引);
		if (!hItem) return;
		TVITEMW tvi = { 0 };
		tvi.mask = TVIF_STATE | TVIF_HANDLE;
		tvi.hItem = hItem;
		tvi.stateMask = TVIS_BOLD;
		tvi.state = 0;
		SendMessageW(窗口句柄, TVM_SETITEMW, 0, (LPARAM)&tvi);
	}

	bool 是否已加粗(int 项目索引) {
		HTREEITEM hItem = _取项目句柄(项目索引);
		if (!hItem) return false;
		TVITEMW tvi = { 0 };
		tvi.mask = TVIF_STATE | TVIF_HANDLE;
		tvi.hItem = hItem;
		tvi.stateMask = TVIS_BOLD;
		if (SendMessageW(窗口句柄, TVM_GETITEMW, 0, (LPARAM)&tvi)) {
			return (tvi.state & TVIS_BOLD) != 0;
		}
		return false;
	}

	void 扩展(int 项目索引) {
		HTREEITEM hItem = _取项目句柄(项目索引);
		if (hItem) {
			SendMessageW(窗口句柄, TVM_EXPAND, TVE_EXPAND, (LPARAM)hItem);
		}
	}

	void 收缩(int 项目索引) {
		HTREEITEM hItem = _取项目句柄(项目索引);
		if (hItem) {
			SendMessageW(窗口句柄, TVM_EXPAND, TVE_COLLAPSE, (LPARAM)hItem);
		}
	}

	void 保证显示(int 项目索引) {
		HTREEITEM hItem = _取项目句柄(项目索引);
		if (hItem) {
			SendMessageW(窗口句柄, TVM_ENSUREVISIBLE, 0, (LPARAM)hItem);
		}
	}

	void 进入编辑(int 项目索引) {
		if (允许编辑) {
			HTREEITEM hItem = _取项目句柄(项目索引);
			if (hItem) {
				SendMessageW(窗口句柄, TVM_EDITLABELW, 0, (LPARAM)hItem);
			}
		}
	}

	int 加入项目(int 父项目索引 = -1, StrW 项目文本 = L"", int 图片索引 = 0, int 选中图片索引 = -1, int 项目数值 = 0, bool 是否加粗 = false, int 检查框状态 = 0) {
		HTREEITEM hParent = TVI_ROOT;
		if (父项目索引 != -1) {
			hParent = _取项目句柄(父项目索引);
			if (!hParent) return -1;
		}

		TVINSERTSTRUCTW tvis = { 0 };
		tvis.hParent = hParent;
		tvis.hInsertAfter = TVI_LAST;
		tvis.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
		tvis.item.pszText = (wchar_t*)(const wchar_t*)项目文本;
		tvis.item.iImage = 图片索引;
		tvis.item.iSelectedImage = (选中图片索引 == -1) ? 图片索引 : 选中图片索引;
		tvis.item.lParam = 项目数值;

		if (是否有检查框) {
			tvis.item.mask |= TVIF_STATE;
			tvis.item.stateMask |= TVIS_STATEIMAGEMASK;
			int stateImg = 0;
			if (检查框状态 == 1) stateImg = 2; // checked
			else if (检查框状态 == 2) stateImg = 3; // mixed
			else stateImg = 1; // unchecked
			tvis.item.state |= INDEXTOSTATEIMAGEMASK(stateImg);
		}

		if (是否加粗) {
			tvis.item.mask |= TVIF_STATE;
			tvis.item.stateMask |= TVIS_BOLD;
			tvis.item.state |= TVIS_BOLD;
		}

		HTREEITEM hItem = (HTREEITEM)SendMessageW(窗口句柄, TVM_INSERTITEMW, 0, (LPARAM)&tvis);
		if (hItem) {
			return _取项目索引(hItem);
		}
		return -1;
	}

	bool 删除项目(int 项目索引) {
		HTREEITEM hItem = _取项目句柄(项目索引);
		if (hItem) {
			bool res = SendMessageW(窗口句柄, TVM_DELETEITEM, 0, (LPARAM)hItem) != 0;
			return res;
		}
		return false;
	}

	void 清空() {
		SendMessageW(窗口句柄, TVM_DELETEITEM, 0, (LPARAM)TVI_ROOT);
	}

	int 取父项目(int 项目索引) {
		HTREEITEM hItem = _取项目句柄(项目索引);
		if (!hItem) return -1;
		HTREEITEM hParent = (HTREEITEM)SendMessageW(窗口句柄, TVM_GETNEXTITEM, TVGN_PARENT, (LPARAM)hItem);
		if (hParent) {
			return _取项目索引(hParent);
		}
		return -1;
	}

	bool 是否有子项目(int 项目索引) {
		HTREEITEM hItem = _取项目句柄(项目索引);
		if (!hItem) return false;
		HTREEITEM hChild = (HTREEITEM)SendMessageW(窗口句柄, TVM_GETNEXTITEM, TVGN_CHILD, (LPARAM)hItem);
		return hChild != NULL;
	}

	bool 是否已扩展(int 项目索引) {
		HTREEITEM hItem = _取项目句柄(项目索引);
		if (!hItem) return false;
		TVITEMW tvi = { 0 };
		tvi.mask = TVIF_STATE | TVIF_HANDLE;
		tvi.hItem = hItem;
		tvi.stateMask = TVIS_EXPANDED;
		if (SendMessageW(窗口句柄, TVM_GETITEMW, 0, (LPARAM)&tvi)) {
			return (tvi.state & TVIS_EXPANDED) != 0;
		}
		return false;
	}

	int 取检查框状态(int 项目索引) {
		HTREEITEM hItem = _取项目句柄(项目索引);
		if (!hItem) return 0;
		TVITEMW tvi = { 0 };
		tvi.mask = TVIF_STATE | TVIF_HANDLE;
		tvi.hItem = hItem;
		tvi.stateMask = TVIS_STATEIMAGEMASK;
		if (SendMessageW(窗口句柄, TVM_GETITEMW, 0, (LPARAM)&tvi)) {
			int state = (tvi.state & TVIS_STATEIMAGEMASK) >> 12;
			if (state == 2) return 1;
			if (state == 3) return 2;
			return 0;
		}
		return 0;
	}

	void 置检查框状态(int 项目索引, int 检查框状态, bool 连同子项目 = false) {
		if (项目索引 == -1 && 连同子项目) {
			HTREEITEM hItem = (HTREEITEM)SendMessageW(窗口句柄, TVM_GETNEXTITEM, TVGN_ROOT, 0);
			while (hItem) {
				_置树检查框状态(hItem, 检查框状态, true);
				hItem = (HTREEITEM)SendMessageW(窗口句柄, TVM_GETNEXTITEM, TVGN_NEXT, (LPARAM)hItem);
			}
		} else {
			HTREEITEM hItem = _取项目句柄(项目索引);
			if (hItem) {
				_置树检查框状态(hItem, 检查框状态, 连同子项目);
			}
		}
	}

	bool 取加粗状态(int 项目索引) {
		HTREEITEM hItem = _取项目句柄(项目索引);
		if (!hItem) return false;
		TVITEMW tvi = { 0 };
		tvi.mask = TVIF_STATE | TVIF_HANDLE;
		tvi.hItem = hItem;
		tvi.stateMask = TVIS_BOLD;
		if (SendMessageW(窗口句柄, TVM_GETITEMW, 0, (LPARAM)&tvi)) {
			return (tvi.state & TVIS_BOLD) != 0;
		}
		return false;
	}

	void 收缩项目(int 项目索引) {
		HTREEITEM hItem = _取项目句柄(项目索引);
		if (hItem) {
			SendMessageW(窗口句柄, TVM_EXPAND, TVE_COLLAPSE, (LPARAM)hItem);
		}
	}

	Bytes 导出项目() {
		int rows = 取项目数();
		Arraybe<StrW> texts;
		Arraybe<int> images;
		Arraybe<int> selectedImages;
		Arraybe<int> values;
		Arraybe<int> states;
		Arraybe<bool> bolds;
		Arraybe<bool> expands;
		Arraybe<int> indents;

		for (int i = 0; i < rows; ++i) {
			texts.push(取项目文本(i));
			images.push(取图片(i));
			selectedImages.push(取选中图片(i));
			values.push(取项目数值(i));
			states.push(取检查框状态(i));
			bolds.push(取加粗状态(i));
			expands.push(是否已扩展(i));
			indents.push(取缩进层次(i));
		}
		return FB(texts, images, selectedImages, values, states, bolds, expands, indents);
	}

	void 载入项目(const Bytes& dat) {
		if (!dat.buf) return;
		Arraybe<StrW> texts;
		Arraybe<int> images;
		Arraybe<int> selectedImages;
		Arraybe<int> values;
		Arraybe<int> states;
		Arraybe<bool> bolds;
		Arraybe<bool> expands;
		Arraybe<int> indents;
		deFB(dat, texts, images, selectedImages, values, states, bolds, expands, indents);

		清空();
		
		Arraybe<int> parentStack;
		parentStack.push(-1);
		int currentLevel = 0;
		
		for (int i = 0; i < texts.count; ++i) {
			int level = indents[i];
			while(currentLevel >= level) {
				parentStack.pop();
				currentLevel--;
			}
			int parentIdx = parentStack[parentStack.count - 1];
			
			int newIdx = 加入项目(parentIdx, texts[i], images[i], selectedImages[i], values[i], bolds[i], states[i]);
			
			parentStack.push(newIdx);
			currentLevel = level;
		}

		for (int i = 0; i < texts.count; ++i) {
			if (expands[i]) 扩展(i);
		}
	}

	virtual void 事件_项目被选择(int 选择方式) {}
	virtual void 事件_双击项目() {}
	virtual bool 事件_开始编辑(int 项目索引) { return true; }
	virtual bool 事件_结束编辑(int 项目索引) { return true; }
	virtual bool 事件_即将扩展(int 项目索引) { return true; }
	virtual bool 事件_即将收缩(int 项目索引) { return true; }
	virtual void 事件_右键单击项目() {}
	virtual void 事件_检查框状态被改变(int 表项索引, int 检查框状态) {}

	virtual LRESULT 挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	void _置树检查框状态(HTREEITEM hItem, int 检查框状态, bool 连同子项目) {
		TVITEMW tvi = { 0 };
		tvi.mask = TVIF_STATE | TVIF_HANDLE;
		tvi.hItem = hItem;
		tvi.stateMask = TVIS_STATEIMAGEMASK;
		int stateImg = 0;
		if (检查框状态 == 1) stateImg = 2; // checked
		else if (检查框状态 == 2) stateImg = 3; // mixed
		else stateImg = 1; // unchecked
		tvi.state = INDEXTOSTATEIMAGEMASK(stateImg);
		SendMessageW(窗口句柄, TVM_SETITEMW, 0, (LPARAM)&tvi);

		if (连同子项目) {
			HTREEITEM hChild = (HTREEITEM)SendMessageW(窗口句柄, TVM_GETNEXTITEM, TVGN_CHILD, (LPARAM)hItem);
			while (hChild) {
				_置树检查框状态(hChild, 检查框状态, true);
				hChild = (HTREEITEM)SendMessageW(窗口句柄, TVM_GETNEXTITEM, TVGN_NEXT, (LPARAM)hChild);
			}
		}
	}

	HTREEITEM _取项目句柄(int index) {
		if (index < 0) return NULL;
		int curr = 0;
		HTREEITEM hItem = (HTREEITEM)SendMessageW(窗口句柄, TVM_GETNEXTITEM, TVGN_ROOT, 0);
		while (hItem) {
			if (curr == index) return hItem;
			curr++;
			HTREEITEM hChild = (HTREEITEM)SendMessageW(窗口句柄, TVM_GETNEXTITEM, TVGN_CHILD, (LPARAM)hItem);
			if (hChild) {
				hItem = hChild;
				continue;
			}
			HTREEITEM hNext = (HTREEITEM)SendMessageW(窗口句柄, TVM_GETNEXTITEM, TVGN_NEXT, (LPARAM)hItem);
			if (hNext) {
				hItem = hNext;
				continue;
			}
			while (!hNext) {
				hItem = (HTREEITEM)SendMessageW(窗口句柄, TVM_GETNEXTITEM, TVGN_PARENT, (LPARAM)hItem);
				if (!hItem) break;
				hNext = (HTREEITEM)SendMessageW(窗口句柄, TVM_GETNEXTITEM, TVGN_NEXT, (LPARAM)hItem);
			}
			hItem = hNext;
		}
		return NULL;
	}

	int _取项目索引(HTREEITEM hTarget) {
		if (!hTarget) return -1;
		int curr = 0;
		HTREEITEM hItem = (HTREEITEM)SendMessageW(窗口句柄, TVM_GETNEXTITEM, TVGN_ROOT, 0);
		while (hItem) {
			if (hItem == hTarget) return curr;
			curr++;
			HTREEITEM hChild = (HTREEITEM)SendMessageW(窗口句柄, TVM_GETNEXTITEM, TVGN_CHILD, (LPARAM)hItem);
			if (hChild) {
				hItem = hChild;
				continue;
			}
			HTREEITEM hNext = (HTREEITEM)SendMessageW(窗口句柄, TVM_GETNEXTITEM, TVGN_NEXT, (LPARAM)hItem);
			if (hNext) {
				hItem = hNext;
				continue;
			}
			while (!hNext) {
				hItem = (HTREEITEM)SendMessageW(窗口句柄, TVM_GETNEXTITEM, TVGN_PARENT, (LPARAM)hItem);
				if (!hItem) break;
				hNext = (HTREEITEM)SendMessageW(窗口句柄, TVM_GETNEXTITEM, TVGN_NEXT, (LPARAM)hItem);
			}
			hItem = hNext;
		}
		return -1;
	}

	void _同步样式() {
		if (!窗口句柄) return;

		LONG_PTR style = GetWindowLongPtrW(窗口句柄, GWL_STYLE);
		style &= ~(TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_EDITLABELS | TVS_SHOWSELALWAYS | TVS_CHECKBOXES | WS_BORDER);
		style |= _取基础风格();
		SetWindowLongPtrW(窗口句柄, GWL_STYLE, style);

		LONG_PTR exstyle = GetWindowLongPtrW(窗口句柄, GWL_EXSTYLE);
		exstyle &= ~(WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE | WS_EX_STATICEDGE | WS_EX_DLGMODALFRAME);
		exstyle |= _取扩展风格();
		SetWindowLongPtrW(窗口句柄, GWL_EXSTYLE, exstyle);

		SetWindowPos(窗口句柄, NULL, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
	}

	DWORD _取基础风格() {
		DWORD style = _窗口风格;
		if (显示加减框) style |= TVS_HASBUTTONS;
		if (显示连线) style |= TVS_HASLINES;
		if (显示根部线) style |= TVS_LINESATROOT;
		if (允许编辑) style |= TVS_EDITLABELS;
		if (始终显示选择项) style |= TVS_SHOWSELALWAYS;
		if (是否有检查框) style |= TVS_CHECKBOXES;
		if (边框 == 树型框边框::单线边框式) style |= WS_BORDER;
		return style;
	}

	DWORD _取扩展风格() {
		switch (边框) {
		case 树型框边框::凹入式: return WS_EX_CLIENTEDGE;
		case 树型框边框::凸出式: return WS_EX_WINDOWEDGE | WS_EX_DLGMODALFRAME;
		case 树型框边框::浅凹入式: return WS_EX_STATICEDGE;
		case 树型框边框::镜框式: return WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE;
		}
		return 0;
	}
};

LRESULT 树型框::挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {

	case BE_REFLECT + WM_NOTIFY: {
		NMHDR* hdr = (NMHDR*)lParam;
		if (hdr->hwndFrom == 窗口句柄) {
			switch (hdr->code) {
			case TVN_SELCHANGEDW:
			case TVN_SELCHANGEDA: {
				NMTREEVIEWW* pnmtv = (NMTREEVIEWW*)lParam;
				现行选中项 = _取项目索引(pnmtv->itemNew.hItem);
				int action = 2; // unknown
				if (pnmtv->action == TVC_BYKEYBOARD) action = 0;
				else if (pnmtv->action == TVC_BYMOUSE) action = 1;
				事件_项目被选择(action);
				break;
			}
			case NM_DBLCLK: {
				事件_双击项目();
				break;
			}
			case NM_RCLICK: {
				事件_右键单击项目();
				break;
			}
			case TVN_BEGINLABELEDITW:
			case TVN_BEGINLABELEDITA: {
				NMTVDISPINFOW* pdi = (NMTVDISPINFOW*)lParam;
				bool allow = 事件_开始编辑(_取项目索引(pdi->item.hItem));
				return allow ? FALSE : TRUE;
			}
			case TVN_ENDLABELEDITW: {
				NMTVDISPINFOW* pdi = (NMTVDISPINFOW*)lParam;
				if (pdi->item.pszText != NULL) {
					结束编辑文本 = pdi->item.pszText;
					bool accept = 事件_结束编辑(_取项目索引(pdi->item.hItem));
					return accept ? TRUE : FALSE;
				}
				break;
			}
			case TVN_ENDLABELEDITA: {
				NMTVDISPINFOA* pdi = (NMTVDISPINFOA*)lParam;
				if (pdi->item.pszText != NULL) {
					结束编辑文本 = AtoW(pdi->item.pszText);
					bool accept = 事件_结束编辑(_取项目索引(pdi->item.hItem));
					return accept ? TRUE : FALSE;
				}
				break;
			}
			case TVN_ITEMEXPANDINGW:
			case TVN_ITEMEXPANDINGA: {
				NMTREEVIEWW* pnmtv = (NMTREEVIEWW*)lParam;
				int idx = _取项目索引(pnmtv->itemNew.hItem);
				if (pnmtv->action == TVE_EXPAND) {
					bool allow = 事件_即将扩展(idx);
					return allow ? FALSE : TRUE;
				} else if (pnmtv->action == TVE_COLLAPSE) {
					bool allow = 事件_即将收缩(idx);
					return allow ? FALSE : TRUE;
				}
				break;
			}

			case NM_CLICK: {
				if (是否有检查框) {
					TVHITTESTINFO ht = { 0 };
					GetCursorPos(&ht.pt);
					ScreenToClient(窗口句柄, &ht.pt);
					TreeView_HitTest(窗口句柄, &ht);
					if (ht.flags & TVHT_ONITEMSTATEICON) {
						SendMessageW(窗口句柄, TVM_SELECTITEM, TVGN_CARET, (LPARAM)ht.hItem);
						
						int idx = _取项目索引(ht.hItem);
						int curState = 取检查框状态(idx);
						int nextState = (curState == 1) ? 0 : 1;
						if (检查框状态数 > 2) nextState = (curState + 1) % 检查框状态数;
						置检查框状态(idx, nextState, false);
						事件_检查框状态被改变(idx, nextState);
						return TRUE;
					}
				}
				break;
			}
			case TVN_KEYDOWN: {
				NMTVKEYDOWN* ptvkd = (NMTVKEYDOWN*)lParam;
				if (ptvkd->wVKey == VK_SPACE && 是否有检查框) {
					HTREEITEM hSel = (HTREEITEM)SendMessageW(窗口句柄, TVM_GETNEXTITEM, TVGN_CARET, 0);
					if (hSel) {
						int idx = _取项目索引(hSel);
						int curState = 取检查框状态(idx);
						int nextState = (curState == 1) ? 0 : 1;
						if (检查框状态数 > 2) nextState = (curState + 1) % 检查框状态数;
						置检查框状态(idx, nextState, false);
						事件_检查框状态被改变(idx, nextState);
						return TRUE;
					}
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
