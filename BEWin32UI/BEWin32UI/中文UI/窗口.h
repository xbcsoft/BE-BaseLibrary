#pragma once
#include "容器类.h"
#include "控件类.h"

定义_枚举型(窗口位置, char,
	通常 = 0, 居中 = 1, 最小化 = 2, 最大化 = 4
);
定义_枚举型(窗口边框, char,
	无边框 = 0, 普通可调边框 = 1, 普通固定边框 = 2,
	窄标题可调边框 = 3, 窄标题固定边框 = 4
);

class 窗口 :public 容器类
{
public:
	struct 参数 {
		float 左边 = 50.0f;
		float 顶边 = 50.0f;
		float 宽度 = 380.0f;
		float 高度 = 250.0f;
		可空<bool> 可视; //该参数为空时由WinMain函数的nCmdShow决定
		bool 禁止 = false;
		StrW 类名 = L"BEWindow";
		StrW 标题 = L"";
		可空<Bytes> 图标; //设置窗口标题栏图标，与图标ID二选一
		可空<uint> 图标ID; //设置窗口标题栏图标，与图标二选一
		bool 总在最前 = false;
		窗口边框 边框 = 窗口边框::普通固定边框;
		可空<窗口位置> 位置;
		颜色 背景颜色 = 颜色::默认底色;
		bool 控制按钮 = true;
		bool 最小化按钮 = false _SUB_; //启用控制按钮时可使用最小化按钮
		bool 最大化按钮 = false _SUB_; //启用控制按钮时可使用最大化按钮
		bool Esc键关闭 = true;
		bool 随意移动 = false;
		bool 在任务栏显示 = true;
		bool 保持标题栏点燃 = false;
		bool 子控件焦点导航 = false;
	}static _df;

	窗口& 初显(int nCmdShow);

	bool 创建(const 参数& cs = 窗口::参数(), 窗口* 父窗口 = 0, bool 模态载入 = 0);

	void 完毕(bool 模态);

	void 总在最前_(bool is = true);

	void 置图标(HICON icon, bool 小图标 = true);

	void 置图标(int 资源ID号);

	void 置图标(const Bytes& zjjIcon);

	void 置托盘图标(HICON icon, const StrW& 提示文本 = L"");

	void 置托盘图标(int 资源ID号, const StrW& 提示文本 = L"");

	void 置托盘图标(const Bytes& zjjIcon, const StrW& 提示文本 = L"");

	void 置前台();

	void 边框_(窗口边框 边框, 可空<int&> 窗口风格 = nil, 可空<int&> 窗口扩展风格 = nil);

	void 位置_(窗口位置 位置);

	bool 销毁();

	/**必须在初始创建参数时采用[在任务栏显示=假]后续才可动态设置此方案
	 * @param is
	 */
	void 在任务栏显示_(bool is = true);

	/**设置快捷键(已存在的同ID将覆盖)，回调方式在[事件_快捷键(cmdID)]
	 * @param 主热键 查询Windows虚拟键代码"VK_"，备注：大写字母ASCII等价于键码
	 * @param 控制键=0 可用位组合的形式拼接[控制键::]开头的常量
	 * @param cmdID<可空> 可自定义热键ID，若为空则使用MAKEWORD(主热键,仅热键)位组合
	 * @return 产生覆盖则返回假，成功插入新ID返回真
	 */
	bool 快捷键_注册(BYTE 主热键, BYTE 控制键 = 0, 可空<WORD> cmdID = 空);

	/**根据快捷键ID反查注册时的主热键和控制键
	 * @param cmdID
	 * @param 主热键<可空> 主热键
	 * @param 控制键<可空> 控制键
	 * @param 是否自定义ID
	 * @return 如果是自定义ID找到返回真找不到返回假，非自定义ID总是返回真
	 */
	bool 快捷键_ID反取(WORD cmdID, 可空<BYTE&> 主热键, 可空<BYTE&> 控制键 = 空,
		bool 是否自定义ID = false);

	/**从已有的快捷键查询cmdID（含两种查询方式）
	 * @param 主热键
	 * @param 控制键=控制键::主热键
	 * @param 是否自定义ID=false 若为自定义ID查询失败返回0成功返回本窗口注册列表中的cmdID
	 * @return
	 */
	WORD 快捷键_取ID(BYTE 主热键, BYTE 控制键 = 0, bool 是否自定义ID = false);

	/**调整控件组的Tab顺序 只需停留顺序调整Z顺序,由Win32内置的Dialog方案自动实现
	 * @param controls<可空>=nil 无参调用时内部会自动枚举子窗口
	 */
	void 调整控件组的Tab顺序(可空<Arraybe<控件类*>&> controls = nil);


	virtual LRESULT 挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	WindowAccel _wa;
	char _初始位置 = -1;

	// 静态窗口过程
	static LRESULT CALLBACK _WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

	void _初始化Esc快捷键();

	static int __比较停留顺序(const void* a, const void* b);

public:
	StrW 类名;
	定义_背景颜色成员;
	定义_标题成员;
	HandleICON _图标小句柄;
	HandleICON _图标大句柄;
	bool 总在最前; 窗口边框 边框;
	窗口位置 位置; bool Esc键关闭_;
	bool 随意移动_; bool 在任务栏显示;
	bool 保持标题栏点燃_, 子控件焦点导航;
	bool _是否有托盘图标 = false;
	bool _首次激活已触发 = false;
public:
	virtual void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	virtual void 事件_创建完毕();
	virtual void 事件_首次激活();
	virtual void 事件_创建本体();
	virtual bool 事件_被关闭();
	virtual void 事件_被销毁();
	virtual void 事件_快捷键(WORD cmdID);
	virtual void 事件_菜单项被单击(int 菜单ID);
	/**事件_托盘
	 * @param 操作类型 1、#单击左键；2、#双击；3、#单击右键。
	 */
	virtual void 事件_托盘(int 操作类型);
	virtual LRESULT 事件_反馈(WPARAM wParam, LPARAM lParam);
	virtual void 事件_尺寸被改变();

	bool 通用事件_背景绘制(HDC hdc);
};

int Win32消息循环();



void 处理事件();
