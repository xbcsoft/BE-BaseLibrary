#pragma once
#include "stdafx.h"
using namespace be;

Arraybe<Bytes> 方案5_Arena内存接管分割(const StrA& src, const StrA& split,
	int 预估最大分片数 = 10, int 单分片预留大小 = 32);

int main();



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
	int 预估最大分片数, int 单分片预留大小);
