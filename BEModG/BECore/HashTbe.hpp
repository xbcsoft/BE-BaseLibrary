#pragma once
#include "behelper.hpp"

// FNV-1a 32-bit Hash 函数
inline uint fHash_FNV1a(const void* data, size_t len)
{
	const unsigned char* ptr = (const unsigned char*)data;
	uint hash = 2166136261u;
	for (size_t i = 0; i < len; ++i) {
		hash ^= ptr[i];
		hash *= 16777619;
	}
	return hash;
}

// 计算不小于x的最小2的幂
inline uint nearPowOf2(uint x) {
	if (x == 0) return 1;
	x--;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	return x + 1;
}

namespace be
{
/** 统一的 Hash 计算函数 (输出参数改为指针以移除全局变量) */
template<typename K>
inline int _hash_i_calc(const K& key, uint mask, uint* firstHash = nullptr)
{
	uint h;
	if constexpr (be::is_integral<K>) {
		h = (uint)key;
	} else if constexpr (be::sames<K, char*, const char*>) {
		h = fHash_FNV1a(key, strlen(key));
	} else if constexpr (be::sames<K, charW*, const charW*>) {
		h = fHash_FNV1a(key, strlen<W>(key) * 2);
	} else if constexpr (be::sames<K, StrA, StrU8, StrW>) {
		h = fHash_FNV1a(key._buf(), key.bytes.size);
	} else {
		h = fHash_FNV1a(&key, sizeof(K));
	}
	if (firstHash) *firstHash = h;
	return h & mask;
}

/** 为 Bytes 类型提供专用重载示例（外部自定义结构体同理，只需在 be 命名空间下重载此函数名即可） */
inline int _hash_i_calc(const Bytes& key, uint mask, uint* firstHash = nullptr)
{
	uint h = fHash_FNV1a(key.buf, key.size); // 基于内容哈希
	if (firstHash) *firstHash = h;
	return h & mask;
}
}


template<typename K, typename V>
class HashTbe
{
public:
	enum State : byte { EMPTY = 0, OCCUPIED = 1, DELETED = 2 };
	struct Node {
		K key;
		V value;
		State state = EMPTY;
	};
	int count;      //当前有效元素数量
	void(*_onDelete)(HashTbe<K, V>&, K&, V&);
	int _capacity() { return tableLen; }

private:
	Node* table;
	uint mask;
	int tableLen;   //hash表中桶的长度（极大可用的元素个数）
	int thrCount;   //扩容阈值

public:
	/**HashTbe
	 * @param tableSize_=128
	 * @param onDelete=nullptr 删除时机的回调函数（仅在 del() 时触发，可调 compact 压缩）
	 */
	HashTbe(int tableSize_ = 128, void(*onDelete)(HashTbe<K, V>&, K&, V&) = nullptr)
	{
		_onDelete = onDelete;
		tableLen = nearPowOf2(tableSize_);
		mask = tableLen - 1;
		table = new Node[tableLen];
		count = 0;
		thrCount = (int)(tableLen * 0.6666666f);
	}

	~HashTbe() {
		delete[] table;
	}

	// 支持 [] 语法读写：如果键不存在则创建一个默认值
	V& operator[](const K& key)
	{
		if (count >= thrCount) _rehash(tableLen << 1);

		int index, insertPos;
		if (_locate(key, index, &insertPos)) return table[index].value;

		index = insertPos;
		table[index].key = key;
		table[index].value = V(); // 插入默认值
		table[index].state = OCCUPIED;
		count++;
		return table[index].value;
	}

	V* find(const K& key)
	{
		int index;
		if (_locate(key, index)) return &table[index].value;
		return nullptr;
	}

	void set(const K& key, const V& val)
	{
		operator[](key) = val;
	}

	bool del(const K& key)
	{
		int index;
		if (_locate(key, index))
		{
			// --- 安全删除逻辑 ---
			count--;
			table[index].state = DELETED;

			// 将数据暂存，防止 hook 里的 rehash 释放 table 导致的引用失效
			K k = (K&&)table[index].key;
			V v = (V&&)table[index].value;

			if (_onDelete) _onDelete(*this, k, v);

			return true;
		}
		return false;
	}

	void clear()
	{
		if (count == 0) return;
		for (int i = 0; i < tableLen; ++i)
		{
			if (table[i].state != EMPTY)
			{
				table[i].state = EMPTY;
				table[i].key = K();
				table[i].value = V();
			}
		}
		count = 0;
	}


	void compact()
	{
		if (count == 0) {
			if (tableLen > 16) _rehash(16);
			return;
		}
		// 计算能够容纳当前元素的最小2的幂，确保负载因子不超过0.666
		int targetSize = nearPowOf2((int)(count * 1.5f + 1.0f));
		if (targetSize < 16) targetSize = 16;
		
		if (targetSize < tableLen) {
			_rehash(targetSize);
		}
	}

	// 用于支持 for(auto& node : hash) 的遍历
	struct Iterator {
		Node* p;
		Node* pEnd;
		Iterator(Node* start, Node* end) : p(start), pEnd(end) {
			while (p < pEnd && p->state != OCCUPIED) p++;
		}
		Node& operator*() { return *p; }
		Node* operator->() { return p; }
		Iterator& operator++() {
			if (p < pEnd) {
				p++;
				while (p < pEnd && p->state != OCCUPIED) p++;
			}
			return *this;
		}
		bool operator!=(const Iterator& other) const { return p != other.p; }
	};
	Iterator begin() const { return Iterator(table, table + tableLen); }
	Iterator end() const { return Iterator(table + tableLen, table + tableLen); }


private:
	// 核心定位函数
	bool _locate(const K& key, int& index, int* insertPos = nullptr)
	{
		uint perturb;
		index = be::_hash_i_calc(key, mask, &perturb);
		int firstDeleted = -1;

		while (table[index].state != EMPTY)
		{
			if (table[index].state == OCCUPIED && table[index].key == key)
			{
				if (insertPos) *insertPos = index;
				return true;
			}
			if (table[index].state == DELETED && firstDeleted == -1)
				firstDeleted = index;

			index = (index * 5 + 1 + perturb) & mask;
			perturb >>= 5;
		}

		if (insertPos)
			*insertPos = (firstDeleted != -1) ? firstDeleted : index;
		return false;
	}

	void _rehash(int newTableSize)
	{
		uint newMask = newTableSize - 1;
		Node* newTable = new Node[newTableSize];
		
		for (int i = 0; i < tableLen; ++i)
		{
			if (table[i].state == OCCUPIED)
			{
				K& key = table[i].key;
				V& val = table[i].value;
				uint perturb;
				int index = be::_hash_i_calc(key, newMask, &perturb);
				while (newTable[index].state == OCCUPIED)
				{
					index = (index * 5 + 1 + perturb) & newMask;
					perturb >>= 5;
				}
				newTable[index].key = key;
				newTable[index].value = (V&&)val;
				newTable[index].state = OCCUPIED;
			}
		}

		delete[] table;
		table = newTable;
		tableLen = newTableSize;
		mask = newMask;
		thrCount = (int)(tableLen * 0.6666666f);
	}
};
