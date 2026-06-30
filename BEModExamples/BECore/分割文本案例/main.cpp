#include "stdafx.h"
using namespace be;

Arraybe<Bytes> 方案5_Arena内存接管分割(const StrA& src, const StrA& split,
	int 预估最大分片数 = 10, int 单分片预留大小 = 32);

int main()
{
	print("===1.0 字符串A 单字符分割为 Array<StrA> ===");
	{
		StrA s = "a,b,c,d";
		Arraybe<StrA> arr;
		Split<StrA>(s, ",", arr);
		print(arr);
	}
	
	print("\n===1.1 字符串W 多字符分割为 Array<StrW> ===");
	{
		StrW s = _W("one<br>two<br>three");
		Arraybe<StrW> arr;
		Split(s, _W("<br>"), arr); //多字符分割
		//注：由于模板类型可以自动(从arr)推导，这里也可以不必写Split<StrW>
		print(arr);
	}
	
	printf("\n————————————————————————————————\n");
	print("\n===2.0 字节集 分割为 Array<Bytes> (拷贝-修改不影响原值) ===");
	{
		char raw[] = "A-B-C";
		Bytes src(ArrayBytes(raw));
		Arraybe<Bytes> arr;
		Split(src, BR("-"), arr);

		print("修改前arr", arr);
		arr[0][0] = 'Z';
		print("修改后arr", arr);
		print("修改后raw", BR(raw));
	}

	print("\n===2.1 字节集 分割为 Array<BR> (引用-修改影响原值) ===");
	{
		char raw[] = "X-Y-Z";
		Bytes src(ArrayBytes(raw));
		Arraybe<BR> arr;
		Split(src, BR("-"), arr);

		// BR 存储的是 const void* p，修改原内容需要"去const"
		char* refPtr = (char*)arr[0].p;
		print("修改前arr", arr);
		*refPtr = 'M';
		print("修改后arr:", arr);
		print("修改后raw", BR(raw));
	}

	printf("\n————————————————————————————————\n");
	print("\n===3.0 堆预分配内存 (Array<BR>) ===");
	{
		// 初始容量 10（位于堆上）
		Arraybe<BR> arr(0, 10);
		Bytes src({ 1, 6, 22, 22, 6, 3, 3, 3 });
		Split(src, BR({ 6 }), arr, true); //第四个参数保持原数组容量
		print(arr);
		printf("Capacity: %d, Count: %d\n", arr._capacity(), arr.count);
	}

	print("\n=== 3.1 栈预分配内存 (Array<BR,5>，未超过5个元素) ===");
	{
		// 栈上预分配5个元素
		Arraybe<BR, 5> arr;
		Split(BR("1,2,3"), BR(","), arr, true); //第四个参数保持原数组容量

		print(arr);
		printf("Capacity: %d, Count: %d\n", arr._capacity(), arr.count);
		printf("  当前是否在栈上: %s\n",
			(void*)arr.pArr == (void*)arr._get_stackBuf() ? "YES" : "NO");
	}

	print("\n=== 3.2 栈预分配内存 (Array<BR,5>，分割后超过5个) ===");
	{
		// 栈上预分配5个元素
		Arraybe<BR, 5> arr;
		// 超过栈容量，触发自动堆分配
		Split(BR("1,2,3,4,5,6"), BR(","), arr, true);

		print(arr);
		printf("Capacity: %d, Count: %d\n", arr._capacity(), arr.count);
		printf("  当前是否在栈上: %s\n",
			(void*)arr.pArr == (void*)arr._get_stackBuf() ? "YES" : "NO");
	}

	printf("\n————————————————————————————————\n");
	print("\n=== 4.0 SplitV<Bytes> 惰性视图遍历 ===");
	{
		Bytes src(BR("AA\0B\0CCC", 8)); //注意带空终止的不能用BR单参构造
		int i = 0; //由于没有arr作为参考这里必须要显式写上模板类型<Bytes>
		for (BR v : SplitV<Bytes>(src, { 0 })){
			printf("  子分片%d视图: ", i++);
			print(v);
		}
	}

	print("\n=== 4.1 SplitV<StrA> 惰性视图遍历 (可带break随时退出) ===");
	{
		StrA src = "a:bb:ccc:dd:eeeeeee";
		int i = 0;
		for (BR v : SplitV<StrA>(src, ":")){
			//BR仅为引用视图不会加入空终止，要么在源上加要么新构造内存
			StrA part = ToStr(v); //这个不是个好办法会产生内存复制
			printf("  子分片%d视图: %s\n", i++, (char*)part); //这里记得上._buf()或强转

			if (i == 3) {
				printf("  ->break(第三个元素后)\n");
				break;
			}
		}
	}

	print("\n=== 4.2 SplitV<StrW> 惰性视图遍历 (自动空终止) ===");
	{
		StrW src = ToStr<W>(_W("Hello World C++")); //注意确保该字符串可写
		for (BR v : 分割V<StrW>(src, _W(" "))){
			charW* pEnd = (charW*)((byte*)v.p+v.size);
			*pEnd = 0; //直接在源字符串尾部加入空终止
			print(L"  子分片文本视图: %s\n", (wchar_t *)v.p);
		}
	}

	print("\n=== 5.0 Arena大块内存接管 (利用公开成员直接注入) ===");
	{
		StrA src = "apple|banana|this_is_a_very_long_fruit_name_that_exceeds_32_bytes|cherry";
		Arraybe<Bytes> arr = 方案5_Arena内存接管分割(src, "|", 10, 32);

		printf("  接管成功，实际分片数: %d, 模型容量: %d\n", arr.count, arr.capacity_);
		print("  数组内容: ", arr);

		// 验证超长转堆的分片
		printf("  第3个分片类型: %s (长度:%d)\n",
			arr[2].size > 32 ? "Heap(转堆)" : "Arena", (int)arr[2].size);

		// 验证修改第一个分片
		print("  修改第一个分片 'apple' -> 'PEAR'");
		memcpy((char*)(byte*)arr[0], "PEAR", 4);
		arr[0].size = 4; // 引用模式下手动修正长度
		print("  修改后的数组: ", arr);
		print("  源字符串(src)不受影响: ", src);
	}
	print("\n=== 5.0Arena大块内存已随数组析构而完美释放 ===\n");

	return 0;
}



/*
Arena名字的起源
	“Arena” 一词源自拉丁语 “harena”，原意是“沙子”。
	在古罗马时期，圆形剧场（如角斗场）的中心区域会铺满沙子，用来吸收角斗士的鲜血。
	后来，这个词演变成了指代**“被围起来的特定场地”**。

	在编程语境下，这个比喻非常形象：

	普通内存分配（malloc/free）：就像是在城市的大街小巷里随机找空位停车，
	每次停车（分配）和挪车（释放）都要和交警（操作系统）打交道，容易产生碎片，管理成本高。
	Arena 模式：就像你直接包下了一个大型停车场（就是你代码里那次唯一的 malloc）。
	在这个停车场内部，你想怎么停就怎么停，不需要再向外界申请。
	最关键的是，当你离开时，直接一把火把整个停车场退掉，而不需要一个个检查车位。
*/

// 方案5：方案5_Arena内存接管分割
Arraybe<Bytes> 方案5_Arena内存接管分割(const StrA& src, const StrA& split,
	int 预估最大分片数, int 单分片预留大小)
{
	// 1. 一次性 malloc 整个 Arena
	// 布局: [Bytes 头部区域(预留)] + [数据槽位区域]
	size_t headerAreaSize = sizeof(Bytes) * 预估最大分片数;
	size_t dataAreaSize = 预估最大分片数 * 单分片预留大小;
	void* block = malloc(headerAreaSize + dataAreaSize);
	byte* dataAreaBase = (byte*)block + headerAreaSize;

	//下面这里其实也能用上StrA/W，内存布局上跟Bytes是完全一致的

	// 2. 初始化容器并开启接管模式
	Arraybe<Bytes> arr; //为了查看内存方便这里就用Bytes来演示
	arr.pArr = (Bytes*)block;          // 直接挂载内存
	arr.capacity_ = 预估最大分片数;    // 接管关键：设置容量，让Arraybe析构时free(pArr)
	arr.count = 0;                     // 初始化计数

	// 3. 执行自定义惰性视图分割填充
	int i = 0;
	for (BR v : SplitV<StrA>(src, split)) {
		if (i >= 预估最大分片数) break;

		Bytes* currentHeader = &arr.pArr[i];
		byte* currentSlot = dataAreaBase + (i * 单分片预留大小);

		if (v.size <= (size_t)单分片预留大小) {
			// 情况A：分片大小在槽位内，使用BRef引用模式挂载到 Arena 槽位
			new(currentHeader) Bytes(currentSlot, BRef, v.size); //这里就地new并非创建堆内存
			memcpy(currentSlot, v.p, v.size); //拷贝数据
		} else {
			// 情况B：超出槽位大小，直接让 Bytes 自己去堆上分配内存（转堆）
			new(currentHeader) Bytes(v.p, v.size);
		}
		i++;
	}
	arr.count = i;

	// 4. 返回数组。所有权会安全转移
	return arr;
}