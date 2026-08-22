#include "stdafx.h"
#include "SciterObj.h"

namespace {
struct _DocCompleteCtx {
	be::function<bool(c_StrU8)> fn;
	HELEMENT el;
	UINT eventCmd;
};

BOOL SC_CALLBACK _onDocComplete(
	LPVOID tag, HELEMENT he, UINT evtg, LPVOID prms)
{
	if (evtg != HANDLE_BEHAVIOR_EVENT) return FALSE;
	auto* bp = (BEHAVIOR_EVENT_PARAMS*)prms;
	auto* p = (_DocCompleteCtx*)tag;

	if (bp->cmd != p->eventCmd) return FALSE;

	SciterObj targetObj;
	g_sapi->ValueInit(&targetObj.val);
	g_sapi->SciterGetExpando(bp->heTarget, &targetObj.val, TRUE);

	// 执行用户的回调，获取返回值
	bool keepListening = p->fn((StrU8)targetObj.取属性("location.href"));
	// 只有当用户返回 false 时，才解绑监听器并释放内存
	if (!keepListening) {
		g_sapi->SciterDetachEventHandler(p->el, _onDocComplete, tag);
		delete p;
	}
	return FALSE;
}
}
class SciterUI;

class EXP SciterDom
{
public:
	HELEMENT ctx = nullptr;
	SciterUI* _st = nullptr;
	SciterDom() {}

	void 嵌入原生控件(c_StrX 选择器, HWND childHwnd)
	{
		HELEMENT found = NULL;
		StrA selector = 选择器;
		g_sapi->SciterSelectElements(ctx, selector, [](HELEMENT he, LPVOID param)
		{
			*(HELEMENT*)param = he;
			return TRUE;
		}, &found);
		if (found && g_sapi) g_sapi->SciterAttachHwndToElement(found, childHwnd);
	}

	typedef be::function<SciterObj(SciterObj&)> JS函数回调;

	bool 注入JS函数(c_StrU8 函数名, JS函数回调 回调)
	{
		HWINDOW hwnd = NULL;
		if (!ctx || !回调 ||
			g_sapi->SciterGetElementHwnd(
				ctx, &hwnd, TRUE) != SCDOM_OK ||
			!hwnd) return false;

		struct 函数记录 {
			JS函数回调 回调;
			HELEMENT ctx;
		};
		auto* record = new 函数记录{ (JS函数回调&&)回调, ctx };
		SciterObj function;
		UINT result = g_sapi->ValueNativeFunctorSet(
			&function.val,
			[](LPVOID tag, UINT argc, const VALUE* argv, VALUE* retval)
		{
			auto* record = (函数记录*)tag;
			SciterObj arg;
			if (argc) {
				g_sapi->ValueCopy(&arg.val, argv);
				if (arg._是JS引用对象())
					arg._jsCtx = record->ctx;
			}
			SciterObj result = record->回调(arg);
			g_sapi->ValueCopy(retval, &result.val);
		},
			[](LPVOID tag)
		{
			delete (函数记录*)tag;
		},
			record);

		if (result != HV_OK) {
			delete record;
			return false;
		}
		return g_sapi->SciterSetVariable(hwnd, 函数名, &function.val) == HV_OK;
	}

	/**获取当前文档的完整 URL 地址（如 file:///D:/.../index.html）
	 * @return 当前页面的 URL 字符串
	 */
	StrU8 取页面地址()
	{
		return 执行JS脚本("location.href");
	}

	StrU8 取页面标题()
	{
		return 执行JS脚本("document.head.querySelector('title').textContent");
	}
	void 置页面标题(c_StrU8 标题, c_StrU8 标题元素选择器 = "titlewin")
	{
		执行JS脚本("document.head.querySelector('title')").置属性("textContent", 标题);
		执行JS脚本("document.querySelector('#titlewin')").置属性("textContent", 标题);
	}


	void 浏览(c_StrU8 路径);

	SciterObj 执行JS脚本(c_StrU8 js代码)
	{
		SciterObj r;
		执行JS脚本_(js代码, r);
		return r;
	}

	bool 执行JS脚本_(
		c_StrU8 js代码,
		可空<SciterObj&> 结果 = 空, 可空<StrU8&> 错误文本 = 空)
	{
		StrW userCode = U8toW(js代码);
		bool 可有结果 = 结果 != 空;

		if (!可有结果 && 错误文本 == 空) {
			return g_sapi->SciterEvalElementScript(
				ctx, userCode, userCode.len(),
				NULL) == SCDOM_OK;
		}

		SciterObj evalResult;
		SciterObj codeArgument =
			可有结果 ? SciterObj() : SciterObj(userCode);
		StrW functionExpression = 可有结果
			? sprintF<W>(
				L"(function(){var res=\n%s\n;return res})",
				(charW*)userCode)
			: StrW(L"(function(code){return (0,eval)(code)})");
		SCDOM_RESULT sres = _调用临时脚本函数(
			functionExpression,
			可有结果 ? NULL : &codeArgument.val,
			可有结果 ? 0 : 1,
			&evalResult.val);

		if (evalResult.val.t == T_STRING &&
			evalResult.val.u == UT_STRING_ERROR) {
			_设置错误(错误文本, WtoU8(evalResult));
			return false;
		}
		if (sres != SCDOM_OK) {
			_设置错误(
				错误文本,
				"SciterJS: 内部错误代码=" + sres);
			return false;
		}

		if (可有结果) {
			if (evalResult._是JS引用对象())
				evalResult._jsCtx = ctx;
			((SciterObj&)结果) = (SciterObj&&)evalResult;
		}
		return true;
	}


	/**调用JS函数
	 * @param 全局函数名
	 * @param 参数 只有一个参数直接投入
	 * @return
	 */
	SciterObj 调用JS函数(c_StrU8 全局函数名, const SciterObj& 单个参数)
	{
		SciterObj r;
		调用JS函数_(全局函数名, 单个参数, r);
		return r;
	}

	/**调用JS函数(多参版)
	 * @param 全局函数名
	 * @param 参数 示例投入{123,"hello",obj}
	 * @return
	 */
	SciterObj 调用JS函数(c_StrU8 全局函数名, std::initializer_list<r_SciterObj> 参数组)
	{
		SciterObj r;
		调用JS函数_(全局函数名, 参数组, r);
		return r;
	}

	/**调用JS函数(无参版)
	 * @param 全局函数名
	 * @param <可空> 投入空时等价于投入0个参数（注意并非投入第一个参数为null）
	 * @param 结果<可空>
	 * @param 错误文本<可空>
	 * @return
	 */
	SciterObj 调用JS函数(c_StrU8 全局函数名)
	{
		SciterObj r;
		调用JS函数_(全局函数名, nil, r);
		return r;
	}

	bool 调用JS函数_(
		c_StrU8 全局函数名,
		std::initializer_list<r_SciterObj> 参数组,
		可空<SciterObj&> 结果 = 空, 可空<StrU8&> 错误文本 = 空)
	{
		bool 可有结果 = 结果 != 空;
		UINT argc = (UINT)参数组.size();

		const VALUE* argv = (argc > 0)
			? (const VALUE*)参数组.begin()
			: NULL;

		SciterObj callResult;
		SCDOM_RESULT sres =
			g_sapi->SciterCallScriptingFunction(
				ctx, 全局函数名, argv,
				argc,
				&callResult.val);

		if (callResult.val.t == T_STRING &&
			callResult.val.u == UT_STRING_ERROR) {
			_设置错误(错误文本, WtoU8(callResult));
			return false;
		}
		if (sres != SCDOM_OK) {
			_设置错误(错误文本, "SciterJS: 函数调用失败错误代码=" + sres);
			return false;
		}

		if (可有结果) {
			if (callResult._是JS引用对象())
				callResult._jsCtx = ctx;
			((SciterObj&)结果) = (SciterObj&&)callResult;
		}
		return true;
	}

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
		可空<SciterObj&> 结果 = 空, 可空<StrU8&> 错误文本 = 空)
	{
		r_SciterObj item(单个参数);
		return 调用JS函数_(全局函数名, { item }, 结果, 错误文本);
	}

	bool 调用JS函数_(
		c_StrU8 全局函数名,
		Nil 零个参数 = 空,
		可空<SciterObj&> 结果 = 空, 可空<StrU8&> 错误文本 = 空)
	{
		std::initializer_list<r_SciterObj> emptyList;
		return 调用JS函数_(全局函数名, emptyList, 结果, 错误文本);
	}

	/**等待文档加载完毕
	 * @param dom
	 * @param 回调
	 * @param 事件标识 默认为DOCUMENT_COMPLETE(页面css/js都加载完毕,注意不包括iframe)，DOCUMENT_READY(仅文档结构完毕)
	 */
	void 等待文档加载完毕(be::function<bool(c_StrU8)> 回调, UINT 事件标识 = DOCUMENT_COMPLETE)
	{
		auto* p = new _DocCompleteCtx{ be::move(回调), ctx, 事件标识 };
		g_sapi->SciterAttachEventHandler(ctx, _onDocComplete, p);
	}

	Bytes 保存截图(可空<SciterObj&> 目标元素 = 空, bool 截取完整滚动页面 = false)
	{
		HELEMENT targetHE = 目标元素 != 空 ? ((SciterObj&)目标元素)._取元素句柄() : ctx;
		if (!targetHE) return Bytes();

		if (!截取完整滚动页面) {
			HIMG hImg = nullptr;
			if (gapi() && gapi()->imageCreateFromElement(&hImg, targetHE) == GRAPHIN_OK && hImg) {
				Bytes pngBytes;
				gapi()->imageSave(hImg, [](LPVOID prm, const BYTE* data, UINT data_len) -> SBOOL {
					Bytes* pBuf = (Bytes*)prm;
					pBuf->append(data, data_len);
					return TRUE;
				}, &pngBytes, SCITER_IMAGE_ENCODING_PNG, 100);
				gapi()->imageRelease(hImg);
				return pngBytes;
			}
			return Bytes();
		} else {
			SciterObj elementObj(targetHE);

			SciterObj resultObj;
			StrW code = L"(function(el){\n"
				L"    if (!el) return null;\n"
				L"    var scroller = el, area = 0, changed = [], nodes = el.querySelectorAll('*');\n"
				L"    for (var i = 0; i < nodes.length; ++i) {\n"
				L"        var node = nodes[i], visibleArea = node.offsetWidth * node.offsetHeight;\n"
				L"        if (node != document.body && node.scrollHeight > node.offsetHeight && visibleArea > area) {\n"
				L"            area = visibleArea; scroller = node;\n"
				L"        }\n"
				L"    }\n"
				L"    function expand(node) {\n"
				L"        changed.push([node,node.style.cssText]);\n"
				L"        node.style.height = 'max-content';\n"
				L"    }\n"
				L"    try {\n"
				L"        for (var p = scroller; p; p = p.parentElement) expand(p);\n"
				L"        scroller.style.overflowY = 'hidden';\n"
				L"        scroller.style.setProperty('vertical-scrollbar','none');\n"
				L"        var ratio = Window.this.devicePixelRatio || window.devicePixelRatio || 1;\n"
				L"        var w = Math.ceil(Math.max(el.offsetWidth,el.scrollWidth || 0) * ratio);\n"
				L"        var h = Math.ceil(Math.max(el.offsetHeight,el.scrollHeight || 0) * ratio);\n"
				L"        if (w <= 0 || h <= 0) return null;\n"
				L"        var img = new Graphics.Image(w, h, el);\n"
				L"        return img ? new Uint8Array(img.toBytes('png')) : null;\n"
				L"    } finally {\n"
				L"        for (var i = changed.length - 1; i >= 0; --i) changed[i][0].style.cssText = changed[i][1];\n"
				L"    }\n"
				L"})";

			SCDOM_RESULT sres = _调用临时脚本函数(code, &elementObj.val, 1, &resultObj.val);
			if (sres == SCDOM_OK) {
				if (resultObj._是JS引用对象()) resultObj._jsCtx = ctx;
				return (Bytes)resultObj;
			}
			return Bytes();
		}
	}
private:

	SCDOM_RESULT _调用临时脚本函数(
		c_StrW functionExpression,
		const VALUE* argv,
		UINT argc,
		VALUE* result)
	{
		StrW functionName =
			L"__be_cpp_tmp_" + ToStr<W>(
			(UINT_PTR)(charW*)functionExpression);
		StrW definition = sprintF<W>(
			L"globalThis.%s=function(){"
			L"delete globalThis.%s;"
			L"return (%s).apply(this,arguments)};null",
			(charW*)functionName,
			(charW*)functionName,
			(charW*)functionExpression);

		SCDOM_RESULT sres =
			g_sapi->SciterEvalElementScript(
				ctx, definition, definition.len(),
				NULL);
		if (sres == SCDOM_OK) {
			StrU8 functionNameU8 = WtoU8(functionName);
			sres = g_sapi->SciterCallScriptingFunction(
				ctx, functionNameU8,
				argv, argc, result);
		}
		if (sres != SCDOM_OK) {
			StrW cleanup =
				L"delete globalThis." + functionName;
			g_sapi->SciterEvalElementScript(
				ctx, cleanup, cleanup.len(), NULL);
		}
		return sres;
	}

	static void _设置错误(
		可空<StrU8&> 错误文本, c_StrU8 text)
	{
		if (错误文本 != 空) 错误文本 = text;
	}
};

SciterDom SciterObj::仅框架元素取子文档模型() const
{
	HELEMENT he = _取元素句柄();
	HELEMENT child = NULL;
	if (g_sapi && g_sapi->SciterGetNthChild(he, 0, &child) == SCDOM_OK) {
		SciterDom r; r.ctx = child;
		return r;
	}
	return SciterDom();
}

void SciterObj::_等待文档加载完毕(be::function<bool(c_StrU8)> 回调, UINT 事件标识)
{
	HELEMENT he = _取元素句柄();
	if (!he) return;
	auto* p = new _DocCompleteCtx{ be::move(回调), he, 事件标识 };
	g_sapi->SciterAttachEventHandler(he, _onDocComplete, p);
}
