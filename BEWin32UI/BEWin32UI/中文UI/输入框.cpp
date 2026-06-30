/*
 * 弹出式输入框组件 (白易 Win32 UI)
 *
 * 避坑说明 / 内存错误防范：
 * 1. 本窗口的子控件（标签、编辑框、按钮等）声明为了窗口类的成员变量，而非指针，但父窗口本身是动态内存的指针（所以引出下面的条例2）
 * 2. 在载入完毕后，必须调用 `_组件列表.clear();` 清除组件列表，以防止框架的容器类在析构时对这些属于成员变量的子控件调用 `delete`（因为它们是栈/对象内部的成员，由 C++ 自动管理生命周期，手动 delete 会引发崩溃）。
 * 3. 虽然子控件的生命周期跟随父窗口，但由于父窗口作为局部变量运行在模态循环中，同步 `销毁()` 会导致模态循环立刻退出，局部变量随之超出作用域而被析构（进而销毁所有子控件成员）。此时因为子控件事件方法尚未从调用栈返回，仍然会引发 use-after-free 崩溃。
 * 4. 因此，不论是动态指针子控件还是成员变量子控件（只要其父窗口是动态内存创建），在子控件事件中销毁父窗口时，都必须使用 `p窗口->销毁异步();`。
 */
#include "窗口.h"
#include "编辑框.h"
#include "按钮.h"
#include "标签.h"

namespace {

// 确认按钮类，处理确认逻辑
struct 确认按钮类 : 按钮
{
	StrW* p输入结果;
	编辑框* p输入编辑框;
	bool* p确认Clicked;
	窗口* p窗口;

	virtual void 事件_被单击() override
	{
		if (p输入编辑框 && p输入结果) {
			*p输入结果 = p输入编辑框->_取内容();
		}
		if (p确认Clicked) {
			*p确认Clicked = true;
		}
		if (p窗口) {
			p窗口->销毁异步();
		}
	}
};

// 取消按钮类，处理取消逻辑
struct 取消按钮类 : 按钮
{
	bool* p确认Clicked;
	窗口* p窗口;

	virtual void 事件_被单击() override
	{
		if (p确认Clicked) {
			*p确认Clicked = false;
		}
		if (p窗口) {
			p窗口->销毁异步();
		}
	}
};

// 输入框窗口类
class 输入框窗口 : public 窗口
{
public:
	StrW 提示信息;
	StrW 初始文本;
	StrW 输入结果;
	bool 确认Clicked = false;

	标签 _提示框标签;
	标签 _提示文本标签;
	编辑框 _输入编辑框;
	确认按钮类 _确认按钮;
	取消按钮类 _取消按钮;

	void 事件_创建本体()
	{
		// 添加 WS_EX_DLGMODALFRAME 扩展风格，此风格不带标题栏图标的核心样式
		窗口_添加扩展风格(窗口句柄, WS_EX_DLGMODALFRAME);
	}

	LRESULT 挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override
	{
		if (msg == WM_CREATE) {
		}
		return 窗口::挂接消息(hwnd, msg, wParam, lParam);
	}


	void 载入(窗口* 父窗 = 0, bool 模态 = 0) override
	{
		if (窗口句柄) return;

		窗口::参数 cs;
		if (标题 == L"") {
			cs.标题 = L"请输入：";
		} else {
			cs.标题 = 标题;
		}
		cs.宽度 = 380;
		cs.高度 = 175;
		cs.边框 = 窗口边框::普通固定边框;
		cs.控制按钮 = true;
		cs.最小化按钮 = false;
		cs.最大化按钮 = false;
		cs.Esc键关闭 = true;
		cs.位置 = 窗口位置::居中;
		cs.子控件焦点导航 = true; // 启用子控件焦点导航以支持 Tab 键
		cs.图标ID = 0;           // 设置为0使LoadImage加载失败，从而让大/小图标句柄保持为NULL
		cs.类名 = L"BEInputBoxWindow"; // 注册独立且无图标的窗口类，使标题栏不置备/不显示图标

		窗口::创建(cs, 父窗, 模态);

		// 注册回车键快捷键以自动触发确认
		快捷键_注册(VK_RETURN);

		// 1. 提示框容器标签 (只提供浅凹入式边框背景)
		标签::参数 cs_frame;
		cs_frame.左边 = 12;
		cs_frame.顶边 = 12;
		cs_frame.宽度 = 340;
		cs_frame.高度 = 48;
		cs_frame.标题 = L"";
		cs_frame.边框 = 标签边框::浅凹入式;
		cs_frame.是否自动折行 = false;
		_提示框标签.创建(cs_frame, this);

		// 2. 提示文本标签 (提供内部的文本显示及内边距间距)
		标签::参数 cs_prompt;
		cs_prompt.左边 = 18; // 12 + 6 像素左内边距
		cs_prompt.顶边 = 17; // 12 + 5 像素顶内边距
		cs_prompt.宽度 = 328; // 340 - 12 像素宽度
		cs_prompt.高度 = 38; // 48 - 10 像素高度
		cs_prompt.标题 = 提示信息;
		cs_prompt.边框 = 标签边框::无边框;
		cs_prompt.是否自动折行 = true;
		_提示文本标签.创建(cs_prompt, this);

		// 2. 输入编辑框 (用户输入框)
		编辑框::参数 cs_input;
		cs_input.左边 = 12;
		cs_input.顶边 = 68;
		cs_input.宽度 = 340;
		cs_input.高度 = 18;
		cs_input.内容 = 初始文本;
		cs_input.输入方式 = 编辑框输入方式::通常方式;
		cs_input.是否允许多行 = false;
		_输入编辑框.创建(cs_input, this);

		// 3. 确认按钮
		_确认按钮.p输入结果 = &输入结果;
		_确认按钮.p输入编辑框 = &_输入编辑框;
		_确认按钮.p确认Clicked = &确认Clicked;
		_确认按钮.p窗口 = this;

		按钮::参数 cs_ok;
		cs_ok.左边 = 87;
		cs_ok.顶边 = 100;
		cs_ok.宽度 = 85;
		cs_ok.高度 = 28;
		cs_ok.标题 = L"确认输入(&O)";
		_确认按钮.创建(cs_ok, this);
		// 设置为默认按钮样式，配合子控件焦点导航可在按回车时自动触发
		窗口_添加风格(_确认按钮.窗口句柄, BS_DEFPUSHBUTTON);

		// 4. 取消按钮
		_取消按钮.p确认Clicked = &确认Clicked;
		_取消按钮.p窗口 = this;

		按钮::参数 cs_cancel;
		cs_cancel.左边 = 192;
		cs_cancel.顶边 = 100;
		cs_cancel.宽度 = 85;
		cs_cancel.高度 = 28;
		cs_cancel.标题 = L"取消(&C)";
		_取消按钮.创建(cs_cancel, this);

		// 清除组件列表防止重复释放（详见文件开头备注）
		_组件列表.clear();
		//当然你也可以不必使用参数二this投入，而改为参数三的窗口句柄版，这样也不需要在此处clear()

		窗口::完毕(模态);
	}

	virtual void 事件_创建完毕() override
	{
		_输入编辑框.获取焦点();
		// 选中所有文本，方便用户直接修改
		_输入编辑框.置被选择字符数(-1, 0);
	}

	virtual void 事件_快捷键(WORD cmdID) override
	{
		if (cmdID == 快捷键_取ID(VK_RETURN)) {
			_确认按钮.事件_被单击();
		}
	}
};

} // namespace

// 弹出输入框函数
bool 输入框(c_AutoStr 提示, c_AutoStr 标题, c_AutoStr 初始文本, StrW& 存放变量)
{
	输入框窗口 w;
	w.提示信息 = (const charW*)提示;
	w.初始文本 = (const charW*)初始文本;
	w.标题 = (const charW*)标题;

	窗口* parent = (窗口*)窗口_句柄取对象(GetActiveWindow());
	w.载入(parent, true);

	if (w.确认Clicked) {
		存放变量 = w.输入结果;
		return true;
	}
	return false;
}