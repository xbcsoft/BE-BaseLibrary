#include <BEMod.h>
#include "../SciterUI.h"

struct __启动窗口 : 窗口
{
	void 事件_创建完毕()
	{
		
	}

#pragma region 组件成员
	struct _st : SciterUI {

		//回调事件里边的URL是以W版为主，而pld->outData取决于原始的网页数据（本质上是字节集）
		UINT 事件_资源加载(LPSCN_LOAD_DATA pld) {
			StrW uri(pld->uri);
			dbg_print("Intercept callback uri = ", uri);

			// 寻找并拦截名为 "my_script.js" 的资源请求
			if (文本_对比右边<W>(uri, L"my_script.js")) {
				StrA zipScript = _BSA(pld->outData, pld->outDataSize);

				// 在 ZIP 原有脚本的基础上追加逻辑并直接通过成员函数同步返回，无需使用静态变量
				StrA finalScript = zipScript +
					"document.querySelector('#intercept-status').innerHTML += ' + <span style=\"color: red;\">且被 C++ 拦截器追加修改！</span>';";

				return 资源返回(pld, finalScript);
			}

			return LOAD_OK;
		}
	} st;

	struct _按钮1 : 按钮 {
		void 事件_被单击() {
			调试输出("原生白易按钮被单击");
			_启动窗口.编辑框1.内容_(L"原生按钮已被点击！");
		}
	} 按钮1;

	struct _编辑框1 : 编辑框 {
		void 事件_内容被改变() {
			调试输出("原生编辑框内容改变：", 内容);
		}
	} 编辑框1;

	struct _选择框1 : 选择框 {
		void 事件_被单击() {
			调试输出("原生选择框状态为：", 选中);
		}
	} 选择框1;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
} _启动窗口;
