#pragma once
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

void main();

// 测试 Arraybe<Arraybe<Str>> 的封包/解包
void testNestedArray();
