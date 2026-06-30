#pragma once
#include "Arraybe.hpp"
#include "HashTbe.hpp"

namespace be {

template <typename T> constexpr bool is_Arraybe = false;
template <typename T, int C> constexpr bool is_Arraybe<Arraybe<T, C>> = true;

/**High-performance lightweight delegate (SOO - Small Object Optimization).
 * Stores lambdas in a fixed buffer to avoid heap allocation.
 */
template<typename T, size_t STACK_CAP = 32>
class function {
	void(*m_invoker)(void*, T) = nullptr;
	void(*m_destructor)(void*) = nullptr;
	alignas(8) char m_storage[STACK_CAP];

public:
	function() = default;
	~function() { clear(); }

	function(const function&) = delete;
	function& operator=(const function&) = delete;

	function(function&& other) {
		move_from(other);
	}

	function& operator=(function&& other) {
		if (this != &other) {
			clear();
			move_from(other);
		}
		return *this;
	}

	void clear() {
		if (m_destructor) {
			m_destructor(m_storage);
			m_destructor = nullptr;
		}
		m_invoker = nullptr;
	}

	template<typename F>
	void operator=(F f) {
		static_assert(sizeof(F) <= STACK_CAP, "Lambda too large for be::function STACK_CAP");
		clear();
		new (m_storage) F(f);
		m_invoker = [](void* storage, T arg) {
			(*(F*)storage)(arg);
		};
		m_destructor = [](void* storage) {
			((F*)storage)->~F();
		};
	}

	// Calling with exact type T
	void operator()(T arg) {
		if (m_invoker) {
			m_invoker(m_storage, (T)arg);
		}
	}
	operator bool() const { return m_invoker != nullptr; }
private:
	void move_from(function& other) {
		m_invoker = other.m_invoker;
		m_destructor = other.m_destructor;
		memcpy(m_storage, other.m_storage, STACK_CAP);
		other.m_invoker = nullptr;
		other.m_destructor = nullptr;
	}
};

template<typename T>
void swap(T& a, T& b) {
	T temp = (T&&)a;
	a = (T&&)b;
	b = (T&&)temp;
}

} // namespace be



#define _BETPL_DEF
#ifdef _WIN32
#define LINEBKPLAT "\r\n"
#define LINEBKPLATW _W("\r\n")
#else
#define LINEBKPLAT "\n"
#define LINEBKPLATW _W("\n")
#endif
#define LINEBK "\r\n"
#define LINEBKW _W("\r\n")
#define QUOTE "\""
#define QUOTEW _W("\"")
#define LQUOTE "“"
#define LQUOTEW _W("“")
#define RQUOTE "”"
#define RQUOTEW _W("”")

#define multiassign(VAL, ...) do { \
    auto _assign_helper = [&](auto&... args) { ((args = (VAL)), ...); }; \
    _assign_helper(__VA_ARGS__); \
} while (0)

#define CHOOSE_CASE_1(v, x)      case v: return x;
#define CHOOSE_CASE_2(v, x, ...) case v: return x; _EXPAND(CHOOSE_CASE_1(v+1, __VA_ARGS__))
#define CHOOSE_CASE_3(v, x, ...) case v: return x; _EXPAND(CHOOSE_CASE_2(v+1, __VA_ARGS__))
#define CHOOSE_CASE_4(v, x, ...) case v: return x; _EXPAND(CHOOSE_CASE_3(v+1, __VA_ARGS__))
#define CHOOSE_CASE_5(v, x, ...) case v: return x; _EXPAND(CHOOSE_CASE_4(v+1, __VA_ARGS__))
#define CHOOSE_CASE_6(v, x, ...) case v: return x; _EXPAND(CHOOSE_CASE_5(v+1, __VA_ARGS__))
#define CHOOSE_CASE_7(v, x, ...) case v: return x; _EXPAND(CHOOSE_CASE_6(v+1, __VA_ARGS__))
#define CHOOSE_CASE_8(v, x, ...) case v: return x; _EXPAND(CHOOSE_CASE_7(v+1, __VA_ARGS__))
#define CHOOSE_CASE_9(v, x, ...) case v: return x; _EXPAND(CHOOSE_CASE_8(v+1, __VA_ARGS__))
#define CHOOSE_CASE_10(v, x, ...) case v: return x; _EXPAND(CHOOSE_CASE_9(v+1, __VA_ARGS__))
#define _EXPAND(x) x
#define GET_MACRO_CHOOSE_HELPER(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,NAME,...) NAME
#define GET_MACRO_CHOOSE_A(args) _EXPAND(GET_MACRO_CHOOSE_HELPER args)
#define GET_MACRO_CHOOSE(...) GET_MACRO_CHOOSE_A((__VA_ARGS__, CHOOSE_CASE_10, CHOOSE_CASE_9, CHOOSE_CASE_8, CHOOSE_CASE_7, CHOOSE_CASE_6, CHOOSE_CASE_5, CHOOSE_CASE_4, CHOOSE_CASE_3, CHOOSE_CASE_2, CHOOSE_CASE_1))
#define _INVOKE(macro, args) _EXPAND(macro args)

template <typename F>
struct ReturnProxy {
	F f; template <typename T>operator T() const { return f(be::_remove_reference<T>{}); }
};
template <typename F> ReturnProxy<F> make_proxy(F f) { return { f }; }
// 结合返回值类型推导可隐式转换
#define multichoose(index, ...) be::make_proxy([&](auto tw) -> typename decltype(tw)::type { \
    switch (index) { \
        _INVOKE(GET_MACRO_CHOOSE(__VA_ARGS__), (0, __VA_ARGS__)) \
        default: throw be::range_error(-1,(ssize_t)index); \
    } \
})

#define choose(cond, a, b) ((cond) ? (a) : (b))

static struct Nil {} nil;

template<typename T>
class NilOpt {
public:
	T val; bool has;
	NilOpt() : has(false) {}
	NilOpt(Nil) : has(false) {}
	NilOpt(const T& v) : val(v), has(true) {}
	NilOpt(const char* s) : has(false) {
		if constexpr (be::same<T, StrA> || be::same<T, StrU8> || be::same<T, BR>) {
			has = (s != nullptr); if (has) val = (T)s;
		}
	}
	NilOpt(const charW* s) : has(false) {
		if constexpr (be::same<T, StrW>) {
			has = (s != nullptr); if (has) val = s;
		}
	}
	operator T&() { return val; }
	operator const T&() const { return val; }
	bool operator ==(Nil) const { return !has; }
	bool operator !=(Nil) const { return has; }
	NilOpt& operator=(const T& v) { has = true; val = v; return *this; }
	void SET(const T& v, bool has_) { has = has_, val = v; }
	const T& OR(const T& default_value) const { return has ? val : default_value; }
	T* operator&() { return &val; }
};

template<typename T>
class NilOpt<T&> {
public:
	T* p;
	NilOpt() : p(nullptr) {}
	NilOpt(Nil) : p(nullptr) {}
	NilOpt(T& v) : p(&v) {}
	operator T&() const { return *p; }
	bool operator==(Nil) const { return p == nullptr; }
	bool operator!=(Nil) const { return p != nullptr; }
	T& OR(T& default_value) { return p ? *p : default_value; }
	T* operator&() { return p; }
	NilOpt& operator=(const T& value) {
		if (p) { *p = value; } else { p = (T*)&value; }
		return *this;
	}
};

// NilOpt<const T&> 偏特化：适用于 c_StrU8/c_StrA/c_StrW 等 const引用别名
// 支持 const char*/const charW* 隐式构造（可用给定模板的主容器类存值）
// 双态持有，若传入已有的 const T& 实参则只保存其指针避免拷贝；传入字面量等临时对象时存入 val 内部存储区。
template<typename T>
class NilOpt<const T&> {
public:
	const T* p;
	T val;
	bool has;

	NilOpt() : p(&val), has(false) {}
	NilOpt(Nil) : p(&val), has(false) {}
	NilOpt(const T& v) : p(&v), has(true) {}
	
	template<typename U, typename = be::enable_if<!be::same<U, T>>>
	NilOpt(const U& s) : val(s), p(&val), has(true) {}

	// StrA/W/U8 对象内部对常指针进行的是引用构造
	NilOpt(const char* s) : val(s ? s : ""), p(&val), has(s != nullptr) {}
	NilOpt(const charW* s) : val(s ? s : L""), p(&val), has(s != nullptr) {}
	NilOpt(const NilOpt& other) : val(other.val), has(other.has) {
		p = (other.p == &other.val) ? &val : other.p;
	}
	NilOpt(NilOpt&& other) noexcept : val((T&&)other.val), has(other.has) {
		p = (other.p == &other.val) ? &val : other.p;
		other.has = false;
		other.p = &other.val;
	}
	operator const T&() const { return *p; }
	bool operator ==(Nil) const { return !has; }
	bool operator !=(Nil) const { return has; }
	NilOpt& operator=(const T& v) { p = &v; has = true; return *this; }
	NilOpt& operator=(const NilOpt& other) {
		if (this != &other) {
			val = other.val;
			has = other.has;
			p = (other.p == &other.val) ? &val : other.p;
		}
		return *this;
	}
	NilOpt& operator=(NilOpt&& other) noexcept {
		if (this != &other) {
			val = (T&&)other.val;
			has = other.has;
			p = (other.p == &other.val) ? &val : other.p;
			other.has = false;
			other.p = &other.val;
		}
		return *this;
	}
	const T& OR(const T& default_value) const { return has ? *p : default_value; }
	const T* operator&() const { return p; }
	const T* operator&() { return p; }
};

#pragma region Split
template<typename T_STR, typename T_CHAR, int STACK_CAP = 0>
Arraybe<T_STR, STACK_CAP>& _SplitStr(const T_STR& str, const T_STR& split, Arraybe<T_STR, STACK_CAP>& arr, bool keep = false) {
	arr.reset(0, false, !keep);
	if (str.bytes.size == 0) return arr;
	if (split.bytes.size == 0) { arr.push(str); return arr; }
	size_t splitLen = split.len(), count = 0, j = 0;
	T_CHAR* str_r = (T_CHAR*)str, *str_p = str_r;
	for (; *str_p; ++str_p) {
		if (*str_p != split[j]) j = 0;
		else if (++j == splitLen) {
			T_CHAR* segEnd = str_p + 1;
			size_t iLen = (size_t)(segEnd - str_r) - splitLen;
			arr.push(iLen > 0 ? T_STR(str_r, iLen) : T_STR());
			j = 0; str_r = segEnd; count++;
		}
	}
	if (count == 0) arr.push(str);
	else if (str_p - str_r) arr.push(T_STR(str_r, (size_t)(str_p - str_r)));
	return arr;
}

template<typename T_B, int STACK_CAP = 0>
Arraybe<T_B, STACK_CAP>& _SplitBytes(const Bytes& zjj, const Bytes& split, Arraybe<T_B, STACK_CAP>& arr, bool keep = false) {
	arr.reset(0, false, !keep);
	if (zjj.size == 0) return arr;
	if (split.size == 0) { if (zjj.buf) arr.push(T_B(zjj.buf, zjj.size)); else arr.push(T_B()); return arr; }
	size_t splitLen = split.size, count = 0, j = 0;
	byte* str_p = zjj, *str_r = str_p, *str_end = str_p + zjj.size;
	while (str_p < str_end) {
		if (*(str_p++) != split[j]) j = 0;
		else if (++j == splitLen) {
			size_t iLen = (size_t)(str_p - str_r) - splitLen;
			arr.push(iLen > 0 ? T_B(str_r, iLen) : T_B());
			j = 0, str_r = str_p, count++;
		}
	}
	if (count == 0) arr.push(T_B(zjj.buf, zjj.size));
	else if (str_p - str_r) arr.push(T_B(str_r, (size_t)(str_p - str_r)));
	return arr;
}

template<typename T, typename T_STR, typename T_SPLIT, int STACK_CAP>
Arraybe<T, STACK_CAP>& Split(const T_STR& str, const T_SPLIT& split, Arraybe<T, STACK_CAP>& arr, bool keep = false) {
	if constexpr (be::same<T, StrA>) return _SplitStr<T, char, STACK_CAP>(str, split, arr, keep);
	else if constexpr (be::same<T, StrW>) return _SplitStr<T, charW, STACK_CAP>(str, split, arr, keep);
	else return _SplitBytes<T, STACK_CAP>(str, split, arr, keep);
}

template<typename T>
struct _SplitView {
	const T& str; const T& split;
	_SplitView(const T& s, const T& sp) : str(s), split(sp) {}
	struct Iterator {
		byte* p, *end, *splitPtr; int splitLen; bool done; BR current;
		Iterator() : p(nullptr), end(nullptr), splitPtr(nullptr), splitLen(0), done(true) {}
		Iterator(const T& s, const T& sp) : p(s._buf()), end(s._buf() ? s._buf() + s._size() : s._buf()), splitPtr(sp._buf()), splitLen(sp._size()), done(false) {
			if (p == nullptr) p = end = nullptr;
			if (splitPtr == nullptr && splitLen == 0) splitPtr = nullptr;
			++(*this);
		}
		BR operator*() const { return current; }
		Iterator& operator++() {
			if (done) return *this;
			if (p >= end) { done = true; return *this; }
			if (splitLen == 0) { current = BR(p, (int)(end - p)); p = end; return *this; }
			byte* found = nullptr; int remaining = (int)(end - p);
			if (splitLen <= remaining && splitPtr) {
				byte* limit = end - splitLen;
				for (byte* i = p; i <= limit; ++i) { if (memcmp(i, splitPtr, splitLen) == 0) { found = i; break; } }
			}
			current = BR(p, found ? (size_t)(found - p) : (size_t)(end - p));
			p = found ? found + splitLen : end;
			return *this;
		}
		bool operator!=(const Iterator& other) const { return done != other.done; }
	};
	Iterator begin() const { return Iterator(str, split); }
	Iterator end() const { return Iterator(); }
};
template<typename T> _SplitView<T> SplitV(const T& str, const T& split) { return _SplitView<T>(str, split); }
#pragma endregion
