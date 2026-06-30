#pragma once
#include "bebase.hpp"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

//遵循W版接口规范引出(通过<W>显式触发)
template<class T> size_t strlen(const charW* s) {
#ifdef _WIN32
	return wcslen(s);
#else
	if (s == nullptr)return 0;
	int i = 0; for (; s[i]; i += 1) {}
	return i;
#endif
}
template<class T> int strcmp(const charW* a, const charW* b) {
#ifdef _WIN32
	return wcscmp(a, b);
#else
	if (!a || !b) return a == b ? 0 : (a ? 1 : -1);
	while (*a && *a == *b) { ++a; ++b; }
	return (int)(*a) - (int)(*b);
#endif
}


/**正式开始字节集类**/
class Bytes
{
public:
	byte* buf;
	size_t size = 0;
	size_t capacity_; //赋引用时会使_capacity变为0，由此来区分无需析构
public:
	/**字节集引用类（用作字符串去空终止的引用,且拼接时可减少较高成本的Bytes构造析构）**/
	struct Ref {
		const void* p; size_t size;
		Ref() : p(0), size(0) {}
		Ref(const void* p, size_t size) :p(p), size(size) {}
		explicit Ref(const char* s) :p(s) { if (s)size = strlen(s); else size = 0; }
		explicit Ref(const charW* s) :p(s) { if (s)size = strlen<W>(s)*2; else size = 0; }
		Ref(std::initializer_list<byte> bytes) :p(bytes.begin()), size(bytes.size()) {
			static volatile byte _trap = *bytes.begin();
		}template<int N>Ref(const byte(&z)[N]) : p(z), size(N) {} //按源字节数组引用构造
		Bytes operator+(const Ref& z) const { return Bytes(p, size, z.p, z.size); }
		Bytes operator+(const Bytes& z) const { return Bytes(p, size, z.buf, z.size); }
		bool operator==(const Ref& z) const {
			if (size!=z.size)return false;
			return memcmp(p, z.p, size)==0;
		}bool operator!=(const Ref& z) const { return !operator==(z); }
		Ref(const StrA& s); Ref(StrA&& s) = delete; //引用没有实体类型
		Ref(const StrW& s); Ref(StrW&& s) = delete; //不可进行右值转移
	};

	/**构造函数**/
	Bytes() { buf = nullptr; size = capacity_ = 0; }

	//支持从字面量的字节集进行引用(不属于拷贝构造)
	Bytes(std::initializer_list<byte> bytes) {
		//魔法！制造静态逃逸假象（否则此段代码会被release优化）
		static volatile byte _trap; _trap = *bytes.begin();
		_ref(bytes.begin(), bytes.size());
	}

	template <int N>
	Bytes(byte(&arr)[N]) {
		_reset(N); memcpy(buf, arr, N); //字节数组(拷贝构造)
	}
	template <int N>
	Bytes(const byte(&arr)[N]) {
		_ref(arr, N); //字节数组(引用构造)
	}
	Bytes(const Ref& v) {
		_ref(v.p, v.size);
	}

	explicit Bytes(size_t size) { _reset(size); }
	Bytes(byte c, size_t size) { _reset(size); memset(buf, c, size); }

	struct RefTag {};
	/**从指针+大小去构造
	 * @param dat
	 * @param RefTag 仅引用构造（需投入全局变量BRef，内部会使capacity_=0）
	 * @param size
	 */
	Bytes(const void* dat, RefTag, size_t size = 0) {
		_ref(dat, size);
	}
	Bytes(const void* dat, size_t size) {
		_reset(size); memcpy(buf, dat, size);
	}
	Bytes(const void* dat1, size_t len1, const void* dat2, size_t len2) {
		_reset(len1+len2);
		memcpy(buf, dat1, len1);
		memcpy(buf+len1, dat2, len2);
	}
	Bytes(const Bytes& zjj) { //拷贝构造
		_reset(zjj.size); memcpy(buf, zjj.buf, zjj.size);
	}
	Bytes(Bytes&& zjj) noexcept { //移动构造
		_move(zjj);
	}
	//移动且调整大小构造（用于 BA/BW 去空终止）
	Bytes(Bytes&& zjj, size_t newLen) {
		_move(zjj); size = newLen;
	}
	~Bytes() {
		//析构函数
		if (capacity_)free(buf);
	}

	void reset(size_t newSize = 0) { if (capacity_)free(buf); _reset(newSize); }
	void reset(byte c, size_t size) { reset(size); memset(buf, c, size); }
	void reset(const void* dat, size_t size) { reset(size); memcpy(buf, dat, size); }

	/**调整内部容量
	 * @param newCapacity 调整到目标容量
	 */
	void _capacity(size_t newCapacity) {
		if (newCapacity < size)
			throw be::range_error{ (ssize_t)size, (ssize_t)newCapacity };
		if (capacity_ == 0) { //字符串初始可能为引用的情况
			byte* bak = buf;
			buf = (byte*)malloc(newCapacity);
			capacity_ = newCapacity;
			if (bak) memcpy(buf, bak, size);
		} else {
			buf = (byte*)realloc(buf, newCapacity);
			capacity_ = newCapacity;
		}
	}
	inline size_t _capacity() { return capacity_; }

	/**赋引用(从内存地址+数据大小)；注意赋引用后若进行扩容操作会解除现有绑定并新开辟内存
	 * @param dat
	 * @param size
	 * @return
	 */
	Bytes& ref(const void* dat, size_t size) {
		if (capacity_)free(buf);
		_ref(dat, size);
		return *this;
	}

	/**sublen创建新引用并递减字节集长度（由于移动语义时它的缺陷，此函数不得在右值中使用）
	 * @param subsize
	 * @return
	 */
	Bytes _SL(int subsize) {
		if (!size)return Bytes();
		return Bytes(buf, RefTag(), size-subsize);
	}

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif
	/**在尾部加入数据(增添字节数)
	 * @param addSize
	 * @return
	 */
	Bytes& append(size_t addSize) {
		if (capacity_ == 0) {
			byte* bak = buf; size_t blen = size;
			size += addSize; reset(size);
			memcpy(buf, bak, blen);
			return *this;
		}
		size += addSize;
		if (size > capacity_) {
			//对<128MB增长策略在自身基础上+0.5倍(即*1.5倍)
			if (capacity_<128 * 1024 * 1024)
				capacity_ += max(addSize, capacity_>>1);
			else //>128MB的*1.25倍
				capacity_ += max(addSize, capacity_>>2);
			buf = (byte*)realloc(buf, capacity_);
		}
		return *this;
	}
	/**向自身尾部添加数据(从内存地址+数据大小拷贝添加)
	 * @param dat
	 * @param size2
	 * @return
	 */
	Bytes& append(const void* dat, size_t size2) {
		if (size2==0)return *this;
		append(size2);
		memcpy(buf+size-size2, dat, size2);
		return *this;
	}
	/**向自身尾部添加数据(从已有字节集)
	 * @param zjj
	 * @return
	 */
	Bytes& append(const Bytes& zjj) {
		return append(zjj.buf, zjj.size);
	}
	Bytes& append(const Ref& v) {
		return append(v.p, v.size);
	}
	template <int N>
	Bytes& append(const byte(&arr)[N]) {
		return append(arr, N);
	}

	void copyFrom(const void* dat, size_t size1) {
		if (dat)memcpy(buf, dat, size1);
	}
	void copyFrom(const Bytes& v) { copyFrom(v.buf, v.size); }
	void copyFrom(const Ref& v) { copyFrom(v.p, v.size); }
	/**从内存拷贝
	 * @param startI 自身偏移（注意不是src的偏移）
	 * @param src
	 * @param size1
	 */
	void copyFrom(size_t startI, const void* src, size_t size1) {
		memcpy(buf+startI, src, size1);
	}
	void copyFrom(size_t startI, const Bytes& v) {
		copyFrom(startI, v.buf, v.size);
	}
	void copyFrom(size_t startI, const Ref& v) {
		copyFrom(startI, v.p, v.size);
	}


	/**重载运算符**/
	Bytes& operator=(const Bytes& zjj) {
		if (this != &zjj) {
			reset(zjj.size); //已判断析构
			memcpy(buf, zjj.buf, zjj.size);
		}
		return *this;
	}
	Bytes& operator=(Bytes&& zjj) noexcept {
		if (this != &zjj) {
			if (capacity_) free(buf);
			_move(zjj);
		}
		return *this;
	}
	Bytes& operator=(Ref v) {
		ref(v.p, v.size);
		return *this;
	}
	template <int N>
	Bytes& operator=(byte(&arr)[N]) {
		reset(arr, N);
		return *this;
	}
	template <int N>
	Bytes& operator=(const byte(&arr)[N]) {
		ref(arr, N); //字节数组(引用构造)
		return *this;
	}
	Bytes operator+(const Bytes& z) const {
		return Bytes(buf, size, z.buf, z.size);
	}
	Bytes operator+(Ref v) const {
		return Bytes(buf, size, v.p, v.size);
	}
	Bytes& operator+=(const Bytes& zjj) {
		append(zjj);
		return *this;
	}
	Bytes& operator+=(Ref v) {
		append(v);
		return *this;
	}
	template <int N>
	Bytes& operator+=(const byte(&arr)[N]) {
		append(arr);
		return *this;
	}
	bool operator==(const Bytes& zjj) const {
		if (buf==NULL)return(zjj.buf==NULL);
		if (size!=zjj.size)return false;
		return memcmp(buf, zjj.buf, size)==0;
	}
	bool operator==(Ref zjj) const {
		if (buf==NULL)return(zjj.p==NULL);
		if (size!=zjj.size)return false;
		return memcmp(buf, zjj.p, size)==0;
	}
	bool operator!=(const Bytes& zjj) const {
		return !(operator==(zjj));
	}
	bool operator!=(Ref zjj) const {
		return !(operator==(zjj));
	}

	/**带越界检查的字节访问
	 * @param i 字节下标（允许负数，-1 = 最后一个字节）
	 * @throw be::range_error 下标越界时抛出，携带 length 和 index
	 */
	byte& at(ssize_t i) const {
		ssize_t len = (ssize_t)size;
		if (i < 0) i += len;
		if (i < 0 || i >= len) throw be::range_error{ len, i };
		return buf[i];
	}

	operator byte*() const { return buf; }
	//byte& operator[](size_t i) const { return *(buf+i); }


	explicit operator bool() const { return size!=0; }

public:
	void _ref(const void* dat, size_t len) {
		size = len; capacity_ = 0; buf = (byte*)dat;
	}
	void _mknull() { size = capacity_ = 0; buf = nullptr; }
	void _reset(size_t newSize) {
		if (newSize) {
			size = newSize; capacity_ = size+2; //为继承的字符串类提供空终止的可能
			buf = (byte*)malloc(capacity_);
		} else {
			_mknull();
		}
	}
	void _move(Bytes& z) {
		capacity_ = z.capacity_; buf = z.buf; size = z.size;
		z.capacity_ = 0;
	}
	inline byte* _buf() const { return buf; }
	inline size_t _size() const { return size; }
};

class StrA
{
public:
	Bytes bytes; inline size_t len() const { return bytes.size; }
	//提供有限部分来自字节集中的低级接口，务必清楚其语义是字节大小
	inline byte* _buf() const { return bytes.buf; }
	inline size_t _size() const { return bytes.size; }

	/**用len个字节数重新构造（之后仅简单填充空终止，不做内容初始化）
	 * @param len A版字符个数(不包括空终止)
	 */
	void reset(size_t len = 0) { _reset(len); }

	StrA(const char* s) { if (s)bytes._ref(s, strlen(s)); else _mknull(); }
	//除了单参的常文本指针构造是赋引用外其余构造内容的方法都是拷贝构造
	StrA() { _mknull(); }
	StrA(char* s) { if (s)_reset(s, strlen(s)); else _mknull(); }
	StrA(const char* s, size_t len) { _reset(s, len); }
	StrA(const StrA& s) { _reset(s.bytes.buf, s.bytes.size); }
	StrA(StrA&& s) noexcept : bytes((Bytes&&)s.bytes) {}
	StrA(char c, size_t len) { reset(len); memset(bytes.buf, c, len); }
	explicit StrA(size_t len) { reset(len); }

	StrA& operator=(const char* s) {
		_reset(s, strlen(s));
		return *this;
	}
	StrA& operator=(const StrA& s) {
		if (this != &s)_reset(s.bytes.buf, s.bytes.size);
		return *this;
	}
	StrA& operator=(const StrX& s);
	StrA& operator=(StrA&& s) noexcept {
		if (this != &s) {
			bytes = (Bytes&&)s.bytes;
			s._mknull();
		}
		return *this;
	}
	StrA& operator+=(const char* s) {
		if (s==nullptr)return *this;
		return _append(s, strlen(s));
	}
	StrA& operator+=(const StrA& s) {
		if (s.bytes.buf==nullptr)return *this;
		return _append(s.bytes.buf, s.bytes.size);
	}
	StrA operator+(const char* s) const {
		if (s==nullptr)return *this;
		size_t add = strlen(s); StrA r(len()+add);
		r.bytes.copyFrom(bytes.buf, bytes.size);
		r.bytes.copyFrom(bytes.size, s, add);
		return r;
	}

	StrA operator+(const StrA& s) const {
		if (s.bytes.buf==nullptr)return *this;
		size_t add = s.len(); StrA r(len()+add);
		r.bytes.copyFrom(bytes.buf, bytes.size);
		r.bytes.copyFrom(bytes.size, s.bytes.buf, add);
		return r;
	}
	friend StrA operator+(const char* s1, const StrA& s2)
	{
		size_t len1 = s1 ? strlen(s1) : 0; StrA r(len1 + s2.len());
		r.bytes.copyFrom(s1, len1);
		r.bytes.copyFrom(len1, s2.bytes.buf, s2.bytes.size);
		return r;
	}
	bool operator==(const char* str) const {
		if (bytes.buf==NULL)return(str==NULL || *str==0);
		if (str==NULL)return(false);
		return strcmp((char*)bytes.buf, str)==0;
	}
	bool operator!=(const char* str) const { return !(operator==(str)); }
	bool operator==(const StrA& str) const { return bytes == str.bytes; }
	bool operator!=(const StrA& str) const { return !(operator==(str)); }
	friend bool operator==(const char* str, const StrA& s) { return s == str; }
	friend bool operator!=(const char* str, const StrA& s) { return s != str; }
	friend bool operator==(char* str, const StrA& s) { return s == (const char*)str; }
	friend bool operator!=(char* str, const StrA& s) { return s != (const char*)str; }

	/**带越界检查的字符访问（以字节为单位，即 ANSI/UTF-8 字节下标）
	 * @param i 字节下标（允许负数，-1 = 最后一个字节）
	 * @throw be::range_error 下标越界时抛出，携带 length 和 index
	 */
	char& at(ssize_t i) const {
		ssize_t len = (ssize_t)bytes.size;
		if (i < 0) i += len;
		if (i < 0 || i >= len) throw be::range_error{ len, i };
		return ((char*)bytes.buf)[i];
	}


	explicit operator bool() const { return bytes.size!=0; }

	operator char*() const { return (char*)bytes.buf; }
private:
	void _mknull() { bytes.size = bytes.capacity_ = 0; bytes.buf = (byte*)""; }
	/**在bytes内部reset时会默认多分配2字节且该函数已自动填充空终止*/
	void _reset(size_t size) {
		if (size) {
			bytes._reset(size);
			*(char*)(bytes.buf + size) = 0;
		} else _mknull();
	}
	void _reset(const void* s, size_t size) { _reset(size); bytes.copyFrom(s, size); }
	StrA& _append(const void* s, size_t size2) {
		if (size2==0)return *this;
		size_t oldSize = bytes.size;
		bytes.append(size2); //统一走Bytes::append的倍增扩容策略
		//确保空终止有空间(Bytes::append不感知字符串空终止)
		if (bytes.size+1>bytes._capacity())bytes._capacity(bytes.size+1);
		//由于自身就已经有空终止了总大小再加进无空终止的s是正好的
		bytes.copyFrom(oldSize, s, size2+1);
		return *this;
	}
};

class StrU8 : public StrA
{
public:
	using StrA::StrA; //继承过来的不能参与隐式转换（但双参可用）
	StrU8() : StrA() {}
	StrU8(const char* s) : StrA(s) {}
	StrU8(char* s) : StrA(s) {}
	StrU8(const StrA& s) : StrA(s) {}
	StrU8(StrA&& s) : StrA((StrA&&)s) {}
	StrU8(const StrU8& s) : StrA(s) {}
	StrU8(StrU8&& s) noexcept : StrA((StrA&&)s) {}
	explicit StrU8(size_t len) : StrA(len) {}
	StrU8& operator=(const char* s) { StrA::operator=(s); return *this; }
	StrU8& operator=(const StrA& s) { StrA::operator=(s); return *this; }
	StrU8& operator=(StrA&& s) { StrA::operator=(s); return *this; }
	StrU8& operator=(const StrU8& s) { StrA::operator=(s); return *this; }
	StrU8& operator=(StrU8&& s) noexcept { StrA::operator=(s); return *this; }
	StrU8& operator+=(const char* s) { StrA::operator+=(s); return *this; }
	StrU8& operator+=(const StrA& s) { StrA::operator+=(s); return *this; }
#ifdef __cpp_char8_t
	//C++20起 u8"..." 类型为 const char8_t*，不再隐式转到 const char*，需显式接管
	StrU8(const char8_t* s) : StrA((const char*)s) {} //单参为引用，其余为拷贝构造
	StrU8(char8_t* s) : StrA((char*)s) {}
	StrU8& operator=(const char8_t* s) { return operator=((const char*)s); }
	StrU8& operator+=(const char8_t* s) { return operator+=((const char*)s); }
	operator char8_t*() const { return (char8_t*)bytes.buf; }
#endif

	/**快速计算UTF-8字符串的字符数
	 * @return 注意: 若存在非法U8字符的情况下该值不准确
	 */
	size_t lenU8() const {
		size_t cnt = 0;
		for (size_t i = 0; i < bytes.size; ++i)
			if ((bytes.buf[i] & 0xC0) != 0x80) cnt++;
		return cnt;
	}

	/**带越界检查的 UTF-8 字符访问
	 * @param i 字符下标（按字符数计，支持负数，-1 = 最后一个字符）
	 * @return BR (Bytes::Ref) 指向该字符所在的字节区间（1-4字节）
	 * @throw be::range_error 下标越界时抛出
	 */
	Bytes::Ref atU8(ssize_t i) const {
		ssize_t totalChar = (ssize_t)lenU8();
		if (i < 0) i += totalChar;
		if (i < 0 || i >= totalChar) throw be::range_error{ totalChar, i };

		ssize_t curChar = 0;
		byte* p = bytes.buf;
		byte* end = bytes.buf + bytes.size;
		// 定位到第 i 个字符的起点
		while (p < end && curChar < i) {
			if ((*p & 0xC0) != 0x80) curChar++; p++;
			while (p < end && (*p & 0xC0) == 0x80) p++; // 跳过后续字节
		}
		// 计算该字符的长度
		byte* start = p;
		if (p < end) {
			p++;
			while (p < end && (*p & 0xC0) == 0x80) p++;
		}
		return Bytes::Ref(start, (size_t)(p - start));
	}
	StrU8& operator=(const StrX& s);
};

class StrW
{
public:
	Bytes bytes; inline size_t len() const { return bytes.size/2; }

	/**提供有限部分来自字节集中的低级接口，务必清楚其语义是字节大小**/
	inline byte* _buf() const { return bytes.buf; }
	inline size_t _size() const { return bytes.size; }
	/**用len*2个字节数重新构造（之后仅简单填充空终止，不做内容初始化）
	 * @param len W版字符个数(不包括空终止)
	 */
	void reset(size_t len = 0) { _reset(len*2); }

	StrW(const charW* s) { if (s)bytes._ref(s, strlen<W>(s)*2); else _mknull(); }
	//除了单参的常文本指针构造是赋引用外其余构造内容的方法都是拷贝构造
	StrW() { _mknull(); }
	StrW(charW* arr) { _reset(arr, strlen<W>(arr)*2); }
	StrW(const charW* s, size_t len) { if (s)_reset(s, len*2); else _mknull(); }
	StrW(const StrW& s) { _reset(s.bytes.buf, s.bytes.size); }
	StrW(StrW&& s) noexcept : bytes((Bytes&&)s.bytes) {}
	StrW(charW c, size_t len) {
		reset(len); charW* buf = (charW*)bytes.buf;
		for (size_t i = 0; i < len; ++i)buf[i] = c;
	}
	explicit StrW(size_t len) { reset(len); }

	StrW& operator=(const charW* s) {
		_reset(s, strlen<W>(s)*2);
		return *this;
	}
	StrW& operator=(const StrW& s) {
		if (this != &s) { _reset(s.bytes.buf, s.bytes.size); }
		return *this;
	}
	StrW& operator=(const StrX& s);
	StrW& operator=(StrW&& s) noexcept {
		if (this != &s) {
			bytes = (Bytes&&)s.bytes;
			s._mknull();
		}
		return *this;
	}
	StrW& operator+=(const charW* s) {
		if (s==nullptr)return *this;
		return _append(s, strlen<W>(s)*2);
	}
	StrW& operator+=(const StrW& s) {
		if (s.bytes.buf==nullptr)return *this;
		return _append(s.bytes.buf, s.bytes.size);
	}
	StrW operator+(const charW* s) const {
		if (s==nullptr)return *this;
		size_t add = strlen<W>(s); StrW r(len()+add);
		r.bytes.copyFrom(bytes.buf, bytes.size);
		r.bytes.copyFrom(bytes.size, s, add*2);
		return r;
	}

	StrW operator+(const StrW& s) const {
		if (s.bytes.buf==nullptr)return *this;
		size_t add = s.len(); StrW r(len()+add);
		r.bytes.copyFrom(bytes.buf, bytes.size);
		r.bytes.copyFrom(bytes.size, s, add*2);
		return r;
	}
	friend StrW operator+(const charW* s1, const StrW& s2)
	{
		size_t len1 = s1 ? strlen<W>(s1) : 0; StrW r(len1 + s2.len());
		r.bytes.copyFrom(s1, len1*2);
		r.bytes.copyFrom(len1*2, s2.bytes.buf, s2.bytes.size);
		return r;
	}
	bool operator==(const charW* str) const {
		if (bytes.buf==NULL)return(str==NULL || *str==0);
		if (str==NULL)return(false);
		return strcmp<W>((charW*)bytes.buf, str)==0;
	}
	bool operator!=(const charW* str) const { return !(*this==str); }
	bool operator==(const StrW& str) const { return bytes == str.bytes; }
	bool operator!=(const StrW& str) const { return !(operator==(str)); }
	friend bool operator==(const charW* str, const StrW& s) { return s == str; }
	friend bool operator!=(const charW* str, const StrW& s) { return s != str; }
	friend bool operator==(charW* str, const StrW& s) { return s == (const charW*)str; }
	friend bool operator!=(charW* str, const StrW& s) { return s != (const charW*)str; }

	/**带越界检查的字符访问（以 charW 字符为单位，非字节）
	 * @param i 字符下标（允许负数，-1 = 最后一个字符）
	 * @throw be::range_error 下标越界时抛出，length 为字符数，index 为传入值
	 */
	charW& at(ssize_t i) const {
		ssize_t wlen = (ssize_t)len();
		if (i < 0) i += wlen;
		if (i < 0 || i >= wlen) throw be::range_error{ wlen, i };
		return ((charW*)bytes.buf)[i];
	}

	explicit operator bool() const { return bytes.size!=0; }

	operator charW*() const { return (charW*)bytes.buf; }
private:
	/**在bytes内部reset时会默认多分配2字节且该函数已自动填充空终止*/
	void _reset(size_t size) {
		if (size) {
			bytes.reset(size);
			*(charW*)(bytes.buf + size) = 0;
		} else _mknull();
	}
	void _reset(const void* s, size_t size) { _reset(size); bytes.copyFrom(s, size); }
	void _mknull() { bytes.size = bytes.capacity_ = 0; bytes.buf = (byte*)_W(""); }
	StrW& _append(const void* s, size_t size2) {
		if (size2==0)return *this;
		size_t oldSize = bytes.size;
		bytes.append(size2); //统一走Bytes::append的倍增扩容策略
		//确保空终止有空间(Bytes::append不感知字符串空终止)
		if (bytes.size+2>bytes._capacity())bytes._capacity(bytes.size+2);
		//由于自身就已经有空终止了总大小再加进无空终止的s是正好的
		bytes.copyFrom(oldSize, s, size2+2);
		return *this;
	}
};

/**到字节集的引用版**/
using BR = Bytes::Ref;
static const Bytes::RefTag BRef;
inline BR::Ref(const StrA& s) { p = s._buf(); size = s.bytes.size; }
inline BR::Ref(const StrW& s) { p = s._buf(); size = s.bytes.size; }

/**提供从字符串指针保留空终止的引用**/
inline BR SR(const char* s) { return BR(s, strlen(s)+1); }
inline BR SR(const charW* s) { return BR(s, (strlen<W>(s)+1)*2); }
inline BR SR(const StrA& s) { return BR(s._buf(), s.bytes.size+1); }
inline BR SR(const StrW& s) { return BR(s._buf(), s.bytes.size+2); }
inline BR SR(StrA&& s) = delete; BR SR(StrW&& s) = delete;

inline Bytes BA(Bytes&&b) { return Bytes((Bytes&&)b, b.size ? b.size-1 : 0); }
inline Bytes BA(Bytes& b) { return Bytes(b, BRef, b.size ? b.size-1 : 0); }
inline Bytes BW(Bytes&&b) { return Bytes((Bytes&&)b, b.size ? b.size-2 : 0); }
inline Bytes BW(Bytes& b) { return Bytes(b, BRef, b.size ? b.size-2 : 0); }

inline constexpr byte BNULLA[] = { 0 }; inline constexpr byte BNULLW[] = { 0, 0 };

//如果使用0填充模式代表当前字节集已经恰好字符串大小只是需要填充0
//常规非0填充的模式适用于原本已经有空终止的字节集只需把其大小-1
template<bool PADZERO = false>
inline StrA BSA(Bytes&&b) {
	StrA s; s.bytes._move(b);
	if constexpr (PADZERO)s.bytes += BNULLA;
	if (s.bytes.size)s.bytes.size -= 1;
	return s;
}
template<bool PADZERO = false>
inline StrA BSA(const Bytes& b) {
	StrA s; if constexpr (PADZERO) {
		(Bytes&)b += BNULLA; ((Bytes&)b).size -= 1;
		s.bytes.ref(b.buf, b.size);
		return s;
	}
	s.bytes.ref(b.buf, b.size ? b.size -1 : 0);
	return s;
}
inline StrA _BSA(const void* p, size_t size) {
	StrA s; s.bytes.ref(p, size);
	return s;
}
inline StrW _BSW(const void* p, size_t size) {
	StrW s; s.bytes.ref(p, size);
	return s;
}
template<bool PADZERO = false>
inline StrW BSW(Bytes&&b) {
	StrW s; s.bytes._move(b);
	if constexpr (PADZERO)s.bytes += BNULLW;
	if (s.bytes.size)s.bytes.size -= 2;
	return s;
}
template<bool PADZERO = false>
inline StrW BSW(const Bytes& b) {
	StrW s; if constexpr (PADZERO) {
		(Bytes&)b += BNULLW; ((Bytes&)b).size -= 2;
		s.bytes.ref(b.buf, b.size);
		return s;
	}
	s.bytes.ref(b.buf, b.size ? b.size -2 : 0);
	return s;
}

/**从任意字面量数组构造字节集引用**/
template <typename T, int N>
inline Bytes ArrayBytes(T(&arr)[N])
{
	return Bytes((void*)arr, BRef, sizeof(T)*N);
}


#pragma region 全局工具函数
template <typename T>
inline Bytes ToBytes(const T& a) {
	return Bytes(&a, sizeof(T));
}
inline Bytes ToBytes(const char* a) {
	return Bytes(a, strlen(a));
}
inline Bytes ToBytes(const charW* a) {
	return Bytes(a, strlen<W>(a)*2);
}
inline Bytes ToBytes(const StrA& a) {
	return Bytes(a.bytes);
}
inline Bytes ToBytes(const StrW& a) {
	return Bytes(a.bytes);
}
inline Bytes ToBytes(const Bytes& a) {
	return Bytes(a);
}

#pragma warning(push)
#pragma warning(disable:4146) //无符号取反会报警告，但实际是安全的
#pragma warning(disable:4996) //屏蔽sprintf不安全库的警告

template<bool SIGNED, typename T, typename CHART>
inline CHART* __my_itoa(T val, CHART* p)
{
	//如果是有符号且<0，则写'-'并取相反数
	if (SIGNED) { //VS2013不支持constexpr，但这样尽可能让编译器优化展开
		if (val < 0) {
			*p++ = (CHART)('-');
			val = -val;
		}
	}T u = val; CHART* start = p;
	do {
		*p++ = (CHART)('0' + (u % 10));
		u /= 10;
	} while (u);
	CHART* end = p - 1;
	while (start < end) {
		CHART c = *start;
		*start++ = *end;
		*end-- = c;
	}
	return p; //指向尾部（不含'\0'）
}

inline StrA sprintF(const char* format, ...)
{
	va_list args; va_start(args, format);
	int len = 0; va_list args_copy;
	va_copy(args_copy, args);
#ifdef _MSC_VER
	len = _vscprintf(format, args_copy);  // 不含 '\0'
#else
	len = vsnprintf(nullptr, 0, format, args_copy); //返回所需长度
#endif
	va_end(args_copy);
	StrA str(len);
	vsnprintf(str, len+1, format, args);
	va_end(args);
	return str;
}

#ifdef _WIN32
/*Win32: 直接使用 wchar_t 的 API*/
template<class T> StrW sprintF(const charW* format, ...)
{
	va_list args; va_start(args, format);
	int len = 0; va_list args_copy;
	va_copy(args_copy, args);
#ifdef _MSC_VER
	len = _vscwprintf(format, args_copy);  //不含 '\0'
#else
	len = vswprintf(nullptr, 0, format, args_copy); //返回所需长度
#endif
	va_end(args_copy);
	StrW str(len);
	vswprintf(str, len+1, format, args);
	va_end(args);
	return str;
}
#else
StrU8 WtoU8(const StrW& wstr); StrW U8toW(const StrU8& str);

// 变参模板辅助类：用于拦截 charW* 参数并转为 UTF-8 (StrA)
template<typename T> struct _SWArg {
	T& v; _SWArg(const T& v) : v(v) {}
	T& V() const { return v; }
};
template<> struct _SWArg<const charW*> {
	StrA s; _SWArg(const charW* v) : s(WtoU8(v)) {}
	byte* V() const { return s._buf(); }
};
template<> struct _SWArg<charW*> {
	StrA s; _SWArg(const charW* v) : s(WtoU8(v)) {}
	byte* V() const { return s._buf(); }
};
template<typename T, typename... Args>
inline StrW sprintF(const charW* format, const Args&... args)
{
	StrA fmt = WtoU8(format);
	int len = snprintf(nullptr, 0, (char*)fmt._buf(), _SWArg<Args>(args).V()...);
	StrU8 out(len);
	snprintf((char*)out._buf(), len + 1, (char*)fmt._buf(), _SWArg<Args>(args).V()...);
	return U8toW(out);
}
#endif

/**将ASCII转到UCS2（源字符串必须有两倍的空间）**/
inline void __AtoW(const char* buf, size_t slen) {
	for (int i = slen; i >= 0; i--)
		((charW*)buf)[i] = (charW)buf[i];
}
inline StrW _AtoW(const StrA& s) {
	size_t size = s.bytes.size;
	StrW w(size); w.bytes.copyFrom(s, size);
	__AtoW((char*)w._buf(), size);
	return w;
}
/**十进制数值转换到ASCII/宽字符（按 IsW 分发），仅Str版**/
template<class T, bool IsW, bool SIGN = false, typename TYPE = int>
inline T _ToStr(TYPE a) {
	if constexpr (IsW) {
		charW out[22]; charW* end = __my_itoa<SIGN, TYPE, charW>(a, out);
		return T(out, (int)(end - out));
	} else {
		char out[22]; char* end = __my_itoa<SIGN, TYPE, char>(a, out);
		return T(out, (int)(end - out));
	}
}
template<class T, bool IsW>
inline T _ToStr(float a) {
	char buf[128];
	int len = sprintf(buf, "%.7g", a);
	if constexpr (IsW) {
		__AtoW(buf, len);
		return T((charW*)buf, len);
	} else {
		return T(buf, len);
	}
}
template<class T, bool IsW>
inline T _ToStr(double a) {
	char buf[128];
	int len = sprintf(buf, "%.15g", a);
	if constexpr (IsW) {
		__AtoW(buf, len);
		return T((charW*)buf, len);
	} else {
		return T(buf, len);
	}
}

//ToStr(返回StrA)
inline StrA ToStr(int a) { return _ToStr<StrA, false, true>(a); }
inline StrA ToStr(unsigned int a) { return _ToStr<StrA, false, false, uint>(a); }
inline StrA ToStr(long a) { return _ToStr<StrA, false, true>(a); }
inline StrA ToStr(unsigned long a) { return _ToStr<StrA, false, false, uint>(a); }
inline StrA ToStr(int64 a) { return _ToStr<StrA, false, true, int64>(a); }
inline StrA ToStr(uint64 a) { return _ToStr<StrA, false, false, uint64>(a); }
inline StrA ToStr(float a) { return _ToStr<StrA, false>(a); }
inline StrA ToStr(double a) { return _ToStr<StrA, false>(a); }
inline StrA ToStr(const Bytes& a) { return StrA((char*)a.buf, a.size); }
inline StrA ToStr(BR a) { return StrA((char*)a.p, a.size); }
inline StrA ToStr(const StrA& a) { return StrA(a); }
//ToStr(返回StrW，通过 <W> 显式触发)
template<class T> StrW ToStr(int a) { return _ToStr<StrW, true, true>(a); }
template<class T> StrW ToStr(unsigned int a) { return _ToStr<StrW, true, false, uint>(a); }
template<class T> StrW ToStr(long a) { return _ToStr<StrW, true, true>(a); }
template<class T> StrW ToStr(unsigned long a) { return _ToStr<StrW, true, false, uint>(a); }
template<class T> StrW ToStr(int64 a) { return _ToStr<StrW, true, true, int64>(a); }
template<class T> StrW ToStr(uint64 a) { return _ToStr<StrW, true, false, uint64>(a); }
template<class T> StrW ToStr(float a) { return _ToStr<StrW, true>(a); }
template<class T> StrW ToStr(double a) { return _ToStr<StrW, true>(a); }
template<class T> StrW ToStr(const Bytes& a) { return StrW((charW*)a.buf, a.size/2); }
template<class T> StrW ToStr(BR a) { return StrW((charW*)a.p, a.size/2); }
template<class T> StrW ToStr(const StrW& a) { return StrW(a); }

/**数值或字节集转字符串
 * - 若为数值则转换到平台ASCII，若为字节集则进行原始拷贝(补空终止)
 * @param a 支持任何整数和浮点数，以及字节集
 * @return 默认类型为平台版 StrPlat (Win32→W, Linux→A)
 */
template<typename T>
StrPlat ToStrPlat(const T& a) {
#ifdef _WIN32
	return ToStr<W>(a);
#else
	return ToStr(a);
#endif
}

inline void __WtoA(const charW* src, char* dst)
{
	while (*src) *dst++ = (char)(*src), src++;
	*dst = '\0';
}
inline int ToInt(const char* s) { return atoi(s); }
inline int ToInt(const charW* s)
{
	char d[12]; __WtoA(s, d);
	return atoi(d);
}
inline short ToShort(const char* s) { return atoi(s); }
inline short ToShort(const charW* s)
{
	char d[12]; __WtoA(s, d);
	return (short)atoi(d);
}
inline int64 ToLong(const char* s) { return atoll(s); }
inline int64 ToLong(const charW* s)
{
	char d[12]; __WtoA(s, d);
	return atoll(d);
}
inline float ToFloat(const char* s) { return strtof(s, NULL); }
inline float ToFloat(const charW* s)
{
	char d[16]; __WtoA(s, d);
	return strtof(d, NULL);
}
inline double ToDouble(const char* s) { return strtod(s, NULL); }
inline double ToDouble(const charW* s)
{
	char d[24]; __WtoA(s, d);
	return strtod(d, NULL);
}

#pragma warning(pop)
#pragma endregion

/**解字节集，字节集到可视化十进制文本值(与易语言输出一致)**/

/**对字节集进行十进制文本转换
 * @param bin 字节集
 * @param withBrackets=true 是否以 "{"开头 与 "}"结尾
 * @return
 */
inline StrA jzjj(const Bytes& bin, char split = ',', bool withBrackets = true)
{
	if (bin.buf == nullptr) return StrA();
	size_t len = bin.size;
	// 预分配保守最大空间：每个字节最多3位数字+1个分隔符，加上括号
	StrA out(len * 4 + 2); char* p = out;
	if (withBrackets) *p++ = '{';
	for (size_t i = 0; i < len; i++)
	{
		p = __my_itoa<false>(bin[i], p); *p++ = split;
	}
	if (len > 0) p--; // 回退最后一个分隔符
	if (withBrackets) *p++ = '}';
	*p = '\0';
	out.bytes.size = (size_t)(p - (char*)out.bytes.buf);
	return out;
}
template<class T> StrW jzjj(const Bytes& bin, char split = ',', bool withBrackets = true) {
	return _AtoW(jzjj(bin, split, withBrackets));
}

/**对字节集进行16进制文本转换
 * @param bin
 * @param withBrackets=false 是否以 "{"开头 与 "}"结尾
 * @return
 */
inline StrA jzjj16(const Bytes& bin, char split = ' ', bool withBrackets = true)
{
	static char tb[] = { '0', '1', '2', '3', '4', '5', '6'
		, '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	if (bin.buf == nullptr) return StrA();
	size_t len = bin.size;
	StrA outbuf(len * 3 + (withBrackets ? 2 : 0)); char* p = outbuf;
	if (withBrackets) *p++ = '{';
	for (size_t i = 0; i < len; i++)
	{
		*p++ = tb[(bin.buf[i] >> 4)];
		*p++ = tb[(bin.buf[i] & 0xF)];
		*p++ = split;
	}
	if (len > 0) p--; // 回退最后一个分隔符
	if (withBrackets) *p++ = '}';
	*p = '\0';
	outbuf.bytes.size = (size_t)(p - (char*)outbuf.bytes.buf);
	return outbuf;
}
template<class T> StrW jzjj16(const Bytes& bin, char split = ' ', bool withBrackets = false) {
	return _AtoW(jzjj16(bin, split, withBrackets));
}

//白易框架约定A版字面量==U8版(无论Win32还是Linux)
//#if defined(_BE_CHARSETUTF8) || !defined(_WIN32)
#define AtoW U8toW
#define WtoA WtoU8
#define AtoU8(str) str
#define U8toA(str) str
//#else
////以下均已成为历史（除非Win32上不依赖白易框架）
//#define AtoW GBKtoW
//#define WtoA WtoGBK
//#define AtoU8 GBKtoU8
//#define U8toA U8toGBK
//#endif

#ifdef _WIN32
/*Win32专用字符集转换函数*/
//#include <windows.h>
extern "C" {
	__declspec(dllimport)
		int __stdcall MultiByteToWideChar(
			unsigned int   CodePage,
			unsigned long  dwFlags,
			const char*    lpMultiByteStr,
			int            cbMultiByte,
			wchar_t*       lpWideCharStr,
			int            cchWideChar
		);
	__declspec(dllimport)
		int __stdcall WideCharToMultiByte(
			unsigned int    CodePage,
			unsigned long   dwFlags,
			const wchar_t*  lpWideCharStr,
			int             cchWideChar,
			char*           lpMultiByteStr,
			int             cbMultiByte,
			const char*     lpDefaultChar,
			int*            lpUsedDefaultChar
		);
}


/**GBKToUnicode(UCS2/UTF16)
 * @param str
 * @return
 */
inline StrW GBKtoW(const StrA& str)
{
	if (str.bytes.size==0)return StrW();
	// GBK最多转成同样多的UTF-16字符
	size_t len = str.bytes.size; StrW result(str.len());
	size_t wLen = MultiByteToWideChar(936, 0, str, (int)len, result, (int)len);
	if (wLen<=0) return StrW();
	result.bytes.size = wLen * 2;
	*(charW*)(result.bytes.buf + result.bytes.size) = 0;
	return result;
}
/**UnicodeToGBK
 * @param wstr
 * @return
 */
inline StrA WtoGBK(const StrW& wstr)
{
	if (wstr.bytes.size==0)return StrA();
	// UTF-16转GBK，每个字符最多2字节（W.size与wlen的关系是自动*2）
	size_t wLen = wstr.len(); StrA result(wstr.bytes.size);
	size_t aLen = WideCharToMultiByte(936, 0, wstr, (int)wLen, result, (int)wLen * 2, 0, 0);
	if (aLen==0) return StrA();
	result.bytes.size = aLen;
	*(char*)(result.bytes.buf + aLen) = 0;
	return result;
}

/**UnicodeToUTF8
 * @param wstr
 * @return
 */
inline StrU8 WtoU8(const StrW& wstr)
{
	if (wstr.bytes.size==0)return StrU8();
	// UTF-16转UTF-8，每个字符最多3字节
	size_t wLen = wstr.len(); StrU8 result(wLen * 3);
	size_t utf8Len = WideCharToMultiByte(65001, 0, wstr, (int)wLen, result, (int)wLen * 3, 0, 0);
	if (utf8Len==0) return StrU8();
	result.bytes.size = utf8Len;
	*(char*)(result.bytes.buf + utf8Len) = 0;
	return result;
}
/**GBKToUTF8
 * @param str
 * @return
 */
inline StrU8 GBKtoU8(const StrA& str)
{
	if (str.bytes.size==0)return StrU8();
	// GBK->UTF-16->UTF-8，预估最大字节数
	size_t len = str.bytes.size; StrW unicode(len);
	size_t wLen = MultiByteToWideChar(936, 0, str, (int)len, unicode, (int)len);
	if (wLen==0) return StrU8();
	StrU8 utf8(wLen * 3);
	size_t u8Len = WideCharToMultiByte(65001, 0, unicode, (int)wLen, utf8, (int)wLen * 3, 0, 0);
	if (u8Len==0) return StrU8();
	utf8.bytes.size = u8Len;
	*(char*)(utf8.bytes.buf + u8Len) = 0;
	return utf8;
}
/**UTF8toGBK
 * @param str
 * @return
 */
inline StrA U8toGBK(const StrU8& str)
{
	if (str=="")return StrA();
	// UTF-8->UTF-16->GBK，预估最大字节数
	size_t len = str.bytes.size; StrW unicode(len);
	size_t wLen = MultiByteToWideChar(65001, 0, str, (int)len, unicode, (int)len);
	if (wLen==0) return StrA();
	StrA result(wLen * 2);
	size_t aLen = WideCharToMultiByte(936, 0, unicode, (int)wLen, result, (int)wLen * 2, 0, 0);
	if (aLen==0) return StrA();
	result.bytes.size = aLen;
	*(char*)(result.bytes.buf + aLen) = 0;
	return result;
}
/**UTF8toUCS2/UTF16
 * @param str
 * @return Bytes
 */
inline StrW U8toW(const StrU8& str)
{
	if (str=="")return StrW();
	// UTF-8转UTF-16，预估最大字符数
	StrW result(str.bytes.size);
	size_t chars = MultiByteToWideChar(65001, 0, str, (int)str.bytes.size, result, (int)str.bytes.size);
	if (chars==0)return StrW();
	result.bytes.size = chars * 2;
	*(charW*)(result.bytes.buf + result.bytes.size) = 0;
	return result;
}
#else
/*Linux: charW=unsigned short, 用iconv做UTF-16LE↔UTF-8转换*/
#include <iconv.h>

/**iconv转换的通用辅助函数
 * @param from_charset 源字符集
 * @param to_charset 目标字符集
 * @param inbuf 输入缓冲区
 * @param inlen 输入字节数
 * @param outbuf 输出缓冲区
 * @param outsize 输出缓冲区大小
 * @return 实际转换的输出字节数，失败返回-1
 */
inline int iconv_convert(const char* from_charset, const char* to_charset,
	const void* inbuf, int inlen, void* outbuf, int outsize)
{
	if (inlen == 0) return 0;

	iconv_t cd = iconv_open(to_charset, from_charset);
	if (cd == (iconv_t)-1) return -1;

	char* in = (char*)inbuf;
	char* out = (char*)outbuf;
	size_t inleft = (size_t)inlen;
	size_t outleft = (size_t)outsize;

	size_t ret = iconv(cd, &in, &inleft, &out, &outleft);
	iconv_close(cd);

	if (ret == (size_t)-1) return -1;
	return (int)(outsize - outleft);
}

/**GBK到Unicode(UTF-16LE)
 * @param str
 * @return
 */
inline StrW GBKtoW(const StrA& str)
{
	if (str.bytes.size == 0) return StrW();

	// 预估最大需要的字节数(GBK最多转成同样多的UTF-16字符)
	int maxBytes = str.bytes.size * 2;
	StrW result(maxBytes / 2); // 按字符数分配(会自动+2字节)

	int outLen = iconv_convert("GBK", "UTF-16LE", str._buf(), str.bytes.size, result.bytes.buf, maxBytes);
	if (outLen <= 0) return StrW();

	result.bytes.size = outLen;
	*(charW*)(result.bytes.buf + outLen) = 0; // 添加空终止
	return result;
}

/**Unicode到GBK
 * @param wstr
 * @return
 */
inline StrA WtoGBK(const StrW& wstr)
{
	if (wstr.bytes.size == 0) return StrA();

	// UTF-16转GBK，预估最大字节数
	int maxBytes = wstr.bytes.size * 2; // 最坏情况每个UTF-16字符转成2字节GBK
	StrA result(maxBytes);

	int outLen = iconv_convert("UTF-16LE", "GBK", wstr._buf(), wstr.bytes.size, result.bytes.buf, maxBytes);
	if (outLen <= 0) return StrA();

	result.bytes.size = outLen;
	*(char*)(result.bytes.buf + outLen) = 0; // 添加空终止
	return result;
}

/**Unicode到UTF8
 * @param wstr
 * @return
 */
inline StrU8 WtoU8(const StrW& wstr)
{
	if (wstr.bytes.size == 0) return StrU8();

	// UTF-16转UTF-8，预估最大字节数(每个UTF-16字符最多转成3字节UTF-8)
	int maxBytes = wstr.bytes.size * 3;
	StrU8 result(maxBytes);

	int outLen = iconv_convert("UTF-16LE", "UTF-8", wstr._buf(), wstr.bytes.size, result.bytes.buf, maxBytes);
	if (outLen <= 0) return StrU8();

	result.bytes.size = outLen;
	*(char*)(result.bytes.buf + outLen) = 0; // 添加空终止
	return result;
}

/**GBK到UTF8
 * @param str
 * @return
 */
inline StrU8 GBKtoU8(const StrA& str)
{
	if (str.bytes.size == 0) return StrU8();

	// GBK转UTF-8，预估最大字节数
	int maxBytes = str.bytes.size * 3;
	StrU8 result(maxBytes);

	int outLen = iconv_convert("GBK", "UTF-8", str._buf(), str.bytes.size, result.bytes.buf, maxBytes);
	if (outLen <= 0) return StrU8();

	result.bytes.size = outLen;
	*(char*)(result.bytes.buf + outLen) = 0; // 添加空终止
	return result;
}

/**UTF8到GBK
 * @param str
 * @return
 */
inline StrA U8toGBK(const StrU8& str)
{
	if (str == "") return StrA();

	// UTF-8转GBK，预估最大字节数
	int maxBytes = str.bytes.size * 2;
	StrA result(maxBytes);

	int outLen = iconv_convert("UTF-8", "GBK", str._buf(), str.bytes.size, result.bytes.buf, maxBytes);
	if (outLen <= 0) return StrA();

	result.bytes.size = outLen;
	*(char*)(result.bytes.buf + outLen) = 0; // 添加空终止
	return result;
}

/**UTF8到Unicode(UTF-16LE)
 * @param str
 * @return
 */
inline StrW U8toW(const StrU8& str)
{
	if (str == "") return StrW();

	// UTF-8转UTF-16，预估最大字节数
	int maxBytes = str.bytes.size * 2;
	StrW result(str.bytes.size); // 按字符数分配(会自动+2字节)

	int outLen = iconv_convert("UTF-8", "UTF-16LE", str._buf(), str.bytes.size, result.bytes.buf, maxBytes);
	if (outLen <= 0) return StrW();

	result.bytes.size = outLen;
	*(charW*)(result.bytes.buf + outLen) = 0; // 添加空终止
	return result;
}
#endif // _WIN32

/**UTF-8 转平台字符串（U8 → StrPlat）
 * Windows: StrU8 → StrW
 * Linux:   直接返回（StrA 在 Linux 上即是 UTF-8）
 * @param str UTF-8 编码的字符串
 * @return 平台原生字符串
 */
inline StrPlat U8ToPlat(const StrU8& str)
{
#ifdef _WIN32
	return U8toW(str);
#else
	return str;
#endif
}

/**平台字符串转 UTF-8（StrPlat → U8）
 * Windows: StrW → StrU8
 * Linux:   直接返回（StrA 在 Linux 上即是 UTF-8）
 * @param str 平台原生字符串
 * @return UTF-8 编码的字符串
 */
inline StrU8 PlatToU8(const StrPlat& str)
{
#ifdef _WIN32
	return WtoU8(str);
#else
	return str;
#endif
}

namespace be {
template <typename T>
constexpr bool is_AutoStr_builtin =
be::is_string_ptr<be::decay<T>> ||
be::is_null_pointer<be::decay<T>> ||
be::is_numeric<be::decay<T>> ||
be::sames<be::decay<T>, StrA, StrW, StrU8, StrX, Bytes, BR, AutoStr>;
}

template <typename T, typename = be::enable_if<be::is_AutoStr_builtin<T>>>
AutoStr __AutoStr__(const T& v);

class AutoStr
{
public:
	const StrU8 str; //为了能够安全缓存W版故这里必须保持str指针不变
	size_t len() const { return str.len(); }

	AutoStr() : str() {}

	AutoStr(int v) : str(ToStr(v)) {}
	AutoStr(uint v) : str(ToStr(v)) {}
	AutoStr(long v) : str(ToStr(v)) {}
	AutoStr(ulong v) : str(ToStr(v)) {}
	AutoStr(int64 v) : str(ToStr(v)) {}
	AutoStr(uint64 v) : str(ToStr(v)) {}
	AutoStr(float v) : str(ToStr(v)) {}
	AutoStr(double v) : str(ToStr(v)) {}

	AutoStr(const Bytes& v) : str(jzjj(v)) {}
	AutoStr(BR v) : str(jzjj(v)) {}
	AutoStr(const StrA& v) : str(AtoU8(v)) {}
	AutoStr(const StrW& v) : str(WtoU8(v)) {}
	AutoStr(const StrU8& v) : str(v) {}
	AutoStr(const char* v) : str(AtoU8(v)) {}
	AutoStr(const charW* v) : str(WtoU8(v)) {}

	template <typename T, typename = be::enable_if<!be::is_AutoStr_builtin<T>>>
	AutoStr(const T& v) : str(__AutoStr__(v).str) {}

	operator const char*() const { return str; }
	operator const charW*() const { return w(); }
	operator char*() const { return str; }
	operator charW*() const { return w(); }
	operator const StrA&() const { return str; }
	operator const StrW&() const { return w(); }
	operator const StrU8&() const { return str; }

	AutoStr& operator+=(const StrU8& s) {
		if (s.bytes.buf==nullptr)return *this;
		(StrU8&)str += s; _flushW();
		return *this;
	}

#ifdef __cpp_char8_t
	AutoStr(const char8_t* v) : str((const char*)v) {}
	operator const char8_t*() const { return (const char8_t*)str.bytes.buf; }
#endif

	// 支持自身进行 << 流式拼接
	template<typename T>
	AutoStr& operator<<(const T& v) {
		const_cast<StrU8&>(str) += AutoStr(v).str;
		return *this;
	}

	void _flushW() { _p = nullptr; }

	explicit operator bool() const { return str.bytes.size!=0; }
	const StrW& w() const {
		if (_p != (char*)str) {
			_w = U8toW(str);
			_p = (char*)str;
		}
		return _w;
	}
private:
	mutable StrW _w;
	mutable char* _p = nullptr;
};

using c_AutoStr = const AutoStr&;
/**严格仅接受文本类型输入的AutoStr（杜绝误触传入整数等参数）*/
class StrX : public AutoStr
{
public:
	StrX() : AutoStr() {}
	StrX(const char* v) : AutoStr(v) {}
	StrX(const charW* v) : AutoStr(v) {}
	StrX(const StrA& v) : AutoStr(v) {}
	StrX(const StrW& v) : AutoStr(v) {}
	StrX(const StrU8& v) : AutoStr(v) {}
#ifdef __cpp_char8_t
	StrX(const char8_t* v) : AutoStr(v) {}
#endif
};
using c_StrX = const StrX&;

inline StrX operator+(c_StrX a, c_StrX b) {
	StrX r; (StrU8&)r.str = a.str + b.str; return r;
}

inline StrA& StrA::operator=(const StrX& s) {
	return *this = s.str;
}
inline StrW& StrW::operator=(const StrX& s) {
	return *this = (const StrW&)s;
}
inline StrU8& StrU8::operator=(const StrX& s) {
	return *this = s.str;
}

using c_Bytes = const Bytes&;
using c_StrA = const StrA&;
using c_StrU8 = const StrU8&;
using c_StrW = const StrW&;