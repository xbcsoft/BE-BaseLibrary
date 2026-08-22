#include "stdafx.h"
#include "其他系统操作.h"
#include "环境存取.h"

class EXP 文件对话框
{
public:
	StrW 标题;
	int 初始过滤器索引 = 0;
	StrW 初始目录;
	bool 不改变当前目录 = true;
	NilOpt<HWND> 父窗口 = nil;

public:
	/**置过滤器
	 * @param 过滤器 格式："文本文件（*.txt）|*.txt|所有文件(*.*)|*.*"
	 */
	void 置过滤器(c_StrX 过滤器)
	{
		_wFilter = 过滤器;
		if (_wFilter) {
			for (charW* p = _wFilter; p < (charW*)_wFilter + _wFilter.len(); ++p) {
				if (*p == L'|') *p = 0;
			}
			static const byte endNull[4] = { 0 };
			_wFilter.bytes += endNull;
		}
	}

	文件对话框(c_StrX 标题 = "", c_StrX 过滤器 = "", int 初始过滤器索引 = 0,
		c_StrX 初始目录 = "", bool 不改变当前目录 = true, NilOpt<HWND> 父窗口 = nil)
		: 标题(标题), 初始过滤器索引(初始过滤器索引),
		初始目录(初始目录), 不改变当前目录(不改变当前目录), 父窗口(父窗口)
	{
		置过滤器(过滤器);
	}

	StrX 打开(bool 支持多文件 = false, NilOpt<Arraybe<StrX>&> 多文件数组 = nil)
	{
		OPENFILENAMEW ofn = { sizeof(ofn) };
		charW szFile[32768] = { 0 };
		ofn.hwndOwner = 父窗口.OR(GetActiveWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = 32768;
		ofn.lpstrFilter = _wFilter ? (const charW*)_wFilter : NULL;
		ofn.nFilterIndex = 初始过滤器索引 + 1;
		ofn.lpstrTitle = 标题;
		ofn.lpstrInitialDir = 初始目录 ? (const charW*)初始目录 : NULL;
		ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
		if (支持多文件) ofn.Flags |= OFN_ALLOWMULTISELECT;
		if (不改变当前目录) ofn.Flags |= OFN_NOCHANGEDIR;

		if (!GetOpenFileNameW(&ofn)) {
			return "";
		}

		// 单文件模式：直接返回完整路径，无需处理多文件数组
		if (!支持多文件) {
			return StrX(szFile);
		}

		// 多文件模式：解析并填充多文件数组
		if (多文件数组 != nil) {
			((Arraybe<StrX>&)多文件数组).clear();
		}

		const charW* p = szFile;
		StrW dir = 路径_保证去尾(p);
		p += wcslen(p) + 1;

		if (!*p) { // 多选模式下仅选了 1 个文件
			if (多文件数组 != nil) {
				((Arraybe<StrX>&)多文件数组).push(dir);
			}
			return dir;
		}

		StrW firstFile;
		while (*p) {
			StrW fullPath = dir + L"\\" + p;
			if (!firstFile) firstFile = fullPath;
			if (多文件数组 != nil) {
				((Arraybe<StrX>&)多文件数组).push(fullPath);
			}
			p += wcslen(p) + 1;
		}
		return firstFile;
	}

	StrX 保存(c_StrX 默认文件名 = "")
	{
		OPENFILENAMEW ofn = { sizeof(ofn) };
		charW szFile[MAX_PATH] = { 0 };
		if (默认文件名) {
			StrW wDef = 默认文件名;
			wcsncpy_s(szFile, wDef, MAX_PATH - 1);
		}
		ofn.hwndOwner = 父窗口.OR(GetActiveWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrFilter = _wFilter ? (const charW*)_wFilter : NULL;
		ofn.nFilterIndex = 初始过滤器索引 + 1;
		ofn.lpstrTitle = 标题;
		ofn.lpstrInitialDir = 初始目录 ? (const charW*)初始目录 : NULL;
		ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
		if (不改变当前目录) ofn.Flags |= OFN_NOCHANGEDIR;

		if (GetSaveFileNameW(&ofn)) {
			return StrX(szFile);
		}
		return "";
	}
private:
	StrW _wFilter;
};

StrX 文件对话框_打开(c_StrX 标题 = "", c_StrX 过滤器 = "", int 初始过滤器索引 = 0,
	c_StrX 初始目录 = "", bool 不改变当前目录 = true, NilOpt<HWND> 父窗口 = nil,
	bool 支持多文件 = false, NilOpt<Arraybe<StrX>&> 多文件数组 = nil)
{
	文件对话框 dlg(标题, 过滤器, 初始过滤器索引, 初始目录, 不改变当前目录, 父窗口);
	return dlg.打开(支持多文件, 多文件数组);
}

StrX 文件对话框_保存(c_StrX 标题 = "", c_StrX 过滤器 = "", int 初始过滤器索引 = 0,
	c_StrX 初始目录 = "", bool 不改变当前目录 = true, NilOpt<HWND> 父窗口 = nil,
	c_StrX 默认文件名 = "")
{
	文件对话框 dlg(标题, 过滤器, 初始过滤器索引, 初始目录, 不改变当前目录, 父窗口);
	return dlg.保存(默认文件名);
}

Arraybe<StrW> 多文件对话框(c_StrX 标题, c_StrX 过滤器, int 初始过滤器索引 = 0,
	c_StrX 初始目录 = "", bool 不改变当前目录 = true, NilOpt<HWND> 父窗口 = nil)
{
	Arraybe<StrX> arr;
	文件对话框_打开(标题, 过滤器, 初始过滤器索引, 初始目录, 不改变当前目录, 父窗口, true, arr);
	Arraybe<StrW> res(arr.count);
	for (auto& item : arr) {
		res.push(item);
	}
	return res;
}