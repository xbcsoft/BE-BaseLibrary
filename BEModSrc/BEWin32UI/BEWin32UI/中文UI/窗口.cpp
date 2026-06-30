#include "容器类.h"
#include "控件类.h"

定义_枚举型(窗口位置, char,
	通常 = 0, 居中 = 1, 最小化 = 2, 最大化 = 4
);
定义_枚举型(窗口边框, char,
	无边框 = 0, 普通可调边框 = 1, 普通固定边框 = 2,
	窄标题可调边框 = 3, 窄标题固定边框 = 4
);

class EXP 窗口 :public 容器类
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

	窗口& 初显(int nCmdShow) {
		_初始位置 = 0;
		可视 = nCmdShow != SW_HIDE;
		if (nCmdShow == SW_SHOWMINIMIZED ||
			nCmdShow == SW_MINIMIZE ||
			nCmdShow == SW_SHOWMINNOACTIVE ||
			nCmdShow == SW_FORCEMINIMIZE)
		{
			_初始位置 = 窗口位置::最小化;
		}
		if (nCmdShow == SW_SHOWMAXIMIZED || nCmdShow == SW_MAXIMIZE)
		{
			_初始位置 = 窗口位置::最大化;
		}
		_初始位置 |= 窗口位置::居中;
		return *this;
	}

	bool 创建(const 参数& cs = 窗口::参数(), 窗口* 父窗口 = 0, bool 模态载入 = 0) {
		类名 = cs.类名;
		左边 = cs.左边;
		顶边 = cs.顶边;
		宽度 = cs.宽度;
		高度 = cs.高度;
		总在最前 = cs.总在最前;
		边框 = cs.边框;
		Esc键关闭_ = cs.Esc键关闭;
		随意移动_ = cs.随意移动;
		在任务栏显示 = cs.在任务栏显示;
		保持标题栏点燃_ = cs.保持标题栏点燃;
		子控件焦点导航 = cs.子控件焦点导航;
		背景颜色 = cs.背景颜色;
		_背景画刷 = CreateSolidBrush(背景颜色);
		可视 = cs.可视.OR(_初始位置 == -1 ? true : 可视);
		位置 = cs.位置.OR((窗口位置)(_初始位置 == -1 ? 窗口位置::居中 : _初始位置));

		int __获取图标ID();
		if (cs.图标 == 空) {
			DWORD 图标ID = cs.图标ID.OR(__获取图标ID());
			_图标小句柄 = 图标::从资源创建(g_hInst, 图标ID, 32);
			_图标大句柄 = 图标::从资源创建(g_hInst, 图标ID);
		} else {
			_图标小句柄 = 图标::从内存创建(cs.图标, 32);
			_图标大句柄 = 图标::从内存创建(cs.图标);
		}
		if (位置 & 窗口位置::居中) { 左边 = _水平居中(宽度); 顶边 = _垂直居中(高度); }

		WNDCLASSEX wndcls = { 0 };
		wndcls.cbSize = sizeof(WNDCLASSEX);
		wndcls.hbrBackground = _背景画刷;//(HBRUSH)GetStockObject(WHITE_BRUSH);
		wndcls.hInstance = g_hInst;
		wndcls.hIcon = _图标大句柄;
		wndcls.hIconSm = _图标小句柄;
		wndcls.lpfnWndProc = _WndProc;
		wndcls.lpszClassName = 类名;
		wndcls.hCursor = LoadCursorW(NULL, IDC_ARROW);
		RegisterClassExW(&wndcls);
		_原窗口过程 = (WNDPROC)DefWindowProcW;

		//父窗口==-1针对_g_interWnd（目的仅是用来搞任务栏图标隐藏但它不计入实际窗口）
		if (父窗口 && (int)父窗口 != -1) {
			this->父窗口 = (窗口*)父窗口;
			父窗口句柄 = this->父窗口->窗口句柄;
			if (模态载入)this->父窗口->禁止_();
		}
		int 窗口风格 = 0, 窗口扩展风格 = 0;
		边框_((窗口边框)边框, 窗口风格, 窗口扩展风格);

		if (cs.最小化按钮) 窗口风格 |= WS_MINIMIZEBOX;
		if (cs.最大化按钮) 窗口风格 |= WS_MAXIMIZEBOX;
		if (总在最前)窗口扩展风格 |= WS_EX_TOPMOST;
		窗口扩展风格 |= WS_EX_CONTROLPARENT;
		_窗口风格 = 窗口风格;

		if (/*在任务栏显示_ == false &&*/ _g_interWnd == 0) {
			//现在也允许子窗口那边创建隐藏控件到该_g_interWnd窗口
			_g_interWnd = new 窗口;
			_g_interWnd->初显(SW_HIDE).创建({}, (窗口*)-1);
			字体::_hdc = GetDC(_g_interWnd->窗口句柄);
			if (在任务栏显示 == false) {
				父窗口句柄 = _g_interWnd->窗口句柄;
			}
		}

		//为_g_interWnd直接使用系统内置静态组件类不走自定义注册窗口类
		if ((int)父窗口 == -1)类名 = L"Static";
		窗口句柄 = CreateWindowExW(窗口扩展风格, 类名, cs.标题,
			窗口风格,
			dpi(左边), dpi(顶边), dpi(宽度), dpi(高度),
			父窗口句柄, NULL, wndcls.hInstance, this
		);
		if (!窗口句柄) return false;

		int initShow;
		if (!可视)
			initShow = SW_HIDE;
		else if (位置 & 窗口位置::最小化)
			initShow = SW_SHOWMINIMIZED;
		else if (位置 & 窗口位置::最大化)
			initShow = SW_SHOWMAXIMIZED;
		else
			initShow = SW_SHOWNORMAL;
		ShowWindow(窗口句柄, initShow);
		if (可视 && !(位置 & 窗口位置::最小化)) {
			UpdateWindow(窗口句柄);
		}

		if ((int)父窗口 != -1) {
			_初始化Esc快捷键();
		}
		return true;
	}

	void 完毕(bool 模态) {
		事件_创建完毕();
		if (模态) {
			MSG msg;
			while (GetMessageW(&msg, NULL, 0, 0) && 窗口句柄) {
				_消息循环处理过程
			}
			if (父窗口) 父窗口->禁止_(false);
		}
		if (_g_wndCount == 0)PostQuitMessage(0);
	}

	void 总在最前_(bool is = true) {
		总在最前 = is;
		SetWindowPos(窗口句柄, is ? HWND_TOPMOST : HWND_NOTOPMOST,
			0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}

	void 置图标(HICON icon, bool 小图标 = true) {
		if (小图标) {
			_图标小句柄 = icon;
			SendMessageW(窗口句柄, WM_SETICON, ICON_SMALL, (LPARAM)_图标小句柄);
		} else {
			_图标大句柄 = icon;
			SendMessageW(窗口句柄, WM_SETICON, ICON_BIG, (LPARAM)_图标大句柄);
		}
	}

	void 置图标(int 资源ID号) {
		置图标(LoadIconW(g_hInst, (LPCTSTR)资源ID号));
	}

	void 置图标(const Bytes& zjjIcon) {
		置图标(图标::从内存创建(zjjIcon, 32), true);
		置图标(图标::从内存创建(zjjIcon), false);
	}

	void 置托盘图标(HICON icon, const StrW& 提示文本 = L"") {
		NOTIFYICONDATAW nid = { 0 };
		nid.cbSize = sizeof(NOTIFYICONDATAW);
		nid.hWnd = 窗口句柄;
		nid.uID = 1;
		
		if (icon == NULL) {
			if (_是否有托盘图标) {
				Shell_NotifyIconW(NIM_DELETE, &nid);
				_是否有托盘图标 = false;
			}
			return;
		}

		nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
		nid.uCallbackMessage = WM_APP+1; // 为托盘预留的消息
		nid.hIcon = icon;
		lstrcpynW(nid.szTip, 提示文本, ARRAYSIZE(nid.szTip));

		if (_是否有托盘图标) {
			Shell_NotifyIconW(NIM_MODIFY, &nid);
		} else {
			Shell_NotifyIconW(NIM_ADD, &nid);
			_是否有托盘图标 = true;
		}
	}

	void 置托盘图标(int 资源ID号, const StrW& 提示文本 = L"") {
		if (资源ID号 || (资源ID号==NULL&&_是否有托盘图标))
			置托盘图标(LoadIconW(g_hInst, (LPCTSTR)资源ID号), 提示文本);
	}

	void 置托盘图标(const Bytes& zjjIcon, const StrW& 提示文本 = L"") {
		置托盘图标(图标::从内存创建(zjjIcon, 32), 提示文本);
	}

	void 置前台() {
		SetForegroundWindow(窗口句柄);
	}

	void 边框_(窗口边框 边框, 可空<int&> 窗口风格 = nil, 可空<int&> 窗口扩展风格 = nil) {
		bool 用户调用 = 窗口风格 == 空;
		switch (边框)
		{
		case 窗口边框::无边框:
			窗口风格 = WS_POPUP;  //无边框
			窗口扩展风格 = 0;
			break;
		case 窗口边框::普通固定边框:
			窗口风格 = WS_CAPTION | WS_SYSMENU; //基准样式
			窗口扩展风格 = 0;
			break;
		case 窗口边框::普通可调边框:
			窗口风格 = WS_CAPTION | WS_SYSMENU | WS_THICKFRAME; //可调大小
			窗口扩展风格 = 0;
			break;
		case 窗口边框::窄标题固定边框:
			窗口风格 = WS_CAPTION | WS_SYSMENU; //固定大小，后续可通过窗口类修改标题高度d
			窗口扩展风格 = WS_EX_TOOLWINDOW;
			break;
		case 窗口边框::窄标题可调边框:
			窗口风格 = WS_CAPTION | WS_SYSMENU | WS_THICKFRAME; //可调大小，窄标题需要自定义窗口类
			窗口扩展风格 = WS_EX_TOOLWINDOW;
			break;
		}

		if (用户调用) {
			窗口_移除风格(窗口句柄, WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME);
			窗口_移除扩展风格(窗口句柄, WS_EX_TOOLWINDOW);

			窗口_添加风格(窗口句柄, 窗口风格);
			窗口_添加扩展风格(窗口句柄, 窗口扩展风格);
		}
	}

	void 位置_(窗口位置 位置) {
		int show;
		if (位置 & 窗口位置::最小化)
			show = SW_SHOWMINIMIZED;
		else if (位置 & 窗口位置::最大化)
			show = SW_SHOWMAXIMIZED;
		else
			show = SW_SHOWNORMAL;
		ShowWindow(窗口句柄, show);
		if (位置 & 窗口位置::居中) {
			移动(_水平居中(宽度), _垂直居中(高度));
		}
		this->位置 = 位置;
	}

	bool 销毁() {
		if (窗口句柄 && _窗口风格) {
			_窗口风格 = 0;
			置托盘图标(NULL);
			_wa.destroy();
			_g_wndCount--;
			容器类::销毁();
			事件_被销毁();
			if (_g_wndCount == 0) {
				PostQuitMessage(0);
			}
			return true;
		}
		return false;
	}

	/**必须在初始创建参数时采用[在任务栏显示=假]后续才可动态设置此方案
	 * @param is
	 */
	void 在任务栏显示_(bool is = true) {
		if (!_g_interWnd)return;
		if (is) {
			窗口_添加扩展风格(窗口句柄, WS_EX_APPWINDOW);
			_g_interWnd->激活(); 激活();
		} else {
			窗口_移除扩展风格(窗口句柄, WS_EX_APPWINDOW);
		}
		在任务栏显示 = is;
	}

	/**设置快捷键(已存在的同ID将覆盖)，回调方式在[事件_快捷键(cmdID)]
	 * @param 主热键 查询Windows虚拟键代码"VK_"，备注：大写字母ASCII等价于键码
	 * @param 控制键=0 可用位组合的形式拼接[控制键::]开头的常量
	 * @param cmdID<可空> 可自定义热键ID，若为空则使用MAKEWORD(主热键,仅热键)位组合
	 * @return 产生覆盖则返回假，成功插入新ID返回真
	 */
	bool 快捷键_注册(BYTE 主热键, BYTE 控制键 = 0, 可空<WORD> cmdID = 空) {
		控制键 |= 1; //FVIRTKEY默认为1
		if (cmdID == 空)cmdID = 快捷键_取ID(主热键, 控制键);
		ACCEL al = { 控制键, 主热键, cmdID };
		for (auto& it : _wa.keys)
		{
			if (it.cmd == cmdID) {
				it = al;
				return false;
			}
		}
		_wa.keys.push(al);
		_wa.update();
		return true;
	}

	/**根据快捷键ID反查注册时的主热键和控制键
	 * @param cmdID
	 * @param 主热键<可空> 主热键
	 * @param 控制键<可空> 控制键
	 * @param 是否自定义ID
	 * @return 如果是自定义ID找到返回真找不到返回假，非自定义ID总是返回真
	 */
	bool 快捷键_ID反取(WORD cmdID, 可空<BYTE&> 主热键, 可空<BYTE&> 控制键 = 空,
		bool 是否自定义ID = false) {
		if (是否自定义ID) {
			for (auto& it : _wa.keys)
			{
				if (it.cmd == cmdID) {
					主热键 = (BYTE)it.key;
					控制键 = it.fVirt;
					return true;
				}
			}
			return false;
		}
		主热键 = LOBYTE(cmdID);
		控制键 = HIBYTE(cmdID);
		return true;
	}

	/**从已有的快捷键查询cmdID（含两种查询方式）
	 * @param 主热键
	 * @param 控制键=控制键::主热键
	 * @param 是否自定义ID=false 若为自定义ID查询失败返回0成功返回本窗口注册列表中的cmdID
	 * @return
	 */
	WORD 快捷键_取ID(BYTE 主热键, BYTE 控制键 = 0, bool 是否自定义ID = false) {
		控制键 |= 1;
		if (是否自定义ID) {
			for (auto& it : _wa.keys)
			{
				if (it.key == 主热键 && it.fVirt == 控制键) {
					return true;
				}
			}
			return 0;
		}
		return MAKEWORD(主热键, 控制键);
	}

	/**调整控件组的Tab顺序 只需停留顺序调整Z顺序,由Win32内置的Dialog方案自动实现
	 * @param controls<可空>=nil 无参调用时内部会自动枚举子窗口
	 */
	void 调整控件组的Tab顺序(可空<Arraybe<控件类*>&> controls = nil) {
		Arraybe<控件类*> _temp_controls;
		if (controls == nil) {
			Arraybe<窗口基类*> obs = 窗口_枚举子窗口对象(窗口句柄, true);
			for (int i = 0; i < obs.count; i++) {
				_temp_controls.push((控件类*)obs[i]);
			}
		}
		Arraybe<控件类*>& refControls = (controls == nil) ? _temp_controls : (Arraybe<控件类*>&)controls;

		if (refControls.count == 0) return;
		qsort(&refControls[0], refControls.count, sizeof(控件类*), __比较停留顺序);

		//让停留顺序最小的控件获得焦点
		if (IsWindow(refControls[0]->窗口句柄) && IsWindowEnabled(refControls[0]->窗口句柄) &&
			IsWindowVisible(refControls[0]->窗口句柄))
		{
			SetFocus(refControls[0]->窗口句柄);
		}

		//调整 Z 顺序
		for (int i = 0; i < refControls.count; i++)
		{
			HWND hAfter = (i == 0) ? HWND_BOTTOM : refControls[i - 1]->窗口句柄;
			SetWindowPos(
				refControls[i]->窗口句柄,
				hAfter,
				0, 0, 0, 0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE
			);
		}
	}


	virtual LRESULT 挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	WindowAccel _wa;
	char _初始位置 = -1;

	// 静态窗口过程
	static LRESULT CALLBACK _WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
		窗口* pThis;
		if (msg == WM_NCCREATE) {
			CREATESTRUCT* pcs = (CREATESTRUCT*)lp;
			pThis = (窗口*)pcs->lpCreateParams;
			pThis->窗口句柄 = hwnd;
			_g_wndCount++;
			pThis->_是否为窗口类对象 = true;
			SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
		} else {
			pThis = (窗口*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		}
		if (pThis) {
			return pThis->挂接消息(hwnd, msg, wp, lp);
		}
		return DefWindowProc(hwnd, msg, wp, lp);
	}

	void _初始化Esc快捷键() {
		Arraybe<ACCEL> keys;
		keys.push({ FVIRTKEY, VK_ESCAPE, 0 }); //按ESC键退出
		_wa.set(窗口句柄, keys);
	}

	static int __比较停留顺序(const void* a, const void* b) {
		const 控件类* ca = *(const 控件类**)a;
		const 控件类* cb = *(const 控件类**)b;

		if (ca->停留顺序_ < cb->停留顺序_) return -1;
		else if (ca->停留顺序_ > cb->停留顺序_) return 1;
		return 0;
	}

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
	virtual void 载入(窗口* 父窗 = 0, bool 模态 = 0) {}
	virtual void 事件_创建完毕() {}
	virtual void 事件_首次激活() {}
	virtual void 事件_创建本体() {}
	virtual bool 事件_被关闭() { return true; }
	virtual void 事件_被销毁() {}
	virtual void 事件_快捷键(WORD cmdID) {}
	virtual void 事件_菜单项被单击(int 菜单ID) {}
	/**事件_托盘
	 * @param 操作类型 1、#单击左键；2、#双击；3、#单击右键。
	 */
	virtual void 事件_托盘(int 操作类型) {}
	virtual LRESULT 事件_反馈(WPARAM wParam, LPARAM lParam) { return 0; }
	virtual void 事件_尺寸被改变() {}

	bool 通用事件_背景绘制(HDC hdc)
	{
		FillRect(hdc, &SIZEToRECT(取用户区大小<D>()), _背景画刷);
		return false;
	}
};


LRESULT 窗口::挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_SIZE: {
		事件_尺寸被改变();
		InvalidateRect(hwnd, NULL, TRUE);
		break;
	}
	case WM_CREATE:
		事件_创建本体();
		break;
	case WM_ACTIVATE:
		if (LOWORD(wParam) != WA_INACTIVE) {
			if (!_首次激活已触发) {
				_首次激活已触发 = true;
				PostMessageW(hwnd, WM_APP + 2, 0, 0);
			}
		}
		break;
	case WM_APP + 2:
		事件_首次激活();
		break;
	case WM_CLOSE:
		if (事件_被关闭() == false) {
			return 0;
		}
		if (父窗口)父窗口->激活();
	case WM_DESTROY:
		销毁();
		break;
	case WM_LBUTTONDOWN:
		if (随意移动_) {
			ReleaseCapture();
			发送消息(WM_NCLBUTTONDOWN, HTCAPTION, 0);
		}
		break;
	case WM_COMMAND:
		if (lParam == 0) {
			if (HIWORD(wParam) == 0) { // 菜单命令
				事件_菜单项被单击((int)LOWORD(wParam));
			} else if (HIWORD(wParam) == 1) { // 快捷键命令
				if (Esc键关闭_ && LOWORD(wParam) == 0) { // Esc 是初始快捷键，cmdID 为 0
					PostMessageW(hwnd, WM_CLOSE, 0, 0);
				}
				事件_快捷键(LOWORD(wParam));
			}
		}
		break;
	case WM_NCACTIVATE:
		if (保持标题栏点燃_)wParam = true;
		break;
	case WM_APP + 1: // 托盘图标事件预留消息
		switch (lParam) {
		case WM_LBUTTONUP:
			事件_托盘(1);
			break;
		case WM_LBUTTONDBLCLK:
			事件_托盘(2);
			break;
		case WM_RBUTTONUP:
			事件_托盘(3);
			break;
		}
		break;
	case 32885: //主窗口也拥有消息反馈事件（仿易语言）
		return 事件_反馈(wParam, lParam);
	}
	return 容器类::挂接消息(hwnd, msg, wParam, lParam);
}

int Win32消息循环()
{
	if (_g_wndCount==0)return 0;
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		_消息循环处理过程
	}
	return (int)msg.wParam;
}

#pragma region BE_IGNORE
//PeekMessage 在拉取并移除 WM_QUIT 消息时，确实已经自动从队列中获取到了它，但接下来的分发环节（DispatchMessage）无法自动处理它：
//
//DispatchMessage 路由机制： DispatchMessage(&msg) 的作用是将消息分发给特定窗口的窗口回调函数（WndProc）。
//但它是通过判断 msg.hwnd（目标窗口句柄）来决定发给谁的。
//WM_QUIT 没有窗口句柄： WM_QUIT 信号是针对整个线程/进程的，所以它的 msg.hwnd 恒为 NULL。
//DispatchMessage 会直接忽略它： 由于没有对应的窗口，当 DispatchMessage 收到 WM_QUIT 时，它会直接忽略，什么都不做。
//为什么内层循环必须手动处理并重新发送？
//在标准的 Windows 编程中，所有消息循环在拉取到消息后，都必须自己主动检查 msg.message == WM_QUIT 并退出循环。
//
//外层主循环是通过 GetMessage 的返回值来判断的：当 GetMessage 收到 WM_QUIT 时会返回 0，从而自然退出主循环。
//内层消息循环（如 处理事件() 中的 PeekMessage）： 如果内层循环拉取到了 WM_QUIT，
//这个消息就会从线程消息队列中被移除（因为设置了 PM_REMOVE 标志，即参数 1）。 
//如果内层循环不主动检查它，也不通过 PostQuitMessage 把它重新扔回队列，
//那么当内层循环结束、控制权交还给外层的 GetMessage 时，队列里就再也没有 WM_QUIT 消息了（程序无法退出）。
#pragma endregion

void 处理事件()
{
	MSG msg;
	while (PeekMessage(&msg, 0, 0, 0, 1))
	{
		if (msg.message == WM_QUIT) {
			PostQuitMessage((int)msg.wParam);
			break;
		}
		_消息循环处理过程;
	}
}
