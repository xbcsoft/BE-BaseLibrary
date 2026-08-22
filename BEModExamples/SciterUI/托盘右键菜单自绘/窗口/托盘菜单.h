#pragma once
#include <BEMod.h>

struct __托盘菜单 : 窗口
{
	SciterUI st;
	SciterDom dom;

	void 事件_创建本体();

	LRESULT 挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

	virtual void 弹出(可空<int> screenX = 空, 可空<int> screenY = 空);
}; extern __托盘菜单 _托盘菜单;
