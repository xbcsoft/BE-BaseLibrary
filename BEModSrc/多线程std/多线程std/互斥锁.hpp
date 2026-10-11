/**在Win32中底层实质是线程许可证（并非内核锁的CreateMutex）
 */
class EXP 互斥锁
{
public:
	互斥锁()
	{
		new (_storage) std::mutex();
	}

	~互斥锁()
	{
		reinterpret_cast<std::mutex*>(_storage)->~mutex();
	}

	互斥锁(const 互斥锁&) = delete;
	互斥锁& operator=(const 互斥锁&) = delete;

	void 加锁()
	{
		reinterpret_cast<std::mutex*>(_storage)->lock();
	}

	/**对于已经上过线程的锁若再继续对(在别的线程)上同个锁则会等待
	   此处可允许线程尝试加锁并立即返回，返回false表示锁当前被其他线程持有
	 * @return
	 */
	bool 尝试加锁()
	{
		return reinterpret_cast<std::mutex*>(_storage)->try_lock();
	}

	void 解锁()
	{
		reinterpret_cast<std::mutex*>(_storage)->unlock();
	}

private:
	alignas(size_t) char _storage[80];
};
