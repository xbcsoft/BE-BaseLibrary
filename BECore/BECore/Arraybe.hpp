#pragma once
#include "BytesStr.hpp"

namespace be {
// 利用编译器内置魔法判定可平凡拷贝以脱离 <type_traits> 头文件依赖
// AI告诉我，无论在 MSVC、GCC 还是 Clang 下，它们检测的魔法口令均一致
template<typename T>
struct is_trivially_relocatable {
	static constexpr bool value = __is_trivially_copyable(T);
};
// 为核心类型发放免检通行证
template<> struct is_trivially_relocatable<Bytes> { static constexpr bool value = true; };
template<> struct is_trivially_relocatable<StrA> { static constexpr bool value = true; };
template<> struct is_trivially_relocatable<StrW> { static constexpr bool value = true; };
template<> struct is_trivially_relocatable<StrU8> { static constexpr bool value = true; };

template<typename T>
struct nothrow_movable {
#ifdef _MSC_VER
	static constexpr bool is = __is_nothrow_constructible(T, T&&);
#else
	template<typename U>
	static auto test(int) -> decltype(U(static_cast<U&&>(*static_cast<U*>(nullptr))), char(0));
	template<typename>
	static int test(...);

	static constexpr bool is = noexcept(T(static_cast<T&&>(*static_cast<T*>(nullptr)))) &&
		sizeof(test<T>(0)) == sizeof(char);
#endif
};
}

#pragma region 数组
//本数组默认没缩减机制使用者需自行_hookDelete，并定制策略逻辑(可使用_shrink)来缩减它
template<typename T_ELE, int STACK_CAP = 0>
class Arraybe
{
public:
	T_ELE* pArr = NULL; //容器指针
	int count;          //当前实际有多少个T_ELE成员
	int capacity_;      //当前容器的内部最大个数(总共可容纳多少个T_ELE成员)
	/**Arraybe构造函数
	 * @param initCount 初始个数
	 * @param capacity 保留容量
	 * @param increment 不足时的线性增长个数
	 */
	Arraybe(int initCount = 0, int capacity = 0, int increment = 5);
	Arraybe(std::initializer_list<T_ELE> arr, int increment = 5);
	Arraybe(const Arraybe& arr) { _copy(arr); }
	Arraybe(Arraybe&& arr) noexcept { __copy(arr); arr.capacity_ = 0; }

	template<typename ANY_ELE>
	Arraybe(const Arraybe<ANY_ELE>& other) : Arraybe() {
		if (other.count > capacity_) _capacity(other.count);
		for (int i = 0; i < other.count; i++) {
			new(&pArr[count++]) T_ELE(other.pArr[i]);
		}
	}
	~Arraybe() { _destruct(); }

public:
	void push(const T_ELE& elem);              //将元素添加到数组最后一个位置
	void push(T_ELE&& elem);                   //移动浅内存复制添最后一个位置
	bool pop(T_ELE& elem);                     //取出并删除最后一个元素
	bool pop();								   //取出并删除最后一个元素
	T_ELE& top();                              //即便count=0也取出空元素不产生异常
	bool del(int index, int delCount = 1);     //删除指定元素
	bool insert(int index, const T_ELE& elem); //向指定位置新增一个元素
	bool insert(int index, const T_ELE* src, int insertCount, bool deepCopy = true); //向指定位置批量新增元素
	void reset(int n = 0, bool retainContent = false, bool shrinkCapacity = false);
	void clear();


public: //操作运算符
	T_ELE& operator[](int i) { return pArr[i]; }
	const T_ELE& operator[](int i) const { return pArr[i]; }
	T_ELE& at(int i);
	const T_ELE& at(int i) const { return const_cast<Arraybe&>(*this).at(i); }
	Arraybe<T_ELE>& operator=(const Arraybe& arr) {
		if (arr.pArr!=pArr) { _destruct(); _copy(arr); }
		return *this;
	}

	//用于支持for(auto& item : arr)的遍历
	struct Iterator {
		T_ELE* p; Iterator(T_ELE* p) :p(p) {}
		T_ELE& operator*() { return *p; }
		Iterator& operator++() { p++; return *this; }
		bool operator!=(const Iterator & other) const { return p!=other.p; }
	};
	Iterator begin() const { return Iterator(pArr); }
	Iterator end() const { return Iterator(pArr + count); }
public: //半公开的内部函数
	int  _capacity();                 //获取容量
	void _capacity(int newCapacity);  //设定到目标容量(扩或缩)
	void _shrink();                   //收缩内存
	typedef void(*FnHookCap)(Arraybe<T_ELE, STACK_CAP>& self);
	void _hookExpand(FnHookCap fn) { _fnHook_exp = fn; } //回调中将传递本数组容器
	void _hookDelete(FnHookCap fn) { _fnHook_del = fn; } //回调中将传递本数组容器
	char* _get_stackBuf() { return _stackBuf; }
private:
	int increment_;               //每次内存不足时的扩容个数
	FnHookCap _fnHook_exp = NULL; //供开发者hook内部扩容时机
	FnHookCap _fnHook_del = NULL; //供开发者hook内部删除时机(可进行缩放机制)

	bool usingStack;              //是否正在使用栈缓冲
	// 【注意：为什么必须加 alignas(T_ELE)？】
	// 首先为了避免直接构造 T_ELE 产生的性能损耗，使用 char 数组来预留"空间大小 (sizeof)"
	// 但如果仅预留空间，char 的默认对齐系数是 1，这意味着 _stackBuf 在本类实例中的起始内存偏移极可能不规整（比如停在偏移值41处）
	// 后续当你在不规整的地址上强行 placement new(在这段内存上放T_ELE对象) 时：
	// 在 ARM/严格指令集芯片 或 使用了SIMD优化的结构体上，只要内存没有对齐到该结构的自然边界，硬件会直接引发总线错误当场崩溃。
	// 所以我们通过 alignas(T_ELE) 强制要求编译器按照 T_ELE 的规矩来，如果前方的 bool 等变量导致了错位，
	// 编译器会自动帮此栈区填补一些 padding (废料占位符)，从而确保 _stackBuf 被分配出来的绝对 起步地址 永远落在安全的对齐边界上！
	alignas(T_ELE) char _stackBuf[STACK_CAP ? sizeof(T_ELE)* STACK_CAP : 1]; //栈上预分配可用容量

	void _construct() {
		pArr = (T_ELE*)malloc(sizeof(T_ELE)*capacity_);
		for (int i = 0; i<count; i++)new(pArr+i)T_ELE();
	}
	void _destruct() {
		if (capacity_) {
			//手动析构已构造的元素
			for (int i = 0; i < count; i++)
				pArr[i].~T_ELE();
			//只有堆内存才需要free
			if (!usingStack)free(pArr);
		}
	}
	//src开始搬移n个元素到dest(适用给内存搬新家以及同数组向左移位，isInnerShift判定是否为内部删除移位)
	void _relocate(T_ELE* dest, T_ELE* src, int n, bool isInnerShift = false) {
		if (n <= 0) return;
		if constexpr (be::is_trivially_relocatable<T_ELE>::value) {
			memmove(dest, src, n * sizeof(T_ELE));
			return;
		}
		if constexpr (be::nothrow_movable<T_ELE>::is) {
			for (int i = 0; i < n; i++) {
				new(&dest[i]) T_ELE((T_ELE&&)src[i]);
				src[i].~T_ELE();
			}
		} else {
			int shifted = 0;
			try {
				for (int i = 0; i < n; i++) {
					new(&dest[i]) T_ELE(src[i]);
					// 左移(内部重叠)时，即刻对源位置析构防盖且占位空洞
					if (isInnerShift) src[i].~T_ELE();
					shifted++;
				}
				// 搬新家场景，所有元素均安稳构造在新家后，再一次性清理旧家
				if (!isInnerShift) {
					for (int i = 0; i < n; i++) {
						src[i].~T_ELE();
					}
				}
			} catch (...) {
				// 无论如何，析构这一轮刚刚新构造的对象
				for (int i = 0; i < shifted; i++) {
					dest[i].~T_ELE();
				}
				if (isInnerShift) {
					//内部移位时截断原本在后侧还未来得及移动的对象
					for (int i = shifted; i < n; i++) {
						src[i].~T_ELE();
					}
					//dest恰好是边界起始位置，dest-pArr就是安全的count长度
					count = (int)(dest - pArr);
				}
				throw;
			}
		}
	}
	//将pArr[index]起的n个元素向右移动offset个位置(反向拷贝防覆盖)
	void _shiftRight(int index, int n, int offset = 1) {
		if (n <= 0 || offset <= 0) return;
		if constexpr (be::is_trivially_relocatable<T_ELE>::value) {
			memmove(pArr+index+offset, pArr+index, n * sizeof(T_ELE));
			return;
		}
		if constexpr (be::nothrow_movable<T_ELE>::is) {
			for (int i = n - 1; i >= 0; i--) {
				new(&pArr[index+offset+i]) T_ELE((T_ELE&&)pArr[index+i]);
				pArr[index+i].~T_ELE();
			}
		} else {
			int shifted = 0;
			try {
				for (int i = n - 1; i >= 0; i--) {
					new(&pArr[index+offset+i]) T_ELE(pArr[index+i]);
					pArr[index+i].~T_ELE();
					shifted++;
				}
			} catch (...) {
				for (int i = 0; i < shifted; i++) {
					pArr[index + offset + n - 1 - i].~T_ELE();
				}
				count = count - shifted; //最大限度保留还未移动的部分
				throw;
			}
		}
	}
	void _copy(const Arraybe& arr) {
		__copy(arr);
		if (arr.usingStack) {
			//源数组在栈上，则就地构造
			pArr = (T_ELE*)_stackBuf;
		} else {
			//源数组在堆上，分配堆内存
			pArr = (T_ELE*)malloc(sizeof(T_ELE)*capacity_);
		}
		for (int i = 0; i < count; i++) {
			new(&pArr[i]) T_ELE(arr.pArr[i]);
		}
	}
	void __copy(const Arraybe& arr) {
		//复制基本成员
		pArr = arr.pArr;
		count = arr.count;
		capacity_ = arr.capacity_;
		increment_ = arr.increment_;
		usingStack = arr.usingStack;
		_fnHook_exp = arr._fnHook_exp;
		_fnHook_del = arr._fnHook_del;
	}
};

template<typename T_ELE, int STACK_CAP /*= 0*/>
bool Arraybe<T_ELE, STACK_CAP>::pop()
{
	if (count==0)return false;
	--count;
	pArr[count].~T_ELE();
	if (_fnHook_del)_fnHook_del(*this);
	return true;
}

template<typename T_ELE, int STACK_CAP>
inline Arraybe<T_ELE, STACK_CAP>::Arraybe(std::initializer_list<T_ELE> arr,
	int increment) :increment_(increment)
{
	count = (int)arr.size();
	if constexpr (STACK_CAP >= 1) {
		// 强制使用栈，超出容量抛异常（arr.size做不了静态断言只能运行时检查）
		if (count > STACK_CAP) {
			throw "Array initializer_list size > STACK_CAP";
		}
		usingStack = true;
		capacity_ = STACK_CAP;
		pArr = (T_ELE*)_stackBuf;
	} else {
		// STACK_CAP = 0，使用堆
		usingStack = false;
		capacity_ = count;
		pArr = (T_ELE*)malloc(count*sizeof(T_ELE));
	}

	// 对缓冲区地址就地拷贝构造元素
	int i = 0;
	for (auto& v : arr) {
		new (&pArr[i++]) T_ELE(v);
	}
}

template<class T_ELE, int STACK_CAP>
inline T_ELE& Arraybe<T_ELE, STACK_CAP>::at(int i)
{
	int n = count;
	if (i < 0) i += n;
	if (i < 0 || i >= n) throw be::range_error{ (ssize_t)n, (ssize_t)i };
	return pArr[i];
}

template<typename T_ELE, int STACK_CAP>
inline Arraybe<T_ELE, STACK_CAP>::Arraybe(int initCount/*=0*/, int capacity/*=0*/, int increment/*=5*/)
{
	increment_ = increment;

	//判断是否使用栈缓冲：STACK_CAP >= 1 且 capacity 参数为0 且 initCount 不超过 STACK_CAP
	if (STACK_CAP >= 1 && capacity == 0 && initCount <= STACK_CAP) {
		// 使用栈缓冲（char数组不会自动构造/析构）
		usingStack = true;
		pArr = (T_ELE*)_stackBuf;
		count = initCount;
		capacity_ = STACK_CAP;
		// 在栈上构造 initCount 个元素
		for (int i = 0; i < initCount; i++) {
			new(&pArr[i]) T_ELE();
		}
	} else {
		// 使用堆分配
		usingStack = false;
		if (initCount > capacity)
			capacity = initCount;
		count = initCount;
		capacity_ = capacity;
		if (capacity_) _construct();
	}
}

template<class T_ELE, int STACK_CAP>
inline bool Arraybe<T_ELE, STACK_CAP>::del(int index, int delCount)
{
	if (index < 0 || index >= count || delCount <= 0)
		return false;

	int endIndex = index + delCount;
	if (endIndex > count)endIndex = count;

	for (int i = index; i < endIndex; ++i)
		pArr[i].~T_ELE(); //手动析构
	int moveCount = count - endIndex;
	if (moveCount > 0)
		_relocate(pArr + endIndex - delCount, pArr + endIndex, moveCount, true);
	count -= delCount;

	if (_fnHook_del)_fnHook_del(*this);
	return true;
}
template<class T_ELE, int STACK_CAP>
inline T_ELE& Arraybe<T_ELE, STACK_CAP>::top()
{
	static T_ELE null_elem = { 0 };
	if (count==0)return null_elem;
	return pArr[count-1];
}

template<class T_ELE, int STACK_CAP>
inline void Arraybe<T_ELE, STACK_CAP>::push(const T_ELE& elem)
{   //拷贝构造
	if (count==capacity_)_capacity(capacity_+increment_);
	new(&pArr[count++])T_ELE(elem);
}
template<typename T_ELE, int STACK_CAP>
inline void Arraybe<T_ELE, STACK_CAP>::push(T_ELE&& elem)
{   //移动构造
	if (count==capacity_)_capacity(capacity_+increment_);
	new (&pArr[count++]) T_ELE(static_cast<T_ELE&&>(elem));
}

template<typename T_ELE, int STACK_CAP>
inline int Arraybe<T_ELE, STACK_CAP>::_capacity()
{
	return capacity_;
}
template<class T_ELE, int STACK_CAP>
inline void Arraybe<T_ELE, STACK_CAP>::_capacity(int newCapacity)
{
	if (newCapacity < capacity_) {
		reset(newCapacity, true, true);
		return;
	}
	if (_fnHook_exp) {
		FnHookCap tempHook = _fnHook_exp;
		_fnHook_exp = NULL;
		tempHook(*this);
		_fnHook_exp = tempHook;
		return;
	}

	if constexpr (be::is_trivially_relocatable<T_ELE>::value) {
		if (!usingStack) {
			//堆上平凡扩容：直接realloc
			capacity_ = newCapacity;
			pArr = (T_ELE*)realloc(pArr, sizeof(T_ELE)*capacity_);
			return;
		}
	}

	//栈切堆 或 堆上非平凡扩容：分配新内存并搬移
	T_ELE* newBuf = (T_ELE*)malloc(sizeof(T_ELE)*newCapacity);
	_relocate(newBuf, pArr, count);
	if (!usingStack) free(pArr);
	pArr = newBuf; usingStack = false; capacity_ = newCapacity;
}
template<class T_ELE, int STACK_CAP>
inline bool Arraybe<T_ELE, STACK_CAP>::insert(int index, const T_ELE& elem)
{
	//判断参数所给的区间是否合理
	if (index<0 || index>count)return false;

	//判断扩容的时机
	if (count == capacity_)_capacity(capacity_+increment_);

	//将从index的位置开始向后转移
	_shiftRight(index, count - index, 1);

	try {
		//调用拷贝构造
		new(&pArr[index])T_ELE(elem);
	} catch (...) {
		//恢复无望且为避免产生空悬空洞，只能截断丢失部分(包括已成功向后移位的数据)
		for (int i = 0; i < count - index; i++) {
			pArr[index + 1 + i].~T_ELE();
		}
		count = index;
		throw;
	}

	//插完后要修正当前实际长度
	count++;
	return true;
}

template<class T_ELE, int STACK_CAP>
inline bool Arraybe<T_ELE, STACK_CAP>::insert(int index, const T_ELE* src, int insertCount, bool deepCopy)
{
	if (index < 0 || index > count || insertCount <= 0 || !src) return false;

	int newCount = count + insertCount;
	if (newCount > capacity_) {
		int targetCap = capacity_ + increment_;
		if (targetCap < newCount) targetCap = newCount;
		_capacity(targetCap);
	}

	_shiftRight(index, count - index, insertCount);

	int constructed = 0;
	try {
		if (deepCopy) {
			for (int i = 0; i < insertCount; i++) {
				new(&pArr[index + i]) T_ELE(src[i]);
				constructed++;
			}
		} else {
			memcpy(&pArr[index], src, insertCount * sizeof(T_ELE));
			constructed = insertCount;
		}
	} catch (...) {
		//清理掉刚刚插入构造成功的部分
		for (int i = 0; i < constructed; i++) {
			pArr[index + i].~T_ELE();
		}
		//截断原本被向后移位保护的数据，避免异常引发内存泄漏或者错位产生的空洞UB
		for (int i = 0; i < count - index; i++) {
			pArr[index + insertCount + i].~T_ELE();
		}
		count = index;
		throw;
	}

	count += insertCount;
	return true;
}
template<class T_ELE, int STACK_CAP>
inline bool Arraybe<T_ELE, STACK_CAP>::pop(T_ELE& elem)
{
	if (count==0)return false;
	elem = pArr[--count];
	pArr[count].~T_ELE();
	if (_fnHook_del)_fnHook_del(*this);
	return true;
}

/**重定义数组
 * @param n 如果n大于现有内容即便保留原有内容也会被析构
 * @param retainContent=false 保留原有内容
 * @param shrinkCapacity=false 收缩容量到与成员数持平
 */
template<typename T_ELE, int STACK_CAP>
inline void Arraybe<T_ELE, STACK_CAP>::reset(int n /*= 0*/, bool retainContent /*= false*/,
	bool shrinkCapacity /*= false*/)
{
	if (capacity_ && n<=capacity_) {
		for (int i = n; i < count; ++i)
			pArr[i].~T_ELE(); //显式析构已超出部分
		if (shrinkCapacity && !usingStack) {
			// 只有在堆上才能 shrink
			T_ELE* tArr = (T_ELE*)malloc(sizeof(T_ELE)*n);
			if (retainContent) {
				_relocate(tArr, pArr, n);
			} else {
				for (int i = 0; i < n; i++) {
					pArr[i].~T_ELE();     //析构前边的
					new(&tArr[i])T_ELE(); //构造新前边的
				}
			}
			free(pArr);
			pArr = tArr;
			capacity_ = n;
		}
		count = n;
	} else {
		if (retainContent) {
			_capacity(n);
		} else {
			_destruct();
			count = capacity_ = n;
			usingStack = false; //扩容时切换到堆
			_construct();
		}
	}
}
template<typename T_ELE, int STACK_CAP>
inline void Arraybe<T_ELE, STACK_CAP>::_shrink()
{
	reset(count, true, true);
}
template<typename T_ELE, int STACK_CAP>
inline void Arraybe<T_ELE, STACK_CAP>::clear()
{
	reset(0);
}


#pragma endregion

template<typename T_ELE, int STACK_CAP>
inline Bytes ArrayBytes(const Arraybe<T_ELE, STACK_CAP>& arr)
{
	return Bytes(arr.pArr, BRef, arr.count*sizeof(T_ELE));
}

template<class T, int STACK_CAP = 0> using c_Arraybe = const Arraybe<T, STACK_CAP>&;