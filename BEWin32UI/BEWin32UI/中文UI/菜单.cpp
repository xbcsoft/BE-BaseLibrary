#include "控件类.h"

class EXP 子菜单;

class EXP 菜单 : public 控件类
{
public:
	菜单() {} // 默认构造函数

	//控件类自带析构函数那边会自动调用这里的虚函数【销毁】
	bool 销毁() override {
		if (菜单句柄) {
			if (父窗口句柄 && IsWindow(父窗口句柄)) {
				if (GetMenu(父窗口句柄) == 菜单句柄) {
					SetMenu(父窗口句柄, NULL);
				}
			}
			if (!_来自资源) {
				DestroyMenu(菜单句柄); //该方法会递归销毁所有子弹出菜单
			}
			菜单句柄 = NULL;
			return true;
		}
		return false;
	}

	// 内部用：从已有 HMENU 创建代理对象（子菜单级）
	菜单(HMENU hPopup, HWND hRoot) {
		菜单句柄 = hPopup;
		父窗口句柄 = hRoot;
		_来自资源 = true; // 代理不拥有所有权，由根菜单销毁时递归清理
	}

	/**从容器类创建菜单
	 * @param 父窗口
	 * @return 必须给出父窗口，否则返回假
	 */
	bool 创建(容器类* 父窗口) {
		父窗口句柄 = 窗口_取祖宗句柄(父窗口->窗口句柄);
		if (!父窗口句柄)return false;
		this->父窗口 = 父窗口;

		菜单句柄 = CreateMenu();
		SetMenu(父窗口句柄, 菜单句柄);
		return true;
	}

	/**从 RC 资源挂接菜单（IDR_MENU*），生命周期由系统管理
	 */
	bool 挂接资源(int 资源ID, 容器类* 父窗口) {
		父窗口句柄 = 窗口_取祖宗句柄(父窗口->窗口句柄);
		if (!父窗口句柄)return false;
		this->父窗口 = 父窗口;

		菜单句柄 = LoadMenuW(g_hInst, MAKEINTRESOURCEW(资源ID));
		if (!菜单句柄) return false;
		_来自资源 = true;

		SetMenu(父窗口句柄, 菜单句柄);
		return true;
	}

	// ── 添加/插入 子菜单（返回可继续操作的菜单对象）───────────────

	/**向当前菜单级追加一个子菜单，返回该子菜单的操作对象*/
	子菜单 添加子菜单(c_StrW 标题);

	/**在指定位置插入一个子菜单，返回该子菜单的操作对象*/
	子菜单 插入子菜单(int 插入位置, c_StrW 标题);

	// ── 当前菜单级的项操作（直接操作 this->菜单句柄）───────────────
	//菜单ID指的是一个自定义ID，后续用于响应WM_COMMAND传入的识别ID

	void 添加项(int 菜单ID, c_StrW 标题) {
		if (!菜单句柄) return;
		AppendMenuW(菜单句柄, MF_STRING, (UINT_PTR)菜单ID, 标题);
	}

	void 添加分隔线() {
		if (!菜单句柄) return;
		AppendMenuW(菜单句柄, MF_SEPARATOR, 0, NULL);
	}

	void 插入项(int 插入位置, int 菜单ID, c_StrW 标题) {
		if (!菜单句柄) return;
		InsertMenuW(菜单句柄, (UINT)插入位置, MF_BYPOSITION | MF_STRING, (UINT_PTR)菜单ID, 标题);
	}

	void 删除项(int 菜单ID) {
		if (!菜单句柄) return;
		DeleteMenu(菜单句柄, (UINT)菜单ID, MF_BYCOMMAND);
	}

	// ── 状态操作（MF_BYCOMMAND 会递归搜索整棵菜单树）────────────────

	void 禁用项(int 菜单ID, bool 禁止 = true) {
		if (!菜单句柄) return;
		UINT flag = 禁止 ? MF_BYCOMMAND | MF_GRAYED : MF_BYCOMMAND | MF_ENABLED;
		EnableMenuItem(菜单句柄, (UINT)菜单ID, flag);
		刷新菜单栏();
	}

	void 置项勾选(int 菜单ID, bool 勾选) {
		if (!菜单句柄) return;
		CheckMenuItem(菜单句柄, (UINT)菜单ID,
			MF_BYCOMMAND | (勾选 ? MF_CHECKED : MF_UNCHECKED));
	}

	void 置项标题(int 菜单ID, c_StrW 新标题) {
		if (!菜单句柄) return;
		MENUITEMINFOW mii = {};
		mii.cbSize = sizeof(mii);
		mii.fMask = MIIM_STRING;
		mii.dwTypeData = 新标题;
		SetMenuItemInfoW(菜单句柄, (UINT)菜单ID, FALSE, &mii);
		刷新菜单栏();
	}

	void 刷新菜单栏() {
		if (父窗口句柄 && IsWindow(父窗口句柄)) {
			DrawMenuBar(父窗口句柄);
		}
	}

public:
	HMENU 菜单句柄 = NULL;
	bool _来自资源 = false;
};

class EXP 子菜单 : public 菜单
{
public:
	HMENU 父菜单句柄 = NULL;
public:
	子菜单() {}
	子菜单(HMENU hPopup, HWND hRoot, HMENU hParentMenu) : 菜单(hPopup, hRoot) {
		父菜单句柄 = hParentMenu;
	}

	/**创建弹出式菜单（常用于托盘右键菜单或普通的右键菜单）
	 * @return
	 */
	bool 创建() {
		if (菜单句柄) { return false; }
		菜单句柄 = CreatePopupMenu();
		return true;
	}

	/**从 RC 资源挂接菜单，生命周期由系统管理
	 * @param 资源ID （IDR_MENU*）
	 * @param 资源菜单项索引=0 默认是该资源菜单的第0个子菜单
	 * @return
	 */
	bool 挂接资源(int 资源ID, int 资源菜单项索引 = 0) {
		HMENU hResMenu = LoadMenuW(g_hInst, MAKEINTRESOURCEW(资源ID));
		if (!hResMenu) return false;

		HMENU hSubMenu = GetSubMenu(hResMenu, 资源菜单项索引);
		if (!hSubMenu) {
			hSubMenu = hResMenu;
		} else {
			RemoveMenu(hResMenu, 资源菜单项索引, MF_BYPOSITION);
			DestroyMenu(hResMenu);
		}

		if (父菜单句柄 && 菜单句柄) {
			int targetPos = -1;
			int count = GetMenuItemCount(父菜单句柄);
			for (int i = 0; i < count; i++) {
				if (GetSubMenu(父菜单句柄, i) == 菜单句柄) {
					targetPos = i;
					break;
				}
			}

			if (targetPos != -1) {
				MENUITEMINFOW mii = { sizeof(mii) };
				mii.cbSize = sizeof(mii);
				mii.fMask = MIIM_SUBMENU;
				mii.hSubMenu = hSubMenu;

				SetMenuItemInfoW(父菜单句柄, targetPos, TRUE, &mii);
			}
		}

		if (菜单句柄) {
			if (!_来自资源) {
				DestroyMenu(菜单句柄);
			}
		}

		菜单句柄 = hSubMenu;
		_来自资源 = true;

		刷新菜单栏();
		return true;
	}

	/**置标题 用于置本子菜单位于父菜单中的标题
	 * @param 新标题
	 */
	void 置标题(c_StrW 新标题) {
		if (父菜单句柄 && 菜单句柄) {
			int targetPos = -1;
			int count = GetMenuItemCount(父菜单句柄);
			for (int i = 0; i < count; i++) {
				if (GetSubMenu(父菜单句柄, i) == 菜单句柄) {
					targetPos = i;
					break;
				}
			}

			if (targetPos != -1) {
				MENUITEMINFOW mii = { sizeof(mii) };
				mii.cbSize = sizeof(mii);
				mii.fMask = MIIM_STRING;
				mii.dwTypeData = 新标题;
				SetMenuItemInfoW(父菜单句柄, targetPos, TRUE, &mii);
				刷新菜单栏();
			}
		}
	}

	/**内部使用，请使用任意窗口控件中的.弹出菜单();
	 */
	void _弹出(HWND 归属窗口句柄, int x, int y) {
		if (菜单句柄) {
			归属窗口句柄 = 窗口_取祖宗句柄(归属窗口句柄);
			SetForegroundWindow(归属窗口句柄);
			TrackPopupMenu(菜单句柄, TPM_RIGHTBUTTON, x, y, 0, 归属窗口句柄, NULL);
			PostMessageW(归属窗口句柄, WM_NULL, 0, 0);
		}
	}
};

子菜单 菜单::添加子菜单(c_StrW 标题) {
	HMENU hPop = CreatePopupMenu();
	AppendMenuW(菜单句柄, MF_POPUP, (UINT_PTR)hPop, 标题);
	刷新菜单栏();
	return 子菜单(hPop, 父窗口句柄, 菜单句柄);
}

子菜单 菜单::插入子菜单(int 插入位置, c_StrW 标题) {
	HMENU hPop = CreatePopupMenu();
	InsertMenuW(菜单句柄, (UINT)插入位置, MF_BYPOSITION | MF_POPUP, (UINT_PTR)hPop, 标题);
	return 子菜单(hPop, 父窗口句柄, 菜单句柄);
}

void 窗口基类::弹出菜单(子菜单& menu, 可空<float> x, 可空<float> y) {
	POINT pt;
	if (x == 空 || y == 空) {
		GetCursorPos(&pt);
	} else {
		POINT pt = { (int)dpi(x), (int)dpi(y) };
		ClientToScreen(窗口句柄, &pt);
	}
	menu._弹出(窗口句柄, pt.x, pt.y);
}
