#pragma once
#include "stdafx.h"
#include <扩展界面库一/超级列表框.h>

// 回调函数类型定义：返回 false 阻止编辑，返回 true 允许编辑
typedef bool(*超级列表框扩展编辑回调)(HWND 列表框句柄, int 准备编辑行, int 准备编辑列);


bool 超级列表框_置扩展编辑(HWND 列表框句柄, 超级列表框扩展编辑回调 回调);

bool 超级列表框_取消扩展编辑(HWND 列表框句柄);
