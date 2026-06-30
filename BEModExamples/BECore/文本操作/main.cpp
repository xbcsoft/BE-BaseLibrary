#include "stdafx.h"
using namespace be;

// Simple assertion macro
#define ASSERT(expr) \
    if (!(expr)) { \
        printf("FAIL: %s (%s:%d)\n", #expr, __FILE__, __LINE__); \
    } else { \
        printf("PASS: %s\n", #expr); \
    }

void TestZeroBased() {
	printf("--- Testing 0-Based Indexing Strictness ---\n");
	StrA s = "0123456789";

	// 1. 取代码 (Asc)
	// Index 0 should be '0'
	ASSERT(取代码(s, 0) == '0');
	// Index 1 should be '1'
	ASSERT(取代码(s, 1) == '1');
	// Index 9 should be '9'
	ASSERT(取代码(s, 9) == '9');

	// 2. 取文本子串 (Substr)
	// Start at 0, take 1 -> "0"
	ASSERT(取文本子串(s, 0, 1) == "0");
	// Start at 1, take 1 -> "1"
	ASSERT(取文本子串(s, 1, 1) == "1");
	// Start at 5, take 2 -> "56"
	ASSERT(取文本子串(s, 5, 2) == "56");

	// 3. 寻找文本 (InStr)
	// Find "0" -> 0
	ASSERT(寻找文本(s, "0", 0, false) == 0);
	// Find "1" -> 1
	ASSERT(寻找文本(s, "1", 0, false) == 1);
	// Find "9" -> 9
	ASSERT(寻找文本(s, "9", 0, false) == 9);
	// Find "0" starting at 1 -> -1 (not found after index 1)
	ASSERT(寻找文本(s, "0", 1, false) == -1);

	// 4. 倒找文本 (InStrRev)
	// Find "8" -> 8
	ASSERT(倒找文本(s, "8", SIZE_MAX, false) == 8);
	// Find "8" starting search at index 7 -> -1 (not found before or at 7? Wait. 倒找 limit.)
	// InStrRev(s, "8", 7). Search up to index 7? Or start FROM 7 backwards?
	// Implementation: if (pos != MAX) idx = min(idx, pos).
	// If pos=7, we search at 7, 6, ... 0.
	// "8" is at index 8. So searching up to 7 will NOT find it.
	ASSERT(倒找文本(s, "8", 7, false) == -1);
	// Searching at index 8 should find it.
	ASSERT(倒找文本(s, "8", 8, false) == 8);

	printf("Strict 0-based tests passed.\n");
}

void TestStrA() {
	TestZeroBased();
	printf("\n--- Testing StrA ---\n");
	StrA s1 = "Hello World";

	// Length
	ASSERT(取文本长度(s1) == 11);

	// Substrings
	StrA left = 取文本左边(s1, 5);
	ASSERT(left == "Hello");

	StrA right = 取文本右边(s1, 5);
	ASSERT(right == "World");

	StrA mid = 取文本子串(s1, 6, 5); // 0-based index 6 is 'W'
	ASSERT(mid == "World");

	// Search (InStr)
	int pos = 寻找文本(s1, "World", 0, false);
	ASSERT(pos == 6);

	pos = 寻找文本(s1, "world", 0, true); // Case insensitive, start 0
	ASSERT(pos == 6);

	pos = 寻找文本(s1, "Foo", 0, false);
	ASSERT(pos == -1);

	// Reverse Search (InStrRev)
	StrA s2 = "aabbaabb";
	pos = 倒找文本(s2, "bb", SIZE_MAX, false);
	ASSERT(pos == 6);

	pos = 倒找文本(s2, "bb", 4, false); // Search before index 4. "aabb" ends at 3. "bb" at 2.
	ASSERT(pos == 2);

	// Replace
	StrA s3 = "Hello World World";
	StrA rep = 子文本替换(s3, "World", "Universe", 0, 1, true);
	ASSERT(rep == "Hello Universe World");

	rep = 子文本替换(s3, "World", "Universe", 0, -1, true); // Replace all
	ASSERT(rep == "Hello Universe Universe");

	// Case conversion
	StrA mixed = "HeLLo";
	ASSERT(到大写(mixed) == "HELLO");
	ASSERT(到小写(mixed) == "hello"); // Manual impl check

	// Repeat/Space
	StrA spaces = 取空白文本(3);
	ASSERT(spaces == "   ");

	StrA repeated = 取重复文本(3, "Ho");
	ASSERT(repeated == "HoHoHo");
}

void TestStrW() {
	printf("\n--- Testing StrW ---\n");
	StrW s1 = L"你好世界";

	// Length (UTF-16 char count)
	ASSERT(取文本长度<W>(s1) == 4);

	// Substrings
	StrW left = 取文本左边<W>(s1, 2);
	ASSERT(left == L"你好");

	StrW right = 取文本右边<W>(s1, 2);
	ASSERT(right == L"世界");

	StrW mid = 取文本子串<W>(s1, 2, 2); // 0-based: 0=你, 1=好, 2=世? Wait. s1="你好世界". 0=你, 1=好, 2=世.
	// If we want "好世", it is start=1, len=2.
	// Before it was "start=2, len=2" -> "好世" (1-based 2 is 好). Now 0-based 1 is 好.
	mid = 取文本子串<W>(s1, 1, 2);
	ASSERT(mid == L"好世");

	// Search
	int pos = 寻找文本<W>(s1, L"世界", 0, false);
	ASSERT(pos == 2);

	// Replace
	StrW s3 = L"你好世界世界";
	StrW rep = 子文本替换<W>(s3, L"世界", L"宇宙", 0, 1, true);

	if (rep == L"你好宇宙世界") {
		printf("PASS: rep check\n");
	} else {
		printf("FAIL: rep check. Len=%zu, Expected=6. Content codes:\n", 取文本长度<W>(rep));
		for (size_t i = 0; i<取文本长度<W>(rep); i++) printf("%d ", (int)rep[i]);
		printf("\nExpected codes:\n");
		StrW exp = L"你好宇宙世界";
		for (size_t i = 0; i<取文本长度<W>(exp); i++) printf("%d ", (int)exp[i]);
		printf("\n");
	}

	// Case conversion (ASCII parts only generally unless full Unicode support implemented manually)
	StrW mixed = L"HeLLo";
	ASSERT(到大写<W>(mixed) == L"HELLO");
	ASSERT(到小写<W>(mixed) == L"hello");
}

void Test_取文本之间A(){
	StrA txt = "start middle end";
	StrA res1 = 文本_取之间(txt, "start ", " end", 空, 空, 0, 0, false, 0);
	ASSERT(res1 == "middle");

	StrA txt2 = "1[a] 2[b] 3[c]";
	StrA res2 = 文本_取之间(txt2, "[", "]", 空, 空, 0, 0, false, 0);
	ASSERT(res2 == "a");

	StrA res3 = 文本_取之间(txt2, "[", "]", 空, 空, 0, 0, true, 0);
	ASSERT(res3 == "c");

	StrA t1 = "123456789";
	ASSERT(文本_取之间(t1, "123", "789", 空, 空, 0, 0, false, 0) == "456");
	ASSERT(文本_取之间(t1, "", "456", 空, 空, 0, 0, false, 0) == "123");     // 左为空
	ASSERT(文本_取之间(t1, "456", "", 空, 空, 0, 0, false, 0) == "789");     // 右为空
	ASSERT(文本_取之间(t1, "", "789", 空, 空, 0, 0, true, 0) == "123456"); // 倒找左为空
	ASSERT(文本_取之间(t1, "123", "", 空, 空, 0, 0, true, 0) == "456789"); // 倒找右为空

	// 偏移量测试
	ASSERT(文本_取之间(t1, "123", "789", 空, 空, -3, 0, false, 0) == "123456"); // 包含左边文本
	ASSERT(文本_取之间(t1, "123", "789", 空, 空, 0, 3, false, 0) == "456789");  // 包含右边文本
	ASSERT(文本_取之间(t1, "xxx", "yyy", 2, 7, 0, 0, false, 0) == "34567");           // 自定义位置
	ASSERT(文本_取之间(t1, "123", "", 空, 7, 0, 0, false, 0) == "4567");              // 单边自定义

	// 异常与越界测试
	ASSERT(文本_取之间("abc", "ab", "bc", 空, 空, 0, 0, false, 0) == ""); // 交叉
	StrA r_underflow = 文本_取之间(t1, "456", "789", 空, 空, -10, 0, false, 0);
	ASSERT(r_underflow == ""); // 左溢出
	StrA r_right_underflow = 文本_取之间(t1, "123", "789", 空, 空, 0, -100, false, 0);
	ASSERT(r_right_underflow == ""); // 右溢出

	// 开始寻找位置测试
	StrA t3 = "a1b a2b a3b";
	ASSERT(文本_取之间(t3, "a", "b", 空, 空, 0, 0, false, 2) == "2");
	ASSERT(文本_取之间(t3, "a", "b", 空, 空, 0, 0, true, 0) == "3");
}

void TestWhitespaceTrim() {
	printf("\n--- Testing Whitespace Trim ---\n");
	StrA sA = " \t\r\nHello World \t\r\n";
	ASSERT(删首空(sA) == "Hello World \t\r\n");
	ASSERT(删尾空(sA) == " \t\r\nHello World");
	ASSERT(删首尾空(sA) == "Hello World");
	StrA resA = 删全部空(sA);
	if (resA != "HelloWorld") {
		printf("resA len=%zu, exp=%zu. Content='", resA.len(), strlen("HelloWorld"));
		for (size_t i = 0; i<resA.len(); ++i) printf("%02x ", (unsigned char)resA[i]);
		printf("'\n");
	}
	ASSERT(resA == "HelloWorld");
}

void TestTextReplacePos() {
	printf("\n--- Testing TextReplacePos ---\n");
	StrA sA = "Hello World";
	ASSERT(文本替换(sA, 6, 5, "Universe") == "Hello Universe");
	StrW sW = L"你好世界";
	ASSERT(文本替换<W>(sW, 2, 2, L"宇宙") == L"你好宇宙");
}

void Test_预分配内存() {
	printf("\n--- Testing Memory Pre-allocation (Demonstration) ---\n");

	// 演示：类似“子文本替换”内部的实现，先通过 bytes._capacity() 预分配内存，
	// 然后再进行文本处理（如频繁拼接），可以有效减少内存重分配次数，提高性能。
	
	StrA s1;
	s1.bytes._capacity(1024); // 预分配 1024 字节容量
	for (int i = 0; i < 20; ++i) {
		s1 += "Element[";
		s1 += 到文本(i);
		s1 += "] ";
	}
	be::print(s1);
	ASSERT(取文本长度(s1) > 0);
	ASSERT(寻找文本(s1, "Element[19]", 0, false) != -1);

	printf("Pre-allocation demo passed.\n");
}

int main() {
	TestStrA();
	TestStrW();
	TestWhitespaceTrim();
	TestTextReplacePos();
	Test_取文本之间A();
	Test_预分配内存();

	printf("\nTests Complete.\n");
	return 0;
}