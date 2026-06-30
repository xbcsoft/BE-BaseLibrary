#include "stdafx.h"
#include "RegExpie.h"

void main()
{
	RegExpie reg;
	
	// 1. 创建并匹配
	bool b1 = reg.创建(R"(^[A-Za-z]+\d+$)", false, "Hello123");
	if (b1) {
		printf("1. 全行匹配结果: %s\n", reg.匹配() ? "true" : "false");
	}

	// 2. 搜索 (重复文本位置测试)
	printf("\n--- 2. 搜索 (含重复文本位置测试) ---\n");
	reg.创建(R"((abc)(\d+)(abc))", false, "xxxabc222abcyyy---abc555abczzz");
	StrU8 matchText;
	子匹配结果 subRes;
	int pos = reg.搜索(0, matchText, subRes);
	printf("匹配文本: %s, 位置: %d\n", (char*)matchText, pos);
	for (int i = 0; i < subRes.子文本.count; i++) {
		printf("  子匹配%d: \"%s\", 位置: %d\n", i + 1, (char*)subRes.子文本[i], subRes.子文本位置[i]);
	}

	// 3. 全部搜索
	Arraybe<StrU8> allTexts;
	Arraybe<int> allPos;
	Arraybe<子匹配结果> allSubs;
	int count = reg.全部搜索(allTexts, allPos, allSubs);
	printf("\n3. 全部搜索找到数量: %d\n", count);
	for (int i = 0; i < count; i++) {
		printf(" -> 位置: %d, 文本: %s\n", allPos[i], (char*)allTexts[i]);
	}

	// 4. 替换
	StrU8 replaced1 = reg.替换("[$1](+$2)");
	printf("\n4. 替换结果(默认文本): %s\n", (char*)replaced1);

	// 4.1 替换(优化测试: 仅替换一次)
	int realCount = 0;
	StrU8 replaced1_limit = reg.替换("[$1](+$2)", nullptr, 0, 1, realCount);
	printf("4.1 替换一次结果: %s, 实际次数: %d\n", (char*)replaced1_limit, realCount);

	// 4.2 替换(全部替换)
	int globalRealCount = 0;
	StrU8 replaced_all = reg.替换("[$1](+$2)", nullptr, 0, -1, globalRealCount);
	printf("4.2 全部替换结果: %s, 实际次数: %d\n", (char*)replaced_all, globalRealCount);

	// 5. 替换(使用回调)
	StrU8 replaced2 = reg.替换("", [](const StrU8& 匹配文本, int 匹配文本所在位置, const 子匹配结果& 子匹配, size_t 附加参数) {
		// 无捕获的 lambda 可以退化为函数指针
		StrU8 result = "[";
		result += 子匹配.子文本[0];
		result += "号]";
		return result;
	});
	printf("\n5. 替换结果(回调): %s\n", (char*)replaced2);

}