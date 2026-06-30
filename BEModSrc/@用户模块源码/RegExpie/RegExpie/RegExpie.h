/**@ModuleTitle: 正则表达式IE版
*  @version:     1.0
*  @platform:    win32(x86|x64)
*  @compiler:    source
*  @author:
*  @datetime:
*  @description: 仅win32平台上采用IE9组件模拟的正则表达式类，本库并非高性能只为体积小，要求性能建议用C++原生regex
*/
#include "stdafx.h"

#pragma warning(push)
#pragma warning(disable: 4996) 
#include <jsrt.h>
#pragma warning(pop)

#pragma comment(lib, "jsrt.lib")


struct 子匹配结果 {
	Arraybe<StrU8> 子文本;
	Arraybe<int> 子文本位置;
};

using 替换回调函数 = StrU8(*)(const StrU8& 匹配文本, int 匹配文本所在位置, const 子匹配结果& 子匹配, size_t 附加参数);

class RegExpie {
public:
	RegExpie() {
#pragma warning(push)
#pragma warning(disable: 4996) 
		JsCreateRuntime(JsRuntimeAttributeNone, JsRuntimeVersionEdge, nullptr, &m_runtime);
#pragma warning(pop)
		JsCreateContext(m_runtime, nullptr, &m_context);
		JsSetCurrentContext(m_context);

		_Eval(L"var __targetText = '';");
		_Eval(L"var __reg = null;");
	}

	~RegExpie() {
		if (m_runtime != JS_INVALID_RUNTIME_HANDLE) {
			JsSetCurrentContext(JS_INVALID_REFERENCE);
			JsDisposeRuntime(m_runtime);
		}
		m_runtime = JS_INVALID_RUNTIME_HANDLE;
		m_context = JS_INVALID_REFERENCE;
	}

	RegExpie(const RegExpie&) = delete;
	RegExpie& operator=(const RegExpie&) = delete;

	RegExpie(RegExpie&& other) noexcept {
		m_runtime = other.m_runtime;
		m_context = other.m_context;
		m_srcText = (StrU8&&)other.m_srcText;
		other.m_runtime = JS_INVALID_RUNTIME_HANDLE;
		other.m_context = JS_INVALID_REFERENCE;
	}

	RegExpie& operator=(RegExpie&& other) noexcept {
		if (this != &other) {
			if (m_runtime != JS_INVALID_RUNTIME_HANDLE) {
				JsSetCurrentContext(JS_INVALID_REFERENCE);
				JsDisposeRuntime(m_runtime);
			}
			m_runtime = other.m_runtime;
			m_context = other.m_context;
			m_srcText = (StrU8&&)other.m_srcText;
			other.m_runtime = JS_INVALID_RUNTIME_HANDLE;
			other.m_context = JS_INVALID_REFERENCE;
		}
		return *this;
	}

	bool 创建(const StrU8& 正则文本, bool 是否不区分大小写 = false, const StrU8& 原文本 = StrU8()) {
		JsSetCurrentContext(m_context);

		StrW flags = L"gm";
		if (是否不区分大小写) {
			flags += L"i";
		}

		_Eval(L"var __pattern = '" + _转义正则字符串(U8toW(正则文本)) + L"';");
		_Eval(L"var __flags = '" + flags + L"';");

		JsValueRef res = _Eval(L"(function(){ try { __reg = new RegExp(__pattern, __flags); return true; } catch(e) { return false; } })();");
		bool success = _ToBool(res);

		if (success && 原文本.len() > 0) {
			置原文本(原文本);
		}
		return success;
	}

	void 置原文本(const StrU8& 原文本) {
		JsSetCurrentContext(m_context);
		_Eval(L"__targetText = '" + _转义正则字符串(U8toW(原文本)) + L"';");
		m_srcText = 原文本;
	}

	bool 匹配() {
		JsSetCurrentContext(m_context);
		JsValueRef res = _Eval(L"(function(){ if(!__reg) return false; var r = new RegExp('^(?:' + __reg.source + ')$', __reg.flags); return r.test(__targetText); })();");
		return _ToBool(res);
	}

	int 搜索(int 搜索开始位置 = 0, 可空<StrU8&> 返回匹配文本 = nil, 可空<子匹配结果&> 返回匹配子结果 = nil) {
		JsSetCurrentContext(m_context);
		bool needSub = (返回匹配子结果 != nil);

		StrW jsCode = L"(function() {\n"
			L"  if (!__reg) return null;\n"
			L"  __reg.lastIndex = 0;\n"
			L"  var t = __targetText;\n"
			L"  var start = " + 到文本<W>(搜索开始位置) + L";\n"
			L"  if (start > 0) { t = t.substr(start); }\n"
			L"  var res = __reg.exec(t);\n"
			L"  if (!res) return null;\n"
			L"  var r = { index: res.index + start, match: res[0] };\n";
		if (needSub) {
			jsCode +=
				L"  var subPos = [];\n"
				L"  var sf = res.index;\n"
				L"  for (var i = 1; i < res.length; i++) {\n"
				L"    if (res[i] === undefined) { subPos.push(-1); }\n"
				L"    else {\n"
				L"      var p = t.indexOf(res[i], sf);\n"
				L"      subPos.push(p === -1 ? -1 : p + start);\n"
				L"      if (p !== -1) sf = p + res[i].length;\n"
				L"    }\n"
				L"  }\n"
				L"  r.sub = res; r.subPos = subPos;\n";
		}
		jsCode += L"  return r;\n})();";

		JsValueRef res = _Eval(jsCode);
		if (_IsNullOrUndefined(res)) {
			return -1;
		}

		JsValueRef indexVal = _GetProperty(res, L"index");
		int index = (int)_ToNumber(indexVal);

		if (返回匹配文本 != nil) {
			JsValueRef matchVal = _GetProperty(res, L"match");
			返回匹配文本 = WtoU8(_ToString(matchVal));
		}

		if (needSub) {
			JsValueRef subVal = _GetProperty(res, L"sub");
			JsValueRef subPosVal = _GetProperty(res, L"subPos");
			(&返回匹配子结果)->子文本.clear();
			(&返回匹配子结果)->子文本位置.clear();
			int len = _GetArrayLength(subVal);
			for (int i = 1; i < len; ++i) {
				JsValueRef itemVal = _GetIndexedProperty(subVal, i);
				if (_IsNullOrUndefined(itemVal)) {
					(&返回匹配子结果)->子文本.push(StrU8());
					(&返回匹配子结果)->子文本位置.push(-1);
				} else {
					(&返回匹配子结果)->子文本.push(WtoU8(_ToString(itemVal)));
					(&返回匹配子结果)->子文本位置.push((int)_ToNumber(_GetIndexedProperty(subPosVal, i - 1)));
				}
			}
		}

		return index;
	}

	int 全部搜索(可空<Arraybe<StrU8>&> 返回匹配文本 = nil,
		可空<Arraybe<int>&> 返回匹配文本的位置 = nil,
		可空<Arraybe<子匹配结果>&> 返回子匹配结果 = nil)
	{
		JsSetCurrentContext(m_context);

		if (返回匹配文本 != nil) (&返回匹配文本)->clear();
		if (返回匹配文本的位置 != nil) (&返回匹配文本的位置)->clear();
		if (返回子匹配结果 != nil) (&返回子匹配结果)->clear();

		StrW jsCode = L"(function() {\n"
			L"  if (!__reg) return [];\n"
			L"  __reg.lastIndex = 0;\n"
			L"  var results = [];\n"
			L"  var res;\n"
			L"  while ((res = __reg.exec(__targetText)) !== null) {\n"
			L"      var subPos = [];\n"
			L"      var sf = res.index;\n"
			L"      for (var i = 1; i < res.length; i++) {\n"
			L"        if (res[i] === undefined) { subPos.push(-1); }\n"
			L"        else {\n"
			L"          var p = __targetText.indexOf(res[i], sf);\n"
			L"          subPos.push(p);\n"
			L"          if (p !== -1) sf = p + res[i].length;\n"
			L"        }\n"
			L"      }\n"
			L"      results.push({ index: res.index, match: res[0], sub: res, subPos: subPos });\n"
			L"      if (__reg.lastIndex === res.index) __reg.lastIndex++;\n"
			L"  }\n"
			L"  return results;\n"
			L"})();";

		JsValueRef resArr = _Eval(jsCode);
		int count = _GetArrayLength(resArr);

		for (int i = 0; i < count; ++i) {
			JsValueRef item = _GetIndexedProperty(resArr, i);

			if (返回匹配文本的位置 != nil) {
				int idx = (int)_ToNumber(_GetProperty(item, L"index"));
				(&返回匹配文本的位置)->push(idx);
			}

			if (返回匹配文本 != nil) {
				StrW matchText = _ToString(_GetProperty(item, L"match"));
				(&返回匹配文本)->push(WtoU8(matchText));
			}

			if (返回子匹配结果 != nil) {
				JsValueRef subVal = _GetProperty(item, L"sub");
				int subLen = _GetArrayLength(subVal);
				子匹配结果 subRes;
				JsValueRef subPosArr = _GetProperty(item, L"subPos");
				for (int j = 1; j < subLen; ++j) {
					JsValueRef subItem = _GetIndexedProperty(subVal, j);
					if (_IsNullOrUndefined(subItem)) {
						subRes.子文本.push(StrU8());
						subRes.子文本位置.push(-1);
					} else {
						subRes.子文本.push(WtoU8(_ToString(subItem)));
						subRes.子文本位置.push((int)_ToNumber(_GetIndexedProperty(subPosArr, j - 1)));
					}
				}
				(&返回子匹配结果)->push(subRes);
			}
		}

		return count;
	}

	StrU8 替换(const StrU8& 默认替换文本 = "", 替换回调函数 替换回调 = nullptr, int 起始位置 = 0, int 替换次数 = -1, 可空<int&> 返回实际替换的次数 = nil, size_t 附加回调参数四 = 0) {
		if (返回实际替换的次数 != nil) 返回实际替换的次数 = 0;

		if (替换回调 == nullptr) {
			JsSetCurrentContext(m_context);
			_Eval(L"var __repStr = '" + _转义正则字符串(U8toW(默认替换文本)) + L"';");
			StrW jsCode = L"(function() {\n"
				L"  if (!__reg) return { count: 0, text: __targetText };\n"
				L"  var t = __targetText;\n"
				L"  var start = " + 到文本<W>(起始位置) + L";\n"
				L"  var limit = " + 到文本<W>(替换次数) + L";\n"
				L"  var prefix = t.substr(0, start);\n"
				L"  var target = t.substr(start);\n"
				L"  var count = 0;\n"
				L"  var repStr = __repStr;\n"
				L"  function doRep() {\n"
				L"      var args = arguments;\n"
				L"      return repStr.replace(/\\$(\\d+)/g, function(m, p1) {\n"
				L"          var idx = parseInt(p1);\n"
				L"          if (idx < args.length - 2) { var val = args[idx]; return val === undefined ? '' : val; }\n"
				L"          return m;\n"
				L"      });\n"
				L"  }\n"
				L"  if (limit === -1) {\n"
				L"      __reg.lastIndex = 0;\n"
				L"      var replaced = target.replace(__reg, function() { count++; return doRep.apply(null, arguments); });\n"
				L"      return { count: count, text: prefix + replaced };\n"
				L"  }\n"
				L"  var f = '';\n"
				L"  if (__reg.ignoreCase) f += 'i';\n"
				L"  if (__reg.multiline) f += 'm';\n"
				L"  var r = new RegExp(__reg.source, f);\n"
				L"  var result = target;\n"
				L"  for (var i = 0; i < limit; i++) {\n"
				L"      var m = r.exec(result);\n"
				L"      if (!m) break;\n"
				L"      var before = result.substr(0, m.index);\n"
				L"      var after = result.substr(m.index + m[0].length);\n"
				L"      var repArgs = [];\n"
				L"      for (var j = 0; j < m.length; j++) repArgs.push(m[j]);\n"
				L"      repArgs.push(m.index, result);\n"
				L"      result = before + doRep.apply(null, repArgs) + after;\n"
				L"      count++;\n"
				L"  }\n"
				L"  return { count: count, text: prefix + result };\n"
				L"})();";

			JsValueRef resObj = _Eval(jsCode);
			if (!_IsNullOrUndefined(resObj)) {
				if (返回实际替换的次数 != nil) 返回实际替换的次数 = (int)_ToNumber(_GetProperty(resObj, L"count"));
				return WtoU8(_ToString(_GetProperty(resObj, L"text")));
			}
			return m_srcText;
		} else {
			Arraybe<StrU8> 匹配文本;
			Arraybe<int> 位置;
			Arraybe<子匹配结果> 子匹配;
			int count = 全部搜索(匹配文本, 位置, 子匹配);

			if (count == 0) return m_srcText;

			int replaceCount = 0;
			StrU8 resultText = 取文本左边(m_srcText, 起始位置);
			int lastIndex = 起始位置;

			for (int i = 0; i < count; ++i) {
				if (位置[i] < 起始位置) continue;
				if (替换次数 != -1 && replaceCount >= 替换次数) break;

				resultText += 取文本子串(m_srcText, lastIndex, 位置[i] - lastIndex);

				StrU8 newStr = 替换回调(匹配文本[i], 位置[i], 子匹配[i], 附加回调参数四);
				resultText += newStr;

				lastIndex = 位置[i] + 匹配文本[i].len();
				replaceCount++;
			}

			if (lastIndex < (int)m_srcText.len()) {
				resultText += 取文本子串(m_srcText, lastIndex, m_srcText.len() - lastIndex);
			}

			if (返回实际替换的次数 != nil) 返回实际替换的次数 = replaceCount;
			return resultText;
		}
	}

private:
	static StrW _转义正则字符串(const StrW& w) {
		StrW a = 子文本替换<W>(w, L"\\", L"\\\\", 0, -1, true);
		a = 子文本替换<W>(a, L"'", L"\\'", 0, -1, true);
		a = 子文本替换<W>(a, L"\r", L"\\r", 0, -1, true);
		a = 子文本替换<W>(a, L"\n", L"\\n", 0, -1, true);
		return a;
	}

	JsRuntimeHandle m_runtime = JS_INVALID_RUNTIME_HANDLE;
	JsContextRef m_context = JS_INVALID_REFERENCE;
	StrU8 m_srcText;

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

	bool _IsNullOrUndefined(JsValueRef val) {
		if (val == JS_INVALID_REFERENCE) return true;
		JsValueType type;
		if (JsGetValueType(val, &type) != JsNoError) return true;
		return (type == JsNull || type == JsUndefined);
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

	JsValueRef _GetProperty(JsValueRef obj, const StrW& propName) {
		JsPropertyIdRef pid;
		JsGetPropertyIdFromName((const wchar_t*)(charW*)propName, &pid);
		JsValueRef res = JS_INVALID_REFERENCE;
		JsGetProperty(obj, pid, &res);
		return res;
	}

	JsValueRef _GetIndexedProperty(JsValueRef obj, int index) {
		JsValueRef idxVal;
		JsIntToNumber(index, &idxVal);
		JsValueRef res = JS_INVALID_REFERENCE;
		JsGetIndexedProperty(obj, idxVal, &res);
		return res;
	}

	int _GetArrayLength(JsValueRef arr) {
		if (_IsNullOrUndefined(arr)) return 0;
		JsValueRef lenVal = _GetProperty(arr, L"length");
		return (int)_ToNumber(lenVal);
	}
};