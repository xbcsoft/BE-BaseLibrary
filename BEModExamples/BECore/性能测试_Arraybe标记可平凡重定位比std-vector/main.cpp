#include "stdafx.h"
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>

// 测试对象：StrA（在 Arraybe 中被标记为 trivially_relocatable）
void test_relocate_stra(int elements, int relocations) {
	std::cout << "=== 测试一：StrA 数组/向量搬迁性能对比 (标记了可平凡重定位) ===" << std::endl;
	std::cout << "元素数量: " << elements << ", 强制搬迁次数: " << relocations << " 次" << std::endl;

	// 1. std::vector<StrA>
	{
		std::vector<StrA> vec;
		vec.resize(elements, "Hello World! A test string that definitely exceeds SSO for std::string.");

		DWORD start = GetTickCount();
		for (int i = 0; i < relocations; ++i) {
			vec.reserve(vec.capacity() + 1);
		}
		DWORD cost = GetTickCount() - start;

		std::cout << "  std::vector<StrA>\n"
			<< "    耗时: " << cost << " 毫秒\n"
			<< "    最终容量: " << vec.capacity() << " 个成员\n";
	}

	// 2. Arraybe<StrA>
	{
		Arraybe<StrA> arr;
		arr.reset(elements);
		for (int i = 0; i < elements; ++i) {
			arr[i] = "Hello World! A test string that definitely exceeds SSO for std::string.";
		}

		DWORD start = GetTickCount();
		for (int i = 0; i < relocations; ++i) {
			arr._capacity(arr._capacity() + 1);
		}
		DWORD cost = GetTickCount() - start;

		std::cout << "  Arraybe<StrA> (白易标签优化)\n"
			<< "    耗时: " << cost << " 毫秒\n"
			<< "    最终容量: " << arr._capacity() << " 个成员\n\n";
	}
}

// 测试对象：std::string（在 Arraybe 中未被标记，走传统搬迁流程）
void test_relocate_string(int elements, int relocations) {
	std::cout << "=== 测试二：std::string 数组/向量搬迁性能对比 (未标记) ===" << std::endl;
	std::cout << "元素数量: " << elements << ", 强制搬迁次数: " << relocations << " 次" << std::endl;

	// 1. std::vector<std::string>
	{
		std::vector<std::string> vec;
		vec.resize(elements, "Hello World! A test string that definitely exceeds SSO for std::string.");

		DWORD start = GetTickCount();
		for (int i = 0; i < relocations; ++i) {
			vec.reserve(vec.capacity() + 1);
		}
		DWORD cost = GetTickCount() - start;

		std::cout << "  std::vector<std::string>\n"
			<< "    耗时: " << cost << " 毫秒\n"
			<< "    最终容量: " << vec.capacity() << " 个成员\n";
	}

	// 2. Arraybe<std::string>
	{
		Arraybe<std::string> arr;
		arr.reset(elements);
		for (int i = 0; i < elements; ++i) {
			arr[i] = "Hello World! A test string that definitely exceeds SSO for std::string.";
		}

		DWORD start = GetTickCount();
		for (int i = 0; i < relocations; ++i) {
			arr._capacity(arr._capacity() + 1);
		}
		DWORD cost = GetTickCount() - start;

		std::cout << "  Arraybe<std::string>\n"
			<< "    耗时: " << cost << " 毫秒\n"
			<< "    最终容量: " << arr._capacity() << " 个成员\n\n";
	}
}

int main() {
#ifdef _DEBUG
	int elements = 10000;
	int relocations = 50;
#else
	int elements = 1000000;
	int relocations = 100;
#endif

	test_relocate_stra(elements, relocations);
	test_relocate_string(elements, relocations);

	return 0;
}