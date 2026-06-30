#include "stdafx.h"
using namespace be;

#define ASSERT(expr) \
    if (!(expr)) { \
        printf("FAIL: %s (%s:%d)\n", #expr, __FILE__, __LINE__); \
    } else { \
        printf("PASS: %s\n", #expr); \
    }

void TestBytesOp() {
	printf("--- Testing Bytes Operations ---\n");
	
	Bytes b1 = BR("0123456789"); // 10 bytes: '0' to '9'

	// 1. 取字节集长度
	ASSERT(取字节集长度(b1) == 10);
	ASSERT(取字节集长度(Bytes()) == 0);

	// 2. 取字节集左边
	ASSERT(取字节集左边(b1, 3) == BR("012"));
	ASSERT(取字节集左边(b1, 0) == BR()); //也可以Bytes()
	ASSERT(取字节集左边(b1, 20) == BR("0123456789")); // 越界获取长度应处理并在内部截断

	// 3. 取字节集右边
	ASSERT(取字节集右边(b1, 3) == BR("789"));
	ASSERT(取字节集右边(b1, 0) == Bytes());
	ASSERT(取字节集右边(b1, 20) == BR("0123456789")); // 越界应截断全返

	// 4. 取字节集子串
	ASSERT(取字节集子串(b1, 2, 3) == BR("234"));
	ASSERT(取字节集子串(b1, 9, 5) == BR("9")); // 超出部分截断
	ASSERT(取字节集子串(b1, 15, 2) == Bytes()); // 起始位置越界
	ASSERT(取字节集子串(b1, 2, 0) == Bytes());

	// 5. 寻找字节集
	ASSERT(寻找字节集(b1, BR("45")) == 4);
	ASSERT(寻找字节集(b1, BR("45"), 4) == 4);
	ASSERT(寻找字节集(b1, BR("45"), 5) == -1); // 越过匹配位置
	ASSERT(寻找字节集(b1, BR("45"), 20) == -1); // 起始位置越界
	ASSERT(寻找字节集(b1, BR("xyz")) == -1);

	// 6. 倒找字节集
	Bytes b2 = BR("ababab");
	ASSERT(倒找字节集(b2, BR("ab")) == 4); // 默认从尾部找
	ASSERT(倒找字节集(b2, BR("ab"), 3) == 2);
	ASSERT(倒找字节集(b2, BR("ab"), 20) == 4); // 起始位置越界，应重置为尾部
	ASSERT(倒找字节集(b2, BR("xy")) == -1);

	// 7. 字节集替换
	ASSERT(字节集替换(b1, 2, 3, BR("xy")) == BR("01xy56789"));
	ASSERT(字节集替换(b1, 8, 5, BR("xy")) == BR("01234567xy")); // 替换长度越界
	ASSERT(字节集替换(b1, 20, 2, BR("xy")) == BR("0123456789xy")); // 起始位置越界，应追加到末尾
	ASSERT(字节集替换(b1, 5, 2) == BR("01234789")); // 删除操作

	// 8. 子字节集替换
	ASSERT(子字节集替换(b2, BR("ab"), BR("xy")) == BR("xyxyxy")); // 替换所有
	ASSERT(子字节集替换(b2, BR("ab"), BR("xy"), 0, 2) == BR("xyxyab")); // 限制次数
	ASSERT(子字节集替换(b2, BR("ab"), BR("xy"), 2) == BR("abxyxy")); // 起始位置
	ASSERT(子字节集替换(b2, BR("ab"), Bytes(), 0, 1) == BR("abab")); // 替换为空(删除)
	ASSERT(子字节集替换(b2, BR("ab"), BR("xy"), 20) == b2); // 起始位置越界

	// 9. 取重复字节集
	ASSERT(取重复字节集(3, BR("12")) == BR("121212"));
	ASSERT(取重复字节集(0, BR("12")) == Bytes());

	// 10. 分割字节集
	数组<Bytes> arr;
	分割字节集(b2, BR("a"), arr);
	ASSERT(arr.count == 4); // "", "b", "b", "b"
	if (arr.count == 4) {
		ASSERT(arr[0] == Bytes());
		ASSERT(arr[1] == BR("b"));
		ASSERT(arr[2] == BR("b"));
		ASSERT(arr[3] == BR("b"));
	}

	// 11. 取字节集之间
	Bytes b_between = BR("start_abcd_end");
	ASSERT(字节集_取之间(b_between, BR("start_"), BR("_end")) == BR("abcd"));
	ASSERT(字节集_取之间(b_between, BR("start_")) == BR("abcd_end"));
	ASSERT(字节集_取之间(b_between, Bytes(), BR("_end")) == BR("start_abcd"));

	be::print("All edge case tests passed.\n");
}

void 测试_数组字节集()
{
	//下面均为引用构造
	be::print("测试_数组字节集");
	int arrint[] = { 1, 2, 3 };
	Bytes a = 数组字节集(arrint);
	be::print(a);

	Arraybe<short> arr = { 1, 2, 3 };
	Bytes b = 数组字节集(arr);
	be::print(b);

	be::print("");
}

void 测试_文本与字节集()
{
	//下面均为引用构造
	be::print("测试_文本与字节集");
	Bytes bs = BR("hello");
	Bytes sr = SR("world");
	be::print(bs, sr);
	Bytes bsW = BR(L"hello");
	Bytes srW = SR(L"world");
	be::print(bsW, srW);

	//从已有的Str引用转换到Bytes（长度不包含空终止）
	bs = StrA("hello").bytes; //这里StrA是右值，走移动赋值
	bsW = StrW(L"hello").bytes;
	be::print(bs, bsW);
	be::print("整数转文本型再转字节集",
		ToStrPlat(123).bytes,
		ToStr<W>(123.2).bytes,
		ToStr(123.1).bytes
	);
	StrA s2 = "hello"; bs = s2.bytes; //这里走的是拷贝赋值

	//字节集转换到拷贝构造的文本
	const char* cs = "hello";
	StrA s = 到文本(BR(cs));
	s.bytes[0] = 'H';
	be::print(s);

	be::print("");
}

int main()
{
	测试_文本与字节集();
	测试_数组字节集();

	TestBytesOp();
	return 0;
}