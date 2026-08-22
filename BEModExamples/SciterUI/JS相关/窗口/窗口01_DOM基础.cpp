#include <BEMod.h>

struct _窗口01_DOM基础 : 窗口
{
	SciterDom dom;
	void 事件_创建完毕()
	{
		标题_(L"DOM基础操作");
		dom = st.取文档模型();

		//DOM对象返回再投入();
		子iframe内执行JS();
	}

	void 子iframe内执行JS()
	{
		// 1. 关于URL跳转问题
		//dom.执行JS脚本("location.href='test_iframe.html'"); //这个用不了，必须下面的！
		dom.执行JS脚本("Window.this.load(document.url('test_iframe.html'));");
		//或下面的做法（必须要写绝对路径！）
		//SciterLoadFile(窗口句柄, LR"(D:\MyDocument\SciterUI\JS相关\out\test_iframe.html)");

		// 2. 用上面跳转后必须重新获取新的文档模型，才能后续操作
		dom = st.取文档模型();
		
		dom.等待文档加载完毕([this](c_StrU8 url) {
			//如果一个主页面有多个子页面url，那么这里回调会多次调用（只能说是Sciter的问题）
			调试输出(路径_取文件名(url));

			//可根据不同时机再进行子过滤
			if (文本_对比右边(url, "test_iframe.html")) {
				//第一个iframe也可以单独监听（用元素对象中的方法）
				SciterObj ifmElem1 = dom.执行JS脚本("document.$('iframe')"); //相当于document.querySelector()
				//注意下面这里必须按C++值捕获ifmElem1，否则出作用域生命周期周期结束
				ifmElem1._等待文档加载完毕([ifmElem1](c_StrU8 url) {
					SciterDom idom = ifmElem1.仅框架元素取子文档模型();
					调试输出("子页面标题", idom.取页面标题());
					return false; //返回false后续停止监听
				});
			}

			//注意下面这里我们仅获取第二个iframe元素
			SciterObj ifmElem2 = dom.执行JS脚本("document.querySelectorAll('iframe')[1]");
			SciterDom idom = ifmElem2.仅框架元素取子文档模型();
			idom.执行JS脚本("document.body.innerText='23333333'");
			//写到这其实只会两次重复改第二个iframe的页面内容，第一个iframe则没有改动

			return true; //返回true表示继续监听
		});
	}

	void DOM对象返回再投入()
	{
		// 1. 在 JS 引擎中注入函数：创建 div 插入 body 并返回该 DOM 元素
		dom.执行JS脚本(R"(
			document.body.innerHTML = '';
			function createAndAppendElement() {
				var el = document.createElement('div');
				el.innerText = 'hello';
				document.body.appendChild(el);
				return el;
			}
		)");

		// 2. C++ 调用 JS 函数获取返回的 DOM 元素对象（第 2 参数为传参 nil，第 3 参数接收返回值结果）
		SciterObj returnedEl = dom.调用JS函数("createAndAppendElement");

		调试输出("【C++ 接收到返回的 DOM 元素】 = ", returnedEl, returnedEl._取元素句柄());

		// 3. 在 C++ 侧直接对接收到的 DOM 元素设置 innerHTML 属性
		returnedEl.置属性("innerHTML", "<h2 style='color: red;'>Hello C++ 直接置属性!</h2>");
	}

#pragma region 组件成员
	struct _st :SciterUI {
	} st;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
} 窗口01_DOM基础;