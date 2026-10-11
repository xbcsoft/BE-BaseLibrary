#pragma comment(lib, "Synchronization.lib")

/**轻量级用户线程事件信号。
 * 不创建 Win32 Event 内核对象，底层使用 WaitOnAddress / WakeByAddress*。
 * 当前为手动复位事件语义：置事件后保持有信号状态，直到显式复位。
 */
class EXP 事件信号
{
public:
	事件信号(bool 初始有信号 = false) : _state(初始有信号 ? 1 : 0) {}

	事件信号(const 事件信号&) = delete;
	事件信号& operator=(const 事件信号&) = delete;

	/**等待事件变为有信号状态。*/
	void 等待()
	{
		while (_state.读取() == 0)
		{
			int expected = 0;
			WaitOnAddress(&_state, &expected, sizeof(expected), INFINITE);
		}
	}

	/**将事件置为有信号状态，并唤醒所有等待线程。*/
	void 置事件()
	{
		_state.写入(1);
		WakeByAddressAll(&_state);
	}

	/**将事件恢复为无信号状态。*/
	void 复位()
	{
		_state.写入(0);
	}

	/**判断当前是否处于有信号状态。*/
	bool 有信号() const
	{
		return _state.读取() != 0;
	}

private:
	原子INT _state;
};


/**轻量级用户线程自动复位事件信号。
 * 不创建 Win32 Event 内核对象，底层使用 WaitOnAddress / WakeByAddressSingle。
 * 每次置事件最多保存一个信号；一个等待线程成功消费后会自动恢复为无信号状态。
 */
class EXP 事件信号自动
{
public:
	事件信号自动(bool 初始有信号 = false) : _state(初始有信号 ? 1 : 0) {}

	事件信号自动(const 事件信号自动&) = delete;
	事件信号自动& operator=(const 事件信号自动&) = delete;

	/**等待并消费一次事件信号。成功返回后事件自动恢复为无信号状态。*/
	void 等待()
	{
		for (;;)
		{
			int expected = 1;
			if (_state.比较交换(expected, 0))
				return;

			expected = 0;
			WaitOnAddress(&_state, &expected, sizeof(expected), INFINITE);
		}
	}

	/**置为有信号状态；若已有信号则继续保持一个信号，不累积。*/
	void 置事件()
	{
		if (_state.交换(1) == 0) WakeByAddressSingle(&_state);
	}

	/**显式清除尚未被消费的信号。*/
	void 复位()
	{
		_state.写入(0);
	}

	/**判断当前是否保存着一个尚未被消费的信号。*/
	bool 有信号() const
	{
		return _state.读取() != 0;
	}

private:
	原子INT _state;
};
