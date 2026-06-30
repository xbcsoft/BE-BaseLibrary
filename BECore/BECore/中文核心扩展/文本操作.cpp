#include "../中文核心.hpp"

template<class StrT> _BETPL_DEF
size_t _取文本长度(const StrT& 文本数据) {
	return 文本数据.len();
}template size_t _取文本长度(const StrW& 文本数据);

template<class StrT = W>
size_t 取文本长度(const StrW& 文本数据) { return _取文本长度(文本数据); }

template<class StrT = U8>
size_t 取文本长度(const StrU8& 文本数据) { return 文本数据.lenU8(); }

/**取文本长度
 * @param 文本数据
 * @return
 */
size_t 取文本长度(const StrA& 文本数据) { return _取文本长度(文本数据); }


// ===== 取文本左边 =====
template<class StrT> _BETPL_DEF
StrT _取文本左边(const StrT& 欲取其部分的文本, size_t 欲取出字符的数目) {
	if (欲取出字符的数目 == 0) return StrT();
	if (欲取出字符的数目 >= 欲取其部分的文本.len()) return 欲取其部分的文本;
	return StrT(欲取其部分的文本, 欲取出字符的数目);
}template StrW _取文本左边(const StrW&, size_t);

template<class T = W>
StrW 取文本左边(const StrW& 欲取其部分的文本, size_t 欲取出字符的数目) {
	return _取文本左边(欲取其部分的文本, 欲取出字符的数目);
}
/**取文本左边
 * @param 欲取其部分的文本
 * @param 欲取出字符的数目
 * @return
 */
StrA 取文本左边(const StrA& 欲取其部分的文本, size_t 欲取出字符的数目) {
	return _取文本左边(欲取其部分的文本, 欲取出字符的数目);
}

// ===== 取文本右边 =====
template<class StrT> _BETPL_DEF
StrT _取文本右边(const StrT& 欲取其部分的文本, size_t 欲取出字符的数目) {
	size_t len = 欲取其部分的文本.len();
	if (欲取出字符的数目 == 0) return StrT();
	if (欲取出字符的数目 >= len) return 欲取其部分的文本;
	return StrT(&欲取其部分的文本[0] + (len - 欲取出字符的数目), 欲取出字符的数目);
}template StrW _取文本右边(const StrW&, size_t);

template<class T = W>
StrW 取文本右边(const StrW& 欲取其部分的文本, size_t 欲取出字符的数目) {
	return _取文本右边(欲取其部分的文本, 欲取出字符的数目);
}
/**取文本右边
 * @param 欲取其部分的文本
 * @param 欲取出字符的数目
 * @return
 */
StrA 取文本右边(const StrA& 欲取其部分的文本, size_t 欲取出字符的数目) {
	return _取文本右边(欲取其部分的文本, 欲取出字符的数目);
}

// ===== 取文本子串 =====
template<class StrT> _BETPL_DEF
StrT _取文本子串(const StrT& 欲取其部分的文本, size_t 起始取出位置, size_t 欲取出字符的数目) {
	size_t len = 欲取其部分的文本.len();
	if (起始取出位置 >= len || 欲取出字符的数目 == 0) return StrT();
	size_t count = 欲取出字符的数目;
	if (起始取出位置 + count > len) count = len - 起始取出位置;
	return StrT(&欲取其部分的文本[0] + 起始取出位置, count);
}template StrW _取文本子串(const StrW&, size_t, size_t);

template<class T = W>
StrW 取文本子串(const StrW& 欲取其部分的文本, size_t 起始取出位置, size_t 欲取出字符的数目) {
	return _取文本子串(欲取其部分的文本, 起始取出位置, 欲取出字符的数目);
}
/**取文本子串
 * @param 欲取其部分的文本
 * @param 起始取出位置
 * @param 欲取出字符的数目
 * @return
 */
StrA 取文本子串(const StrA& 欲取其部分的文本, size_t 起始取出位置, size_t 欲取出字符的数目) {
	return _取文本子串(欲取其部分的文本, 起始取出位置, 欲取出字符的数目);
}

// ===== 字符 =====
template<class StrT, class CharT> _BETPL_DEF
StrT _字符(byte 欲取其字符的字符代码) {
	CharT c = (CharT)欲取其字符的字符代码;
	return StrT(&c, 1);
}template StrW _字符<StrW, charW>(byte);

template<class T = W>
StrW 字符(byte 欲取其字符的字符代码) { return _字符<StrW, charW>(欲取其字符的字符代码); }
/**字符
 * @param 欲取其字符的字符代码
 * @return
 */
StrA 字符(byte 欲取其字符的字符代码) { return _字符<StrA, char>(欲取其字符的字符代码); }

// ===== 取代码 =====
template<class StrT> _BETPL_DEF
int _取代码(const StrT& 欲取字符代码的文本, size_t 欲取其代码的字符位置) {
	if (欲取其代码的字符位置 >= 欲取字符代码的文本.len()) return 0;
	return 欲取字符代码的文本[欲取其代码的字符位置];
}template int _取代码(const StrW&, size_t);

template<class T = W>
int 取代码(const StrW& 欲取字符代码的文本, size_t 欲取其代码的字符位置 = 0) {
	return _取代码(欲取字符代码的文本, 欲取其代码的字符位置);
}
/**取代码
 * @param 欲取字符代码的文本
 * @param 欲取其代码的字符位置=0
 * @return
 */
int 取代码(const StrA& 欲取字符代码的文本, size_t 欲取其代码的字符位置 = 0) {
	return _取代码(欲取字符代码的文本, 欲取其代码的字符位置);
}

// ===== 寻找文本 =====
namespace {
// ===== 内部辅助：大小写比较 =====
template<typename CharT>
CharT _ascii_tolower(CharT ch) {
	if (ch >= (CharT)'A' && ch <= (CharT)'Z') {
		return ch + (CharT)('a' - 'A');
	}
	return ch;
}

template<typename CharT>
int _strnicmp_ascii(const CharT* s1, const CharT* s2, size_t len) {
	if (len == 0) return 0;
	if (s1 == nullptr || s2 == nullptr) {
		return (s1 == s2) ? 0 : (s1 ? 1 : -1);
	}
	for (size_t i = 0; i < len; ++i) {
		CharT c1 = _ascii_tolower(s1[i]);
		CharT c2 = _ascii_tolower(s2[i]);
		if (c1 != c2) {
			return (c1 < c2) ? -1 : 1;
		}
		if (c1 == 0) {
			return 0;
		}
	}
	return 0;
}
}

// ===== 寻找文本 =====
template<class StrT, class CharT> _BETPL_DEF
ssize_t _寻找文本(const StrT& 被搜寻的文本, const StrT& 欲寻找的文本, size_t 起始搜寻位置, bool 是否不区分大小写) {
	size_t l1 = 被搜寻的文本.len();
	size_t l2 = 欲寻找的文本.len();
	if (l1 == 0 || l2 == 0 || 起始搜寻位置 >= l1 || l2 > l1) return -1;
	const CharT* s1 = (const CharT*)被搜寻的文本._buf();
	const CharT* s2 = (const CharT*)欲寻找的文本._buf();
	size_t maxI = l1 - l2;
	for (size_t i = 起始搜寻位置; i <= maxI; ++i) {
		bool match = 是否不区分大小写
			? (_strnicmp_ascii(s1 + i, s2, l2) == 0)
			: (memcmp(s1 + i, s2, l2 * sizeof(CharT)) == 0);
		if (match) return (ssize_t)i;
	}
	return -1;
}template ssize_t _寻找文本<StrW, charW>(const StrW&, const StrW&, size_t, bool);

template<class T = W>
ssize_t 寻找文本(const StrW& 被搜寻的文本, const StrW& 欲寻找的文本, size_t 起始搜寻位置 = 0, bool 是否不区分大小写 = false) {
	return _寻找文本<StrW, charW>(被搜寻的文本, 欲寻找的文本, 起始搜寻位置, 是否不区分大小写);
}
/**寻找文本
 * @param 被搜寻的文本
 * @param 欲寻找的文本
 * @param 起始搜寻位置=0
 * @param 是否不区分大小写=false
 * @return
 */
ssize_t 寻找文本(const StrA& 被搜寻的文本, const StrA& 欲寻找的文本, size_t 起始搜寻位置 = 0, bool 是否不区分大小写 = false) {
	return _寻找文本<StrA, char>(被搜寻的文本, 欲寻找的文本, 起始搜寻位置, 是否不区分大小写);
}

// ===== 倒找文本 =====
template<class StrT, class CharT> _BETPL_DEF
ssize_t _倒找文本(const StrT& 被搜寻的文本, const StrT& 欲寻找的文本, size_t 起始搜寻位置, bool 是否不区分大小写) {
	size_t len = 被搜寻的文本.len();
	size_t subLen = 欲寻找的文本.len();
	if (len == 0 || subLen == 0 || subLen > len) return -1;
	size_t startIdx = len - subLen;
	if (起始搜寻位置 != SIZE_MAX && 起始搜寻位置 < startIdx) startIdx = 起始搜寻位置;
	const CharT* s1 = (const CharT*)被搜寻的文本._buf();
	const CharT* s2 = (const CharT*)欲寻找的文本._buf();
	for (int i = (int)startIdx; i >= 0; i--) {
		bool match = 是否不区分大小写
			? (_strnicmp_ascii(s1 + i, s2, subLen) == 0)
			: (memcmp(s1 + i, s2, subLen * sizeof(CharT)) == 0);
		if (match) return i;
	}
	return -1;
}template ssize_t _倒找文本<StrW, charW>(const StrW&, const StrW&, size_t, bool);

template<class T = W>
ssize_t 倒找文本(const StrW& 被搜寻的文本, const StrW& 欲寻找的文本, size_t 起始搜寻位置 = SIZE_MAX, bool 是否不区分大小写 = false) {
	return _倒找文本<StrW, charW>(被搜寻的文本, 欲寻找的文本, 起始搜寻位置, 是否不区分大小写);
}
/**倒找文本
 * @param 被搜寻的文本
 * @param 欲寻找的文本
 * @param 起始搜寻位置=SIZE_MAX
 * @param 是否不区分大小写=false
 * @return
 */
ssize_t 倒找文本(const StrA& 被搜寻的文本, const StrA& 欲寻找的文本, size_t 起始搜寻位置 = SIZE_MAX, bool 是否不区分大小写 = false) {
	return _倒找文本<StrA, char>(被搜寻的文本, 欲寻找的文本, 起始搜寻位置, 是否不区分大小写);
}

// ===== 到大写 =====
template<class StrT, class CharT> _BETPL_DEF
StrT _到大写(const StrT& 欲变换的文本) {
	StrT ret = 欲变换的文本;
	size_t len = ret.len();
	CharT* p = (CharT*)ret.bytes.buf;
	for (size_t i = 0; i < len; ++i)
		if (p[i] >= (CharT)'a' && p[i] <= (CharT)'z') p[i] -= 32;
	return ret;
}template StrW _到大写<StrW, charW>(const StrW&);

template<class T = W>
StrW 到大写(const StrW& 欲变换的文本) { return _到大写<StrW, charW>(欲变换的文本); }
/**到大写
 * @param 欲变换的文本
 * @return
 */
StrA 到大写(const StrA& 欲变换的文本) {
	return _到大写<StrA, char>(欲变换的文本);
}

// ===== 到小写 =====
template<class StrT, class CharT> _BETPL_DEF
StrT _到小写(const StrT& 欲变换的文本) {
	StrT ret = 欲变换的文本;
	size_t len = ret.len();
	CharT* p = (CharT*)ret.bytes.buf;
	for (size_t i = 0; i < len; ++i)
		if (p[i] >= (CharT)'A' && p[i] <= (CharT)'Z') p[i] += 32;
	return ret;
}template StrW _到小写<StrW, charW>(const StrW&);

template<class T = W>
StrW 到小写(const StrW& 欲变换的文本) { return _到小写<StrW, charW>(欲变换的文本); }
/**到小写
 * @param 欲变换的文本
 * @return
 */
StrA 到小写(const StrA& 欲变换的文本) {
	return _到小写<StrA, char>(欲变换的文本);
}

// ===== 到全角/半角内部辅助 =====
namespace {
charW _ToFull(charW c) {
	if (c == 0x20) return 0x3000;
	if (c >= 0x21 && c <= 0x7E) return c + 0xFEE0;
	return c;
}
charW _ToHalf(charW c) {
	if (c == 0x3000) return 0x20;
	if (c >= 0xFF01 && c <= 0xFF5E) return c - 0xFEE0;
	return c;
}
}

template<class StrT> _BETPL_DEF
StrT _到全角(const StrT& 欲变换的文本) {
	if constexpr (be::same<StrT, StrW>) {
		StrW ret = 欲变换的文本;
		size_t len = ret.len();
		for (size_t i = 0; i < len; i++) ret[i] = _ToFull(ret[i]);
		return ret;
	} else {
		StrW w = AtoW(欲变换的文本);
		StrW retW = w;
		size_t len = retW.len();
		for (size_t i = 0; i < len; i++) retW[i] = _ToFull(retW[i]);
		return WtoA(retW);
	}
}template StrW _到全角<StrW>(const StrW&);

template<class T = W>
StrW 到全角(const StrW& 欲变换的文本) { return _到全角(欲变换的文本); }
/**到全角
 * @param 欲变换的文本
 * @return
 */
StrA 到全角(const StrA& 欲变换的文本) {
	return _到全角(欲变换的文本);
}

template<class StrT> _BETPL_DEF
StrT _到半角(const StrT& 欲变换的文本) {
	if constexpr (be::same<StrT, StrW>) {
		StrW ret = 欲变换的文本;
		size_t len = ret.len();
		for (size_t i = 0; i < len; i++) ret[i] = _ToHalf(ret[i]);
		return ret;
	} else {
		StrW w = AtoW(欲变换的文本);
		StrW retW = w;
		size_t len = retW.len();
		for (size_t i = 0; i < len; i++) retW[i] = _ToHalf(retW[i]);
		return WtoA(retW);
	}
}template StrW _到半角<StrW>(const StrW&);

template<class T = W>
StrW 到半角(const StrW& 欲变换的文本) { return _到半角(欲变换的文本); }
/**到半角
 * @param 欲变换的文本
 * @return
 */
StrA 到半角(const StrA& 欲变换的文本) {
	return _到半角(欲变换的文本);
}

// ===== 删首空 =====
template<class StrT, class CharT> _BETPL_DEF
StrT _删首空(const StrT& 欲删除空格的文本) {
	size_t len = 欲删除空格的文本.len();
	size_t i = 0;
	const CharT* p = (const CharT*)欲删除空格的文本;
	while (i < len) {
		CharT c = p[i];
		if (c == (CharT)0x20 || c == (CharT)'\t' || c == (CharT)'\r' || c == (CharT)'\n') { i++; continue; }
		break;
	}
	if (i == 0) return 欲删除空格的文本;
	if (i == len) return StrT();
	return StrT(p + i, len - i);
}template StrW _删首空<StrW, charW>(const StrW&);

template<class T = W>
StrW 删首空(const StrW& 文本数据) { return _删首空<StrW, charW>(文本数据); }
/**删首空
 * @param 欲删除空格的文本
 * @return
 */
StrA 删首空(const StrA& 欲删除空格的文本) {
	return _删首空<StrA, char>(欲删除空格的文本);
}

// ===== 删尾空 =====
template<class StrT, class CharT> _BETPL_DEF
StrT _删尾空(const StrT& 欲删除空格的文本) {
	size_t len = 欲删除空格的文本.len();
	if (len == 0) return StrT();
	int i = (int)len;
	const CharT* p = (const CharT*)欲删除空格的文本;
	while (i > 0) {
		CharT c = p[i - 1];
		if (c == (CharT)0x20 || c == (CharT)'\t' || c == (CharT)'\r' || c == (CharT)'\n') { i--; continue; }
		break;
	}
	if (i == (int)len) return 欲删除空格的文本;
	return StrT(p, i);
}template StrW _删尾空<StrW, charW>(const StrW&);

template<class T = W>
StrW 删尾空(const StrW& 文本数据) { return _删尾空<StrW, charW>(文本数据); }
/**删尾空
 * @param 欲删除空格的文本
 * @return
 */
StrA 删尾空(const StrA& 欲删除空格的文本) {
	return _删尾空<StrA, char>(欲删除空格的文本);
}

// ===== 删首尾空 =====
template<class T = W>
StrW 删首尾空(const StrW& txt) { return _删尾空<StrW, charW>(_删首空<StrW, charW>(txt)); }
/**删首尾空
 * @param txt
 * @return
 */
StrA 删首尾空(const StrA& txt) {
	return _删尾空<StrA, char>(_删首空<StrA, char>(txt));
}

// ===== 删全部空 =====
template<class StrT, class CharT> _BETPL_DEF
StrT _删全部空(const StrT& txt) {
	size_t len = txt.len();
	size_t count = 0;
	const CharT* p = (const CharT*)txt;
	for (size_t i = 0; i < len; i++) {
		CharT c = p[i];
		if (c != (CharT)0x20 && c != (CharT)'\t' && c != (CharT)'\r' && c != (CharT)'\n') count++;
	}
	if (count == len) return txt;
	StrT ret(count);
	CharT* dst = (CharT*)ret._buf();
	size_t n = 0;
	for (size_t i = 0; i < len; i++) {
		CharT c = p[i];
		if (c != (CharT)0x20 && c != (CharT)'\t' && c != (CharT)'\r' && c != (CharT)'\n') dst[n++] = c;
	}
	return ret;
}template StrW _删全部空<StrW, charW>(const StrW&);

template<class T = W>
StrW 删全部空(const StrW& txt) { return _删全部空<StrW, charW>(txt); }
/**删全部空
 * @param txt
 * @return
 */
StrA 删全部空(const StrA& txt) {
	return _删全部空<StrA, char>(txt);
}

// ===== 文本替换（按位置）=====
template<class StrT, class CharT> _BETPL_DEF
StrT _文本替换(const StrT& 欲被替换的文本, size_t 起始替换位置, size_t 替换长度, const StrT& 用作替换的文本) {
	size_t len = 欲被替换的文本.len();
	size_t start = 起始替换位置;
	if (start > len) start = len;
	size_t delLen = 替换长度;
	if (start + delLen > len) delLen = len - start;
	size_t replLen = 用作替换的文本.len();
	StrT ret(len - delLen + replLen);
	CharT* d = (CharT*)ret._buf();
	const CharT* s = (const CharT*)欲被替换的文本;
	if (start > 0) memcpy(d, s, start * sizeof(CharT));
	if (replLen > 0) memcpy(d + start, 用作替换的文本._buf(), replLen * sizeof(CharT));
	if (start + delLen < len) memcpy(d + start + replLen, s + start + delLen, (len - (start + delLen)) * sizeof(CharT));
	return ret;
}template StrW _文本替换<StrW, charW>(const StrW&, size_t, size_t, const StrW&);

template<class T = W>
StrW 文本替换(const StrW& 欲被替换的文本, size_t 起始替换位置, size_t 替换长度, const StrW& 用作替换的文本) {
	return _文本替换<StrW, charW>(欲被替换的文本, 起始替换位置, 替换长度, 用作替换的文本);
}
/**文本替换
 * @param 欲被替换的文本
 * @param 起始替换位置
 * @param 替换长度
 * @param 用作替换的文本
 * @return
 */
StrA 文本替换(const StrA& 欲被替换的文本, size_t 起始替换位置, size_t 替换长度, const StrA& 用作替换的文本) {
	return _文本替换<StrA, char>(欲被替换的文本, 起始替换位置, 替换长度, 用作替换的文本);
}

// ===== 子文本替换 =====
template<typename StrT, typename CharT> _BETPL_DEF
StrT _子文本替换(const StrT& 欲被替换的文本, const StrT& 欲被替换的子文本,
	const StrT& 用作替换的子文本, size_t 进行替换的起始位置, int 替换进行的次数, bool 是否区分大小写)
{
	if (欲被替换的子文本.len() == 0) return 欲被替换的文本;
	StrT ret;
	size_t extra = (sizeof(CharT) == sizeof(char)) ? 100 : 200;
	ret.bytes._capacity(欲被替换的文本.bytes.size + extra);

	size_t len = 欲被替换的文本.len();
	size_t subLen = 欲被替换的子文本.len();
	const CharT* s = (const CharT*)欲被替换的文本;
	size_t start = 进行替换的起始位置;
	if (start > len) start = len;

	if (start > 0) ret += StrT(s, start);

	size_t cur = start;
	int count = 0;
	while (cur < len) {
		if (替换进行的次数 != -1 && count >= 替换进行的次数) {
			ret.bytes.append(s + cur, (len - cur) * sizeof(CharT));
			break;
		}
		bool match = false;
		if (cur + subLen <= len) {
			const CharT* target = (const CharT*)欲被替换的子文本;
			if (是否区分大小写)
				match = (memcmp(s + cur, target, subLen * sizeof(CharT)) == 0);
			else
				match = (_strnicmp_ascii((CharT*)(s + cur), (CharT*)target, subLen) == 0);
		}
		if (match) {
			if (用作替换的子文本.len() > 0) ret += 用作替换的子文本;
			cur += subLen;
			count++;
		} else {
			ret.bytes.append(s + cur, sizeof(CharT));
			cur++;
		}
	}
	if (ret.bytes.buf) {
		size_t sz = ret.bytes.size;
		size_t need = sz + sizeof(CharT);
		if (ret.bytes._capacity() < need) ret.bytes._capacity(need);
		*(CharT*)(ret.bytes.buf + sz) = 0;
	}
	return ret;
}template StrW _子文本替换<StrW, charW>(const StrW&, const StrW&, const StrW&, size_t, int, bool);

template<class T = W>
StrW 子文本替换(const StrW& 欲被替换的文本, const StrW& 欲被替换的子文本, const StrW& 用作替换的子文本, size_t 起始位置 = 0, int 次数 = -1, bool 区分大小写 = true) {
	return _子文本替换<StrW, charW>(欲被替换的文本, 欲被替换的子文本, 用作替换的子文本, 起始位置, 次数, 区分大小写);
}
/**子文本替换
 * @param 欲被替换的文本
 * @param 欲被替换的子文本
 * @param 用作替换的子文本
 * @param 进行替换的起始位置=0
 * @param 替换进行的次数=-1
 * @param 是否区分大小写=true
 * @return
 */
StrA 子文本替换(const StrA& 欲被替换的文本, const StrA& 欲被替换的子文本, const StrA& 用作替换的子文本,
	size_t 进行替换的起始位置 = 0, int 替换进行的次数 = -1, bool 是否区分大小写 = true) {
	return _子文本替换<StrA, char>(欲被替换的文本, 欲被替换的子文本, 用作替换的子文本, 进行替换的起始位置, 替换进行的次数, 是否区分大小写);
}

// ===== 取空白文本 =====
template<class StrT> _BETPL_DEF
StrT _取空白文本(size_t 重复次数) {
	if constexpr (be::same<StrT, StrA>) return StrA(' ', 重复次数);
	else                                 return StrW(32, 重复次数);
}template StrW _取空白文本<StrW>(size_t);

template<class StrT = W>
StrW 取空白文本(size_t 重复次数) { return _取空白文本<StrW>(重复次数); }
/**取空白文本
 * @param 重复次数
 * @return
 */
StrA 取空白文本(size_t 重复次数) {
	return _取空白文本<StrA>(重复次数);
}

// ===== 取重复文本 =====
template<class StrT> _BETPL_DEF
StrT _取重复文本(size_t 重复次数, const StrT& 待重复文本) {
	if (重复次数 == 0 || 待重复文本.len() == 0) return StrT();
	size_t subLen = 待重复文本.len();
	size_t charSize = (be::same<StrT, StrA> ? 1 : 2);
	StrT ret(subLen * 重复次数);
	for (size_t i = 0; i < 重复次数; i++)
		memcpy(ret.bytes.buf + i * subLen * charSize, 待重复文本, subLen * charSize);
	return ret;
}template StrW _取重复文本<StrW>(size_t, const StrW&);

template<class T = W>
StrW 取重复文本(size_t 重复次数, const StrW& 待重复文本) { return _取重复文本(重复次数, 待重复文本); }
/**取重复文本
 * @param 重复次数
 * @param 待重复文本
 * @return
 */
StrA 取重复文本(size_t 重复次数, const StrA& 待重复文本) {
	return _取重复文本(重复次数, 待重复文本);
}

// ===== 指针到文本 =====
template<class T = W>
StrW 指针到文本(const void* 内存文本指针) {
	if (内存文本指针 == nullptr) return StrW();
	return ToStr<W>((const charW*)内存文本指针);
}
/**指针到文本
 * @param 内存文本指针
 * @return
 */
StrA 指针到文本(const void* 内存文本指针) {
	if (内存文本指针 == nullptr) return StrA();
	return ToStr((const char*)内存文本指针);
}

template<class T = W, int STACK_CAP>
Arraybe<StrW, STACK_CAP>& 分割文本(const StrW& 原内容, const StrW& 分割内容,
	Arraybe<StrW, STACK_CAP>& 结果数组, bool 保持容量 = false) {
	return _SplitStr<StrW, charW, STACK_CAP>(原内容, 分割内容, 结果数组, 保持容量);
}
/**分割文本
 * @param 原内容
 * @param 分割内容
 * @param 结果数组 [IN,OUT]
 * @param 保持容量 默认为假(即不需要保持arr的原有容量er总是先销毁)
 * @return
 */
template<int STACK_CAP>
Arraybe<StrA, STACK_CAP>& 分割文本(const StrA& 原内容, const StrA& 分割内容,
	Arraybe<StrA, STACK_CAP>& 结果数组, bool 保持容量 = false) {
	return _SplitStr<StrA, char, STACK_CAP>(原内容, 分割内容, 结果数组, 保持容量);
}


#pragma region 文本扩展操作

// ===== 文本_取之间 =====
template<typename StrT> _BETPL_DEF
StrT _文本_取之间(const StrT& 完整内容, const StrT& 左边文本, const StrT& 右边文本,
	可空<size_t> 自定义左边位置, 可空<size_t> 自定义右边位置,
	ssize_t 左偏移, ssize_t 右偏移, bool 是否倒找, size_t 开始寻找位置,
	ssize_t(*fn寻找)(const StrT&, const StrT&, size_t, bool),
	ssize_t(*fn倒找)(const StrT&, const StrT&, size_t, bool))
{
	size_t len = 完整内容.len();
	if (len == 0) return StrT();

	ssize_t _leftIndex = 0;
	ssize_t _rightIndex = (ssize_t)len;
	bool null_indexRight = (自定义右边位置 == 空);

	if (自定义右边位置 != 空) _rightIndex = 自定义右边位置;

	if (是否倒找) {
		if (右边文本.len() > 0 && null_indexRight) {
			ssize_t pos = fn倒找(完整内容, 右边文本, SIZE_MAX, false);
			if (pos < 0) return StrT();
			_rightIndex = (size_t)((ssize_t)pos + 右偏移);
		}
		if (左边文本.len() > 0 && 自定义左边位置 == 空) {
			ssize_t pos = fn倒找(完整内容, 左边文本, _rightIndex, false);
			if (pos < 0) return StrT();
			_leftIndex = (size_t)((ssize_t)pos + (ssize_t)左边文本.len() + 左偏移);
		} else if (自定义左边位置 != 空) {
			_leftIndex = 自定义左边位置;
		}
	} else {
		if (左边文本.len() > 0 && 自定义左边位置 == 空) {
			ssize_t pos = fn寻找(完整内容, 左边文本, 开始寻找位置, false);
			if (pos < 0) return StrT();
			_leftIndex = (size_t)((ssize_t)pos + (ssize_t)左边文本.len() + 左偏移);
		} else if (自定义左边位置 != 空) {
			_leftIndex = 自定义左边位置;
		}
		if (右边文本.len() > 0 && null_indexRight) {
			ssize_t pos = fn寻找(完整内容, 右边文本, _leftIndex, false);
			if (pos < 0) return StrT();
			_rightIndex = (size_t)((ssize_t)pos + 右偏移);
		}
	}

	if (_leftIndex >= _rightIndex || _leftIndex < 0 || _leftIndex >= (ssize_t)len) return StrT();
	size_t count = (size_t)(_rightIndex - _leftIndex);
	if ((size_t)_leftIndex + count > len) count = len - (size_t)_leftIndex;
	return StrT(完整内容 + _leftIndex, count);
}template StrW _文本_取之间<StrW>(const StrW&, const StrW&, const StrW&, 可空<size_t>, 可空<size_t>, ssize_t, ssize_t, bool, size_t, ssize_t(*)(const StrW&, const StrW&, size_t, bool), ssize_t(*)(const StrW&, const StrW&, size_t, bool));

template<class T = W>
StrW 文本_取之间(const StrW& 完整内容, const StrW& 左边文本 = StrW(), const StrW& 右边文本 = StrW(),
	可空<size_t> 自左 = 空, 可空<size_t> 自右 = 空, ssize_t 左偏 = 0, ssize_t 右偏 = 0, bool 倒找 = false, size_t 起始 = 0) {
	return _文本_取之间<StrW>(完整内容, 左边文本, 右边文本, 自左, 自右, 左偏, 右偏, 倒找, 起始, 寻找文本<W>, 倒找文本<W>);
}

/**文本_取之间
 * @param 完整内容
 * @param 左边文本=StrA()
 * @param 右边文本=StrA()
 * @param 自定义左边位置<可空> 填了自定义左边后不得填左边文本
 * @param 自定义右边位置<可空>
 * @param 左偏移=0 对左边文本取到的位置进行偏移(如3，-1) #必须保证左边文本不为空
 * @param 右偏移=0
 * @param 是否倒找=false
 * @param 开始寻找位置=0
 * @return
 */
StrA 文本_取之间(const StrA& 完整内容, const StrA& 左边文本 = StrA(), const StrA& 右边文本 = StrA(),
	可空<size_t> 自定义左边位置 = 空, 可空<size_t> 自定义右边位置 = 空,
	ssize_t 左偏移 = 0, ssize_t 右偏移 = 0, bool 是否倒找 = false, size_t 开始寻找位置 = 0) {
	return _文本_取之间<StrA>(完整内容, 左边文本, 右边文本, 自定义左边位置, 自定义右边位置, 左偏移, 右偏移, 是否倒找, 开始寻找位置, 寻找文本, 倒找文本);
}

// ===== 文本_对比左边 =====
template<class StrT, class CharT> _BETPL_DEF
bool _文本_对比左边(const StrT& 原文本, const StrT& 左边文本, bool 是否不区分大小写) {
	size_t len1 = 原文本.len();
	size_t len2 = 左边文本.len();
	if (len2 == 0) return true;
	if (len1 < len2) return false;

	const CharT* s1 = (const CharT*)原文本._buf();
	const CharT* s2 = (const CharT*)左边文本._buf();

	if (是否不区分大小写) {
		return _strnicmp_ascii(s1, s2, len2) == 0;
	} else {
		return memcmp(s1, s2, len2 * sizeof(CharT)) == 0;
	}
}template bool _文本_对比左边<StrW, charW>(const StrW&, const StrW&, bool);

template<class T = W>
bool 文本_对比左边(const StrW& 原文本, const StrW& 左边文本, bool 是否不区分大小写 = false) { return _文本_对比左边<StrW, charW>(原文本, 左边文本, 是否不区分大小写); }
/**文本_对比左边
 * @param 原文本 完整文本
 * @param 左边文本 欲对比的左侧开始文本
 * @param 是否不区分大小写=false
 * @return 失败返回假，满足包含该左边返回真
 */
bool 文本_对比左边(const StrA& 原文本, const StrA& 左边文本, bool 是否不区分大小写 = false) {
	return _文本_对比左边<StrA, char>(原文本, 左边文本, 是否不区分大小写);
}

// ===== 文本_对比右边 =====
template<class StrT, class CharT> _BETPL_DEF
bool _文本_对比右边(const StrT& 原文本, const StrT& 右边文本, bool 是否不区分大小写) {
	size_t len1 = 原文本.len();
	size_t len2 = 右边文本.len();
	if (len2 == 0) return true;
	if (len1 < len2) return false;

	const CharT* s1 = (const CharT*)原文本._buf() + (len1 - len2);
	const CharT* s2 = (const CharT*)右边文本._buf();

	if (是否不区分大小写) {
		return _strnicmp_ascii(s1, s2, len2) == 0;
	} else {
		return memcmp(s1, s2, len2 * sizeof(CharT)) == 0;
	}
}template bool _文本_对比右边<StrW, charW>(const StrW&, const StrW&, bool);

template<class T = W>
bool 文本_对比右边(const StrW& 原文本, const StrW& 右边文本, bool 是否不区分大小写 = false) { return _文本_对比右边<StrW, charW>(原文本, 右边文本, 是否不区分大小写); }
/**文本_对比_右边
 * @param 原文本 完整文本
 * @param 右边文本 欲对比的右侧结尾文本
 * @param 是否不区分大小写=false
 * @return 失败返回假，满足包含该右边返回真
 */
bool 文本_对比右边(const StrA& 原文本, const StrA& 右边文本, bool 是否不区分大小写 = false) {
	return _文本_对比右边<StrA, char>(原文本, 右边文本, 是否不区分大小写);
}

// ===== 文本_对比 =====
template<class StrT, class CharT> _BETPL_DEF
bool _文本_对比(const StrT& 原文本, const StrT& 左边文本, const StrT& 右边文本, bool 是否不区分大小写) {
	if (左边文本.len() > 0 && !_文本_对比左边<StrT, CharT>(原文本, 左边文本, 是否不区分大小写)) {
		return false;
	}
	if (右边文本.len() > 0 && !_文本_对比右边<StrT, CharT>(原文本, 右边文本, 是否不区分大小写)) {
		return false;
	}
	return true;
}template bool _文本_对比<StrW, charW>(const StrW&, const StrW&, const StrW&, bool);

template<class T = W>
bool 文本_对比(const StrW& 原文本, const StrW& 左边文本 = StrW(), const StrW& 右边文本 = StrW(), bool 是否不区分大小写 = false) { return _文本_对比<StrW, charW>(原文本, 左边文本, 右边文本, 是否不区分大小写); }
/**文本_对比
 * @param 原文本 完整文本
 * @param 左边文本 欲对比的开头文本（为空则代表不校验此侧）
 * @param 右边文本 欲对比的结尾文本（为空则代表不校验此侧）
 * @param 是否不区分大小写=false
 * @return 同时校验是否以指定的左边开头并且以指定的右侧结尾，二者均满足时则返回真。某侧参数不填代表不校验那一侧。
 */
bool 文本_对比(const StrA& 原文本, const StrA& 左边文本 = StrA(), const StrA& 右边文本 = StrA(), bool 是否不区分大小写 = false) {
	return _文本_对比<StrA, char>(原文本, 左边文本, 右边文本, 是否不区分大小写);
}

#pragma endregion


#pragma region 路径相关文本操作

// ===== 路径处理内部辅助 =====
template<class StrT, class CharT> _BETPL_DEF
StrT _路径_取文件名(const StrT& 路径, const CharT* ptr) {
	if (路径.len() == 0) return StrT();
	size_t len = 路径.len();
	for (ptrdiff_t i = (ptrdiff_t)len - 1; i >= 0; --i) {
		if (ptr[i] == '\\' || ptr[i] == '/') {
			return StrT(ptr + i + 1, len - i - 1);
		}
	}
	return 路径;
}template StrW _路径_取文件名<StrW, charW>(const StrW&, const charW*);

template<class StrT, class CharT> _BETPL_DEF
StrT _路径_去文件名(const StrT& 路径, const CharT* ptr) {
	if (路径.len() == 0) return StrT();
	size_t len = 路径.len();
	for (ptrdiff_t i = (ptrdiff_t)len - 1; i >= 0; --i) {
		if (ptr[i] == '\\' || ptr[i] == '/') {
			return StrT(ptr, i);
		}
	}
	return StrT();
}template StrW _路径_去文件名<StrW, charW>(const StrW&, const charW*);

template<class StrT, class CharT> _BETPL_DEF
StrT _路径_保证去尾(const StrT& 路径, const CharT* ptr) {
	if (路径.len() == 0) return StrT();
	size_t len = 路径.len();
	while (len > 0 && (ptr[len - 1] == '\\' || ptr[len - 1] == '/')) {
		len--;
	}
	return StrT(ptr, len);
}template StrW _路径_保证去尾<StrW, charW>(const StrW&, const charW*);

template<class StrT, class CharT> _BETPL_DEF
StrT _路径_取后缀(const StrT& 路径, const CharT* ptr, bool 保留点号) {
	if (路径.len() == 0) return StrT();
	size_t len = 路径.len();
	for (ptrdiff_t i = (ptrdiff_t)len - 1; i >= 0; --i) {
		if (ptr[i] == '\\' || ptr[i] == '/') break;
		if (ptr[i] == '.') {
			if (保留点号) return StrT(ptr + i, len - i);
			else return StrT(ptr + i + 1, len - i - 1);
		}
	}
	return StrT();
}template StrW _路径_取后缀<StrW, charW>(const StrW&, const charW*, bool);

template<class StrT, class CharT> _BETPL_DEF
StrT _路径_去后缀(const StrT& 路径, const CharT* ptr) {
	if (路径.len() == 0) return StrT();
	size_t len = 路径.len();
	for (ptrdiff_t i = (ptrdiff_t)len - 1; i >= 0; --i) {
		if (ptr[i] == '\\' || ptr[i] == '/') return 路径;
		if (ptr[i] == '.') {
			return StrT(ptr, i);
		}
	}
	return 路径;
}template StrW _路径_去后缀<StrW, charW>(const StrW&, const charW*);

template<class StrT, class CharT> _BETPL_DEF
StrT _路径_取不带后缀文件名(const StrT& 路径, const CharT* ptr) {
	StrT 文件名 = _路径_取文件名(路径, ptr);
	return _路径_去后缀(文件名, (const CharT*)文件名);
}template StrW _路径_取不带后缀文件名<StrW, charW>(const StrW&, const charW*);

// =================== 公开接口 ===================
template<class T = W>
StrW 路径_取文件名(const StrW& 路径) { return _路径_取文件名(路径, (const charW*)路径); }
/**路径_取文件名
 * @param 路径 包含完整路径的文本
 * @return 失败返回空，成功返回诸如“test.txt”
 */
StrA 路径_取文件名(const StrA& 路径) {
	return _路径_取文件名(路径, (const char*)路径);
}

template<class T = W>
StrW 路径_去文件名(const StrW& 路径) { return _路径_去文件名(路径, (const charW*)路径); }
/**路径_去文件名
 * @param 路径 包含完整路径的文本
 * @return 返回去掉文件名之后的路径，例如“C:\temp”
 */
StrA 路径_去文件名(const StrA& 路径) {
	return _路径_去文件名(路径, (const char*)路径);
}

template<class T = W>
StrW 路径_保证去尾(const StrW& 路径) { return _路径_保证去尾(路径, (const charW*)路径); }
/**路径_保证去尾
 * @param 路径 给定的文件 or 目录路径
 * @return 抹除路径末尾的斜杠或反斜杠，如将“C:\temp\”或“C:\temp/”变为“C:\temp”
 */
StrA 路径_保证去尾(const StrA& 路径) {
	return _路径_保证去尾(路径, (const char*)路径);
}

template<class T = W>
StrW 路径_取后缀(const StrW& 路径, bool 保留点号 = false) { return _路径_取后缀(路径, (const charW*)路径, 保留点号); }
/**路径_取后缀
 * @param 路径 目标路径 or 文件名
 * @param 保留点号=false 是否保留开头的"."，默认不保留
 * @return 返回诸如"txt" or ".txt"。若不存在后缀则返回空
 */
StrA 路径_取后缀(const StrA& 路径, bool 保留点号 = false) {
	return _路径_取后缀(路径, (const char*)路径, 保留点号);
}

template<class T = W>
StrW 路径_去后缀(const StrW& 路径) { return _路径_去后缀(路径, (const charW*)路径); }
/**路径_去后缀
 * @param 路径 给定的文件全称
 * @return 去除了末尾特定后缀的部分，例如由“C:\a\b.txt”变为“C:\a\b”
 */
StrA 路径_去后缀(const StrA& 路径) {
	return _路径_去后缀(路径, (const char*)路径);
}

template<class T = W>
StrW 路径_取不带后缀文件名(const StrW& 路径) { return _路径_取不带后缀文件名(路径, (const charW*)路径); }
/**路径_取不带后缀文件名
 * @param 路径 目标路径文本
 * @return 单独抽出纯文件名，例如由“C:\temp\test.txt”变为“test”
 */
StrA 路径_取不带后缀文件名(const StrA& 路径) {
	return _路径_取不带后缀文件名(路径, (const char*)路径);
}
#pragma endregion

