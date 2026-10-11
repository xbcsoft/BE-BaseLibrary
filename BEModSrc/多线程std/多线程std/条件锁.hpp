/*condition_variable 底层封装的是 (Vista之后)Win32 原生提供的条件变量API原语
cv.wait()       →  SleepConditionVariableCS()
cv.notify_one() →  WakeConditionVariable()
cv.notify_all() →  WakeAllConditionVariable()
//通常能在用户态完成短程操作，只有真正需要睡眠时才陷入内核。
*/

//类似于用户态的CreateEvent（但这里还封装了可自定义条件版）
class EXP 条件锁
{
public:
	条件锁()
	{
		new (_storage) std::condition_variable_any();
	}

	~条件锁()
	{
		reinterpret_cast<std::condition_variable_any*>(_storage)->~condition_variable_any();
	}

	条件锁(const 条件锁&) = delete;
	条件锁& operator=(const 条件锁&) = delete;

	void 等待(互斥锁& lock)
	{
		std::mutex& m = *reinterpret_cast<std::mutex*>(&lock);
		reinterpret_cast<std::condition_variable_any*>(_storage)->wait(m);
	}

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

	void 唤醒一个()
	{
		reinterpret_cast<std::condition_variable_any*>(_storage)->notify_one();
	}

	void 唤醒全部()
	{
		reinterpret_cast<std::condition_variable_any*>(_storage)->notify_all();
	}

private:
	alignas(void*) char _storage[128];
};
