#pragma once

/**32位整数原子操作。
 * 所有接口默认使用 std::memory_order_seq_cst，提供最直观的跨线程顺序语义。
 */
class EXP 原子INT
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

原子INT::原子INT(int value)
{
	new (_storage) std::atomic<int>(value);
}

原子INT::~原子INT()
{
	reinterpret_cast<std::atomic<int>*>(_storage)->~atomic();
}

int 原子INT::读取() const
{
	return reinterpret_cast<const std::atomic<int>*>(_storage)->load();
}

void 原子INT::写入(int value)
{
	reinterpret_cast<std::atomic<int>*>(_storage)->store(value);
}

int 原子INT::交换(int value)
{
	return reinterpret_cast<std::atomic<int>*>(_storage)->exchange(value);
}

bool 原子INT::比较交换(int& expected, int desired)
{
	return reinterpret_cast<std::atomic<int>*>(_storage)->compare_exchange_strong(expected, desired);
}

int 原子INT::加(int value)
{
	return reinterpret_cast<std::atomic<int>*>(_storage)->fetch_add(value);
}

int 原子INT::减(int value)
{
	return reinterpret_cast<std::atomic<int>*>(_storage)->fetch_sub(value);
}


/**64位整数原子操作。*/
class EXP 原子INT64
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

原子INT64::原子INT64(int64 value)
{
	new (_storage) std::atomic<int64>(value);
}

原子INT64::~原子INT64()
{
	reinterpret_cast<std::atomic<int64>*>(_storage)->~atomic();
}

int64 原子INT64::读取() const
{
	return reinterpret_cast<const std::atomic<int64>*>(_storage)->load();
}

void 原子INT64::写入(int64 value)
{
	reinterpret_cast<std::atomic<int64>*>(_storage)->store(value);
}

int64 原子INT64::交换(int64 value)
{
	return reinterpret_cast<std::atomic<int64>*>(_storage)->exchange(value);
}

bool 原子INT64::比较交换(int64& expected, int64 desired)
{
	return reinterpret_cast<std::atomic<int64>*>(_storage)->compare_exchange_strong(expected, desired);
}

int64 原子INT64::加(int64 value)
{
	return reinterpret_cast<std::atomic<int64>*>(_storage)->fetch_add(value);
}

int64 原子INT64::减(int64 value)
{
	return reinterpret_cast<std::atomic<int64>*>(_storage)->fetch_sub(value);
}
