#include "../BEWin32UI.h"
#include "../中文UI/菜单.h"

struct _窗口1 : 窗口
{
	void 事件_菜单项被单击(int 菜单ID)
	{
		信息框("你点击了菜单 ID: "+到文本(菜单ID));
	}

#pragma region 组件成员
	菜单 主菜单;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
} 窗口1;