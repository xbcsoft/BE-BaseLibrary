#include <BEMod.h>

struct __启动窗口 : 窗口
{
	SciterDom dom;
	void 事件_菜单项被单击(int 菜单ID)
	{
		switch (菜单ID)
		{
		case 1001: // 浏览html
		{
			StrA file = 文件对话框_打开(L"浏览html", L"网页文件(*.html;*.htm)|*.html;*.htm|所有文件(*.*)|*.*", 0, 取运行目录(), true);
			if (file) {
				dom.浏览(file);
			}
			break;
		}
		}
	}

	void 事件_快捷键(WORD cmdID)
	{
		事件_菜单项被单击(cmdID);
	}

	void 事件_创建完毕()
	{
		dom = st.取文档模型();
		快捷键_注册('O', 控制键::Ctrl键, 1001);
	}

#pragma region 组件成员
	菜单 主菜单;
	struct _st :SciterUI {

	} st;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
} _启动窗口;