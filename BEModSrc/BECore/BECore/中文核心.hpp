#pragma once
#include "beout.hpp"

/* 白易基础核心类型取中文别名 */
using 字节集 = Bytes;
using 文本型 = StrA; using 文本型W = StrW; using 宽文型 = StrW;
using 文本型U8 = StrU8; using 通用文本 = AutoStr;
using 文本型X = StrX; using 文本平台型 = StrPlat;
using 整数型 = int; using 整型 = int; using 无符整型 = uint;
using 字节型 = byte; using 字符型 = char; using 宽字型 = charW;
using 短整数型 = short; using 短整型 = short; using 无符短整 = ushort;
using 长整数型 = int64; using 长整型 = int64; using 无符长整 = uint64;

using c_字节集 = c_Bytes; using c_文本型 = c_StrA;
using c_文本型W = c_StrW; using c_文本型U8 = c_StrU8;
using c_通用文本 = c_AutoStr; using c_文本型X = c_StrX;

template<class T, int STACK_CAP = 0> using 数组 = Arraybe<T, STACK_CAP>;
template<class T, int STACK_CAP = 0> using c_数组 = const Arraybe<T, STACK_CAP>&;
template<typename T> using 可空 = NilOpt<T>;
using 空型 = Nil;
#define 空 nil
#ifdef _WIN32
#define 平台换行 "\r\n"
#define 平台换行W _W("\r\n")
#else
#define 平台换行 "\n"
#define 平台换行W _W("\n")
#endif
#define 换行符 "\r\n"
#define 换行符W _W("\r\n")
#define 引号 "\""
#define 引号W _W("\"")
#define 左引号 "“"
#define 左引号W _W("“")
#define 右引号 "”"
#define 右引号W _W("”")
#define 选择 choose
#define 连续赋值 multiassign
#define 多项选择 multichoose


//**总结字节集/文本型相关函数方便记忆的宏（带To的都是内存拷贝版，不带To的为引用版）
//**/
//#define 到字节集 ToBytes  //文本型指针或原始数值转换到字节集拷贝（根据类型自动识别到原始字节集）
//#define 到文本   ToStr    //数值或字节集转换到文本拷贝（宽文本W版可用 到文本<W> ）
//*补充：
//* 1.若不想文本到字节集产生内存拷贝可用[BR]
//	1.1:支持各种文本型指针或字节集自动识别投入构造无空终止的字节集引用版
//*   1.2:此外还可以由任意指针(+长度)双参来决定取到字节集的引用
//* 2.从文本型指针计算其长度并赋予含空终止长度的文本引用[SR]
//**/
//#define 数组字节集 ArrayBytes             //从数组构造字节集引用
//* 原先含空终止的文本转换到相关类型数值 */
//#define 到整数 ToInt
//#define 到短整 ToShort
//#define 到长整 ToLong
//#define 到小数 ToFloat
//#define 到数值 ToDouble

//PS：因宏容易污染全局，现已改为以下的全局函数形式


/**到字节集
 * @param v 支持char/charW指针,任何实体类型(非数组)
 * @return 无空终止字节集
 */
template<typename V>
Bytes 到字节集(V&& v) { return ToBytes(v); }

/**数值或字节集转字符串（默认 A/UTF-8 版接口）
 * @param a 支持任何数值类型、字节集、字符串
 * @return 按照规范，默认（公版）接口始终返回 StrA (UTF-8)
 */
template<typename V>
StrA 到文本(V&& v) { return ToStr(v); }

template<typename StrT, typename V>
StrW 到文本(V&& v) { return ToStr<W>(v); }

/**文本到整数
 * @param s char*或charW*
 * @return
 */
template<typename T>
int 到整数(T&& s) { return ToInt(s); }

/**文本到短整
 * @param s char*或charW*
 * @return
 */
template<typename T>
int 到短整(T&& s) { return ToShort(s); }

/**文本到长整
 * @param s char*或charW*
 * @return
 */
template<typename T>
int64 到长整(T&& s) { return ToLong(s); }

/**文本到浮点型
 * @param s char*或charW*
 * @return
 */
template<typename T>
float 到小数(T&& s) { return ToFloat(s); }

/**文本到双精度浮点型
 * @param s char*或charW*
 * @return
 */
template<typename T>
double 到数值(T&& s) { return ToDouble(s); }

#define 格式化文本 sprintF
#define 数组字节集 ArrayBytes
#define 分割V SplitV

/* 白易输出 */
#define 调试输出 be::debug
#define 打印输出 be::print

/**交换两个变量的数据(类似std::swap)
 * @param a 变量1
 * @param b 变量2
 */
template<typename T>
void 交换变量(T& a, T& b) {
	T temp = (T&&)a;
	a = (T&&)b;
	b = (T&&)temp;
}

template<typename T, typename T_STR, typename T_SPLIT, int STACK_CAP>
Arraybe<T, STACK_CAP>& 分割(const T_STR& 原文本, const T_SPLIT& 分割子,
	Arraybe<T, STACK_CAP>& 输出数组, bool 保持原数组容量 = false)
{
	return Split(原文本, 分割子, 输出数组, 保持原数组容量);
}

#define 定义_枚举型(Name, ValueType, ...) \
struct Name { \
    enum { __VA_ARGS__ }; \
    ValueType _; \
    constexpr Name() : _(0) {} \
    constexpr Name(int v) : _((ValueType)v) {} \
    constexpr operator ValueType() const { return _; } \
}

/**可调用多次设置多个结束处理子程序
 * @param fn
 */
inline void 设置结束处理子程序(void(__cdecl*fn)()) {
	atexit(fn);
}
/**结束程序
 * @param code 进程退出代码
 */
inline void 结束(int code = 0)
{
	fflush(stdout);
	exit(code);
}