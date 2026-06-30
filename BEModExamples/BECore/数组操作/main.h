#pragma once
#include "stdafx.h"
using namespace be;

#define ASSERT(expr)

void TestArray();


// 自定义容量扩张策略函数（普通静态函数）
void CustomExpandHook(Arraybe<int>& self);
void 数组自动扩容与缩放演示();

int main();
