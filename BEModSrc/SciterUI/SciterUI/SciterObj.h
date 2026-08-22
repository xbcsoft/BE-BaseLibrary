#pragma once
#include "stdafx.h"
void _SciterValueSet(VALUE& val, c_Bytes bytes);

class r_SciterObj;
class SciterObj
{
	friend class SciterDom;
public:
	VALUE val;
	SciterObj();

	SciterObj(const SciterObj& other);

	SciterObj(SciterObj&& other) noexcept;

	/**从元素句柄转换到SciterObj
	 * @param he
	 */
	SciterObj(HELEMENT he);

	/**务必保证此对象是HTML的元素类型
	 * @return 非元素返回nullptr
	 */
	HELEMENT _取元素句柄() const;

	SciterDom 仅框架元素取子文档模型() const;

	SciterObj(int n);

	SciterObj(int64 n);

	SciterObj(double d);

	SciterObj(bool b);

	SciterObj(const char* str);

	SciterObj(const charW* str);

	SciterObj(c_Bytes bytes);

	template <int N>
	SciterObj(const byte(&arr)[N]) { _SciterValueSet(val, Bytes(arr)); }

	SciterObj(c_StrU8 str);

	SciterObj(c_StrW str);

	~SciterObj();

	SciterObj& operator=(const SciterObj& other);

	SciterObj& operator=(SciterObj&& other) noexcept;

	operator int() const;

	operator int64() const;

	operator double() const;

	operator bool() const;

	operator StrW() const;
	operator StrU8() const;

	operator Bytes() const;

	operator const VALUE& () const;

	operator VALUE& ();

private:
	SciterObj _取单级属性(c_StrW 属性名) const;

	bool _置单级属性(c_StrW 属性名, const SciterObj& 属性值);

public:
	SciterObj 取属性(c_StrX 属性名, char 分隔符 = '.') const;

	bool 置属性(c_StrX 属性名, const SciterObj& 属性值, char 分隔符 = '.');

	int 取数组成员数() const;

	bool 加入成员(const SciterObj& 成员);

	SciterObj 取成员(int 索引) const;

	bool 置成员(int 索引, const SciterObj& 成员);

	SciterObj operator[](int 索引) const;
	SciterObj operator[](int 索引);

	SciterObj 方法(c_StrU8 方法名, const SciterObj& 单个参数);
	SciterObj 方法(c_StrU8 方法名);
	SciterObj 方法(
		c_StrU8 方法名,
		std::initializer_list<r_SciterObj> 参数);

	bool 方法_(c_StrU8 方法名, const SciterObj& 单个参数, 可空<SciterObj&> 结果 = 空, 可空<StrU8&> 错误文本 = 空);
	bool 方法_(c_StrU8 方法名, Nil 零个参数 = 空, 可空<SciterObj&> 结果 = 空, 可空<StrU8&> 错误文本 = 空);
	bool 方法_(
		c_StrU8 方法名,
		std::initializer_list<r_SciterObj> 参数,
		可空<SciterObj&> 结果 = 空, 可空<StrU8&> 错误文本 = 空);


	/**等待文档加载完毕
	 * @param dom 用document.querySelector('iframe')之类所取得的SciterObj
	 * @param 回调
	 * @param 事件标识 默认为DOCUMENT_COMPLETE(页面css/js都加载完毕,注意不包括iframe)，DOCUMENT_READY(仅文档结构完毕)
	 */
	void _等待文档加载完毕(be::function<bool(c_StrU8)> 回调, UINT 事件标识 = DOCUMENT_COMPLETE);

private:
	HELEMENT _jsCtx = NULL;

	bool _是JS引用对象() const;

	void _释放值();
};

StrX __AutoStr__(const SciterObj& obj);

//用于std::initializer_list<r_SciterObj>的代理
class r_SciterObj
{
public:
	VALUE val;
	r_SciterObj();
	r_SciterObj(const SciterObj& obj);
	r_SciterObj(int n);
	r_SciterObj(int64 n);
	r_SciterObj(double d);
	r_SciterObj(bool b);
	r_SciterObj(const char* str);
	r_SciterObj(const charW* str);
	r_SciterObj(c_StrU8 str);
	r_SciterObj(c_StrW str);
	r_SciterObj(c_Bytes bytes);

	template <int N>
	r_SciterObj(const byte(&arr)[N]) { _SciterValueSet(val, Bytes(arr)); }
};
