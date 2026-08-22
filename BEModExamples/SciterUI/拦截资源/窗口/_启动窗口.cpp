#include <BEMod.h>

struct __启动窗口 : 窗口
{
	void 事件_创建本体()
	{
		SciterUI::全局初始化();
		SciterUI::参数 cs;
		cs.内存_zip = 读入文件("test_res.zip");
		st.创建(cs, this);
	}

	void 事件_创建完毕()
	{
		调试输出("你好啊白易语言");
	}

#pragma region 组件成员
	struct _st : SciterUI {

		//回调事件里边的URL是以W版为主，而pld->outData取决于原始的网页数据（本质上是字节集）
		UINT 事件_资源加载(LPSCN_LOAD_DATA pld) override{
			StrW uri(pld->uri);
			dbg_print("Intercept callback uri = ", uri);

			// 寻找并拦截名为 "my_script.js" 的资源请求
			if (文本_对比右边<W>(uri, L"my_script.js")) {
				StrA zipScript = _BSA(pld->outData, pld->outDataSize);

				// 在 ZIP 原有脚本的基础上追加逻辑并直接通过成员函数同步返回，无需使用静态变量
				StrA finalScript = zipScript +
					"document.querySelector('#intercept-status').innerHTML += ' "
					+ "<span style = \"color: red;\">且被 C++ 拦截器追加修改！</span>';";

				return 资源返回(pld, finalScript);
			}

			return LOAD_OK;
		}
	} st;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
} _启动窗口;