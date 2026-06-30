#include "../中文核心.hpp"

size_t 取字节集长度(const Bytes& 字节集数据) {
	return 字节集数据.size;
}

Bytes 取字节集左边(const Bytes& 欲取其部分的字节集, size_t 欲取出字节的数目) {
	if (欲取出字节的数目 == 0) return Bytes();
	if (欲取出字节的数目 > 欲取其部分的字节集.size) 欲取出字节的数目 = 欲取其部分的字节集.size;
	return Bytes(欲取其部分的字节集.buf, 欲取出字节的数目);
}

Bytes 取字节集右边(const Bytes& 欲取其部分的字节集, size_t 欲取出字节的数目) {
	if (欲取出字节的数目 == 0 || 欲取其部分的字节集.size == 0) return Bytes();
	if (欲取出字节的数目 > 欲取其部分的字节集.size) 欲取出字节的数目 = 欲取其部分的字节集.size;
	return Bytes(欲取其部分的字节集.buf + 欲取其部分的字节集.size - 欲取出字节的数目, 欲取出字节的数目);
}

Bytes 取字节集子串(const Bytes& 欲取其部分的字节集, size_t 起始取出位置, size_t 欲取出字节的数目) {
	if (起始取出位置 >= 欲取其部分的字节集.size || 欲取出字节的数目 == 0) return Bytes();
	if (起始取出位置 + 欲取出字节的数目 > 欲取其部分的字节集.size) {
		欲取出字节的数目 = 欲取其部分的字节集.size - 起始取出位置;
	}
	return Bytes(欲取其部分的字节集.buf + 起始取出位置, 欲取出字节的数目);
}

ssize_t 寻找字节集(const Bytes& 被搜寻的字节集, const Bytes& 欲寻找的字节集, size_t 起始搜寻位置 = 0) {
	if (欲寻找的字节集.size == 0 || 被搜寻的字节集.size == 0) return -1;
	if (起始搜寻位置 >= 被搜寻的字节集.size) return -1;
	if (被搜寻的字节集.size - 起始搜寻位置 < 欲寻找的字节集.size) return -1;

	size_t end = 被搜寻的字节集.size - 欲寻找的字节集.size;
	for (size_t i = 起始搜寻位置; i <= end; ++i) {
		if (memcmp(被搜寻的字节集.buf + i, 欲寻找的字节集.buf, 欲寻找的字节集.size) == 0) {
			return (ssize_t)i;
		}
	}
	return -1;
}

ssize_t 倒找字节集(const Bytes& 被搜寻的字节集, const Bytes& 欲寻找的字节集, size_t 起始搜寻位置 = -1) {
	if (欲寻找的字节集.size == 0 || 被搜寻的字节集.size < 欲寻找的字节集.size) return -1;

	size_t maxPos = 被搜寻的字节集.size - 欲寻找的字节集.size;
	size_t startPos = (起始搜寻位置 == (size_t)-1 || 起始搜寻位置 > maxPos) ? maxPos : 起始搜寻位置;

	for (ssize_t i = (ssize_t)startPos; i >= 0; --i) {
		if (memcmp(被搜寻的字节集.buf + i, 欲寻找的字节集.buf, 欲寻找的字节集.size) == 0) {
			return i;
		}
	}
	return -1;
}

Bytes 字节集替换(const Bytes& 欲替换其部分的字节集, size_t 起始替换位置, size_t 替换长度, const Bytes& 用作替换的字节集 = Bytes()) {
	if (起始替换位置 > 欲替换其部分的字节集.size) 起始替换位置 = 欲替换其部分的字节集.size;
	if (起始替换位置 + 替换长度 > 欲替换其部分的字节集.size) 替换长度 = 欲替换其部分的字节集.size - 起始替换位置;

	size_t newSize = 欲替换其部分的字节集.size - 替换长度 + 用作替换的字节集.size;
	Bytes ret(newSize);

	// Copy left part
	if (起始替换位置 > 0) {
		ret.copyFrom(0, 欲替换其部分的字节集.buf, 起始替换位置);
	}
	// Copy center part
	if (用作替换的字节集.size > 0) {
		ret.copyFrom(起始替换位置, 用作替换的字节集.buf, 用作替换的字节集.size);
	}
	// Copy right part
	size_t rightSize = 欲替换其部分的字节集.size - 起始替换位置 - 替换长度;
	if (rightSize > 0) {
		ret.copyFrom(起始替换位置 + 用作替换的字节集.size, 欲替换其部分的字节集.buf + 起始替换位置 + 替换长度, rightSize);
	}

	return ret;
}

Bytes 子字节集替换(const Bytes& 欲被替换的字节集, const Bytes& 欲被替换的子字节集, const Bytes& 用作替换的子字节集 = Bytes(), size_t 进行替换的起始位置 = 0, int 替换进行的次数 = -1) {
	if (欲被替换的子字节集.size == 0 || 进行替换的起始位置 >= 欲被替换的字节集.size) return 欲被替换的字节集;

	Bytes ret;
	size_t cur = 进行替换的起始位置;
	size_t len = 欲被替换的字节集.size;
	const byte* s = 欲被替换的字节集.buf;
	size_t subLen = 欲被替换的子字节集.size;

	if (cur > 0) {
		ret.append(s, cur);
	}

	int count = 0;
	while (cur < len) {
		if (替换进行的次数 != -1 && count >= 替换进行的次数) {
			ret.append(s + cur, len - cur);
			break;
		}

		bool match = false;
		if (cur + subLen <= len && memcmp(s + cur, 欲被替换的子字节集.buf, subLen) == 0) {
			match = true;
		}

		if (match) {
			if (用作替换的子字节集.size > 0) ret.append(用作替换的子字节集.buf, 用作替换的子字节集.size);
			cur += subLen;
			count++;
		} else {
			ret.append(s + cur, 1);
			cur++;
		}
	}
	return ret;
}

Bytes 取空白字节集(size_t 零字节数目) {
	return Bytes((byte)0, 零字节数目);
}

Bytes 取重复字节集(size_t 重复次数, const Bytes& 待重复的字节集) {
	if (重复次数 == 0 || 待重复的字节集.size == 0) return Bytes();
	Bytes ret(待重复的字节集.size * 重复次数);
	for (size_t i = 0; i < 重复次数; ++i) {
		ret.copyFrom(i * 待重复的字节集.size, 待重复的字节集.buf, 待重复的字节集.size);
	}
	return ret;
}

Bytes 指针到字节集(const void* 内存数据指针, size_t 内存数据长度) {
	return Bytes(内存数据指针, 内存数据长度);
}

int 指针到整数(const void* 内存数据指针) {
	if (!内存数据指针) return 0;
	return *(const int*)内存数据指针;
}

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
	ssize_t 左偏移 = 0, ssize_t 右偏移 = 0, bool 是否倒找 = false, size_t 开始寻找位置 = 0)
{
	size_t len = 完整内容.size;
	if (len == 0) return Bytes();

	ssize_t _leftIndex = 0;
	ssize_t _rightIndex = (ssize_t)len;
	bool null_indexRight = (自定义右边位置 == 空);

	if (自定义右边位置 != 空) _rightIndex = 自定义右边位置;

	if (是否倒找) {
		// 倒找：先找右边，再找左边
		if (右边字节集.size > 0 && null_indexRight) {
			ssize_t pos = 倒找字节集(完整内容, 右边字节集, -1);
			if (pos < 0) return Bytes();
			_rightIndex = (size_t)((ssize_t)pos + 右偏移);
		}

		if (左边字节集.size > 0 && 自定义左边位置 == 空) {
			ssize_t pos = 倒找字节集(完整内容, 左边字节集, _rightIndex);
			if (pos < 0) return Bytes();
			_leftIndex = (size_t)((ssize_t)pos + (ssize_t)左边字节集.size + 左偏移);
		} else if (自定义左边位置 != 空) {
			_leftIndex = 自定义左边位置;
		}
	} else {
		// 正找：先找左边，再找右边
		if (左边字节集.size > 0 && 自定义左边位置 == 空) {
			ssize_t pos = 寻找字节集(完整内容, 左边字节集, 开始寻找位置);
			if (pos < 0) return Bytes();
			_leftIndex = (size_t)((ssize_t)pos + (ssize_t)左边字节集.size + 左偏移);
		} else if (自定义左边位置 != 空) {
			_leftIndex = 自定义左边位置;
		}

		if (右边字节集.size > 0 && null_indexRight) {
			ssize_t pos = 寻找字节集(完整内容, 右边字节集, _leftIndex);
			if (pos < 0) return Bytes();
			_rightIndex = (size_t)((ssize_t)pos + 右偏移);
		}
	}

	if (_leftIndex >= _rightIndex || _leftIndex < 0 || _leftIndex >= (ssize_t)len) return Bytes();
	size_t count = (size_t)(_rightIndex - _leftIndex);
	if ((size_t)_leftIndex + count > len) count = len - (size_t)_leftIndex;
	return Bytes(完整内容.buf + _leftIndex, count);
}

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
