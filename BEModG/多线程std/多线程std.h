#pragma once
/**@ModuleTitle:
*  @version:     1.0
*  @platform:    win32(x86|x64) / linux
*  @compiler:    source
*  @description: 零STL头文件依赖的跨平台轻量多线程与互斥锁封装
*/
#include "stdafx.h"


#ifdef _WIN32
#include <windows.h>
#define BE_THREAD_HANDLE HANDLE
#else
#include <pthread.h>
#define BE_THREAD_HANDLE pthread_t
#endif

/**32位整数原子操作。
 * 所有接口默认使用 std::memory_order_seq_cst，提供最直观的跨线程顺序语义。
 */
class 原子INT
{
public:
	原子INT(int value = 0);
	~原子INT();

	原子INT(const 原子INT&) = delete;
	原子INT& operator=(const 原子INT&) = delete;

	int 读取() const;
	void 写入(int value);
	int 交换(int value);
	bool 比较交换(int& expected, int desired);
	int 加(int value);
	int 减(int value);

private:
	alignas(int) char _storage[4];
};


/**64位整数原子操作。*/
class 原子INT64
{
public:
	原子INT64(int64 value = 0);
	~原子INT64();

	原子INT64(const 原子INT64&) = delete;
	原子INT64& operator=(const 原子INT64&) = delete;

	int64 读取() const;
	void 写入(int64 value);
	int64 交换(int64 value);
	bool 比较交换(int64& expected, int64 desired);
	int64 加(int64 value);
	int64 减(int64 value);

private:
	alignas(int64) char _storage[8];
};
/**在Win32中底层实质是线程许可证（并非内核锁的CreateMutex）
 */
class 互斥锁
{
public:
	互斥锁();

	~互斥锁();

	互斥锁(const 互斥锁&) = delete;
	互斥锁& operator=(const 互斥锁&) = delete;

	void 加锁();

	/**对于已经上过线程的锁若再继续对(在别的线程)上同个锁则会等待
	   此处可允许线程尝试加锁并立即返回，返回false表示锁当前被其他线程持有
	 * @return
	 */
	bool 尝试加锁();

	void 解锁();

private:
	alignas(size_t) char _storage[80];
};
/*condition_variable 底层封装的是 (Vista之后)Win32 原生提供的条件变量API原语
cv.wait()       →  SleepConditionVariableCS()
cv.notify_one() →  WakeConditionVariable()
cv.notify_all() →  WakeAllConditionVariable()
//通常能在用户态完成短程操作，只有真正需要睡眠时才陷入内核。
*/

//类似于用户态的CreateEvent（但这里还封装了可自定义条件版）
class 条件锁
{
public:
	条件锁();

	~条件锁();

	条件锁(const 条件锁&) = delete;
	条件锁& operator=(const 条件锁&) = delete;

	void 等待(互斥锁& lock);

	//template <typename Predicate>
	//void 等待(互斥锁& lock, Predicate pred)
	//{
	//	std::mutex& m = *reinterpret_cast<std::mutex*>(&lock);
	//	reinterpret_cast<std::condition_variable_any*>(_storage)->wait(m, pred);
	//}
	// 实际上C++这个带回调参的底层实现跟上边的简单while循环包装是一样的
	template <typename Predicate>
	void 等待(互斥锁& lock, Predicate pred)
	{
		while (!pred()) 等待(lock);
	}
	//（但由于模板会完整保留原始定义，我们这里希望不暴露任何C++接口）

	void 唤醒一个();

	void 唤醒全部();

private:
	alignas(void*) char _storage[128];
};
/*condition_variable 底层封装的是 (Vista之后)Win32 原生提供的SRWLOCK API原语
shared_lock/unlock   →  AcquireSRWLockShared()    / ReleaseSRWLockShared()
unique_lock/unlock   →  AcquireSRWLockExclusive() / ReleaseSRWLockExclusive()
//通常能在用户态完成短程操作，只有真正需要睡眠时才陷入内核。
*/

class 读写锁
{
public:
	读写锁();

	~读写锁();

	读写锁(const 读写锁&) = delete;
	读写锁& operator=(const 读写锁&) = delete;

	void 写_加锁();

	bool 写_尝试加锁();

	void 写_解锁();

	void 读_加锁();

	bool 读_尝试加锁();

	void 读_解锁();

private:
	alignas(void*) char _storage[80];
};
/**构造时可自动上锁、出了作用域就自动解锁的锁套壳
 */
template <bool 立即加锁 = true, typename MutexType = 互斥锁>
class 自动套锁
{
public:
	explicit 自动套锁(MutexType& mtx) : mtx(mtx)
	{
		if constexpr (立即加锁)
			mtx.加锁();
	}
	
	~自动套锁()
	{
		mtx.解锁();
	}

	自动套锁(const 自动套锁&) = delete;
	自动套锁& operator=(const 自动套锁&) = delete;
	MutexType& mtx;
};
#ifdef _WIN32
#pragma comment(lib, "Synchronization.lib")

/**轻量级用户线程事件信号。
 * 不创建 Win32 Event 内核对象，底层使用 WaitOnAddress / WakeByAddress*。
 * 当前为手动复位事件语义：置事件后保持有信号状态，直到显式复位。
 */
class 事件信号
{
public:
	事件信号(bool 初始有信号 = false);

	事件信号(const 事件信号&) = delete;
	事件信号& operator=(const 事件信号&) = delete;

	/**等待事件变为有信号状态。*/
	void 等待();

	/**将事件置为有信号状态，并唤醒所有等待线程。*/
	void 置事件();

	/**将事件恢复为无信号状态。*/
	void 复位();

	/**判断当前是否处于有信号状态。*/
	bool 有信号() const;

private:
	原子INT _state;
};


/**轻量级用户线程自动复位事件信号。
 * 不创建 Win32 Event 内核对象，底层使用 WaitOnAddress / WakeByAddressSingle。
 * 每次置事件最多保存一个信号；一个等待线程成功消费后会自动恢复为无信号状态。
 */
class 事件信号自动
{
public:
	事件信号自动(bool 初始有信号 = false);

	事件信号自动(const 事件信号自动&) = delete;
	事件信号自动& operator=(const 事件信号自动&) = delete;

	/**等待并消费一次事件信号。成功返回后事件自动恢复为无信号状态。*/
	void 等待();

	/**置为有信号状态；若已有信号则继续保持一个信号，不累积。*/
	void 置事件();

	/**显式清除尚未被消费的信号。*/
	void 复位();

	/**判断当前是否保存着一个尚未被消费的信号。*/
	bool 有信号() const;

private:
	原子INT _state;
};
#endif

class 线程
{
public:
	using 线程函数 = be::function<void(), 128>;

	线程();

	explicit 线程(线程函数 fn);

	~线程();

	线程(线程&& other) noexcept;

	线程& operator=(线程&& other) noexcept;

	线程(const 线程&) = delete;
	线程& operator=(const 线程&) = delete;

	void 分离();

	void 等待();

	/**判断该 std::thread 对象是否关联着一个真实的、可操作的线程。
		PS：只有创建后、且未 join/detach 的线程对象才是 true。
	 * @return
	 */
	bool 可等待() const;

	/**获取底层原生线程句柄（Win32下为 HANDLE，Linux下为 pthread_t）
	 * @return 原生线程句柄
	 */
	BE_THREAD_HANDLE 取句柄();

	/**创建线程后自动分离
	 * @param fn
	 */
	static void 启动(线程函数 fn);

private:
	alignas(void*) char _storage[16];
};
