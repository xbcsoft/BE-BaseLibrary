#include "stdafx.h"

namespace {
void _SciterValueSet(VALUE& val, int n) {
	g_sapi->ValueInit(&val);
	g_sapi->ValueIntDataSet(&val, n, T_INT, 0);
}

void _SciterValueSet(VALUE& val, int64 n) {
	g_sapi->ValueInit(&val);
	g_sapi->ValueInt64DataSet(&val, n, T_BIG_INT, 0);
}

void _SciterValueSet(VALUE& val, double d) {
	g_sapi->ValueInit(&val);
	g_sapi->ValueFloatDataSet(&val, d, T_FLOAT, 0);
}

void _SciterValueSet(VALUE& val, bool b) {
	g_sapi->ValueInit(&val);
	g_sapi->ValueIntDataSet(&val, b ? 1 : 0, T_BOOL, 0);
}

void _SciterValueSet(VALUE& val, const char* str) {
	g_sapi->ValueInit(&val);
	if (str) {
		StrW wstr = U8toW(str);
		g_sapi->ValueStringDataSet(&val, wstr, (UINT)wstr.len(), 0);
	}
}

void _SciterValueSet(VALUE& val, const charW* str) {
	g_sapi->ValueInit(&val);
	if (str) {
		g_sapi->ValueStringDataSet(&val, str, (UINT)wcslen(str), 0);
	}
}

void _SciterValueSet(VALUE& val, c_StrU8 str) {
	g_sapi->ValueInit(&val);
	StrW wstr = U8toW(str);
	g_sapi->ValueStringDataSet(&val, wstr, (UINT)wstr.len(), 0);
}

void _SciterValueSet(VALUE& val, c_StrW str) {
	g_sapi->ValueInit(&val);
	g_sapi->ValueStringDataSet(&val, str, (UINT)str.len(), 0);
}
}
void _SciterValueSet(VALUE& val, c_Bytes bytes) {
	g_sapi->ValueInit(&val);
	g_sapi->ValueBinaryDataSet(&val, bytes._buf(), (UINT)bytes._size(), T_BYTES, 0);
}

class r_SciterObj;
class EXP SciterObj
{
	friend class SciterDom;
public:
	VALUE val;
	SciterObj() {
		g_sapi->ValueInit(&val);
	}

	SciterObj(const SciterObj& other) {
		g_sapi->ValueInit(&val);
		g_sapi->ValueCopy(&val, &other.val);
		_jsCtx = other._jsCtx;
	}

	SciterObj(SciterObj&& other) noexcept {
		val = other.val;
		_jsCtx = other._jsCtx;
		g_sapi->ValueInit(&other.val);
		other._jsCtx = NULL;
	}

	/**从元素句柄转换到SciterObj
	 * @param he
	 */
	SciterObj(HELEMENT he) {
		g_sapi->ValueInit(&val);
		g_sapi->SciterGetExpando(he, &val, TRUE);
		if (_是JS引用对象()) {
			HWINDOW hwnd = NULL;
			g_sapi->SciterGetElementHwnd(he, &hwnd, TRUE);
			g_sapi->SciterGetRootElement(hwnd, &_jsCtx);
		}
	}

	/**务必保证此对象是HTML的元素类型
	 * @return 非元素返回nullptr
	 */
	HELEMENT _取元素句柄() const{
		HELEMENT he = NULL;
		g_sapi->SciterElementUnwrap(&val, &he);
		return he;
	}

	SciterDom 仅框架元素取子文档模型() const;

	SciterObj(int n) { _SciterValueSet(val, n); }

	SciterObj(int64 n) { _SciterValueSet(val, n); }

	SciterObj(double d) { _SciterValueSet(val, d); }

	SciterObj(bool b) { _SciterValueSet(val, b); }

	SciterObj(const char* str) { _SciterValueSet(val, str); }

	SciterObj(const charW* str) { _SciterValueSet(val, str); }

	SciterObj(c_Bytes bytes) { _SciterValueSet(val, bytes); }

	template <int N>
	SciterObj(const byte(&arr)[N]) { _SciterValueSet(val, Bytes(arr)); }

	SciterObj(c_StrU8 str) { _SciterValueSet(val, str); }

	SciterObj(c_StrW str) { _SciterValueSet(val, str); }

	~SciterObj() {
		g_sapi->ValueClear(&val);
	}

	SciterObj& operator=(const SciterObj& other) {
		if (this != &other) {
			_释放值();
			g_sapi->ValueCopy(&val, &other.val);
			_jsCtx = other._jsCtx;
		}
		return *this;
	}

	SciterObj& operator=(SciterObj&& other) noexcept {
		if (this != &other) {
			_释放值();
			val = other.val;
			_jsCtx = other._jsCtx;
			g_sapi->ValueInit(&other.val);
			other._jsCtx = NULL;
		}
		return *this;
	}

	operator int() const {
		if (val.t == T_INT || val.t == T_BOOL) {
			INT n = 0;
			g_sapi->ValueIntData(&val, &n);
			return n;
		}
		if (val.t == T_FLOAT) {
			double d = 0;
			g_sapi->ValueFloatData(&val, &d);
			return (int)d;
		}
		if (val.t == T_STRING) {
			LPCWSTR text = nullptr;
			UINT length = 0;
			g_sapi->ValueStringData(&val, &text, &length);
			if (text && length)
				return ToInt(WtoU8(StrW(text, length)));
		}
		return 0;
	}

	operator int64() const {
		if (val.t == T_BIG_INT || val.t == T_DATE) {
			INT64 n = 0;
			g_sapi->ValueInt64Data(&val, &n);
			return n;
		}
		if (val.t == T_INT || val.t == T_BOOL) {
			INT n = 0;
			g_sapi->ValueIntData(&val, &n);
			return n;
		}
		if (val.t == T_FLOAT) {
			double d = 0;
			g_sapi->ValueFloatData(&val, &d);
			return (int64)d;
		}
		if (val.t == T_STRING) {
			LPCWSTR text = nullptr;
			UINT length = 0;
			g_sapi->ValueStringData(&val, &text, &length);
			if (text && length)
				return ToLong(StrW(text, length));
		}
		return 0;
	}

	operator double() const {
		if (val.t == T_FLOAT) {
			double d = 0;
			g_sapi->ValueFloatData(&val, &d);
			return d;
		}
		if (val.t == T_INT || val.t == T_BOOL) {
			INT n = 0;
			g_sapi->ValueIntData(&val, &n);
			return n;
		}
		if (val.t == T_STRING) {
			LPCWSTR text = nullptr;
			UINT length = 0;
			g_sapi->ValueStringData(&val, &text, &length);
			if (text && length)
				return ToDouble(WtoU8(StrW(text, length)));
		}
		return 0;
	}

	operator bool() const {
		if (val.t == T_UNDEFINED || val.t == T_NULL) return false;
		if (val.t == T_BOOL || val.t == T_INT) {
			int n; g_sapi->ValueIntData(&val, &n);
			return n != 0;
		}
		return true;
	}

	operator StrW() const {
		if (val.t == T_UNDEFINED || val.t == T_NULL)
			return L"";

		VALUE text;
		g_sapi->ValueInit(&text);
		g_sapi->ValueCopy(&text, &val);
		if (val.t != T_STRING)
			g_sapi->ValueToString(&text, CVT_JSON_LITERAL);

		LPCWSTR chars = nullptr;
		UINT length = 0;
		g_sapi->ValueStringData(&text, &chars, &length);
		StrW result = chars
			? StrW(chars, length) : StrW();
		g_sapi->ValueClear(&text);
		return result;
	}
	operator StrU8() const {
		return WtoU8((StrW)*this);
	}

	operator Bytes() const {
		LPCBYTE data = nullptr;
		UINT length = 0;
		if (_是JS引用对象()) {
			SciterObj selfCopy = *this;
			SciterObj bufferObj = selfCopy.取属性("buffer");
			if (bufferObj.val.t != T_UNDEFINED && bufferObj.val.t != T_NULL) {
				if (g_sapi->ValueBinaryData(&bufferObj.val, &data, &length) == 0 && data && length > 0) {
					return Bytes(data, length);
				}
			}
		} else {
			if (g_sapi->ValueBinaryData(&val, &data, &length) == 0 && data && length > 0) {
				return Bytes(data, length);
			}
		}
		return Bytes();
	}

	operator const VALUE& () const {
		return val;
	}

	operator VALUE& () {
		return val;
	}

private:
	SciterObj _取单级属性(c_StrW 属性名) const {
		SciterObj res;
		if (val.t != T_MAP && !_是JS引用对象())
			return res;

		VALUE keyVal;
		g_sapi->ValueInit(&keyVal);
		g_sapi->ValueStringDataSet(
			&keyVal, 属性名, (UINT)属性名.len(), 0);

		if (_是JS引用对象() && _jsCtx) {
			VALUE args[2] = { val, keyVal };
			g_sapi->SciterCallScriptingFunction(
				_jsCtx, "Reflect.get",
				args, 2, &res.val);
		} else {
			g_sapi->ValueGetValueOfKey(
				&val, &keyVal, &res.val);
		}
		g_sapi->ValueClear(&keyVal);
		if (res._是JS引用对象())
			res._jsCtx = _jsCtx;
		return res;
	}

	bool _置单级属性(c_StrW 属性名, const SciterObj& 属性值) {
		if (val.t != T_MAP && !_是JS引用对象()) {
			g_sapi->ValueFromString(&val, L"{}", 2, CVT_JSON_LITERAL);
			_jsCtx = NULL;
		}

		VALUE keyVal; g_sapi->ValueInit(&keyVal);
		g_sapi->ValueStringDataSet(
			&keyVal, 属性名, (UINT)属性名.len(), 0);
		UINT r = HV_BAD_PARAMETER;
		if (!_是JS引用对象()) {
			r = g_sapi->ValueSetValueToKey(
				&val, &keyVal, &属性值.val);
		}

		if (r != HV_OK && _jsCtx) {
			if (属性值._是JS引用对象() && 属性值._jsCtx != _jsCtx) {
				r = HV_BAD_PARAMETER;
			} else {
				VALUE args[3] = {
					val, keyVal, 属性值.val
				};
				VALUE setResult;
				g_sapi->ValueInit(&setResult);
				g_sapi->SciterCallScriptingFunction(
					_jsCtx, "Reflect.set",
					args, 3, &setResult);
				INT success = 0;
				g_sapi->ValueIntData(
					&setResult, &success);
				g_sapi->ValueClear(&setResult);
				r = success
					? HV_OK : HV_BAD_PARAMETER;
			}
		}

		g_sapi->ValueClear(&keyVal);
		return r == HV_OK;
	}

public:
	SciterObj 取属性(c_StrX 属性名, char 分隔符 = '.') const {
		const StrW& fullKey = 属性名;
		const wchar_t* p = fullKey;
		const wchar_t* dot = wcschr(p, 分隔符);
		if (!dot) return _取单级属性(fullKey);

		SciterObj current = *this;
		while (dot && current) {
			StrW key(p, dot - p);
			current = current._取单级属性(key);
			p = dot + 1;
			dot = wcschr(p, 分隔符);
		}
		if (current)
			current = current._取单级属性(p);
		return current;
	}

	bool 置属性(c_StrX 属性名, const SciterObj& 属性值, char 分隔符 = '.') {
		const StrW& fullKey = 属性名;
		const wchar_t* p = fullKey;
		const wchar_t* dot = wcschr(p, 分隔符);
		if (!dot) return _置单级属性(fullKey, 属性值);

		StrW currentKey(p, dot - p);
		StrW subKeys(dot + 1);

		SciterObj child = _取单级属性(currentKey);
		bool ok = child.置属性(subKeys, 属性值, 分隔符);
		if (ok && !child._是JS引用对象())
			_置单级属性(currentKey, child);
		return ok;
	}

	int 取数组成员数() const {
		if (_是JS引用对象() && _jsCtx) {
			SciterObj lenObj = _取单级属性(L"length");
			return (int)lenObj;
		}
		INT n = 0;
		g_sapi->ValueElementsCount(&val, &n);
		return n;
	}

	bool 加入成员(const SciterObj& 成员) {
		int len = 取数组成员数();
		return 置成员(len, 成员);
	}

	SciterObj 取成员(int 索引) const {
		SciterObj res;
		if (_是JS引用对象() && _jsCtx) {
			VALUE keyVal; g_sapi->ValueInit(&keyVal);
			g_sapi->ValueIntDataSet(&keyVal, 索引, T_INT, 0);
			VALUE args[2] = { val, keyVal };
			g_sapi->SciterCallScriptingFunction(
				_jsCtx, "Reflect.get", args, 2, &res.val);
			g_sapi->ValueClear(&keyVal);
			if (res._是JS引用对象()) res._jsCtx = _jsCtx;
			return res;
		}
		g_sapi->ValueNthElementValue(&val, 索引, &res.val);
		if (res._是JS引用对象()) res._jsCtx = _jsCtx;
		return res;
	}

	bool 置成员(int 索引, const SciterObj& 成员) {
		if (_是JS引用对象() && _jsCtx) {
			VALUE keyVal; g_sapi->ValueInit(&keyVal);
			g_sapi->ValueIntDataSet(&keyVal, 索引, T_INT, 0);
			VALUE args[3] = { val, keyVal, 成员.val };
			VALUE setResult; g_sapi->ValueInit(&setResult);
			g_sapi->SciterCallScriptingFunction(
				_jsCtx, "Reflect.set", args, 3, &setResult);
			INT success = 0; g_sapi->ValueIntData(&setResult, &success);
			g_sapi->ValueClear(&keyVal);
			g_sapi->ValueClear(&setResult);
			return success != 0;
		}
		if (val.t != T_ARRAY) {
			g_sapi->ValueFromString(&val, L"[]", 2, CVT_JSON_LITERAL);
			_jsCtx = NULL;
		}
		UINT r = g_sapi->ValueNthElementValueSet(&val, 索引, &成员.val);
		return r == HV_OK;
	}

	SciterObj operator[](int 索引) const { return 取成员(索引); }
	SciterObj operator[](int 索引) { return 取成员(索引); }

	SciterObj 方法(c_StrU8 方法名, const SciterObj& 单个参数) {
		SciterObj r;
		方法_(方法名, 单个参数);
		return r;
	}
	SciterObj 方法(c_StrU8 方法名) {
		SciterObj r;
		方法_(方法名, nil);
		return r;
	}
	SciterObj 方法(
		c_StrU8 方法名,
		std::initializer_list<r_SciterObj> 参数) {
		SciterObj r;
		方法_(方法名, 参数);
		return r;
	}

	bool 方法_(c_StrU8 方法名, const SciterObj& 单个参数, 可空<SciterObj&> 结果 = 空, 可空<StrU8&> 错误文本 = 空);
	bool 方法_(c_StrU8 方法名, Nil 零个参数 = 空, 可空<SciterObj&> 结果 = 空, 可空<StrU8&> 错误文本 = 空);
	bool 方法_(
		c_StrU8 方法名,
		std::initializer_list<r_SciterObj> 参数,
		可空<SciterObj&> 结果 = 空, 可空<StrU8&> 错误文本 = 空)
	{
		SciterObj fn = 取属性(方法名);

		UINT argc = (UINT)参数.size();
		const VALUE* argv = (argc > 0) ? (const VALUE*)参数.begin() : NULL;

		SciterObj callResult;
		UINT ures = g_sapi->ValueInvoke(&fn.val, (VALUE*)&val,
			argc, argv, &callResult.val, NULL);

		if (callResult.val.t == T_STRING && callResult.val.u == UT_STRING_ERROR) {
			if (错误文本 != 空) {
				错误文本 = (StrU8)callResult;
			}
			return false;
		}

		if (ures != HV_OK) {
			if (错误文本 != 空) {
				错误文本 = "SciterJS: 方法调用失败错误代码=" + 到文本(ures);
			}
			return false;
		}

		if (结果 != 空) {
			if (callResult._是JS引用对象()) callResult._jsCtx = _jsCtx;
			((SciterObj&)结果) = (SciterObj&&)callResult;
		}
		return true;
	}


	/**等待文档加载完毕
	 * @param dom 用document.querySelector('iframe')之类所取得的SciterObj
	 * @param 回调
	 * @param 事件标识 默认为DOCUMENT_COMPLETE(页面css/js都加载完毕,注意不包括iframe)，DOCUMENT_READY(仅文档结构完毕)
	 */
	void _等待文档加载完毕(be::function<bool(c_StrU8)> 回调, UINT 事件标识 = DOCUMENT_COMPLETE);

private:
	HELEMENT _jsCtx = NULL;

	bool _是JS引用对象() const
	{
		return val.t == T_OBJECT || val.t == T_RESOURCE;
	}

	void _释放值()
	{
		g_sapi->ValueClear(&val);
		g_sapi->ValueInit(&val);
		_jsCtx = NULL;
	}
};

StrX __AutoStr__(const SciterObj& obj) {
	HELEMENT he = NULL;
	if (g_sapi->SciterElementUnwrap(&obj.val, &he) == HV_OK && he != NULL) {
		LPCSTR tag = NULL;
		g_sapi->SciterGetElementType(he, &tag);
		if (tag && *tag) {
			return "Element(" + StrU8(tag) + ")";
		}
		return "Element";
	}
	if (obj.val.t == T_BYTES) {
		return (StrU8)jzjj((Bytes)obj);
	}
	if (obj.val.t == T_OBJECT || obj.val.t == T_RESOURCE) {
		Bytes b = (Bytes)obj;
		if (b._size() > 0) {
			return (StrU8)jzjj(b);
		}
	}
	return (StrU8)obj;
}

//用于std::initializer_list<r_SciterObj>的代理
class EXP r_SciterObj
{
public:
	VALUE val;
	r_SciterObj() { g_sapi->ValueInit(&val); }
	r_SciterObj(const SciterObj& obj) : val(obj.val) {}
	r_SciterObj(int n) { _SciterValueSet(val, n); }
	r_SciterObj(int64 n) { _SciterValueSet(val, n); }
	r_SciterObj(double d) { _SciterValueSet(val, d); }
	r_SciterObj(bool b) { _SciterValueSet(val, b); }
	r_SciterObj(const char* str) { _SciterValueSet(val, str); }
	r_SciterObj(const charW* str) { _SciterValueSet(val, str); }
	r_SciterObj(c_StrU8 str) { _SciterValueSet(val, str); }
	r_SciterObj(c_StrW str) { _SciterValueSet(val, str); }
	r_SciterObj(c_Bytes bytes) { _SciterValueSet(val, bytes); }

	template <int N>
	r_SciterObj(const byte(&arr)[N]) { _SciterValueSet(val, Bytes(arr)); }
};

bool SciterObj::方法_(
	c_StrU8 方法名,
	const SciterObj& 单个参数,
	可空<SciterObj&> 结果,
	可空<StrU8&> 错误文本)
{
	r_SciterObj item(单个参数);
	return 方法_(方法名, { item }, 结果, 错误文本);
}

bool SciterObj::方法_(
	c_StrU8 方法名,
	Nil nil,
	可空<SciterObj&> 结果,
	可空<StrU8&> 错误文本)
{
	std::initializer_list<r_SciterObj> emptyList;
	return 方法_(方法名, emptyList, 结果, 错误文本);
}