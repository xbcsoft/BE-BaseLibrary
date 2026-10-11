/**@ModuleTitle:
*  @version:     1.0
*  @platform:    win32(x86|x64) / linux
*  @compiler:    source
*  @description: 零STL头文件依赖的跨平台轻量多线程与互斥锁封装
*/
#include "stdafx.h"

#pragma region BE_IGNORE
#include <thread>
#include <atomic>
#include <mutex>
#include <utility>
#include <condition_variable>
#include <shared_mutex>
#pragma endregion
#ifdef _WIN32
#include <windows.h>
#define BE_THREAD_HANDLE HANDLE
#else
#include <pthread.h>
#define BE_THREAD_HANDLE pthread_t
#endif

#pragma region BE_INLINE
#include "原子操作.hpp"
#include "互斥锁.hpp"
#include "条件锁.hpp"
#include "读写锁.hpp"
#include "自动套锁.hpp"
#ifdef _WIN32
#include "事件信号Win32.hpp" //C++17目前并无替代品
#endif
#pragma endregion

class EXP 线程
{
public:
	using 线程函数 = be::function<void(), 128>;

	线程()
	{
		new (_storage) std::thread();
	}

	explicit 线程(线程函数 fn)
	{
		new (_storage) std::thread([fn = be::move(fn)]() mutable {
			if (fn) fn();
		});
	}

	~线程()
	{
		auto* t = reinterpret_cast<std::thread*>(_storage);
		if (t->joinable()) {
			t->detach();
		}
		t->~thread();
	}

	线程(线程&& other) noexcept
	{
		auto* ot = reinterpret_cast<std::thread*>(other._storage);
		new (_storage) std::thread(std::move(*ot));
	}

	线程& operator=(线程&& other) noexcept
	{
		if (this != &other) {
			auto* t = reinterpret_cast<std::thread*>(_storage);
			auto* ot = reinterpret_cast<std::thread*>(other._storage);
			if (t->joinable()) {
				t->detach();
			}
			*t = std::move(*ot);
		}
		return *this;
	}

	线程(const 线程&) = delete;
	线程& operator=(const 线程&) = delete;

	void 分离()
	{
		reinterpret_cast<std::thread*>(_storage)->detach();
	}

	void 等待()
	{
		reinterpret_cast<std::thread*>(_storage)->join();
	}

	/**判断该 std::thread 对象是否关联着一个真实的、可操作的线程。
		PS：只有创建后、且未 join/detach 的线程对象才是 true。
	 * @return
	 */
	bool 可等待() const
	{
		return reinterpret_cast<const std::thread*>(_storage)->joinable();
	}

	/**获取底层原生线程句柄（Win32下为 HANDLE，Linux下为 pthread_t）
	 * @return 原生线程句柄
	 */
	BE_THREAD_HANDLE 取句柄()
	{
		return reinterpret_cast<std::thread*>(_storage)->native_handle();
	}

	/**创建线程后自动分离
	 * @param fn
	 */
	static void 启动(线程函数 fn)
	{
		线程 t(be::move(fn));
		t.分离();
	}

private:
	alignas(void*) char _storage[16];
};
