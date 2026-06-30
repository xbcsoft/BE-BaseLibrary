#pragma once
/**@ModuleTitle: 白易核心基础库
*  @version:     0.6.0
*  @platform:    anysys(x86|x64)
*  @compiler:    msvc141
*  @author:      xbcsoft
*  @datetime:    06-30
*  @description:
*/
#define  _BE_CHARSETUTF8
#include "BytesStr.hpp"
#include "Arraybe.hpp"
#include "HashTbe.hpp"
#include "behelper.hpp"
#include "beout.hpp"
#include "中文核心.hpp"
#include "中文核心扩展/文本操作.h"
#include "中文核心扩展/字节集操作.h"
#include "中文核心扩展/数组操作.h"
#include "中文核心扩展/日期时间型.h"

#ifdef _WIN32
#ifdef _MSC_VER //当文件编码改UTF-8，A版识别问题MSVC编译器报警告，但实际安全！
#pragma warning(disable:4819) //该文件包含不能在当前代码页(0)中表示的字符。
#pragma warning(disable:4474) //printf: 格式字符串中传递的参数太多
#pragma warning(disable:4477) //printf: 格式字符串"%s"需要类型“char*”的参数
#pragma warning(disable:4275) //非 dll 接口 class 用作 dll 接口
#pragma warning(disable:4251) //需要有 dll 接口由 class 的客户端使用
#endif
inline int _ = SetConsoleOutputCP(65001);
static_assert(*"\u1234"==-31, "必须启用 /utf-8 编译器配置才能编译本项目！");
#endif
