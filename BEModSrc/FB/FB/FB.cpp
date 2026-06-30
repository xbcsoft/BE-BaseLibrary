/**@ModuleTitle: 白菜通信封包
*  @version:     1.0
*  @platform:    win32(x86|x64)
*  @compiler:    source
*  @author:
*  @datetime:
*  @description: 基于白菜跨语言数据类型封包与解包标准V1.0所设计的FB/deFB
*/
#include "stdafx.h"

byte* gFB_p(byte* dat, int i)
{
	return dat+(*(int*)(dat+4*(i + 1)));
}
byte* gFB_p(byte* dat, int i, size_t& size)
{
	byte* p = dat+(*(int*)(dat+4*(i + 1)));
	byte* p2 = dat+(*(int*)(dat+4*(i + 2)));
	size = p2-p; return p;
}
int gFBi(byte* dat, int i)
{
	return *(int*)gFB_p(dat, i);
}
double gFBd(byte* dat, int i)
{
	return *(double*)gFB_p(dat, i);
}
float gFBf(byte* dat, int i)
{
	return *(float*)gFB_p(dat, i);
}
bool gFBb(byte* dat, int i)
{
	return *(bool*)gFB_p(dat, i);
}
StrA gFBs(byte* dat, int i)
{
	size_t sz = 0;
	byte* p = gFB_p(dat, i, sz);
	return ToStr(BR(p, sz));
}
template<class StrT = W>
StrW gFBs(byte* dat, int i)
{
	size_t sz = 0;
	byte* p = gFB_p(dat, i, sz);
	return ToStr<W>(BR(p, sz));
}
Bytes gFB(byte* dat, int i)
{
	Bytes b; b.buf = gFB_p(dat, i, b.size);
	return b;
}
template<class T>
T& gFBpo(byte* dat, int i)
{
	T** p = (T**)gFB_p(dat, i);
	if (*p) {
		return **p;
	}
	static T _o = T();
	return _o;
}

template<typename T> BR __FB__(const T& a) { return BR(&a, sizeof(T)); }
BR __FB__(const char* a) { return BR(a); }
BR __FB__(const charW* a) { return BR(a); }
BR __FB__(const StrA& a) { return BR(a.bytes.buf, a.bytes.size); }
BR __FB__(const StrW& a) { return BR(a.bytes.buf, a.bytes.size); }
BR __FB__(const Bytes& a) { return BR(a.buf, a.size); }

template<typename T>
constexpr bool fb_is_var_len = be::sames<be::remove_cv<T>, Bytes, StrA, StrW, StrU8, StrX>
|| be::is_Arraybe<be::remove_cv<T>>;

template<typename T> Bytes build_FBPart(const T& a);

template<class T>
Bytes _FB_Array(const Arraybe<T>& aa)
{
	int n = aa.count;
	if constexpr (fb_is_var_len<T>) {
		byte type = be::same<T, Bytes> ? 'b' : 's';
		Arraybe<Bytes> parts;
		for (int i = 0; i < n; i++) {
			parts.push(build_FBPart(aa[i]));
		}
		int k = (n + 2) * 4, m = k;
		Bytes out(m);
		byte* p = out.buf;
		*((int*)p) = n;
		for (int i = 0; i < n; i++) {
			p += 4; *((int*)p) = m;
			m += parts[i].size;
		}
		p += 4; *((int*)p) = m;
		out = out + Bytes(m - k + 1);
		p = out.buf + k;
		for (int i = 0; i < n; i++) {
			Bytes(p, BRef).copyFrom(parts[i]);
			p += parts[i].size;
		}
		*p = type;
		return out;
	} else {
		int m = 4 + n * sizeof(T) + 1;
		Bytes out(m);
		byte* p = out.buf;
		*((int*)p) = n;
		p += 4;
		for (int i = 0; i < n; i++) {
			*(T*)p = aa[i];
			p += sizeof(T);
		}
		*p = (byte)sizeof(T);
		return out;
	}
}

template<typename T>
Bytes build_FBPart(const T& a) {
	if constexpr (be::is_Arraybe<T>) {
		return _FB_Array(a);
	} else {
		BR br = __FB__(a);
		return Bytes(br.p, BRef, br.size);
	}
}

/*FB可变参模板*/
template<typename... T>
Bytes FB(const T&... aa)
{
	Bytes parts[] = { build_FBPart(aa)... };
	int n = (int)(sizeof...(T));
	int k = (n + 2) * 4;
	int payload = 0;
	for (int i = 0; i < n; i++) payload += parts[i].size;

	Bytes out(k + payload);
	byte* p = out.buf;
	*((int*)p) = n;

	int m = k;
	for (int i = 0; i < n; i++)
	{
		p += 4;
		*((int*)p) = m;
		m += (int)parts[i].size;
	}
	p += 4;
	*((int*)p) = m;

	p = out.buf + k;
	for (int i = 0; i < n; i++)
	{
		Bytes(p, BRef).copyFrom(parts[i]);
		p += parts[i].size;
	}
	return out;
}

/*deFB解包提取*/
template<typename T>
void _deFB_get(byte* dat, int i, T& out) {
	out = *(T*)gFB_p(dat, i);
}

void _deFB_get(byte* dat, int i, StrA& out) {
	size_t sz = 0;
	byte* p = gFB_p(dat, i, sz);
	out = ToStr(BR(p, sz));
}

void _deFB_get(byte* dat, int i, StrW& out) {
	size_t sz = 0;
	byte* p = gFB_p(dat, i, sz);
	out = ToStr<W>(BR(p, sz));
}

void _deFB_get(byte* dat, int i, Bytes& out) {
	size_t sz = 0;
	byte* p = gFB_p(dat, i, sz);
	out = Bytes(p, sz);
}

template<class T>
void _deFB_get(byte* dat, int i, Arraybe<T>& out) {
	size_t sz = 0;
	byte* p = gFB_p(dat, i, sz);
	if (sz < 4) return;
	int count = *(int*)p;
	out = Arraybe<T>();
	if constexpr (fb_is_var_len<T>) {
		for (int j = 0; j < count; j++) {
			T item;
			_deFB_get(p, j, item);
			out.push(item);
		}
	} else {
		byte* data_ptr = p + 4;
		for (int j = 0; j < count; j++) {
			T item = *(T*)data_ptr;
			out.push(item);
			data_ptr += sizeof(T);
		}
	}
}

void _deFB_impl(byte* dat, int i) {}

template<typename T, typename... Args>
void _deFB_impl(byte* dat, int i, T& first, Args&... rest) {
	_deFB_get(dat, i, first);
	_deFB_impl(dat, i + 1, rest...);
}

template<typename... Args>
void deFB(byte* dat, Args&... args) {
	_deFB_impl(dat, 0, args...);
}

template<typename... Args>
void deFB(const Bytes& fbData, Args&... args) {
	if (fbData.buf) _deFB_impl(fbData.buf, 0, args...);
}