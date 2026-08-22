#pragma once
#include "stdafx.h"
#include "其他系统操作.h"
#include "环境存取.h"

class 文件对话框
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
	void 置过滤器(c_StrX 过滤器);

	文件对话框(c_StrX 标题 = "", c_StrX 过滤器 = "", int 初始过滤器索引 = 0,
		c_StrX 初始目录 = "", bool 不改变当前目录 = true, NilOpt<HWND> 父窗口 = nil);

	StrX 打开(bool 支持多文件 = false, NilOpt<Arraybe<StrX>&> 多文件数组 = nil);

	StrX 保存(c_StrX 默认文件名 = "");
private:
	StrW _wFilter;
};

StrX 文件对话框_打开(c_StrX 标题 = "", c_StrX 过滤器 = "", int 初始过滤器索引 = 0,
	c_StrX 初始目录 = "", bool 不改变当前目录 = true, NilOpt<HWND> 父窗口 = nil,
	bool 支持多文件 = false, NilOpt<Arraybe<StrX>&> 多文件数组 = nil);

StrX 文件对话框_保存(c_StrX 标题 = "", c_StrX 过滤器 = "", int 初始过滤器索引 = 0,
	c_StrX 初始目录 = "", bool 不改变当前目录 = true, NilOpt<HWND> 父窗口 = nil,
	c_StrX 默认文件名 = "");

Arraybe<StrW> 多文件对话框(c_StrX 标题, c_StrX 过滤器, int 初始过滤器索引 = 0,
	c_StrX 初始目录 = "", bool 不改变当前目录 = true, NilOpt<HWND> 父窗口 = nil);
