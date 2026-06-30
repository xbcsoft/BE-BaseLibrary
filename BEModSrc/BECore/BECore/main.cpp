#include "stdafx.h"
#include <windows.h>

// User defines their formatter once (Single parameter version)
inline AutoStr __AutoStr__(const RECT& r) {
	AutoStr s;
	s << "{ left:" << r.left
		<< ", top:" << r.top
		<< ", right:" << r.right
		<< ", bottom:" << r.bottom
		<< " }";
	return s;
}

struct MyStruct
{
	int x, y;
};

inline AutoStr __AutoStr__(const MyStruct& pt) {
	AutoStr s;
	s << "Point(" << pt.x << ", " << pt.y << ")";
	return s;
}

void main()
{
	StrA a = "asd";

	// 1. 测试自定义结构体隐式转换和流式输出
	MyStruct pt = { 1, 2 };
	RECT rc = { 10, 20, 100, 200 };
	be::print("--- 测试结构体 ---");
	be::print("MyStruct: ", pt);
	be::print("RECT: ", rc);
	be::print("");

	// 2. 测试 Arraybe 自动格式化（包括基础类型、文本型、结构体）
	be::print("--- 测试 Arraybe ---");
	Arraybe<int> arrInt = { 1, 2, 3, 4, 5 };
	be::print("整数数组: ", arrInt);
	
	Arraybe<StrU8> arrStr = { "Hello", "World", "BE", "C++" };
	be::print("文本数组: ", arrStr);

	Arraybe<MyStruct> arrStruct = { {1,2}, {3,4}, {5,6} };
	be::print("结构体数组: ", arrStruct);
	be::print("");

	// 3. 测试 HashTbe 自动格式化
	be::print("--- 测试 HashTbe ---");
	HashTbe<StrU8, int> hash;
	hash["One"] = 1;
	hash["Two"] = 2;
	hash["Three"] = 3;
	be::print("哈希表: ", hash);
	be::print("");

	// 4. 测试日期时间型自动格式化
	be::print("--- 测试日期时间型 ---");
	日期时间型 dt = 取现行时间();
	be::print("当前时间: ", dt);
	
	日期时间型 dt2 = 到时间("2026-06-13 20:00:00");
	be::print("解析时间: ", dt2);
	be::print("");

	// 5. 终极套娃测试（哈希表里嵌套数组）
	be::print("--- 测试终极嵌套 ---");
	HashTbe<StrU8, Arraybe<int>> complexHash;
	complexHash["Evens"] = { 2, 4, 6, 8 };
	complexHash["Odds"] = { 1, 3, 5, 7 };
	be::print("复杂嵌套哈希表: ", complexHash);
}