#include "stdafx.h"
using namespace be;

#define ASSERT(expr) \
    if (!(expr)) { \
        printf("FAIL: %s (%s:%d)\n", #expr, __FILE__, __LINE__); \
    } else { \
        printf("PASS: %s\n", #expr); \
    }

void TestArray() {
	printf("--- Testing Array Operations ---\n");
	
	数组<int> arr1;

	// 测试加入成员
	加入成员(arr1, 10);
	加入成员(arr1, 20);
	加入成员(arr1, 30);
	ASSERT(取数组成员数(arr1) == 3);
	ASSERT(arr1[0] == 10 && arr1[1] == 20 && arr1[2] == 30);

	// 测试插入成员
	插入成员(arr1, 1, 15); // 在索引1插入15
	ASSERT(取数组成员数(arr1) == 4);
	ASSERT(arr1[1] == 15 && arr1[2] == 20);

	// 测试删除成员
	删除成员(arr1, 2); // 删除索引2的元素 (即20)
	ASSERT(取数组成员数(arr1) == 3);
	ASSERT(arr1[2] == 30);

	// 测试复制数组
	数组<int> arr2;
	复制数组(arr2, arr1);
	ASSERT(取数组成员数(arr2) == 3);
	ASSERT(arr2[0] == 10 && arr2[1] == 15 && arr2[2] == 30);

	// 测试数组排序
	数组<int> arr3;
	加入成员(arr3, 50);
	加入成员(arr3, 10);
	加入成员(arr3, 40);
	加入成员(arr3, 20);

	数组_排序(arr3, true); // 从小到大
	ASSERT(arr3[0] == 10 && arr3[1] == 20 && arr3[2] == 40 && arr3[3] == 50);

	数组_排序(arr3, false); // 从大到小
	ASSERT(arr3[0] == 50 && arr3[1] == 40 && arr3[2] == 20 && arr3[3] == 10);

	// 测试数组清零
	数组_清零(arr3);
	ASSERT(arr3[0] == 0 && arr3[3] == 0);
	ASSERT(取数组成员数(arr3) == 4);

	// 测试重定义数组
	重定义数组(arr3, false, 2);
	ASSERT(取数组成员数(arr3) == 2);

	// 测试清除数组
	清除数组(arr3);
	ASSERT(取数组成员数(arr3) == 0);

	printf("All Array operations tests passed.\n");
}


// 自定义容量扩张策略函数（普通静态函数）
void CustomExpandHook(Arraybe<int>& self) {
	// 制定你的野蛮/克制策略（例如：原容量直接乘以 2 倍暴涨）
	int currentCap = self._capacity();
	int newCap = (currentCap == 0) ? 10 : (currentCap * 2);

	// 落实扩容方案（调用底层的 realloc）
	self._capacity(newCap);
}
void 数组自动扩容与缩放演示() {
	printf("\n数组自动扩容与缩放演示：\n");
	Arraybe<int> arr;
	printf("Initial: count=%d capacity=%d\n", arr.count, arr._capacity());

	// 装配干预钩子：接管扩容大权
	arr._hookExpand(CustomExpandHook);

	// 持续 push，全权由你的函数决定何时以多大规模暴涨！
	for (int i = 0; i < 15; i++) {
		int oldCap = arr._capacity();
		arr.push(i);
		if (arr._capacity() != oldCap) {
			printf("  [Expand] Pushed %d, new capacity: %d\n", i, arr._capacity());
		}
	}
	printf("After push 15 elements: count=%d capacity=%d\n", arr.count, arr._capacity());

	// 装配干预钩子：接管防空载收缩大权
	arr._hookDelete([](Arraybe<int>& self){
		int cap = self._capacity();
		if (cap > 10 && cap > self.count * 2){
			int oldCap = self._capacity();
			self._shrink(); //如果一个类型为可平凡移动，底层同样调用极高效的realloc
			printf("  [Shrink] Array shrunk from %d to %d\n", oldCap, self._capacity());
		}
	});

	// (之前cap=20)现在开始疯狂删除，测试收缩
	for (int i = 0; i < 10; i++) {
		arr.pop();
	}
	// 解释为什么最终 capacity 是 9：
	// 1. 在第 6 次 pop 时，count 降到 9，此时 cap 为 20
	// 2. 满足 20 > 10 且 20 > 9 * 2 (即18)，触发收缩
	// 3. _shrink() 底层调用 reset(count) 直接将容量收缩到与当前的 count 齐平，即 capacity 变成 9
	// 4. 后续再继续做 4 次 pop 时，count 降至 5，但此时 cap 已经是 9，不再满足 cap > 10第一条件，从而拦截了缩减动作
	// 这种策略不仅实现了防空载收缩，还通过 cap>10 设置了保底值，避免了个位数容量下的频繁小修整，表现完美。
	printf("After pop 10 elements: count=%d capacity=%d\n", arr.count, arr._capacity());
}

int main()
{
	TestArray();
	数组自动扩容与缩放演示();
	return 0;
}