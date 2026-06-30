#pragma once
/**@ModuleTitle:
*  @version:     1.0
*  @platform:    win32(x86|x64)
*  @compiler:    source
*  @author:
*  @datetime:
*  @description:
*/
#include "BEMod/BEMod.h"
// #define USE_EDGEMODE_JSRT // 注释掉，使用传统 jscript9 实现（支持Win7+IE10/11）
#pragma warning(push)
#pragma warning(disable: 4996) // 屏蔽针对 jscript9 环境下的 JsRuntimeVersionEdge 弃用警告
#include <jsrt.h>
#pragma warning(pop)

#pragma comment(lib, "jsrt.lib") // jscript9.dll对应的导入库


inline StrU8 json通用转义(const StrU8& 值, bool 不转义单引号 = false, bool 转义双引号 = false) {
	StrW a = U8toW(值);
	a = 子文本替换<W>(a, L"\\", L"\\\\", 0, -1, true);
	if (!不转义单引号) {
		a = 子文本替换<W>(a, L"'", L"\\'", 0, -1, true);
	}
	if (转义双引号) {
		a = 子文本替换<W>(a, L"\"", L"\\\"", 0, -1, true);
	}

	a = 子文本替换<W>(a, L"\r\n", L"\\r\\n", 0, -1, true);
	a = 子文本替换<W>(a, L"\n", L"\\r\\n", 0, -1, true);
	a = 子文本替换<W>(a, L"\r", L"\\r\\n", 0, -1, true);
	return WtoU8(a);
}

inline StrU8 json通用反转义(const StrU8& 值) {
	StrW a = U8toW(值);
	a = 子文本替换<W>(a, L"\\r\\n", L"\r\n", 0, -1, true);
	a = 子文本替换<W>(a, L"\\n", L"\r\n", 0, -1, true);
	a = 子文本替换<W>(a, L"\\'", L"'", 0, -1, true);
	a = 子文本替换<W>(a, L"\\\\", L"\\", 0, -1, true);
	return WtoU8(a);
}

class JSONie
{
public:
	JSONie() {
#pragma warning(push)
#pragma warning(disable: 4996) 
		JsCreateRuntime(JsRuntimeAttributeNone, JsRuntimeVersionEdge, nullptr, &m_runtime);
#pragma warning(pop)
		JsCreateContext(m_runtime, nullptr, &m_context);
		JsSetCurrentContext(m_context);
		_Eval(m_basePath + L" = {};");
	}

	// 禁用拷贝
	JSONie(const JSONie&) = delete;
	JSONie& operator=(const JSONie&) = delete;

	// 移动构造
	JSONie(JSONie&& other) noexcept {
		m_runtime = other.m_runtime;
		m_context = other.m_context;
		m_isRef = other.m_isRef;
		m_basePath = (StrW&&)other.m_basePath;

		other.m_runtime = JS_INVALID_RUNTIME_HANDLE;
		other.m_context = JS_INVALID_REFERENCE;
	}

	// 移动赋值
	JSONie& operator=(JSONie&& other) noexcept {
		if (this != &other) {
			_销毁();
			m_runtime = other.m_runtime;
			m_context = other.m_context;
			m_isRef = other.m_isRef;
			m_basePath = (StrW&&)other.m_basePath;

			other.m_runtime = JS_INVALID_RUNTIME_HANDLE;
			other.m_context = JS_INVALID_REFERENCE;
		}
		return *this;
	}

	~JSONie() {
		_销毁();
	}

	// _销毁
	void _销毁() {
		if (!m_isRef && m_runtime != JS_INVALID_RUNTIME_HANDLE) {
			JsSetCurrentContext(JS_INVALID_REFERENCE);
			JsDisposeRuntime(m_runtime);
		}
		m_runtime = JS_INVALID_RUNTIME_HANDLE;
		m_context = JS_INVALID_REFERENCE;
	}

	// 解析
	bool 导入(const StrU8& 数据文本) {
		return 导入W(U8toW(数据文本));
	}

	// 解析W
	bool 导入W(const StrW& 宽字符文本) {
		StrW safeText = 宽字符文本.len() == 0 ? StrW(L"{}") : 宽字符文本;
		JsValueRef res = _Eval(m_basePath + L" = " + safeText + L";");
		if (res == JS_INVALID_REFERENCE) {
			_Eval(m_basePath + L" = {};");
			return false;
		}
		return true;
	}

	// 清空，可空: 0空对象,1空数组
	void 清空(int 重置为类型 = 0) {
		if (重置为类型 == 0) {
			_Eval(m_basePath + L" = {};");
		} else {
			_Eval(m_basePath + L" = [];");
		}
	}

	// 到json文本
	StrU8 导出() {
		return WtoU8(导出W());
	}

	StrW 导出W() {
		return _ExecTextMethod(L"JSON.stringify(" + m_basePath + L")");
	}

	friend AutoStr __AutoStr__(JSONie& json) {
		return json.导出();
	}

	// 删属性
	void 删属性(const StrU8& 对象名, NilOpt<c_StrU8> 属性名 = nil) {
		StrW path = _取对象名(对象名);
		if (属性名 != nil) path += L"['" + _转义json单引号反斜杠(属性名) + L"']";
		_Eval(L"delete " + path + L";");
	}


	/**置属性
	 * @param 对象名 可为""表示在根目录内取属性，其对象名可以多级直到最后一级属性
	    但期间需要按JS对象规范书写如有特殊字符需要使用['xxx']的方案
	 * @param 属性名<可空> 这是最后一级访问
	 * @param 值 可为字符串、C++字面量任意数值、C++字面量bool型、字符串对象(需开启参数4)
	 * @param 值为对象=false 开启后在字符串中写对象
	 */
	template<typename T>
	void 置属性(const StrU8& 对象名, 可空<c_StrU8> 属性名, const T& 值, bool 值为对象 = false) {
		StrW path = _取对象名(对象名);
		if (属性名 != nil) path += L"['" + _转义json单引号反斜杠(属性名) + L"']";

		StrW code = path + L" = ";
		if constexpr (be::is_boolean<T>) {
			code += (值 ? L"true;" : L"false;");
		} else if constexpr (be::is_numeric<T>) {
			code += 到文本<W>((double)值) + L";";
		} else {
			if (值为对象) {
				code += U8toW(StrU8(值)) + L";";
			} else {
				code += L"'" + U8toW(json通用转义(值)) + L"';";
			}
		}
		_Eval(code);
	}


	// 取属性数值
	double 取属性数值(const StrU8& 对象名, NilOpt<c_StrU8> 属性名 = nil) {
		StrW path = _取对象名(对象名);
		if (属性名 != nil) path += L"['" + _转义json单引号反斜杠(属性名) + L"']";
		return _ExecNumMethod(path);
	}

	// 取属性 (主入口：处理路径+属性)
	StrU8 取属性(const StrU8& 对象名, NilOpt<c_StrU8> 属性名 = nil, bool 是否格式化 = false) {
		StrW path = _取对象名(对象名);
		if (属性名 != nil) path += L"['" + _转义json单引号反斜杠(属性名) + L"']";
		return WtoU8(_ExecTextMethod(是否格式化 ? (L"JSON.stringify(" + path + L")") : path));
	}

	// 取属性对象
	JSONie 取属性对象(const StrU8& 对象名, NilOpt<c_StrU8> 属性名 = nil) {
		StrW path = _取对象名(对象名);
		if (属性名 != nil) path += L"['" + _转义json单引号反斜杠(属性名) + L"']";
		return JSONie(m_runtime, m_context, path);
	}


	// 取数组成员数
	int 取数组成员数(const StrU8& 对象名) {
		return (int)_ExecNumMethod(_取对象名(对象名) + L".length");
	}

	// 加数组成员
	void 加数组成员(const StrU8& 对象名, const StrU8& 成员值, bool 为对象或整数 = false) {
		StrW _对象名 = _取对象名(对象名);
		StrW _值 = 为对象或整数 ? U8toW(成员值) : (L"'" + U8toW(json通用转义(成员值)) + L"'");
		StrW code = L"if (Object.prototype.toString.call(" + _对象名 + L") != '[object Array]') { " +
			_对象名 + L" = []; } " +
			_对象名 + L".push(" + _值 + L");";
		_Eval(code);
	}

	// 取数组成员对象
	JSONie 取数组成员对象(const StrU8& 对象名, int 索引) {
		StrW path = _取对象名(对象名) + L"[" + 到文本<W>(索引) + L"]";
		return JSONie(m_runtime, m_context, path);
	}

	// 取数组成员
	StrU8 取数组成员(const StrU8& 对象名, int 索引, NilOpt<c_StrU8> 属性名 = nil, bool 是否格式化 = false) {
		StrW path = _取对象名(对象名) + L"[" + 到文本<W>(索引) + L"]";
		if (属性名 != nil) path += L"['" + _转义json单引号反斜杠(属性名) + L"']";
		return WtoU8(_ExecTextMethod(是否格式化 ? (L"JSON.stringify(" + path + L")") : path));
	}

	// 置数组成员
	void 置数组成员(const StrU8& 对象名, int 索引, const StrU8& 成员值, bool 为对象或整数 = false) {
		StrW _对象名 = _取对象名(对象名);
		StrW _成员值 = 为对象或整数 ? U8toW(成员值) : (L"'" + U8toW(json通用转义(成员值)) + L"'");
		_Eval(_对象名 + L"[" + 到文本<W>(索引) + L"] = " + _成员值 + L";");
	}

	// 删数组成员
	void 删数组成员(const StrU8& 对象名, int 索引) {
		_Eval(_取对象名(对象名) + L".splice(" + 到文本<W>(索引) + L", 1);");
	}

	// 取数组成员数值
	double 取数组成员数值(const StrU8& 对象名, int 索引) {
		return _ExecNumMethod(_取对象名(对象名) + L"[" + 到文本<W>(索引) + L"]");
	}

	// 对象是否存在
	bool 对象是否存在(const StrU8& 对象名) {
		return _ExecBoolMethod(_取对象名(对象名) + L" != null");
	}

	// 取所有属性名
	int 取所有属性名(const StrU8& 对象名, Arraybe<StrU8>& 属性名数组) {
		属性名数组.clear();
		JsValueRef keysArr = _Eval(L"Object.keys(" + _取对象名(对象名) + L")");
		if (keysArr == JS_INVALID_REFERENCE) return 0;

		JsPropertyIdRef lengthId;
		JsGetPropertyIdFromName(L"length", &lengthId);
		JsValueRef lengthVal;
		JsGetProperty(keysArr, lengthId, &lengthVal);
		int count = (int)_ToNumber(lengthVal);

		for (int i = 0; i < count; i++) {
			JsValueRef indexVal, keyVal;
			JsIntToNumber(i, &indexVal);
			JsGetIndexedProperty(keysArr, indexVal, &keyVal);
			属性名数组.push(WtoU8(_ToString(keyVal)));
		}
		return count;
	}

	// 取类型 0:空,1:数,2:对象,3:数组,4:文本
	int 取类型(const StrU8& 对象名) {
		StrW _对象名 = _取对象名(对象名);
		if (_ExecBoolMethod(_对象名 + L" == null")) return 0;

		StrW typeStr = _ExecTextMethod(L"Object.prototype.toString.call(" + _对象名 + L")");
		if (typeStr == L"[object Number]") return 1;
		if (typeStr == L"[object Object]") return 2;
		if (typeStr == L"[object Array]") return 3;
		if (typeStr == L"[object String]") return 4;

		return -1; // unknown
	}

private:
	JsRuntimeHandle m_runtime = JS_INVALID_RUNTIME_HANDLE;
	JsContextRef m_context = JS_INVALID_REFERENCE;
	bool m_isRef = false;
	StrW m_basePath = L"d";

	// 专供内部用来生成引用对象的私有构造函数
	JSONie(JsRuntimeHandle rt, JsContextRef ctx, const StrW& path)
		: m_runtime(rt), m_context(ctx), m_isRef(true), m_basePath(path) {}

	StrW _取对象名(const StrU8& objName) {
		if (objName.len() == 0) return m_basePath;
		StrW wObjName = U8toW(objName);
		if (objName[0] == '[') return m_basePath + wObjName;
		return m_basePath + L"." + wObjName;
	}

	static StrW _转义json单引号反斜杠(const StrU8& 值) {
		StrW w = U8toW(值);
		StrW a = 子文本替换<W>(w, L"\\", L"\\\\", 0, -1, true);
		return 子文本替换<W>(a, L"'", L"\\'", 0, -1, true);
	}

	JsValueRef _Eval(const StrW& code) {
		if (m_context == JS_INVALID_REFERENCE) return JS_INVALID_REFERENCE;

		JsValueRef result = JS_INVALID_REFERENCE;
		JsContextRef currentParamsContext = JS_INVALID_REFERENCE;
		JsGetCurrentContext(&currentParamsContext);
		if (currentParamsContext != m_context) {
			JsSetCurrentContext(m_context);
		}

		JsErrorCode err = JsRunScript((const wchar_t*)(charW*)code, 0, L"", &result);
		if (err != JsNoError) {
			bool hasException = false;
			JsHasException(&hasException);
			if (hasException) {
				JsValueRef exception;
				JsGetAndClearException(&exception);
			}
			return JS_INVALID_REFERENCE;
		}
		return result;
	}

	StrW _ToString(JsValueRef val) {
		if (val == JS_INVALID_REFERENCE) return L"";

		JsValueRef strVal;
		if (JsConvertValueToString(val, &strVal) != JsNoError) return L"";

		const wchar_t* buf = nullptr;
		size_t len = 0;
		if (JsStringToPointer(strVal, &buf, &len) == JsNoError && buf) {
			return StrW((charW*)buf, len);
		}
		return L"";
	}

	double _ToNumber(JsValueRef val) {
		if (val == JS_INVALID_REFERENCE) return 0.0;
		double d = 0;
		if (JsNumberToDouble(val, &d) == JsNoError) return d;
		JsValueRef numVal;
		if (JsConvertValueToNumber(val, &numVal) == JsNoError) {
			if (JsNumberToDouble(numVal, &d) == JsNoError) return d;
		}
		return 0.0;
	}

	bool _ToBool(JsValueRef val) {
		if (val == JS_INVALID_REFERENCE) return false;
		bool b = false;
		if (JsBooleanToBool(val, &b) == JsNoError) return b;
		return false;
	}

	StrW _ExecTextMethod(const StrW& code) { return _ToString(_Eval(code)); }
	double _ExecNumMethod(const StrW& code) { return _ToNumber(_Eval(code)); }
	bool _ExecBoolMethod(const StrW& code) { return _ToBool(_Eval(code)); }
};
