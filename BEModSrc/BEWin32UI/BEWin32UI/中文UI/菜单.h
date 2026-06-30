#pragma once
#include "控件类.h"

class 子菜单;

class 菜单 : public 控件类
{
public:
	菜单(); // 默认构造函数

	//控件类自带析构函数那边会自动调用这里的虚函数【销毁】
	bool 销毁() override;

	// 内部用：从已有 HMENU 创建代理对象（子菜单级）
	菜单(HMENU hPopup, HWND hRoot);

	/**从容器类创建菜单
	 * @param 父窗口
	 * @return 必须给出父窗口，否则返回假
	 */
	bool 创建(容器类* 父窗口);

	/**从 RC 资源挂接菜单（IDR_MENU*），生命周期由系统管理
	 */
	bool 挂接资源(int 资源ID, 容器类* 父窗口);

	// ── 添加/插入 子菜单（返回可继续操作的菜单对象）───────────────

	/**向当前菜单级追加一个子菜单，返回该子菜单的操作对象*/
	子菜单 添加子菜单(c_StrW 标题);

	/**在指定位置插入一个子菜单，返回该子菜单的操作对象*/
	子菜单 插入子菜单(int 插入位置, c_StrW 标题);

	// ── 当前菜单级的项操作（直接操作 this->菜单句柄）───────────────
	//菜单ID指的是一个自定义ID，后续用于响应WM_COMMAND传入的识别ID

	void 添加项(int 菜单ID, c_StrW 标题);

	void 添加分隔线();

	void 插入项(int 插入位置, int 菜单ID, c_StrW 标题);

	void 删除项(int 菜单ID);

	// ── 状态操作（MF_BYCOMMAND 会递归搜索整棵菜单树）────────────────

	void 禁用项(int 菜单ID, bool 禁止 = true);

	void 置项勾选(int 菜单ID, bool 勾选);

	void 置项标题(int 菜单ID, c_StrW 新标题);

	void 刷新菜单栏();

public:
	HMENU 菜单句柄 = NULL;
	bool _来自资源 = false;
};

class 子菜单 : public 菜单
{
public:
	HMENU 父菜单句柄 = NULL;
public:
	子菜单();
	子菜单(HMENU hPopup, HWND hRoot, HMENU hParentMenu);

	/**创建弹出式菜单（常用于托盘右键菜单或普通的右键菜单）
	 * @return
	 */
	bool 创建();

	/**从 RC 资源挂接菜单，生命周期由系统管理
	 * @param 资源ID （IDR_MENU*）
	 * @param 资源菜单项索引=0 默认是该资源菜单的第0个子菜单
	 * @return
	 */
	bool 挂接资源(int 资源ID, int 资源菜单项索引 = 0);

	/**置标题 用于置本子菜单位于父菜单中的标题
	 * @param 新标题
	 */
	void 置标题(c_StrW 新标题);

	/**内部使用，请使用任意窗口控件中的.弹出菜单();
	 */
	void _弹出(HWND 归属窗口句柄, int x, int y);
};
