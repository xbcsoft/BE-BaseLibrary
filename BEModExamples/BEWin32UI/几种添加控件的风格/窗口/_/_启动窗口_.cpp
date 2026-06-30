#include "../_启动窗口.h"

void __启动窗口::载入(窗口* 父窗, bool 模态)
{
	if (窗口句柄)return;
	窗口::创建({});

	静态标签1.创建({ 10, 10, 50, 30 }, this);

	{
		按钮::参数 cs(60, 10, 50, 30);
		cs.标题 = L"按钮1";
		静态按钮1.创建(cs, this);
	}

	{
		按钮::参数 cs(60+50, 10, 50, 30);
		cs.标题 = L"按钮2";
		静态按钮2.创建(cs, this);
	}

	__启动窗口::完毕(模态);
}


//这里主要完成用户自己的后期动态创建
void __启动窗口::完毕(bool 模态)
{
	//为了不污染定义出的"_"类，可用块作用域括起来
	{
		struct _ : 按钮 {
			void 事件_被单击() {
				_启动窗口.发送消息(WM_反馈, 123, 22);
			}
		}*btn1; btn1 = new _;
		btn1->创建({ 10, 60, 50, 30 }, this);
	}

	//也可以用下面的形式

	按钮* btn2;
	{
		struct _ : 按钮 {
			void 事件_被单击() {
				_启动窗口.发送消息(WM_反馈, 123, 33);
			}
		}; btn2 = new _;
		btn2->创建(按钮::参数(60, 60, 50, 30), this);
	}

	//还可用下面lambda的形式
	auto btn3 = [] {struct _ :按钮 {
		void 事件_被单击() {
			_启动窗口.发送消息(WM_反馈, 123, 44);
		}
	}; return new _; }();
	btn3->创建(按钮::参数(120, 60, 50, 30), this);


	/*【自定义动态按钮类，且可以后期动态绑定事件】
	  下面所做的事情就是希望把启动窗口的this传递到按钮回调内部来实现委托
	*/
	class 按钮_动态 : public 按钮 {
	public:
		// 传统方案真是一坨...
		typedef void(*RawCallback)(按钮_动态* sender, void* user_data);
		RawCallback m_legacy_callback = nullptr;
		void* m_user_data = nullptr;
		void 绑定_被单击_传统回调(RawCallback callback, void* data = nullptr) {
			m_legacy_callback = callback;
			m_user_data = data;
		}

		// 现代方案直接一行定义即可！
		be::function<按钮_动态&> 绑定_被单击_现代回调;

		void 事件_被单击() override {
			绑定_被单击_现代回调(*this); //无需自行判断，内部会自动判断即便为空

			if (m_legacy_callback) { //若不手动判断当m_legacy_callback为空时必蹦
				m_legacy_callback(this, m_user_data);
			}

			按钮::事件_被单击();
		}
	};


	// 第四种方式：使用 按钮_动态 类 搞事件委托(传统 全局函数指针 风格)
	按钮_动态* btn4 = new 按钮_动态();
	btn4->创建(按钮::参数(10, 110, 120, 30), this);
	btn4->绑定_被单击_传统回调([](按钮_动态* sender, void* data) {
		// 传统方案: 需要手动传递赋值this
		__启动窗口* pThis = (__启动窗口*)data;
		pThis->发送消息(WM_反馈, 123, 88);
		sender->标题_(L"Raw Callback Triggered");
	}, this);
	//原因是在 C++ 中，Lambda 有一条铁律：
	//只有不捕获任何变量的 Lambda（即 []），才能退化成原始函数指针（Raw Function Pointer）。


	// 第五种方式：使用 按钮_动态 类 搞事件委托(现代 std/be::function 风格)
	按钮_动态* btn5 = new 按钮_动态();
	btn5->创建(按钮::参数(10, 150, 120, 30), this);
	btn5->绑定_被单击_现代回调 = [this](按钮_动态& sender) {
		// 直接使用lambda捕获this功能并闭包在 绑定_被单击_现代回调 成员属性中
		this->发送消息(WM_反馈, 123, 99);
		sender.标题_(L"be::function Triggered");
	};

	窗口::完毕(模态);
}
