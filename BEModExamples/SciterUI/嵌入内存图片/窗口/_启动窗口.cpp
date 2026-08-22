#include <BEMod.h>

struct __启动窗口 : 窗口
{
	SciterDom dom;

	void 事件_创建完毕()
	{
		调试输出("你好啊白易语言");

		dom = st.取文档模型();

		// C++ 端直接将 R::apple 内存图片字节集作为参数调用 JS 函数
		dom.调用JS函数("设置背景字节集", { ".bg-box", R::apple });
	}

#pragma region 组件成员
	struct _st : SciterUI {
		UINT 事件_资源加载(LPSCN_LOAD_DATA pld)
		{
			StrW uri(pld->uri);
			// 拦截匹配 apple.png 资源请求，注入内存中的图片数据
			if (文本_对比右边<W>(uri, L"apple.png")) {
				return 资源返回(pld, R::apple);
			}
			return LOAD_OK;
		}
	} st;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
} _启动窗口;