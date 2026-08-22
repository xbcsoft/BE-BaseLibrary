#pragma once
#include "stdafx.h"
#include <shlobj.h>

#pragma region 动态库部分
class 动态库
{
public:
	HMODULE hModule = NULL;
	~动态库();

	/**加载动态库
	 * @param 动态库路径 欲加载的动态库文件路径
	 * @return 成功返回真，失败返回假
	 */
	bool 加载(c_StrX 动态库路径);

	/**加载动态库并获取指定函数
	 * @param 动态库路径 欲加载的动态库文件路径
	 * @param 函数名 欲获取的函数名
	 * @return 成功返回函数地址，失败返回 nullptr
	 */
	FARPROC 加载(c_StrX 动态库路径, c_StrA 函数名);

	/**卸载动态库
	 */
	void 卸载();

	/**取函数地址
	 * @param 函数名 欲获取地址的函数名称
	 * @return 成功返回函数地址，失败返回 nullptr
	 */
	FARPROC 取函数(c_StrA 函数名) const;
};
#pragma endregion

StrW _取命令行(Arraybe<StrW>& 命令行数组);
template<class StrT = W>
StrW 取命令行(Arraybe<StrW>& 命令行数组) { return _取命令行(命令行数组); }

StrA 取命令行(Arraybe<StrA>& 命令行数组);

StrX 取运行目录();

StrX 取当前目录();

bool 置当前目录(c_StrX 新当前目录);

StrW 取模块路径(void* 模块句柄 = NULL);

StrX 取执行文件名();

/**读环境变量
 * @param 环境变量名称
 * @param 环境类别=2 0系统级，1用户级，2进程级
 * @return
 */
StrX 读环境变量(c_StrX 环境变量名称, int 环境类别 = 2);

/**写环境变量
 * @param 环境变量名称
 * @param 欲写入内容
 * @param 环境类别=2 0系统级，1用户级，2进程级
 * @return
 */
bool 写环境变量(c_StrX 环境变量名称, c_StrX 欲写入内容, int 环境类别 = 2);

定义_枚举型(目录, int,
	我的文档 = 1,
	我的收藏夹 = 2,
	系统桌面 = 3,
	系统字体 = 4,
	开始菜单组 = 5,
	程序菜单组 = 6,
	启动菜单组 = 7,
	程序数据目录 = 8,
	Windows安装目录 = 9,
	Windows系统目录 = 10,
	临时文件目录 = 11,
	系统缓存目录 = 12
);
/**取特定目录
 * @param 欲获取目录类型=目录::系统桌面，可使用 目录:: 相关枚举值
 * @return 尾部一定没有"\"，与易语言不同要注意！
 */
StrX 取特定目录(int 欲获取目录类型 = 目录::系统桌面);


typedef NTSTATUS(WINAPI* RtlGetVersion_PTR)(void*);
/**取系统版本
 * @param 系统版本 <参考 可空> 返回系统名称文本，如 "Windows 10"
 * @param 内核NT版本 <参考 可空> 返回内核版本，如 "10.0"
 * @return 返回操作系统内部版本号 (BuildVersion)
 */
int 取系统版本(NilOpt<StrA&> 系统版本 = nil, NilOpt<StrA&> 内核NT版本 = nil);
