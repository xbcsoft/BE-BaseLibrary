#pragma once
#include "../中文核心.hpp"

size_t 取字节集长度(const Bytes& 字节集数据);

Bytes 取字节集左边(const Bytes& 欲取其部分的字节集, size_t 欲取出字节的数目);

Bytes 取字节集右边(const Bytes& 欲取其部分的字节集, size_t 欲取出字节的数目);

Bytes 取字节集子串(const Bytes& 欲取其部分的字节集, size_t 起始取出位置, size_t 欲取出字节的数目);

ssize_t 寻找字节集(const Bytes& 被搜寻的字节集, const Bytes& 欲寻找的字节集, size_t 起始搜寻位置 = 0);

ssize_t 倒找字节集(const Bytes& 被搜寻的字节集, const Bytes& 欲寻找的字节集, size_t 起始搜寻位置 = -1);

Bytes 字节集替换(const Bytes& 欲替换其部分的字节集, size_t 起始替换位置, size_t 替换长度, const Bytes& 用作替换的字节集 = Bytes());

Bytes 子字节集替换(const Bytes& 欲被替换的字节集, const Bytes& 欲被替换的子字节集, const Bytes& 用作替换的子字节集 = Bytes(), size_t 进行替换的起始位置 = 0, int 替换进行的次数 = -1);

Bytes 取空白字节集(size_t 零字节数目);

Bytes 取重复字节集(size_t 重复次数, const Bytes& 待重复的字节集);

Bytes 指针到字节集(const void* 内存数据指针, size_t 内存数据长度);

int 指针到整数(const void* 内存数据指针);

/**字节集_取之间
 * @param 完整内容          被截取的原始字节集
 * @param 左边字节集          从此字节集出现位置开始截取(可为空，若提供自定义左边位置则忽略)
 * @param 右边字节集          截取到此字节集前结束(可为空，若提供自定义右边位置则忽略)
 * @param 自定义左边位置<可空> 直接指定左边界索引，优先级高于左边字节集匹配
 * @param 自定义右边位置<可空> 直接指定右边界索引，优先级高于右边字节集匹配
 * @param 左偏移            对左边界再加上偏移量，可为正负
 * @param 右偏移            对右边界再加上偏移量，可为正负
 * @param 是否倒找          为真时优先自右向左匹配右边、左边字节集
 * @param 开始寻找位置      正向查找左边字节集时的起始位置
 * @return 截得的子字节集，若任一步失败则返回空字节集
 */
Bytes 字节集_取之间(const Bytes& 完整内容, const Bytes& 左边字节集 = Bytes(), const Bytes& 右边字节集 = Bytes(),
	可空<size_t> 自定义左边位置 = 空, 可空<size_t> 自定义右边位置 = 空,
	ssize_t 左偏移 = 0, ssize_t 右偏移 = 0, bool 是否倒找 = false, size_t 开始寻找位置 = 0);

/**分割字节集
 * @param 原内容
 * @param 分割内容
 * @param 结果数组 [IN,OUT]
 * @param 保持容量 默认为假(即不需要保持arr的原有容量而总是先销毁)
 * @return
 */
template<int STACK_CAP>
Arraybe<Bytes, STACK_CAP>& 分割字节集(const Bytes& 原内容, const Bytes& 分割内容,
	Arraybe<Bytes, STACK_CAP>& 结果数组, bool 保持容量 = false) {
	return _SplitBytes<Bytes, STACK_CAP>(原内容, 分割内容, 结果数组, 保持容量);
}

/**分割字节引集
 * @param 原内容
 * @param 分割内容
 * @param 结果数组 [IN,OUT]
 * @param 保持容量 默认为假(即不需要保持arr的原有容量而总是先销毁)
 * @return
 */
template<int STACK_CAP>
Arraybe<BR, STACK_CAP>& 分割字节引集(const Bytes& 原内容, const Bytes& 分割内容,
	Arraybe<BR, STACK_CAP>& 结果数组, bool 保持容量 = false) {
	return _SplitBytes<BR, STACK_CAP>(原内容, 分割内容, 结果数组, 保持容量);
}
