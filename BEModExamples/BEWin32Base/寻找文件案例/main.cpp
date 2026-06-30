#include "stdafx.h"

void main()
{
	// ---- 测试1：枚举 Windows 目录下所有 .exe 文件 ----
	be::print(LR"(=== 测试1：枚举 C:\Windows\*.exe ===)");
	{
		寻找文件 f;

		StrW 名 = f.开始寻找(LR"(C:\Windows\*.exe)");
		while (名!=L"") {
			be::print(名);
			名 = f.下一个();
		}
	}

	// ---- 测试2：只枚举寻找文件::子目录 ----
	be::print(LR"(
=== 测试2：枚举 C:\Windows 下的寻找文件::子目录 ===(支持A版字符串))");
	{
		寻找文件 f;
		StrA 名 = f.开始寻找(R"(C:\Windows\*)", 寻找文件::子目录);
		while (名!="") {
			// 跳过 . 和 ..
			if (名 != "." && 名 != "..") {
				be::print(名);
			}
			名 = f.下一个();
		}
	}

	// ---- 测试3：两个实例同时工作（验证无 TLS(线程本地存储) 互串问题） ----
	be::print(_W("\n=== 测试3：两个寻找文件实例并发嵌套 ==="));
	{
		寻找文件 outer, inner;
		StrW 外 = outer.开始寻找(LR"(C:\Windows\sys*)", 寻找文件::子目录);
		while (外.len() > 0) {
			if (外 != L"." && 外 != L"..") {
				// 对每个寻找文件::子目录再枚举其内部的 .dll
				StrW 子路径 = LR"(C:\Windows\)" + 外 + LR"(\*.dll)";
				StrW 内 = inner.开始寻找(子路径);
				if (内.len() > 0) {
					be::print(外 + L"  ->  " + 内);   // 只打第一个
					inner.关闭(); // 提前结束内层，节省句柄
				}
			}
			外 = outer.下一个();
		}
	}

	// ---- 测试4：枚举文件（回调模式，类型传入 nil 则默认为 正常文件）----
	be::print(LR"(
=== 测试4：文件_枚举 默认(空)类型（非递归）===)");
	文件_枚举(LR"(C:\Windows)", false, nil, [](const StrW& 名) -> bool {
		be::print(名);
		return true;
	});

	// ---- 测试5：枚举目录（回调模式）----
	be::print(LR"(
=== 测试5：文件_枚举 子目录（非递归）===)");
	文件_枚举(取当前目录(), false, 寻找文件::子目录,
		[](const StrW& 名) -> bool { be::print(名); return true; });

	// ---- 测试6：收集模式，枚举全部（正常文件 | 子目录）----
	be::print(LR"(
=== 测试6：文件_枚举<false,true> 收集返回数组（非递归）===)");
	{
		Arraybe<StrW> 列表 = 文件_枚举<false, true>(LR"(C:\Windows)", false, 寻找文件::正常文件 | 寻找文件::子目录);
		be::print("共找到条目：" + 到文本(列表.count));
	}

	// ---- 测试7：收集模式，带完整路径，枚举子目录（递归）----
	be::print(LR"(
=== 测试7：文件_枚举<true,true> 子目录 收集返回数组（递归）===)");
	{
		Arraybe<StrW> 列表 = 文件_枚举<true, true>(取当前目录(), true, 寻找文件::子目录);
		be::print("共找到条目：" + 到文本(列表.count), 列表);
	}
}