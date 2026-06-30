#include "stdafx.h"
#include <iostream>
#include <string>
#include <windows.h>

void test1(int iterations) {
	std::cout << "测试常量字符串构造拼接后赋值(str=string('hello')+'World'): "<<iterations<<"次：\n";

	DWORD start = GetTickCount();
	std::string str;
	for (int i = 0; i < iterations; i++) {
		str = std::string("hello")+" World";
	}
	std::cout << "std::string\n"
		<< "  耗时: " << GetTickCount() - start << " 毫秒\n"
		<< "  最终长度: " << str.length() << " 字节\n"
		<< "  最终容量: " << str.capacity() << " 字节\n";

	StrA sa;
	start = GetTickCount();
	for (int i = 0; i < iterations; i++) {
		sa = StrA("hello")+" World";
	}
	std::cout << "白菜字符串\n"
		<< "  耗时: " << GetTickCount() - start << " 毫秒（这个比原生较慢主要是std::string短数据有SSO栈缓存）\n"
		<< "  最终长度: " << sa.len() << " 字节\n"
		<< "  最终容量: " << sa.bytes.capacity_ << " 字节\n\n";
}

void test1_no_sso(int iterations) {
	std::cout << "测试超长常量字符串构造拼接后赋值(绕过SSO): "<<iterations<<"次：\n";

	DWORD start = GetTickCount();
	std::string str;
	for (int i = 0; i < iterations; i++) {
		str = std::string("hello, this is a very long string that exceeds the small string optimization limit of fifteen bytes") + " And another long suffix to make sure it definitely allocates heap.";
	}
	std::cout << "std::string\n"
		<< "  耗时: " << GetTickCount() - start << " 毫秒\n"
		<< "  最终长度: " << str.length() << " 字节\n"
		<< "  最终容量: " << str.capacity() << " 字节\n";

	StrA sa;
	start = GetTickCount();
	for (int i = 0; i < iterations; i++) {
		sa = StrA("hello, this is a very long string that exceeds the small string optimization limit of fifteen bytes") + " And another long suffix to make sure it definitely allocates heap.";
	}
	std::cout << "白菜字符串\n"
		<< "  耗时: " << GetTickCount() - start << " 毫秒\n"
		<< "  最终长度: " << sa.len() << " 字节\n"
		<< "  最终容量: " << sa.bytes.capacity_ << " 字节\n\n";
}

void test2(int iterations) {
	std::cout << "测试字符串自身赋值非扩容拼接(str=str+'World'): "<<iterations<<"次：\n";

	std::string str = "Hello";
	DWORD start = GetTickCount();

	for (int i = 0; i < iterations; i++) {
		str = str+" World";
	}

	std::cout << "std::string\n"
		<< "  耗时: " << GetTickCount() - start << " 毫秒\n"
		<< "  最终长度: " << str.length() << " 字节\n"
		<< "  最终容量: " << str.capacity() << " 字节\n";

	StrA sa = "Hello";
	start = GetTickCount();
	for (int i = 0; i < iterations; i++) {
		sa = sa+" World";
	}

	std::cout << "白菜字符串\n"
		<< "  耗时: " << GetTickCount() - start << " 毫秒\n"
		<< "  最终长度: " << sa.len() << " 字节\n"
		<< "  最终容量: " << sa.bytes.capacity_ << " 字节\n\n";
}

void test3(int iterations) {
	std::cout << "测试字符串自身扩容拼接(str+='World'): "<<iterations<<"次：\n";

	std::string str = "Hello, This is a longer string to exceed SSO.";
	DWORD start = GetTickCount();

	for (int i = 0; i < iterations; i++) {
		str += " World!";
	}

	std::cout << "std::string\n"
		<< "  耗时: " << GetTickCount() - start << " 毫秒\n"
		<< "  最终长度: " << str.length() << " 字节\n"
		<< "  最终容量: " << str.capacity() << " 字节\n";


	StrA sa = "Hello, This is a longer string to exceed SSO.";
	start = GetTickCount();
	for (int i = 0; i < iterations; i++) {
		sa += " World!";
	}

	std::cout << "白菜字符串\n"
		<< "  耗时: " << GetTickCount() - start << " 毫秒\n"
		<< "  最终长度: " << sa.len() << " 字节\n"
		<< "  最终容量: " << sa.bytes.capacity_ << " 字节\n\n";
}


int main()
{
#ifdef _DEBUG
	int iterations = 100000;
#else
	int iterations = 10000000;
#endif // DEBUG


	test1(iterations);
	test1_no_sso(iterations);
	test2(iterations/100);
	test3(iterations);

	return 0;
}