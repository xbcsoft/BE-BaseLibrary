#include "../中文核心.hpp"

template<typename T, int C>
void 重定义数组(Arraybe<T, C>& 数组变量, bool 是否保留以前的内容 = false,
	int 成员个数 = 0, bool 是否收缩容量 = false)
{
	数组变量.reset(成员个数, 是否保留以前的内容, 是否收缩容量);
}

template<typename T, int C>
int 取数组成员数(const Arraybe<T, C>& 数组) {
	return 数组.count;
}

template<typename T, int C>
void 复制数组(Arraybe<T, C>& 复制到的数组变量, const Arraybe<T, C>& 待复制的数组数据) {
	复制到的数组变量 = 待复制的数组数据;
}

//为什么需要独立的模板参数 E？
//如果这里直接使用 T&&，由于 T 已经在 Arraybe<T, C>& 中被固定，参数 T&& 将成为普通的右值引用，导致无法传入左值参数。
//引入新的模板参数 E 后，E&& 才能独立出来构成了C++的“万能引用”（Forwarding Reference）。
//这样使用 be::forward<E> 就能实现完美的类型转发，既能接收左值也能接收右值，进而调用底层 Arraybe::push 的拷贝或移动重载。
template<typename T, int C, typename E>
void 加入成员(Arraybe<T, C>& 欲加入成员的数组变量, E&& 欲加入的成员数据) {
	欲加入成员的数组变量.push(be::forward<E>(欲加入的成员数据));
}

template<typename T, int C, typename E>
bool 插入成员(Arraybe<T, C>& 欲插入成员的数组变量, int 欲插入的位置, E&& 欲插入的成员数据) {
	return 欲插入成员的数组变量.insert(欲插入的位置, be::forward<E>(欲插入的成员数据));
}

template<typename T, int C>
bool 删除成员(Arraybe<T, C>& 欲删除成员的数组变量, int 欲删除的位置, int 欲删除的成员数目 = 1) {
	return 欲删除成员的数组变量.del(欲删除的位置, 欲删除的成员数目);
}

template<typename T, int C>
void 清除数组(Arraybe<T, C>& 欲删除成员的数组变量) {
	欲删除成员的数组变量.clear();
}




#pragma region 数组操作扩展

template<typename T, int C>
bool 数组_批量插入成员(Arraybe<T, C>& 欲插入成员的数组变量, int 欲插入的位置,
	const T* 欲插入的成员数据指针, int 欲插入的成员数目, bool 是否深拷贝 = true)
{
	return 欲插入成员的数组变量.insert(欲插入的位置, 欲插入的成员数据指针, 欲插入的成员数目, 是否深拷贝);
}

template<typename T, int C>
void 数组_排序(Arraybe<T, C>& 数值数组变量, bool 排序方向是否为从小到大 = true) {
	if (数值数组变量.count <= 1) return;
	bool changed;
	int len = 数值数组变量.count;
	for (int i = 0; i < len - 1; i++) {
		changed = false;
		for (int j = 0; j < len - 1 - i; j++) {
			bool swapIt;
			if (排序方向是否为从小到大) {
				swapIt = (数值数组变量.pArr[j] > 数值数组变量.pArr[j + 1]);
			} else {
				swapIt = (数值数组变量.pArr[j] < 数值数组变量.pArr[j + 1]);
			}
			if (swapIt) {
				交换变量(数值数组变量.pArr[j], 数值数组变量.pArr[j + 1]);
				changed = true;
			}
		}
		if (!changed) break;
	}
}

template<typename T, int C, typename E>
int 数组_查找成员(const Arraybe<T, C>& 数组变量, E&& 欲查找的成员数据) {
	int len = 数组变量.count;
	for (int i = 0; i < len; i++) {
		if (欲查找的成员数据 == 数组变量.pArr[i]) {
			return i;
		}
	}
	return -1;
}

template<typename T, int C, typename E>
bool 数组_加入不重复成员(Arraybe<T, C>& 数组变量, E&& 欲加入的成员数据) {
	if (数组_查找成员(数组变量, be::forward<E>(欲加入的成员数据)) == -1) {
		数组变量.push(be::forward<E>(欲加入的成员数据));
		return true;
	}
	return false;
}

template<typename T, int C, typename E>
bool 数组_删除元素值成员(Arraybe<T, C>& 数组变量, E&& 欲删除的成员数据, bool 删除全部 = false) {
	if (!删除全部) {
		int i = 数组_查找成员(数组变量, 欲删除的成员数据);
		if (i == -1) return false;
		数组变量.del(i);
		return true;
	} else {
		int rCount = 数组变量.count;
		for (int i = 数组变量.count - 1; i >= 0; i--) {
			if (数组变量.pArr[i] == 欲删除的成员数据) {
				数组变量.del(i);
			}
		}
		return rCount != 数组变量.count;
	}
}

template<typename T, int C1, int C2, int C3>
void 数组_元素值取并集(const Arraybe<T, C1>& 数组1, const Arraybe<T, C2>& 数组2,
	Arraybe<T, C3>& 返回数组)
{
	返回数组.clear();
	int len1 = 数组1.count;
	for (int i = 0; i < len1; i++) {
		数组_加入不重复成员(返回数组, 数组1.pArr[i]);
	}
	int len2 = 数组2.count;
	for (int i = 0; i < len2; i++) {
		数组_加入不重复成员(返回数组, 数组2.pArr[i]);
	}
}

template<typename T, int C1, int C2, int C3>
void 数组_元素值取交集(const Arraybe<T, C1>& 数组1, const Arraybe<T, C2>& 数组2,
	Arraybe<T, C3>& 返回数组)
{
	返回数组.clear();
	int len1 = 数组1.count;
	for (int i = 0; i < len1; i++) {
		if (数组_查找成员(数组2, 数组1.pArr[i]) != -1) {
			数组_加入不重复成员(返回数组, 数组1.pArr[i]);
		}
	}
}

// 元素值取差集（取出数组1中不存在于数组2里的独有成员）
template<typename T, int C1, int C2, int C3>
void 数组_元素值取差集(const Arraybe<T, C1>& 数组1, const Arraybe<T, C2>& 数组2,
	Arraybe<T, C3>& 返回数组)
{
	返回数组.clear();
	int len1 = 数组1.count;
	for (int i = 0; i < len1; i++) {
		if (数组_查找成员(数组2, 数组1.pArr[i]) == -1) {
			数组_加入不重复成员(返回数组, 数组1.pArr[i]);
		}
	}
}

/**数组倒序。首尾成员互换，直至中心
 * @param 数组变量
 */
template<typename T, int C>
void 数组_反转(Arraybe<T, C>& 数组变量) {
	int len = 数组变量.count;
	int half = len / 2;
	for (int i = 0; i < half; i++) {
		be::swap(数组变量.pArr[i], 数组变量.pArr[len - 1 - i]);
	}
}

/**数组本身保留唯一元素，删除重复项。不产生任何新数组内存分配，直接在原内存上重排。
 * @param 数组变量
 */
template<typename T, int C>
void 数组_去重(Arraybe<T, C>& 数组变量) {
	int len = 数组变量.count;
	if (len <= 1) return;

	int uniqueCount = 1; // 索引0由于是第一个，必定是唯一的
	for (int i = 1; i < len; i++) {
		bool isDuplicate = false;
		for (int j = 0; j < uniqueCount; j++) {
			if (数组变量.pArr[i] == 数组变量.pArr[j]) {
				isDuplicate = true;
				break;
			}
		}
		if (!isDuplicate) {
			if (i != uniqueCount) {
				be::swap(数组变量.pArr[uniqueCount], 数组变量.pArr[i]);
			}
			uniqueCount++;
		}
	}

	// 截断尾部被替换掉的重复数据（显式析构）
	if (uniqueCount < len) {
		for (int i = uniqueCount; i < len; i++) {
			数组变量.pArr[i].~T();
		}
		数组变量.count = uniqueCount;
	}
}

/**将数组中每个成员清回以T的默认构造函数值
 * @param 数值数组变量
 */
template<typename T, int C>
void 数组_清零(Arraybe<T, C>& 数值数组变量) {
	int len = 数值数组变量.count;
	for (int i = 0; i < len; i++) {
		数值数组变量.pArr[i] = T();
	}
}
#pragma endregion
