#include "stdafx.h"
#include "FB.h" //由MakeBEC.exe自动生成
#include <stdio.h>

// 一个简单的结构体，用于演示 gFBpo
struct Point
{
	int x;
	int y;
};

// 前置声明：测试嵌套数组（Arraybe<Arraybe<Str>>）的封包/解包
void testNestedArray();

void main()
{
	// ========== gFBpo 示例 ==========
	// gFBpo 用于解包指针类型的数据。
	// 当用 FB() 打包一个指针时，指针值本身被写入封包。
	// gFBpo 从封包中读出指针，然后解引用它，返回所指对象的值。

	Point pt{ 10, 20 };

	// 1. 打包指针（注意只是存储其地址并非实体）
	Bytes fbData = FB(&pt);
	be::print("Packed FBData (pointer to Point)", fbData);

	// 2. 用 gFBpo 解包
	Point& outPt = gFBpo<Point>(fbData.buf, 0); //这里可以返回实体也可以返回引用
	//注：若返回的是实体接收则会进行该类的拷贝构造新对象

	printf("--- gFBpo Unpack Result ---\n");
	printf("outPt.x = %d\n", outPt.x);
	printf("outPt.y = %d\n", outPt.y);





	printf("\n");

	// ========== 也可以打包多个不同类型的值，混合使用 ==========
	int inInt = 12345;
	double inDouble = 3.14159;
	Arraybe<StrA> inArr;
	inArr.push("Hello");
	inArr.push("Baiyi");

	Arraybe<int> inArrInt;
	inArrInt.push(100);
	inArrInt.push(200);

	Bytes fbData2 = FB(inInt, inDouble, inArr, inArrInt);
	be::print("Packed FBData (mixed types)", fbData2);

	// 解包
	int outInt = 0;
	double outDouble = 0.0;
	Arraybe<StrA> outArr;
	Arraybe<int> outArrInt;

	deFB(fbData2, outInt, outDouble, outArr, outArrInt);

	printf("\n--- Unpack Results (mixed types) ---\n");
	printf("outInt: %d\n", outInt);
	printf("outDouble: %f\n", outDouble);
	if (outArr.count == 2) {
		printf("outArr[0]: %s\n", outArr[0].bytes.buf);
		printf("outArr[1]: %s\n", outArr[1].bytes.buf);
	}
	if (outArrInt.count == 2) {
		printf("outArrInt[0]: %d\n", outArrInt[0]);
		printf("outArrInt[1]: %d\n", outArrInt[1]);
	}

	// ========== 测试嵌套数组 ==========
	testNestedArray();
}

// 测试 Arraybe<Arraybe<Str>> 的封包/解包
void testNestedArray()
{
	printf("\n\n\n========== Test Nested Array (Arraybe<Arraybe<Str>>) ==========\n");

	// 构建嵌套数组：两个内部数组，每个包含若干字符串
	Arraybe<Arraybe<StrA>> nestedIn;

	Arraybe<StrA> inner1;
	inner1.push("Apple");
	inner1.push("Banana");
	inner1.push("Cherry");

	Arraybe<StrA> inner2;
	inner2.push("Dog");
	inner2.push("Elephant");

	nestedIn.push(inner1);
	nestedIn.push(inner2);

	// 打包
	Bytes fbData = FB(nestedIn);
	be::print("Packed FBData (nested array)", fbData);

	// 解包
	Arraybe<Arraybe<StrA>> nestedOut;
	deFB(fbData, nestedOut);

	printf("\n--- Unpack Results (nested array) ---\n");
	printf("nestedOut.count = %d\n", nestedOut.count);
	for (int i = 0; i < nestedOut.count; i++)
	{
		printf("  inner[%d].count = %d\n", i, nestedOut[i].count);
		for (int j = 0; j < nestedOut[i].count; j++)
		{
			printf("    inner[%d][%d] = %s\n", i, j, nestedOut[i][j].bytes.buf);
		}
	}
}
