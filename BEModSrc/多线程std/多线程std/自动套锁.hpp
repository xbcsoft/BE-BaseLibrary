/**构造时可自动上锁、出了作用域就自动解锁的锁套壳
 */
template <bool 立即加锁 = true, typename MutexType = 互斥锁>
class EXP 自动套锁
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
