#include "窗口基类.h"
class 窗口;

//1.该容器类机制意义在于保存动态内存申请的控件让其父容器销毁时自动将它们安全释放内存
//注意：容器类不一定采用Win32原生父子窗口之间的关系（也就是不一定将子窗口进行置父）
//2.以及接管一些Win32控件它必须由父容器来进行处理的消息由白易再给它反射转发回去
class EXP 容器类 :public 窗口基类
{
public:
	struct 参数 {
		float 左边;
		float 顶边;
		float 宽度;
		float 高度;
		bool 可视 = true;
		bool 禁止 = false;
	};

	Arraybe<组件基类*> _组件列表;

	~容器类() {
		销毁(true);
	}

	bool 销毁(bool 同步 = false) {
		if (!窗口句柄) return false;
		if (!同步) {
			PostMessageW(窗口句柄, WM_APP, 0, 0);
			return true;
		}
		if (_组件列表.count) {
			for (auto& it : _组件列表)
			{
				it->销毁(true);
				if (IsNotModuleAddr(it)) {
					delete it;
				}
			}
			_组件列表.clear();
		}
		return 窗口基类::销毁(true);
	}

	virtual LRESULT 挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		switch (msg) {
		case WM_COMMAND:
		case WM_CTLCOLORBTN:
		case WM_CTLCOLOREDIT:
		case WM_CTLCOLORLISTBOX:
		case WM_CTLCOLORMSGBOX:
		case WM_CTLCOLORSCROLLBAR:
		case WM_CTLCOLORDLG:
		case WM_HSCROLL:
		case WM_VSCROLL:
		{
			HWND hCtrl = (HWND)lParam;
			if (hCtrl) {
				LRESULT res = SendMessageW(hCtrl, BE_REFLECT + msg, wParam, lParam);
				if (res) return res;
			}
			break;
		}
		case WM_CTLCOLORSTATIC:
		{
			HWND hCtrl = (HWND)lParam;
			if (hCtrl) {
				return SendMessageW(hCtrl, BE_REFLECT + msg, wParam, lParam);
			}
		}
		case WM_NOTIFY:
		{
			NMHDR* hdr = (NMHDR*)lParam;
			if (hdr && hdr->hwndFrom)
				return SendMessageW(hdr->hwndFrom, BE_REFLECT + msg, wParam, lParam);
			break;
		}
		case WM_DRAWITEM:
		case WM_MEASUREITEM:
		case WM_COMPAREITEM:
		case WM_DELETEITEM: {
			if (wParam != 0) {
				HWND hCtrl = GetDlgItem(hwnd, (int)wParam);
				if (hCtrl) {
					return SendMessageW(hCtrl, BE_REFLECT + msg, wParam, lParam);
				}
			}
			break;
		}
		case WM_PRINTCLIENT: //这个是背景透明的关键
		{
			// 我们通过子控件在 WM_PAINT 时挂在父容器上的属性来精确得知是谁在索要背景！
			HWND hChild = (HWND)GetPropW(hwnd, L"BE_PAINT_CLIENT");
			if (hChild) {
				//问一下子控件自己要不要接管
				LRESULT res = SendMessageW(hChild, BE_REFLECT + WM_PRINTCLIENT, wParam, lParam);
				if (res==1)return 1; //若返回1则直接阻止
				//使用在父容器绘制子窗口所需的DC(wParam)来搞背景透明
				if (res==-1)return SendMessageW(hwnd, WM_ERASEBKGND, wParam, 0);

				//默认情况由Windows原生绘制背景
				//注意：主窗口的原过程就是DefWindowProc，而子容器(外形框等)底层是原生Static，
				//Static自带的WM_PRINTCLIENT会去取自己父窗口的WM_CTLCOLORSTATIC画刷(默认白色)铺满子控件区域，
				//导致不透明的特殊子控件(如单选框/选择框)的背景变白。这里让Static容器也走DefWindowProc，与主窗口行为保持一致。
				WCHAR szCls[16] = {}; GetClassNameW(hwnd, szCls, 16);
				if (wcscmp(szCls, L"Static") == 0)
					return DefWindowProcW(hwnd, msg, wParam, lParam);
			}
		}
		}
		return 窗口基类::挂接消息(hwnd, msg, wParam, lParam);
	}
	operator 窗口*() {
		return (窗口*)this;
	}

protected:
	//继承自容器类的容器应当实现【背景颜色】（不过目前已经取消这一设定，放在窗口基类中了默认是背景黑色）
	//virtual 颜色 _取背景颜色(颜色** out = 0) = 0;
	//virtual void 背景颜色_(颜色 cr, bool 是否重画 = true) = 0;

	//WS_CLIPCHILDREN，当父容器绘制时排除所有子窗口的区域，不在子窗口所在区域绘制内容（避免父容器的重绘覆盖子窗口）
	//WS_CLIPSIBLINGS，当同级（同父容器）窗口重叠时，当前窗口的绘制区域不包含同级兄弟窗口区域（避免兄弟窗口重叠时的绘制冲突）
	bool _子初始(const 参数& cs, 容器类* 父容器, HWND 父句柄) {
		//有一种场景，让当前子组件的生命周期由实例父容器掌管，而窗口显示的置父却可以在别的窗口里
		父窗口句柄 = 父句柄 ? 父句柄 : ((父容器) ? 父容器->窗口句柄 : 0);
		if (父窗口句柄 == 0)return false;
		this->父容器 = 父容器;
		左边 = cs.左边;
		顶边 = cs.顶边;
		宽度 = cs.宽度;
		高度 = cs.高度;
		可视 = cs.可视;
		_窗口风格 = WS_CHILD;// | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
		if (可视)_窗口风格 |= WS_VISIBLE;
		组件基类::_子入父列表();
		return true;
	}
};

void 窗口基类::窗口置父(容器类* 新父容器, bool 加入父容器列表)
{
	if (!新父容器) return;
	if (加入父容器列表) {
		// 独立创建的动态控件可能尚无逻辑父容器；这种情况下直接加入新父即可。
		// 若已有旧父，则先从旧父生命周期列表中摘除，避免后续被旧父重复销毁。
		if (父容器 && 父容器 != 新父容器) {
			数组_删除元素值成员(父容器->_组件列表, this);
		}
		if (父容器 != 新父容器) {
			新父容器->_组件列表.push(this);
		}
	}
	父容器 = 新父容器;
	窗口置父(新父容器->窗口句柄);
}

void 组件基类::_子入父列表(bool 静态组件不入父)
{
	if (父容器 && (!静态组件不入父 || IsNotModuleAddr(this))) {
		父容器->_组件列表.push(this);
	}
}
