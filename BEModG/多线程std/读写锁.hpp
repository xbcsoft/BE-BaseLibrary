/*condition_variable 底层封装的是 (Vista之后)Win32 原生提供的SRWLOCK API原语
shared_lock/unlock   →  AcquireSRWLockShared()    / ReleaseSRWLockShared()
unique_lock/unlock   →  AcquireSRWLockExclusive() / ReleaseSRWLockExclusive()
//通常能在用户态完成短程操作，只有真正需要睡眠时才陷入内核。
*/

class EXP 读写锁
{
public:
	读写锁()
	{
		new (_storage) std::shared_mutex();
	}

	~读写锁()
	{
		reinterpret_cast<std::shared_mutex*>(_storage)->~shared_mutex();
	}

	读写锁(const 读写锁&) = delete;
	读写锁& operator=(const 读写锁&) = delete;

	void 写_加锁()
	{
		reinterpret_cast<std::shared_mutex*>(_storage)->lock();
	}

	bool 写_尝试加锁()
	{
		return reinterpret_cast<std::shared_mutex*>(_storage)->try_lock();
	}

	void 写_解锁()
	{
		reinterpret_cast<std::shared_mutex*>(_storage)->unlock();
	}

	void 读_加锁()
	{
		reinterpret_cast<std::shared_mutex*>(_storage)->lock_shared();
	}

	bool 读_尝试加锁()
	{
		return reinterpret_cast<std::shared_mutex*>(_storage)->try_lock_shared();
	}

	void 读_解锁()
	{
		reinterpret_cast<std::shared_mutex*>(_storage)->unlock_shared();
	}

private:
	alignas(void*) char _storage[80];
};
