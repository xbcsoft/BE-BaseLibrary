#pragma once
#include "stdafx.h"
#include "SciterObj.h"
class SciterUI;

class SciterDom
{
public:
	HELEMENT ctx = nullptr;
	SciterUI* _st = nullptr;
	SciterDom();

	void 嵌入原生控件(c_StrX 选择器, HWND childHwnd);

	typedef be::function<SciterObj(SciterObj&)> JS函数回调;

	bool 注入JS函数(c_StrU8 函数名, JS函数回调 回调);

	/**获取当前文档的完整 URL 地址（如 file:///D:/.../index.html）
	 * @return 当前页面的 URL 字符串
	 */
	StrU8 取页面地址();

	StrU8 取页面标题();
	void 置页面标题(c_StrU8 标题, c_StrU8 标题元素选择器 = "titlewin");


	void 浏览(c_StrU8 路径);

	SciterObj 执行JS脚本(c_StrU8 js代码);

	bool 执行JS脚本_(
		c_StrU8 js代码,
		可空<SciterObj&> 结果 = 空, 可空<StrU8&> 错误文本 = 空);


	/**调用JS函数
	 * @param 全局函数名
	 * @param 参数 只有一个参数直接投入
	 * @return
	 */
	SciterObj 调用JS函数(c_StrU8 全局函数名, const SciterObj& 单个参数);

	/**调用JS函数(多参版)
	 * @param 全局函数名
	 * @param 参数 示例投入{123,"hello",obj}
	 * @return
	 */
	SciterObj 调用JS函数(c_StrU8 全局函数名, std::initializer_list<r_SciterObj> 参数组);

	/**调用JS函数(无参版)
	 * @param 全局函数名
	 * @param <可空> 投入空时等价于投入0个参数（注意并非投入第一个参数为null）
	 * @param 结果<可空>
	 * @param 错误文本<可空>
	 * @return
	 */
	SciterObj 调用JS函数(c_StrU8 全局函数名);

	bool 调用JS函数_(
		c_StrU8 全局函数名,
		std::initializer_list<r_SciterObj> 参数组,
		可空<SciterObj&> 结果 = 空, 可空<StrU8&> 错误文本 = 空);

	/**调用JS函数(单参版)
	 * @param 全局函数名
	 * @param 单个参数
	 * @param 结果<可空>
	 * @param 错误文本<可空>
	 * @return
	 */
	bool 调用JS函数_(
		c_StrU8 全局函数名,
		const SciterObj& 单个参数,
		可空<SciterObj&> 结果 = 空, 可空<StrU8&> 错误文本 = 空);

	bool 调用JS函数_(
		c_StrU8 全局函数名,
		Nil 零个参数 = 空,
		可空<SciterObj&> 结果 = 空, 可空<StrU8&> 错误文本 = 空);

	/**等待文档加载完毕
	 * @param dom
	 * @param 回调
	 * @param 事件标识 默认为DOCUMENT_COMPLETE(页面css/js都加载完毕,注意不包括iframe)，DOCUMENT_READY(仅文档结构完毕)
	 */
	void 等待文档加载完毕(be::function<bool(c_StrU8)> 回调, UINT 事件标识 = DOCUMENT_COMPLETE);

	Bytes 保存截图(可空<SciterObj&> 目标元素 = 空, bool 截取完整滚动页面 = false);
private:

	SCDOM_RESULT _调用临时脚本函数(
		c_StrW functionExpression,
		const VALUE* argv,
		UINT argc,
		VALUE* result);

	static void _设置错误(
		可空<StrU8&> 错误文本, c_StrU8 text);
};
