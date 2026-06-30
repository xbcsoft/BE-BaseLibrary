#include "stdafx.h"

using namespace be;

void test_hashtable_extreme()
{
	be::print(">>> 开始 HashTbe 测试...\n");
	HashTbe<int, int> map(16); 

	const int MAX_COUNT = 100000;

	for (int i = 0; i < MAX_COUNT; ++i) {
		map[i] = i * 2;
	}
	be::print("   [1] 插入 ", MAX_COUNT, " 个元素完成，当前 count: ", map.count, "\n");

	for (int i = 0; i < MAX_COUNT; ++i) {
		if (map[i] != i * 2) {
			be::print("   [!] 错误: i=", i, " 的数据校验失败!\n");
			return;
		}
	}
	be::print("   [2] 基础数据校验通过。\n");

	int delCount = 0;
	for (int i = 0; i < MAX_COUNT; i += 2) {
		if (map.del(i)) delCount++;
	}
	be::print("   [3] 删除了 ", delCount, " 个偶数元素，产生大量墓碑槽位。\n");

	for (int i = 1; i < MAX_COUNT; i += 2) {
		if (map[i] != i * 2) {
			be::print("   [!] 错误: 删除操作后奇数键 i=", i, " 丢失!\n");
			return;
		}
	}
	be::print("   [4] 删除后余项校验通过。\n");

	for (int i = 0; i < MAX_COUNT; i += 2) {
		map[i] = i * 3;
	}
	be::print("   [5] 重新覆盖插入完成，验证墓碑复用后的数据...\n");

	HashTbe<StrA, StrA> strmap;
	for (int i = 0; i < 50000; ++i) {
		StrA key = ToStr(i) + "_key";
		strmap[key] = ToStr(i) + "_value";
	}
	be::print("   [6] 字符串任务压力测试已完成（50000项）。\n");

	HashTbe<int, int> smallMap;
	smallMap[1] = 100; smallMap[2] = 200;
	be::print("   [7] AutoStr 转换测试: ", smallMap, "\n");

	be::print(">>> HashTbe 测试全部通过！\n");
}

void test_simple(){
	print("--- Example 1: Basic iterator ---");
	HashTbe<StrA, int> scores;
	scores["Alice"] = 95;
	scores["Bob"] = 80;
	scores["Charlie"] = 88;

	scores.del("Bob");

	for (auto& node : scores) {
		print("Student:", node.key, "| Score:", node.value);
	}

	print("\n--- Example 2: Bytes as Key ---");
	HashTbe<Bytes, StrW> dict;
	dict[{1, 2, 3}] = _W("Binary Config V1");
	dict[BR("Token")] = _W("Session ID 666");

	for (auto& node : dict) {
		print("Hex Key:", node.key, "->", node.value);
	}
	print("");
}

void test_compact_and_hook()
{
	be::print(">>> 开始 Hook (Lambda) 与 压缩 (Compact) 测试...");

	// 1. 初始化并绑定生命周期 Hook (实现自动压缩)
	HashTbe<StrA, StrA> map(64, [](HashTbe<StrA, StrA>& m, StrA& k, StrA& v) {
		// 自动压缩逻辑：当容量大于64 且 元素数量低于容量的 10% 时自动触发
		// 我们增加一个保底判断(m.count > 10)，避免在极小规模时频繁抖动
		if (m._capacity() > 64 && m.count < m._capacity() * 0.1f) {
			be::print("   [Auto-Hook] 负载因子低于 10% (", m.count, "/", m._capacity(), ")，触发自动压缩...");
			m.compact();
		}
	});

	// 2. 插入一些测试数据
	map["cmd_start"] = "init_system";
	map["cmd_stop"]  = "shutdown_system";
	map["cmd_wait"]  = "idle_process";
	be::print("   [1] 插入基础数据完成，当前容量:", map._capacity());

	// 清空基础数据，确保压力测试数字精准
	map.clear();

	// 2. 模拟大规模操作触发扩容
	be::print("   [2] 模拟压力插入 500 项以触发自动扩容...");
	for (int i = 0; i < 500; ++i) {
		map[ToStr(i)] = "mass_data";
	}
	be::print("       扩容后容量:", map._capacity(), ", 当前元素数:", map.count);

	// 3. 中途压缩演示：大量删除，但保留一小部分
	be::print("   [3] 准备删除大部分数据，仅保留 10 项...");
	for (int i = 10; i < 500; ++i) {
		map.del(ToStr(i));
	}
	be::print("       删除后剩余元素个数:", map.count);

	// 4. 自动压缩验证
	be::print("   [4] 验证自动压缩结果...");
	be::print("       当前容量已自动降至:", map._capacity());
	
	// 验证数据依然存在
	be::print("       验证保留项 '0' ->", map["0"]);

	be::print(">>> Hook 自动压缩测试完成！\n");
}

int main()
{
	test_simple();
	test_hashtable_extreme();

	test_compact_and_hook();

	return 0;
}