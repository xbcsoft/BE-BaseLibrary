#pragma once
#include "../中文核心.hpp"

template<class StrT>
size_t _取文本长度(const StrT& 文本数据);
template size_t _取文本长度(const StrW& 文本数据);

template<class StrT = W>
size_t 取文本长度(const StrW& 文本数据) { return _取文本长度(文本数据); }

template<class StrT = U8>
size_t 取文本长度(const StrU8& 文本数据) { return 文本数据.lenU8(); }

/**取文本长度
 * @param 文本数据
 * @return
 */
size_t 取文本长度(const StrA& 文本数据);


// ===== 取文本左边 =====
template<class StrT>
StrT _取文本左边(const StrT& 欲取其部分的文本, size_t 欲取出字符的数目);
template StrW _取文本左边(const StrW&, size_t);

template<class T = W>
StrW 取文本左边(const StrW& 欲取其部分的文本, size_t 欲取出字符的数目) {
	return _取文本左边(欲取其部分的文本, 欲取出字符的数目);
}
/**取文本左边
 * @param 欲取其部分的文本
 * @param 欲取出字符的数目
 * @return
 */
StrA 取文本左边(const StrA& 欲取其部分的文本, size_t 欲取出字符的数目);

// ===== 取文本右边 =====
template<class StrT>
StrT _取文本右边(const StrT& 欲取其部分的文本, size_t 欲取出字符的数目);
template StrW _取文本右边(const StrW&, size_t);

template<class T = W>
StrW 取文本右边(const StrW& 欲取其部分的文本, size_t 欲取出字符的数目) {
	return _取文本右边(欲取其部分的文本, 欲取出字符的数目);
}
/**取文本右边
 * @param 欲取其部分的文本
 * @param 欲取出字符的数目
 * @return
 */
StrA 取文本右边(const StrA& 欲取其部分的文本, size_t 欲取出字符的数目);

// ===== 取文本子串 =====
template<class StrT>
StrT _取文本子串(const StrT& 欲取其部分的文本, size_t 起始取出位置, size_t 欲取出字符的数目);
template StrW _取文本子串(const StrW&, size_t, size_t);

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
StrA 取文本子串(const StrA& 欲取其部分的文本, size_t 起始取出位置, size_t 欲取出字符的数目);

// ===== 字符 =====
template<class StrT, class CharT>
StrT _字符(byte 欲取其字符的字符代码);
template StrW _字符<StrW, charW>(byte);

template<class T = W>
StrW 字符(byte 欲取其字符的字符代码) { return _字符<StrW, charW>(欲取其字符的字符代码); }
/**字符
 * @param 欲取其字符的字符代码
 * @return
 */
StrA 字符(byte 欲取其字符的字符代码);

// ===== 取代码 =====
template<class StrT>
int _取代码(const StrT& 欲取字符代码的文本, size_t 欲取其代码的字符位置);
template int _取代码(const StrW&, size_t);

template<class T = W>
int 取代码(const StrW& 欲取字符代码的文本, size_t 欲取其代码的字符位置 = 0) {
	return _取代码(欲取字符代码的文本, 欲取其代码的字符位置);
}
/**取代码
 * @param 欲取字符代码的文本
 * @param 欲取其代码的字符位置=0
 * @return
 */
int 取代码(const StrA& 欲取字符代码的文本, size_t 欲取其代码的字符位置 = 0);

// ===== 寻找文本 =====

// ===== 寻找文本 =====
template<class StrT, class CharT>
ssize_t _寻找文本(const StrT& 被搜寻的文本, const StrT& 欲寻找的文本, size_t 起始搜寻位置, bool 是否不区分大小写);
template ssize_t _寻找文本<StrW, charW>(const StrW&, const StrW&, size_t, bool);

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
ssize_t 寻找文本(const StrA& 被搜寻的文本, const StrA& 欲寻找的文本, size_t 起始搜寻位置 = 0, bool 是否不区分大小写 = false);

// ===== 倒找文本 =====
template<class StrT, class CharT>
ssize_t _倒找文本(const StrT& 被搜寻的文本, const StrT& 欲寻找的文本, size_t 起始搜寻位置, bool 是否不区分大小写);
template ssize_t _倒找文本<StrW, charW>(const StrW&, const StrW&, size_t, bool);

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
ssize_t 倒找文本(const StrA& 被搜寻的文本, const StrA& 欲寻找的文本, size_t 起始搜寻位置 = SIZE_MAX, bool 是否不区分大小写 = false);

// ===== 到大写 =====
template<class StrT, class CharT>
StrT _到大写(const StrT& 欲变换的文本);
template StrW _到大写<StrW, charW>(const StrW&);

template<class T = W>
StrW 到大写(const StrW& 欲变换的文本) { return _到大写<StrW, charW>(欲变换的文本); }
/**到大写
 * @param 欲变换的文本
 * @return
 */
StrA 到大写(const StrA& 欲变换的文本);

// ===== 到小写 =====
template<class StrT, class CharT>
StrT _到小写(const StrT& 欲变换的文本);
template StrW _到小写<StrW, charW>(const StrW&);

template<class T = W>
StrW 到小写(const StrW& 欲变换的文本) { return _到小写<StrW, charW>(欲变换的文本); }
/**到小写
 * @param 欲变换的文本
 * @return
 */
StrA 到小写(const StrA& 欲变换的文本);

// ===== 到全角/半角内部辅助 =====

template<class StrT>
StrT _到全角(const StrT& 欲变换的文本);
template StrW _到全角<StrW>(const StrW&);

template<class T = W>
StrW 到全角(const StrW& 欲变换的文本) { return _到全角(欲变换的文本); }
/**到全角
 * @param 欲变换的文本
 * @return
 */
StrA 到全角(const StrA& 欲变换的文本);

template<class StrT>
StrT _到半角(const StrT& 欲变换的文本);
template StrW _到半角<StrW>(const StrW&);

template<class T = W>
StrW 到半角(const StrW& 欲变换的文本) { return _到半角(欲变换的文本); }
/**到半角
 * @param 欲变换的文本
 * @return
 */
StrA 到半角(const StrA& 欲变换的文本);

// ===== 删首空 =====
template<class StrT, class CharT>
StrT _删首空(const StrT& 欲删除空格的文本);
template StrW _删首空<StrW, charW>(const StrW&);

template<class T = W>
StrW 删首空(const StrW& 文本数据) { return _删首空<StrW, charW>(文本数据); }
/**删首空
 * @param 欲删除空格的文本
 * @return
 */
StrA 删首空(const StrA& 欲删除空格的文本);

// ===== 删尾空 =====
template<class StrT, class CharT>
StrT _删尾空(const StrT& 欲删除空格的文本);
template StrW _删尾空<StrW, charW>(const StrW&);

template<class T = W>
StrW 删尾空(const StrW& 文本数据) { return _删尾空<StrW, charW>(文本数据); }
/**删尾空
 * @param 欲删除空格的文本
 * @return
 */
StrA 删尾空(const StrA& 欲删除空格的文本);

// ===== 删首尾空 =====
template<class T = W>
StrW 删首尾空(const StrW& txt) { return _删尾空<StrW, charW>(_删首空<StrW, charW>(txt)); }
/**删首尾空
 * @param txt
 * @return
 */
StrA 删首尾空(const StrA& txt);

// ===== 删全部空 =====
template<class StrT, class CharT>
StrT _删全部空(const StrT& txt);
template StrW _删全部空<StrW, charW>(const StrW&);

template<class T = W>
StrW 删全部空(const StrW& txt) { return _删全部空<StrW, charW>(txt); }
/**删全部空
 * @param txt
 * @return
 */
StrA 删全部空(const StrA& txt);

// ===== 文本替换（按位置）=====
template<class StrT, class CharT>
StrT _文本替换(const StrT& 欲被替换的文本, size_t 起始替换位置, size_t 替换长度, const StrT& 用作替换的文本);
template StrW _文本替换<StrW, charW>(const StrW&, size_t, size_t, const StrW&);

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
StrA 文本替换(const StrA& 欲被替换的文本, size_t 起始替换位置, size_t 替换长度, const StrA& 用作替换的文本);

// ===== 子文本替换 =====
template<typename StrT, typename CharT>
StrT _子文本替换(const StrT& 欲被替换的文本, const StrT& 欲被替换的子文本,
	const StrT& 用作替换的子文本, size_t 进行替换的起始位置, int 替换进行的次数, bool 是否区分大小写);
template StrW _子文本替换<StrW, charW>(const StrW&, const StrW&, const StrW&, size_t, int, bool);

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
	size_t 进行替换的起始位置 = 0, int 替换进行的次数 = -1, bool 是否区分大小写 = true);

// ===== 取空白文本 =====
template<class StrT>
StrT _取空白文本(size_t 重复次数);
template StrW _取空白文本<StrW>(size_t);

template<class StrT = W>
StrW 取空白文本(size_t 重复次数) { return _取空白文本<StrW>(重复次数); }
/**取空白文本
 * @param 重复次数
 * @return
 */
StrA 取空白文本(size_t 重复次数);

// ===== 取重复文本 =====
template<class StrT>
StrT _取重复文本(size_t 重复次数, const StrT& 待重复文本);
template StrW _取重复文本<StrW>(size_t, const StrW&);

template<class T = W>
StrW 取重复文本(size_t 重复次数, const StrW& 待重复文本) { return _取重复文本(重复次数, 待重复文本); }
/**取重复文本
 * @param 重复次数
 * @param 待重复文本
 * @return
 */
StrA 取重复文本(size_t 重复次数, const StrA& 待重复文本);

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
StrA 指针到文本(const void* 内存文本指针);

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
template<typename StrT>
StrT _文本_取之间(const StrT& 完整内容, const StrT& 左边文本, const StrT& 右边文本,
	可空<size_t> 自定义左边位置, 可空<size_t> 自定义右边位置,
	ssize_t 左偏移, ssize_t 右偏移, bool 是否倒找, size_t 开始寻找位置,
	ssize_t(*fn寻找)(const StrT&, const StrT&, size_t, bool),
	ssize_t(*fn倒找)(const StrT&, const StrT&, size_t, bool));
template StrW _文本_取之间<StrW>(const StrW&, const StrW&, const StrW&, 可空<size_t>, 可空<size_t>, ssize_t, ssize_t, bool, size_t, ssize_t(*)(const StrW&, const StrW&, size_t, bool), ssize_t(*)(const StrW&, const StrW&, size_t, bool));

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
	ssize_t 左偏移 = 0, ssize_t 右偏移 = 0, bool 是否倒找 = false, size_t 开始寻找位置 = 0);

// ===== 文本_对比左边 =====
template<class StrT, class CharT>
bool _文本_对比左边(const StrT& 原文本, const StrT& 左边文本, bool 是否不区分大小写);
template bool _文本_对比左边<StrW, charW>(const StrW&, const StrW&, bool);

template<class T = W>
bool 文本_对比左边(const StrW& 原文本, const StrW& 左边文本, bool 是否不区分大小写 = false) { return _文本_对比左边<StrW, charW>(原文本, 左边文本, 是否不区分大小写); }
/**文本_对比左边
 * @param 原文本 完整文本
 * @param 左边文本 欲对比的左侧开始文本
 * @param 是否不区分大小写=false
 * @return 失败返回假，满足包含该左边返回真
 */
bool 文本_对比左边(const StrA& 原文本, const StrA& 左边文本, bool 是否不区分大小写 = false);

// ===== 文本_对比右边 =====
template<class StrT, class CharT>
bool _文本_对比右边(const StrT& 原文本, const StrT& 右边文本, bool 是否不区分大小写);
template bool _文本_对比右边<StrW, charW>(const StrW&, const StrW&, bool);

template<class T = W>
bool 文本_对比右边(const StrW& 原文本, const StrW& 右边文本, bool 是否不区分大小写 = false) { return _文本_对比右边<StrW, charW>(原文本, 右边文本, 是否不区分大小写); }
/**文本_对比_右边
 * @param 原文本 完整文本
 * @param 右边文本 欲对比的右侧结尾文本
 * @param 是否不区分大小写=false
 * @return 失败返回假，满足包含该右边返回真
 */
bool 文本_对比右边(const StrA& 原文本, const StrA& 右边文本, bool 是否不区分大小写 = false);

// ===== 文本_对比 =====
template<class StrT, class CharT>
bool _文本_对比(const StrT& 原文本, const StrT& 左边文本, const StrT& 右边文本, bool 是否不区分大小写);
template bool _文本_对比<StrW, charW>(const StrW&, const StrW&, const StrW&, bool);

template<class T = W>
bool 文本_对比(const StrW& 原文本, const StrW& 左边文本 = StrW(), const StrW& 右边文本 = StrW(), bool 是否不区分大小写 = false) { return _文本_对比<StrW, charW>(原文本, 左边文本, 右边文本, 是否不区分大小写); }
/**文本_对比
 * @param 原文本 完整文本
 * @param 左边文本 欲对比的开头文本（为空则代表不校验此侧）
 * @param 右边文本 欲对比的结尾文本（为空则代表不校验此侧）
 * @param 是否不区分大小写=false
 * @return 同时校验是否以指定的左边开头并且以指定的右侧结尾，二者均满足时则返回真。某侧参数不填代表不校验那一侧。
 */
bool 文本_对比(const StrA& 原文本, const StrA& 左边文本 = StrA(), const StrA& 右边文本 = StrA(), bool 是否不区分大小写 = false);

#pragma endregion


#pragma region 路径相关文本操作

// ===== 路径处理内部辅助 =====
template<class StrT, class CharT>
StrT _路径_取文件名(const StrT& 路径, const CharT* ptr);
template StrW _路径_取文件名<StrW, charW>(const StrW&, const charW*);

template<class StrT, class CharT>
StrT _路径_去文件名(const StrT& 路径, const CharT* ptr);
template StrW _路径_去文件名<StrW, charW>(const StrW&, const charW*);

template<class StrT, class CharT>
StrT _路径_保证去尾(const StrT& 路径, const CharT* ptr);
template StrW _路径_保证去尾<StrW, charW>(const StrW&, const charW*);

template<class StrT, class CharT>
StrT _路径_取后缀(const StrT& 路径, const CharT* ptr, bool 保留点号);
template StrW _路径_取后缀<StrW, charW>(const StrW&, const charW*, bool);

template<class StrT, class CharT>
StrT _路径_去后缀(const StrT& 路径, const CharT* ptr);
template StrW _路径_去后缀<StrW, charW>(const StrW&, const charW*);

template<class StrT, class CharT>
StrT _路径_取不带后缀文件名(const StrT& 路径, const CharT* ptr);
template StrW _路径_取不带后缀文件名<StrW, charW>(const StrW&, const charW*);

// =================== 公开接口 ===================
template<class T = W>
StrW 路径_取文件名(const StrW& 路径) { return _路径_取文件名(路径, (const charW*)路径); }
/**路径_取文件名
 * @param 路径 包含完整路径的文本
 * @return 失败返回空，成功返回诸如“test.txt”
 */
StrA 路径_取文件名(const StrA& 路径);

template<class T = W>
StrW 路径_去文件名(const StrW& 路径) { return _路径_去文件名(路径, (const charW*)路径); }
/**路径_去文件名
 * @param 路径 包含完整路径的文本
 * @return 返回去掉文件名之后的路径，例如“C:\temp”
 */
StrA 路径_去文件名(const StrA& 路径);

template<class T = W>
StrW 路径_保证去尾(const StrW& 路径) { return _路径_保证去尾(路径, (const charW*)路径); }
/**路径_保证去尾
 * @param 路径 给定的文件 or 目录路径
 * @return 抹除路径末尾的斜杠或反斜杠，如将“C:\temp\”或“C:\temp/”变为“C:\temp”
 */
StrA 路径_保证去尾(const StrA& 路径);

template<class T = W>
StrW 路径_取后缀(const StrW& 路径, bool 保留点号 = false) { return _路径_取后缀(路径, (const charW*)路径, 保留点号); }
/**路径_取后缀
 * @param 路径 目标路径 or 文件名
 * @param 保留点号=false 是否保留开头的"."，默认不保留
 * @return 返回诸如"txt" or ".txt"。若不存在后缀则返回空
 */
StrA 路径_取后缀(const StrA& 路径, bool 保留点号 = false);

template<class T = W>
StrW 路径_去后缀(const StrW& 路径) { return _路径_去后缀(路径, (const charW*)路径); }
/**路径_去后缀
 * @param 路径 给定的文件全称
 * @return 去除了末尾特定后缀的部分，例如由“C:\a\b.txt”变为“C:\a\b”
 */
StrA 路径_去后缀(const StrA& 路径);

template<class T = W>
StrW 路径_取不带后缀文件名(const StrW& 路径) { return _路径_取不带后缀文件名(路径, (const charW*)路径); }
/**路径_取不带后缀文件名
 * @param 路径 目标路径文本
 * @return 单独抽出纯文件名，例如由“C:\temp\test.txt”变为“test”
 */
StrA 路径_取不带后缀文件名(const StrA& 路径);
#pragma endregion
