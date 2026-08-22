#pragma once
#include <stddef.h> // for size_t

#ifdef _MSC_VER
#ifndef _INITIALIZER_LIST_
#define _INITIALIZER_LIST_
namespace std {
template<class _E> class initializer_list {
public:
	using value_type = _E; using size_type = decltype(sizeof(0));
	using iterator = const _E*; using const_iterator = const _E*;
	constexpr initializer_list() noexcept : _First(nullptr), _Last(nullptr) {}
	constexpr initializer_list(const _E* f, const _E* l) noexcept : _First(f), _Last(l) {}
	constexpr const _E* begin() const noexcept { return _First; }
	constexpr const _E* end()   const noexcept { return _Last; }
	constexpr size_type size()  const noexcept { return (size_type)(_Last - _First); }
private:
	const _E* _First; const _E* _Last;
};
}
#endif
#else
#include <initializer_list>
#endif

#ifdef _MSC_VER
#ifndef __PLACEMENT_NEW_INLINE
#define __PLACEMENT_NEW_INLINE
inline void* operator new (decltype(sizeof(0)), void* p) noexcept { return p; }
inline void  operator delete(void*, void*) noexcept {          }
#endif
#else
#include <new>
#endif

using byte = unsigned char; using uchar = unsigned char;
using ushort = unsigned short; using uint = unsigned int;
using ulong = unsigned long;
using int64 = long long; using uint64 = unsigned long long;

/**前置一些Win32与Linux兼容特性**/
#ifdef _WIN32
#include <tchar.h>
using charW = wchar_t;
#define _W(str) L##str
#ifdef _M_IX86
using ssize_t = int;
#else
using ssize_t = int64;
#endif
#else //!_WIN32
using charW = char16_t;
#define _W(str) u##str
#define _T(str) str
#endif

/**前置声明字符串类**/
class StrA; class StrU8;
class StrW; class AutoStr; class StrX;
struct W {}; struct U8 {}; //仅模板标记

/**平台默认字符串类型：Windows 默认 StrW，Linux 默认 StrA**/
#ifdef _WIN32
using charPlat = charW;
using StrPlat = StrW;
#else
using charPlat = char;
using StrPlat = StrA;
#endif

namespace be {
/**代替 <type_traits> 的核心轻量实现，保持白易核心零STL头文件依赖**/
template<bool B, class T = void> struct _enable_if {};
template<class T> struct _enable_if<true, T> { using type = T; };
template<bool B, class T = void> using enable_if = typename _enable_if<B, T>::type;

template<class T> struct _remove_reference { using type = T; };
template<class T> struct _remove_reference<T&> { using type = T; };
template<class T> struct _remove_reference<T&&> { using type = T; };
template <typename T> using remove_reference = typename _remove_reference<T>::type;

template<class T> struct _remove_cv { using type = T; };
template<class T> struct _remove_cv<const T> { using type = T; };
template<class T> struct _remove_cv<volatile T> { using type = T; };
template<class T> struct _remove_cv<const volatile T> { using type = T; };
template <typename T> using remove_cv = typename _remove_cv<T>::type;

//decay就是remove_cv+remove_reference+数组退化回指针类型（即把类型扒光到最原始态）
template<class T> struct _decay_impl { using type = typename _remove_cv<T>::type; };
template<class T, size_t N> struct _decay_impl<T[N]> { using type = T*; };
template<class T> struct _decay_impl<T[]> { using type = T*; };
template<class T> using decay = typename _decay_impl<remove_reference<T>>::type;


/**类型判断辅助（不依赖标准库），等价于 std::is_same_v**/
template<typename A, typename B> constexpr bool same = false;
template<typename A>             constexpr bool same<A, A> = true;
/**优雅的多类型匹配判断 (C++17 折叠表达式) **/
template<typename K, typename... Ts> constexpr bool sames = (same<K, Ts> || ...);

template <typename T> constexpr bool is_reference = false;
template <typename T> constexpr bool is_reference<T&> = true;
template <typename T> constexpr bool is_reference<T&&> = true;

template<class T> constexpr bool is_pointer = false;
template<class T> constexpr bool is_pointer<T*> = true;
template<class T> constexpr bool is_null_pointer = same<remove_cv<T>, decltype(nullptr)>;

template<class T> constexpr bool is_integral = sames<remove_cv<T>,
	bool, char, uchar, short, ushort, int, uint, long, ulong, int64, uint64
>;
template<class T> constexpr bool is_floating_point = sames<remove_cv<T>, float, double, long double>;
template <typename T> constexpr bool is_numeric = is_integral<T> || is_floating_point<T>;
template <typename T> constexpr bool is_boolean = same<remove_cv<T>, bool>;

template <typename T> constexpr bool is_string_ptr = sames<T, char*, const char*, wchar_t*, const wchar_t*, char16_t*, const char16_t*, char32_t*, const char32_t*
#ifdef __cpp_char8_t
	, char8_t*, const char8_t*
#endif
>;

template <typename T>
constexpr T&& forward(remove_reference<T>& arg) noexcept {
	return static_cast<T&&>(arg);
}

template <typename T>
constexpr remove_reference<T>&& move(T&& t) noexcept {
	return static_cast<remove_reference<T>&&>(t);
}

/**越界访问异常
 * @field length 容器当前长度（元素/字符个数）
 * @field index  实际传入的越界下标
 */
struct range_error {
	ssize_t length; //容器当前长度
	ssize_t index;  //越界的下标值
	range_error() : length(0), index(0) {}
	range_error(ssize_t l, ssize_t i) : length(l), index(i) {}
};


/**High-performance lightweight delegate (SOO - Small Object Optimization).
 * Stores lambdas in a fixed buffer to avoid heap allocation.
 */
template<typename Signature, size_t STACK_CAP = 32>
class function;

template<typename Ret, typename... Args, size_t STACK_CAP>
class function<Ret(Args...), STACK_CAP> {
	Ret(*m_invoker)(void*, Args...) = nullptr;
	void(*m_destructor)(void*) = nullptr;
	alignas(8) char m_storage[STACK_CAP];

public:
	function() = default;
	~function() { clear(); }

	function(const function&) = delete;
	function& operator=(const function&) = delete;

	function(function&& other) noexcept {
		move_from(other);
	}

	function& operator=(function&& other) noexcept {
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
	function(F f) {
		*this = f;
	}

	template<typename F>
	function& operator=(F f) {
		static_assert(sizeof(F) <= STACK_CAP, "Lambda too large for be::function STACK_CAP");
		clear();
		new (m_storage) F(f);
		m_invoker = [](void* storage, Args... args) -> Ret {
			return (*(F*)storage)(be::forward<Args>(args)...);
		};
		m_destructor = [](void* storage) {
			((F*)storage)->~F();
		};
		return *this;
	}

	Ret operator()(Args... args) const {
		if (m_invoker) {
			return m_invoker(const_cast<void*>((const void*)m_storage), be::forward<Args>(args)...);
		}
		return Ret();
	}

	explicit operator bool() const { return m_invoker != nullptr; }

private:
	void move_from(function& other) {
		m_invoker = other.m_invoker;
		m_destructor = other.m_destructor;
		memcpy(m_storage, other.m_storage, STACK_CAP);
		other.m_invoker = nullptr;
		other.m_destructor = nullptr;
	}
};


} // namespace be

#ifdef _MSC_VER
#pragma warning(disable:4267) //“参数”: 从“size_t”转换到“DWORD”
#pragma warning(disable:4244) //“参数”: 从“ssize_t”转换到“int”
#pragma warning(disable:6387) //“buf”可能是“0”: 这不符合函数“memcpy”的规范
#pragma warning(disable:6308) //“realloc”可能返回 null 指针
#pragma warning(disable:26495) //未初始化变量 Arraybe<int,0>::_stackBuf。
#pragma warning(disable:26812) //枚举型问题

#endif