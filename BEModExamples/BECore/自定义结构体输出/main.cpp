#include "stdafx.h"
#include <windows.h>
using namespace be;

//用户自定义结构体输出示例
struct Point {
	int x, y;
	Point() : x(0), y(0) {}
	Point(int x, int y) : x(x), y(y) {}
};

// 1. 对于自定义结构体，只需要实现单参数的全局 __AutoStr__
AutoStr __AutoStr__(const Point& p) {
	return sprintF("Point{%d,%d}", p.x, p.y);
	//也可以用下面这个（内部自动转到UTF8）
	//AutoStr s("Point{"); s << p.x << "," << p.y << "}"; return s;
}

//自定义结构体也可以在类内部定义为友元函数
struct Point2 {
	int x, y;
	Point2() : x(0), y(0) {}
	Point2(int x, int y) : x(x), y(y) {}
	friend AutoStr __AutoStr__(const Point2& p) {
		return sprintF("Point2{%d,%d}", p.x, p.y);
	}
};

// 2. 对于系统内置的结构体（如 RECT），也同样只需要实现单参数 __AutoStr__
AutoStr __AutoStr__(const RECT& r) {
	return sprintF("RECT{%d,%d,%d,%d}", r.left, r.top, r.right, r.bottom);
}

void 信息框也能直接用RECT(c_AutoStr s)
{
	MessageBoxW(0, s, L"", 0);
}

int main()
{
	printf("=== AutoStr 自动化字符串测试 ===\n");

	printf("1. AutoStr 直接转换测试:\n");
	Point p1(10, 20); Point2 p2(111, 222);
	print("   print Direct", p1, 123, p2);
	//或者流式输出
	be::cout << "   be::cout Direct" << " | " << p1 << " | " << 123 << be::endl;

	printf("2. Array 自动输出自定义结构体输出测试:\n");
	Arraybe<Point> points;
	points.push(Point(1, 2));
	points.push(Point(3, 4));
	points.push(Point(5, 6));
	be::cout << "   Array<Point>: " << points << be::endl;

	printf("\n3. debugout 输出自定义结构体输出测试:\n");
	Point p3(100, 200);
	be::cout << p3 << " | " << points << be::endl;
	printf("   (debug后续请在 VS 调试输出窗口查看结果)\n");
	debug(p3, 10086, "hello");


	printf("\n4. 已有类 (如 RECT) 重载 测试:\n");
	RECT rc = { 10, 20, 100, 200 };
	be::print("   print RECT:", rc);

	信息框也能直接用RECT(rc);

	return 0;
}