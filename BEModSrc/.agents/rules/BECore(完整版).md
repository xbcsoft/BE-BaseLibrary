---
trigger: model_decision
---

# BECore 核心库 — Skills 完整版

> **版本**: 0.6
>
> **英文底层核心**：`bebase.hpp`、`BytesStr.hpp`、`Arraybe.hpp`、`HashTbe.hpp`、`behelper.hpp`、`beout.hpp` — 定义二级基本数据类型、通用类型输出、字符串语义与其转编码、基础类别辅助工具...
>
> **中文底层核心：**中文核心.cpp #其表层接口名翻译自英文底层核心，二者地位等价
>

---

## 前言

#### 什么是"二级基本数据类型"

`Bytes`、`StrA`、`StrW`、`Arraybe` 在白易核心库中被称为**二级基本数据类型**——相对于语言内置的一级类型（`int`、`char`、`byte` 等），它们是由此构筑的具有明确语义的复合数据结构，同时也是整个库的最小基石。

#### 最小化设计原则

白易核心类型的设计遵循**"最小职责"**原则：

- **定型**：清晰的内存布局与构造/析构语义（引用 vs 拷贝。利用底层细粒度的手动内存管控，造就了高层使用者无需干预的全自动化体验）。
- **运算符**：仅提供核心可支配的运算符（赋值、拼接、比较、下标等），使其像内置类型那样易于参与表达式计算。
- **工具方法剥离**：类的成员方法尽可能少，甚至没有业务工具方法（诸如"查找子串"、"文本替换"等）。这些扩展能力全部由**外部全局函数**提供支援。

```text
[白易核心基础] Bytes / StrA / StrW / StrU8 / AutoStr / Arraybe / HashTable
          ↓ #最小实现（定型 + 内存模型管理 + 极简成员 + 运算符）
[中文核心扩展] 文本操作 / 字节集操作 / 数组操作 (即核心扩展的别名) / 日期时间型
          ↓ #跨平台，以全局外部函数形式注入平台业务操作
[中文Win32扩展] 系统日期时间 / 文件读写 / 磁盘操作 / ...
```

#### 为什么核心类型的扩展采用外部全局函数设计？

1. **保证核心极简**：核心库 `BytesStr.hpp` 及成员函数不会膨胀爆炸，永远只负责内存模型本身的数据安放机制。
2. **隔离平台依赖**：外部函数可自由包含如 `<windows.h>` 、`<iconv.h>`的实现，而不污染基础头文件。
3. **多语言开发支持（如英文版分离与自然语言解耦）**：由于类内成员在底层实现时只能使用唯一一种语言（通常是英文）来编写，日后若直接在类内直接添加其他语言的成员将无法维护且难以扩展。因此，必须将这些扩展业务剥离到类外，依赖外部全局函数来实现，故基础核心类的本身让其实现要足够小巧不臃肿。
   白易不排斥英文，既然有“中文核心”、“中文基础扩展”等业务操作入口，未来也可以随时再开英文扩展文件夹单独实现一套外部接口，以此彻底完成自然语言的解耦与灵活扩展，保证底层核心对象绝对不受任何特定自然语言生态的死板绑定。
---

## 本篇目录大纲 (仅讲述底层核心部分)

1. [类型别名与宏](#1-类型别名与宏)
2. [Bytes 类 — 字节集](#2-bytes-类--字节集)
3. [Bytes::Ref (BR) — 字节集引用](#3-bytesref-br--字节集引用)
4. [StrA / StrW 类 —字符串](#4-stra--strw-类-字符串)
5. [StrU8 — 平台无关的 UTF-8类型](#5-stru8--平台无关的-utf-8类型)

6. [AutoStr — 统一自动转换类与UTF-8优先设计](#6-autostr--统一自动转换类与utf-8优先设计)
7. [全局工具函数与编码转换](#7-全局工具函数与编码转换)
8. [Arraybe 类 — 静态栈/动态堆数组](#8-arraybe-类-静态栈动态堆数组)
9. [Split 与 SplitV — 分割函数](#9-split-与-splitv-分割函数)
10. [HashTbe 类 — 哈希表 (K:V)](#10-hashtbe-类--哈希表-kv)
11. [NilOpt — 可空类型](#11-nilopt--可空类型)
12. [be::print / be::cout — 输出系统](#12-beprint--becout--输出系统)
13. [内存管理约定](#13-内存管理约定)
14. [已知设计要点与陷阱](#14-已知设计要点与陷阱)

---

## 1. 类型别名与宏

> **💡 架构提示**：白易 0.6+ 已将所有的 C 标准库引入、平台跨平台宏探测、基础别名（如 `byte`、`uint64`）以及所有的底层元编程特征结构（Traits）独立剥离到了 `bebase.hpp` 中。`bebase.hpp` 彻底实现了无类依赖和极致轻量化，是整个白易底层库的**“万物基石”**。

```cpp
using byte   = unsigned char;
using uchar  = unsigned char;
using ushort = unsigned short;
using uint   = unsigned int;
using int64  = long long;
using uint64 = unsigned long long;

// ssize_t 定义（以平台与架构为中心，Linux已自带ssize_t）
#ifdef _WIN32
#ifdef _M_IX86
using ssize_t = int;
#else
using ssize_t = int64;
#endif
#endif

// Win32
using charW = wchar_t;        // 2字节
#define _W(str) L##str

// Linux
using charW = char16_t;       // 2字节
#define _W(str) u##str
#define _T(str) str

// W版低级接口（通过 <W> 模板参数显式调用，在Linux下亦有手写实现保证跨平台一致）
template<class T> size_t strlen(const charW* s);
template<class T> int strcmp(const charW* a, const charW* b);

// 字符串类型别名（以平台为中心设计）
#ifdef _WIN32
using charPlat = charW;
using StrPlat = StrW;          // Win32 平台默认为宽字符串
#else
using charPlat = char;
using StrPlat = StrA;          // Linux 平台默认为窄字符串
#endif

class StrU8 : public StrA;     // UTF-8 字符串：继承 StrA
using BR      = Bytes::Ref;    // 字节集引用的简写
static const Bytes::RefTag BRef; // 引用标记类型实例
inline constexpr byte BNULLA[] = { 0 }; // A版空终止字节
inline constexpr byte BNULLW[] = { 0, 0 }; // W版空终止字节

// 编码转换宏（基于 _BE_CHARSETUTF8 约定，窄串 A 始终为 UTF-8）
#define AtoW  U8toW            // A(U8) -> UTF-16
#define WtoA  WtoU8            // UTF-16 -> A(U8)
#define AtoU8(str) str         // A -> U8（直接返回，零拷贝）
#define U8toA(str) str         // U8 -> A（直接返回，零拷贝）

using c_Bytes   = const Bytes&;
using c_StrA    = const StrA&;
using c_StrU8   = const StrU8&;
using c_StrW    = const StrW&;
using c_StrX    = const StrX&;
using c_AutoStr = const AutoStr&;
template<class T, int STACK_CAP = 0> using c_Arraybe = const Arraybe<T, STACK_CAP>&;

// behelper.hpp 中的实用宏
#define choose(cond, a, b) ((cond) ? (a) : (b)) // 条件选择宏，类似于易语言的：选择(条件, 待选择值1, 待选择值2)
```



---



## 2. Bytes 类 — 字节集

### 2.1 内存布局

```cpp
class Bytes {
public:
    byte*  buf;        // 数据指针
    size_t size;       // 数据的实际字节数
    size_t capacity_;  // 内部容量（capacity_==0 表示引用模式，不析构）
};
```

> **额外的 2 字节预留**：默认构造（以及 `_reset` 等重新分配）都会在 `size` 之外多分配 2 字节容量，保证 StrA/StrW 在引用 Bytes 内存时可无损写入空终止符。后续 append/拼接时同样会检查 `capacity_` 是否足够，不够则自动扩容。

**核心规则**：`capacity_ == 0` 时为**引用模式**（`buf` 指向外部内存，析构时不 `free`）；`capacity_ > 0` 时为**拥有模式**（`buf` 由自身 `malloc`，析构时 `free`）。

### 2.2 构造函数

| 构造方式 | 语义 | 示例 |
|---------|------|:----:|
| `Bytes()` | 空构造，buf=nullptr, size=capacity_=0 | `Bytes b;` |
| `Bytes({1,2,3})` | **引用**字面量 initializer_list（栈上分配，零堆拷贝） | `Bytes b = {1,2,3};` |
| `Bytes(byte(&arr)[N])` | **拷贝**非 const 字节数组 | `byte a[4]; Bytes b(a);` |
| `Bytes(const byte(&arr)[N])` | **引用**const 字节数组（零堆拷贝） | `const byte a[4] = {1,2,3,4}; Bytes b(a);` |
| `Bytes(const Ref& v)` | **引用**BR的数据（不拷贝） | `Bytes b(BR("hello"));` |
| `explicit Bytes(size_t size)` | 分配 size 字节（**不初始化**内容） | `Bytes b(100);` |
| `Bytes(byte c, size_t size)` | 分配并用 c 填充 | `Bytes b(0, 64);` |
| `Bytes(const void* dat, size_t size)` | **拷贝**构造 | `Bytes b(ptr, len);` |
| `Bytes(dat, BRef, size)` | **引用**构造（通过 RefTag 标记） | `Bytes b(ptr, BRef, len);` |
| `Bytes(dat1, len1, dat2, len2)` | 拼接两段数据（**拷贝**） | - |
| `Bytes(const Bytes& zjj)` | **拷贝**构造 | `Bytes b2(b1);` |
| `Bytes(Bytes&& zjj)` | **移动**构造 | `Bytes b2(std::move(b1));` |

### 2.3 公开方法

```cpp
// 重置（先析构旧内存再分配新的）
void reset(size_t newSize = 0);
void reset(byte c, size_t size);            // 重置+填充
void reset(const void* dat, size_t size);   // 重置+拷贝

// 容量管理
void   _capacity(size_t newCapacity);  // 设置容量（realloc）
size_t _capacity();                    // 获取当前容量

// 引用绑定（先析构旧内存）
Bytes& ref(const void* dat, size_t size);

// 尾部追加
Bytes& append(size_t addSize);              // 仅扩容，不初始化
Bytes& append(const void* dat, size_t s2);  // 追加数据
Bytes& append(const Bytes& zjj);
Bytes& append(const Ref& v);

// 内存拷贝
void copyFrom(const void* dat, size_t size1);           // 从头覆盖
void copyFrom(size_t startI, const void* src, size_t s); // 从偏移位置覆盖

// 运算符
Bytes  operator+(const Bytes& z) const;   // 拼接（新分配）
Bytes& operator+=(const Bytes& zjj);      // 尾部追加
bool   operator==(const Bytes& zjj) const; // 逐字节比较
operator byte*() const;                   // 隐式转到 byte*

// 带越界检查的访问
byte& at(ssize_t i) const;                // 支持负数索引，-1为最后一个
byte& operator[](size_t i) const;         // 低级不检查访问

explicit operator AutoStrW() const;       // 十进制可视化文本

```

### 2.4 内部方法（public 但以下划线开头）

```cpp
void _ref(const void* dat, size_t len);  // 赋引用（不析构旧的）
void _mknull();                           // 置空（buf=nullptr, size=capacity_=0）
void _reset(size_t newSize);              // 分配新内存（不析构旧的）
void _move(Bytes& z);                     // 移动内部状态（不析构旧的）

当初设计下面这个函数原本是SL公开方法的，但有坑现已变为内部方法，待会说怎么使用BA/BW来避坑！
/**sublen创建新引用并递减字节集长度（由于移动语义时它的缺陷，此函数不得在右值中使用）
 * @param subsize
 * @return
*/
Bytes _SL(int subsize) {
	if (!size)return Bytes();
	return Bytes(buf, RefTag(), size-subsize);
}
```

### 2.5 append 扩容策略

```
if capacity_ < 32MB:   增长 max(addSize, capacity_ >> 1)  // 即 ×1.5 倍
if capacity_ >= 32MB:  增长 max(addSize, capacity_ >> 2)  // 即 ×1.25 倍
```

如果当前是引用模式（`capacity_==0`），会先备份旧 `buf` 指针，分配新内存，再 `memcpy` 旧数据过来。

---



## 3. Bytes::Ref (BR) — 字节集引用

**轻量级只读引用**，用于减少 `Bytes` 构造析构的开销（当然Bytes本身也能作为引用，但结尾处析构函数可能还是会有），而BR不拥有容器内存、完全无析构函数，以及字符串去掉空终止符后的引用传递。

```cpp
struct Ref {
    const void* p;    // 数据指针
    size_t      size; // 字节数
};
```

### 3.1 构造

| 构造方式 | 说明 |
|---------|------|
| `BR()` | 空引用 |
| `BR(ptr, size)` | 指针+字节数 |
| `BR("hello")` | **explicit**，从 char* 构造（size = strlen，**不含空终止**） |
| `BR(L"hello")` | **explicit**，从 charW* 构造（size = strlen<W>*2，**不含空终止**） |
| `BR({1,2,3})` | 从 initializer_list 字面量构造 |
| `BR(byte_array)` | 从 byte[N] 数组构造 |
| `BR(strA)` | 从 StrA 构造（size = bytes.size，**不含空终止**） |
| `BR(strW)` | 从 StrW 构造（size = bytes.size，**不含空终止**） |

> **⚠️ 禁止右值引用**：设计时对 `BR(StrA&&)` 和 `BR(StrW&&)` 均 `= delete`，防止悬垂引用。

####  字面量字节集创建

`Bytes` 和 `BR` 都支持直接从 `{...}` 字面量创建；若基于已有的 `byte bs[]` 静态数组，`Bytes` 会拷贝数据，而 `BR` 仍保持引用：

```cpp
// Bytes 字面量（引用模式，不拷贝）
Bytes a = {1, 2, 3};           // initializer_list<byte>
Bytes b({1, 2, 3});            // 同上
Bytes c = {0xFF, 0x00, 0xAB};  // 支持十六进制

// BR 字面量（同样是引用）
BR r1 = {1, 2, 3};             // initializer_list<byte>
BR r2({5, 6});                 // 直接构造
byte bs[] = {7, 8, 9};         // 已有数组

Bytes d = bs;                  // 拷贝构造，独立数据
BR r3 = bs;                    // 引用构造，不拷贝

// 可直接用于函数参数
Split(src, BR({6}), arr);       // 用字面量 {6} 作为分割符
c.append({4, 5, 6});            // 尾部追加字面量字节

// ⚠️ 注意：判断/比较时也要显式写 BR({ ... })，否则 if(a == {1,2}) 会触发语法错误
if (a == BR({1, 2})) { /* ... */ }
```

#### 从字符指针构造 BR 的注意事项

`BR` 从 `const char*` / `const charW*` 构造是 **explicit** 的，必须显式写出：

```cpp
// ✅ 正确
BR r1 = BR("hello");           // explicit 构造，size=5
BR r2 = BR("AB\0CD", 5);       // 带 \0 的数据，必须手动指定 size
BR r3 = BR(_W("你好"));         // 宽字符，size=4 (2字符×2字节)

// ❌ 编译错误（explicit 不允许隐式转换）
// BR r4 = "hello";            // 不允许

// 但在函数参数位置可以用字面量 {}
Split(src, BR(","), arr);       // OK：显式构造
Split(src, {44}, arr);          // OK：44=',' 的ASCII码，走 initializer_list
```

#### SR 辅助函数—从字符串保留空终止构造BR

```cpp
BR SR(const char* s);   // size = strlen(s)+1       （含1字节空终止）
BR SR(const charW* s);  // size = (strlen<W>(s)+1)*2  （含2字节空终止）
BR SR(const StrA& s);   // size = bytes.size+1
BR SR(const StrW& s);   // size = bytes.size+2
// ⚠️ SR(StrA&&) 和 SR(StrW&&) 均 = delete，防止悬垂引用
```

### 3.2 BR 与 SR 的核心区别 

#### 空终止问题

字符串（StrA/StrW）内部的 `bytes.size` **不含空终止符**，但内存中 `bytes.buf[bytes.size]` 处总有 `\0`。当字节集需要引用字符串的数据时，就产生了一个问题：**要不要包含空终止符？**

- **`BR`（不含空终止）**：用于纯二进制数据拼接等场景
- **`SR`（包含空终止）**：用于需要保留空终止的等场景

```cpp
StrA s = "Hello";

// BR: 引用纯数据（5字节: H,e,l,l,o）
BR ref = s;              // ref.size = 5, 不含 '\0'
// 等价于：BR(s._buf(), s.bytes.size)

// SR: 引用含空终止（6字节: H,e,l,l,o,\0）
BR sref = SR(s);         // sref.size = 6, 包含 '\0'
// 等价于：BR(s._buf(), s.bytes.size + 1)

// StrW 同理（但空终止是2字节）
StrW ws = _W("Hi");
BR wref = ws;            // wref.size = 4  (2字符 × 2字节)
BR wsref = SR(ws);       // wsref.size = 6  (4 + 2字节空终止)
```

#### 使用场景对照

| 场景 | 用 BR | 用 SR |
|------|-------|-------|
| 字节集拼接 `+` | ✅ `BR("abc") + BR("def")` → 6字节 | ❌ 会把中间的 `\0` 也拼进去 |
| Split 分割符 | ✅ `Split(src, BR(","), arr)` | ❌ 分割符不需要空终止 |
| 比较 `==` | ✅ `BR(s1) == BR(s2)` | ❌ 除非刻意要比含空终止的版本 |
| 序列化到二进制流 | ❌ 读取方不知道字符串在哪结束 | ✅ 保留空终止，读取方可用 strlen 恢复 |
| 传递给需要 `\0` 结尾的 C API | ❌ 数据不含 `\0` | ✅ 安全传递 |
| 字节集视图遍历 | ✅ `for(BR v : SplitV<Bytes>(...))` | — |



### 3.3 BA 与 BW 助手 — 借引字节集容器使用权并去空终止

先上代码：

```cpp
inline Bytes BA(Bytes&&b) { return Bytes((Bytes&&)b, b.size ? b.size-1 : 0); }
inline Bytes BA(Bytes& b) { return Bytes(b, BRef, b.size ? b.size-1 : 0); }
inline Bytes BW(Bytes&&b) { return Bytes((Bytes&&)b, b.size ? b.size-2 : 0); }
inline Bytes BW(Bytes& b) { return Bytes(b, BRef, b.size ? b.size-2 : 0); }
```

这里需要特别注意的是：
- **对于右值（临时对象）**：`BA(Bytes&& s)` 和 `BW(Bytes&& s)` 会通过移动构造接管临时对象的所有权，并去除空终止符（安全）。
- **对于左值（非临时对象）**：`BA(Bytes& s)` 和 `BW(Bytes& s)` **仅执行引用构造（浅拷贝，`capacity_ = 0`），绝非深拷贝**！这意味着返回的 `Bytes` 依然指向原对象的缓冲区，必须保证原对象 `s` 的生命周期长于返回的引用对象，否则会导致悬垂指针。

- **`BA`**：去除 1 字节的空终止，用于 ANSI / UTF-8。
- **`BW`**：去除 2 字节的空终止，用于 UTF-16 (Unicode)。

```cpp
Bytes a = BA(sprintF("...")); // 安全！BA 接管了 sprintF 产生的临时对象并去掉了 \0
```

⚠️ **不要在临时对象上链式调用** `_SL(-1)` 或 `.buf`，那会产生指针悬垂，始终使用 `BA` 或 `BW` 来接管返回类型为 `Bytes` 的函数。



### 3.4 BSA / BSW — 字节集无损转到字符串对象 (Zero-Copy Wrap)

当通过 `SplitV`、`atU8` 或读取文件等拿到 `Bytes` 实体，并需要将其“提升”为 `StrA` / `StrW` 对象实例以传递给业务函数时，可以使用这组包装函数。这组函数采用模板参数 `PADZERO` 来支持不同的源字节集状态，以达到零拷贝的移动/拷贝包装效果。

> 💡 **使用与性能示例**：关于此包装在处理非常规字符编码（如 GBK 汉字字面量）时的性能开销、内存安全及堆分配对比，请参阅：[14.7.2 核心技巧：结合 BSA 包装非常规字符集（如 GBK）](#1472-核心技巧结合-bsa-包装非常规字符集如-gbk)。

- **`BSA<PADZERO>(Bytes)`**：可无损转换为 `StrA` (A/UTF8)。
- **`BSW<PADZERO>(Bytes)`**：可无损转换为 `StrW` (UTF-16)。


#### 模式参数 `PADZERO` 区别：
- **默认模式（`PADZERO = false`）**：如 `BSA(bytes)` / `BSW(bytes)`。适用于原始字节流**已包含**空终止符的场景。它会接管内存并直接将字符串的 `size` 减 1 (A) 或 减 2 (W)，以符合字符串的长度语义，无需内存分配与拷贝。
- **填充模式（`PADZERO = true`）**：如 `BSA<true>(bytes)` / `BSW<true>(bytes)`。适用于原始字节流**不包含**空终止符的场景（如纯数据内存）。它会自动先在尾部追加/填充空终止符（该操作在原字节集容量不足时会触发扩容重分配，不会产生多余的数据拷贝），随后递减长度将其安全赋予生成的字符串对象（无数据区拷贝）。

> ⚠️ **注意**：这组函数期望返回一个合法的、符合白易字符串内存约定的 `StrA`/`StrW`（长度不计入 `\0`，但结尾必定保留 `\0`）。

#### 代码实现参考：
```cpp
template<bool PADZERO = false>
inline StrA BSA(Bytes&&b) {
	StrA s; s.bytes._move(b);
	if constexpr (PADZERO)s.bytes += BNULLA;
	if (s.bytes.size)s.bytes.size -= 1;
	return s;
}
template<bool PADZERO = false>
inline StrA BSA(const Bytes& b) {
	StrA s; if constexpr (PADZERO) {
		(Bytes&)b += BNULLA; ((Bytes&)b).size -= 1;
		s.bytes.ref(b.buf, b.size);
		return s;
	}
	s.bytes.ref(b.buf, b.size ? b.size -1 : 0);
	return s;
}
template<bool PADZERO = false>
inline StrW BSW(Bytes&&b) {
	StrW s; s.bytes._move(b);
	if constexpr (PADZERO)s.bytes += BNULLW;
	if (s.bytes.size)s.bytes.size -= 2;
	return s;
}
template<bool PADZERO = false>
inline StrW BSW(const Bytes& b) {
	StrW s; if constexpr (PADZERO) {
		(Bytes&)b += BNULLW; ((Bytes&)b).size -= 2;
		s.bytes.ref(b.buf, b.size);
		return s;
	}
	s.bytes.ref(b.buf, b.size ? b.size -2 : 0);
	return s;
}
```

### 3.5 _BSA / _BSW — 指针级零拷贝字符串包装

为了对外部任意数据指针和长度进行无开销的直接字符串引用（不使用 `Bytes` 实体中介），白易核心库提供了更底层的 `_BSA` 和 `_BSW` 全局包装函数。

```cpp
inline StrA _BSA(const void* p, size_t size);
inline StrW _BSW(const void* p, size_t size);
```

#### 核心特征与机制
1. **零堆分配与零拷贝**：直接利用 `s.bytes.ref(p, size)` 方式在引用模式（`capacity_ = 0`）下引用参数指针所指向的外部数据，完全不发生任何数据拷贝与内存分配。
2. **COA 自动保护**：如果对生成的 `StrA` 或 `StrW` 进行修改（例如 `s += "tail"`），底层会检测到引用模式并自动转为拥有模式以拷贝数据，避免污染原始外部缓冲区。

> [!WARNING]
> **⚠️ 不保证空终止符风险**
> 不同于 `BSA / BSW` 能通过在原字节集尾部探测或追加 `\0` 来确保空终止安全性，`_BSA / _BSW` **不保证生成的字符串以空终止符结尾**。
> - **安全场景**：将得到的字符串只读引用传递给框架内置的、基于字节长度（`len()` 或 `bytes.size`）的文本处理函数（如 `寻找子文本`、`子文本替换`、`Split` 等）。
> - **极度危险场景**：将得到的字符串直接隐式转换并传递给 Win32 API 或 C 风格字符串函数（如 `printf("%s")`、`MessageBox` 等）。如果源指针在指定 `size` 边界处没有 `\0`，会导致越界读取、数据泄露或进程奔溃！若需要将非空终止的数据段安全地转为包含空终止符的字符串实体，必须采用拷贝构造 `StrA(p, size)`。

------



## 4. StrA / StrW 类 —字符串

### 4.1.1 StrA内存布局

```cpp
class StrA {
public:
    Bytes bytes;  // 内部存储（bytes.size = 字符串长度，不含空终止符）
};
```

**StrA 不继承 Bytes，而是包含一个 Bytes 成员**。字符串数据存在 `bytes.buf` 内，`bytes.size` 只记录有效字符字节数（不含 `\0`），但内存中在 `bytes.buf[bytes.size]` 处总是有一个 `\0`。

> ✅ **空终止自动保证（StrA）**：StrA 在任何分配路径下（构造、`reset`、`append` 扩容等）都会在 `bytes.size` 之外额外预留 **至少 2 字节** 并在StrA下清零尾部后1字节。因此 `bytes.buf[bytes.size]` 始终均为 `\0`。**无需也不应该**在使用前手动 `bytes.append(1)` 来补充空终止，直接使用即可。

### 4.1.2 StrA构造语义

| 构造方式 | 语义 |
|---------|------|
| `StrA()` | 空构造 |
| `StrA("hello")` | **引用**常量字符串（不拷贝！capacity_=0） |
| `StrA(char* arr)` | **拷贝**构造（运行时计算长度） |
| `StrA("hello", 5)` | **拷贝**构造 |
| `StrA(const StrA& s)` | **拷贝**构造 |
| `StrA(StrA&& s)` | **移动**构造 |
| `explicit StrA(size_t len)` | 分配 len 字节+空终止 |
| `StrA('x', 10)` | 分配 10 字节并用 'x' 填充 |
| `StrA(const StrA& s, size_t len)` | **子串拷贝**：取 s 的前 len 个字符（若 len > s.len 则取全部） |

> **⚠️ 单参 `const char*` 构造是引用**：`StrA s = "hello"` 是引用，如果之后对 `s` 做 `+=` 等修改操作，内部会自动切换到拷贝模式（因为 `append` 检测 `capacity_==0` 时会先备份再 malloc）。
>
> **`char*` 指针构造是拷贝**：传入指针时走拷贝构造，会运行时计算长度（`strlen`），这避免了编译时数组退化的问题。

### 4.1.3 StrA赋值与拼接

```cpp
StrA& operator=(const char* s);    // 拷贝赋值
StrA& operator=(const StrA& s);    // 拷贝赋值
StrA& operator=(StrA&& s);         // 移动赋值
StrA& operator+=(const char* s);   // 尾部追加
StrA& operator+=(const StrA& s);
StrA  operator+(const char* s) const;
StrA  operator+(const StrA& s) const;
friend StrA operator+(const char* s1, const StrA& s2);
```

### 4.1.4 StrA隐式转换与指针使用

StrA 提供了到 `char*` 的隐式类型转换，使得它能**直接投入**到大多数需要字符串指针的函数中（变参函数除外）。同时提供了 `_buf()` 方法来获取底层的 `byte*` 内存指针。

```cpp
operator char*() const { return (char*)bytes.buf; } // 可投入需要C风格字符串的函数参数
inline byte* _buf() const;           // 获取底层字节序列的首地址 byte*
```

> **💡 提示：白易的字符串取到的字符串指针通常保证不会是 NULL**
>
> 无论是空构造 `StrA()`、空 `reset()` 还是 `std::move` 后的源对象，其内部 `bytes.buf` 始终保证指向合法的空字符串字面量 `""`（或 `_W("")`）。
>
> 此外，**请直接让对象发生隐式转换**（例如直接作为参数传给强类型函数），或者使用强转 `(char*)s`。告别 `(char*)s._buf()` 这种啰嗦且无必要的写法。`_buf()` 返回的是 `byte*`，主要用于字节级内存操作。

**✅ 隐式转换可用场景**：
- **C 标准库**：`fopen(s, "r")`, `strcmp(s, "text")`
- **Win32 API**：`SetWindowTextA(hwnd, s)`

**❌ 不可用场景**：
- **变参函数**：`printf("%s", s)` —— 请见 [14.5 章节](#145--警惕-printfscanf-类变参函数的直接传参)

### 4.1.5 StrA 字符访问
```cpp
char& at(ssize_t i) const; // 带越界检查，支持负数。返回的是 ANSI/UTF-8 原始字节引用。
```



------

### 4.2.1 StrW内存布局

结构与 StrA 完全对称，但每个字符占 2 字节。

```cpp
class StrW {
public:
    Bytes bytes;  // bytes.size = 字符串长度 × 2（size不含空终止的2字节，但内部容量会保留2字节）
};
```

#### **StrW的定位**

**W 版统一为 UTF-16**：无论平台如何，`StrW`/`charW` 的编码语义始终是 UTF-16（即固定占 2 字节）。这一设计彻底规避了在 Linux 等系统中原生 `wchar_t` 长度为 4 字节（UTF-32）所导致的跨平台内存体积与处理逻辑不一致的深坑。
在字面量声明上：`_W()` 宏在 Windows 上等价 `L##str`（因其 `wchar_t` 恰为 2 字节），在 Linux 上等价 `u##str`（强制使用 2 字节的 `char16_t`）。这确保了 W 版代码天然跨平台，只需加 `_W()` 宏包裹即可。

####  与 StrA 的核心差异

- `reset(size_t len)` 内部调用字节版 `_reset(len * 2)`

- 空终止是 2 字节的 `charW(0)`

- `StrW(const charW* s)` **裸指针走引用**，`StrW(charW* arr)` **指针走拷贝**（运行时计算长度）

- len() 与 byte.size

  |                               | `StrA`                                | `StrW`                          |
  | ----------------------------- | ------------------------------------- | ------------------------------- |
  | **`len()`**                   | 返回 `bytes.size`（字符数 == 字节数） | 返回 `bytes.size / 2`（字符数） |
  | **`bytes.size` 或 `_size()`** | 字节数（== 字符数）                   | 字节数（== 字符数 × 2）         |

  对于 **StrA**：`len() == bytes.size == _size()`（三者等价）

  对于 **StrW**：`len() == bytes.size / 2`，`bytes.size == _size() == len() * 2`

- **快速判空支持**：
  对 `Bytes`、`StrA`、`StrW`、`StrX` 均已重载 `explicit operator bool`，直接使用 `if (obj)` 或 `if (!obj)` 即可完成快速且语义准确的判空。

- ✅ **双空终止自动保证（StrW）**：StrW 在任何分配路径下均能保证 `bytes.buf` 永不为 NULL（空状态下指向 `_W("")`）。同时在任何分配路径下（构造、`reset`、`append` 扩容等）都会在 `bytes.size` 之外额外预留 **至少 2 字节** **并清零**，即 `bytes.buf[bytes.size]` 和 `bytes.buf[bytes.size + 1]` 均为 `0`。这同时满足了 `SHFileOperationW` 等 Win32 API 对路径字符串必须以**双 `\0`**（即一个 `charW(0)`）结尾的强制要求。**无需也不应该**在使用前手动 `bytes.append(2)` 来补充双空终止，直接使用 StrW 对象即可。



### 4.2.2 StrW构造语义

| 构造方式 | 语义 |
|---------|------|
| `StrW()` | 空构造 |
| `StrW(L"hello")` | **引用**常量字符串（不拷贝！capacity_=0） |
| `StrW(charW* arr)` | **拷贝**构造（运行时计算长度） |
| `StrW(L"hello", 5)` | **拷贝**构造（指定长度） |
| `StrW(const StrW& s)` | **拷贝**构造 |
| `StrW(StrW&& s)` | **移动**构造 |
| `explicit StrW(size_t len)` | 分配 len 字符（字节为 len*2）+空终止 |
| `StrW(charW c, size_t len)` | 分配 len 字符并用 c 填充 |
> **⚠️ 单参 `const charW*` 构造是引用**：`StrW s = L"hello"` 是引用，如果之后对 `s` 做 `+=` 等修改操作，内部会自动切换到拷贝模式（因为 `append` 检测 `capacity_==0` 时会先备份再 malloc）。
>
> **`charW*` 指针构造是拷贝**：传入指针时走拷贝构造，会运行时计算长度（`strlen<W>`），这避免了编译时数组退化的问题。

### 4.2.3 StrW赋值与拼接（同StrA）

### 4.2.4 StrW 隐式转换与指针使用（同StrA）

### 4.2.5 StrW 字符访问（同StrA）



------



## 5. StrU8 — 平台无关的 UTF-8类型

`StrU8` 继承自 `StrA`，尽管在“白易框架”约定下，**A 版字面量即为 U8 版**（无论 Win32 还是 Linux），

而**`StrU8`**：表达显式的 "UTF-8 字符集" 语义（内含对C++20的 `char8_t*` 字面量u8"..."支持、专属的U8版字符长度统计与索引访问），它与 A/W 的平台参考系无关。它的定位是**跨与平台交换层**（网络报文、HTTP 报文体、配置文件等以 UTF-8 为编码约定的场景），同时也提供 `lenU8()` 等专属字符索引方法。



------

### 5.1 StrU8 专属方法实现

#### C++20 u8"" 字面量支持

`StrU8` 专门支持 C++20 的 `u8""` 字面量类型（`const char8_t*`），解决了 C++20 起 `u8"..."` 类型不再隐式转到 `const char*` 的问题：

```cpp
#ifdef __cpp_char8_t
//C++20起 u8"..." 类型为 const char8_t*，不再隐式转到 const char*，需显式接管
StrU8(const char8_t* s) : StrA((const char*)s) {} //单参为引用，其余为拷贝构造
template<int N> StrU8(const char8_t(&arr)[N]) { _reset((const char*)arr, N); }
StrU8& operator=(const char8_t* s) { return operator=((const char*)s); }
StrU8& operator+=(const char8_t* s) { return operator+=((const char*)s); }
#endif
```

> **优势**：在 C++20 环境下可以直接使用 `StrU8 str = u8"中文测试";` 而无需手动类型转换。

#### 字符计数与访问

```cpp
/**快速计算UTF-8字符串的字符数
 * @return 注意: 若存在非法U8字符的情况下该值不准确
 */
Bytes::Ref atU8(ssize_t i) const;
```

#### 使用建议
1. **字符级操作优先使用 `lenU8()` 和 `atU8()`**：这些方法提供了 UTF-8 字符语义的正确处理（返回的是 BR 视图，指向 1-4 字节的字符序列）。
2. **性能考虑**：二者均是线性时间复杂度。
3. **字节级操作可继承 `StrA` 方法**：如需字节级处理（如查找特定 ASCII 分隔符），可直接使用继承自 `StrA` 的方法。

### 5.2 跨平台转码与 U8ToPlat

```
StrPlat U8ToPlat(const StrU8& str);   // U8 -> 平台原生（Win32:W, Linux:A）
StrU8   PlatToU8(const StrPlat& str); // 平台原生 -> U8

跨平台交换层（StrU8）   →  U8ToPlat / PlatToU8  →   系统调用层（StrPlat / StrW / StrA）
```

> **在 Linux 上**：`StrA` 与 `StrU8` 完全等价，无需任何转换；
> **在 Windows 上**：Win32 API 因接受 `StrW`，需经 `U8ToPlat`（即 `U8toW`）转换后才能传入系统调用。

👉 **若打算做跨平台的设计，详情：[文本扩展函数开发规范.html](./文本扩展函数开发规范.html) 。**



---

## 6. AutoStr — 统一自动转换类与UTF-8优先设计

**这是本库最核心的设计之一。** `AutoStr` 采用**UTF-8优先**的架构，能从几乎任何类型自动构造字符串，并智能缓存W版：

> 💡 **UTF-8为首要字符集**：Win32通过 `_BE_CHARSETUTF8` 宏（BECore.cpp中已定义），确立UTF-8作为核心编码。A版在编译层面等价于UTF-8，实现跨平台一致性。
> 
> 💡 **智能缓存机制**：内部以StrU8为主存储，需要W版时通过指针比较实现按需转换和缓存。**约定内部str不可变**，确保缓存指针比较的准确性和后续零开销。
> 
> 💡 **AutoStr 统一接口**：全局别名 `using c_AutoStr = const AutoStr&;` 提供统一的简短自动化字符串参数类型，所有API均接收AutoStr常引用。

```cpp
class AutoStr {
public:
    const StrU8 str;     // 主存储：UTF-8字符串
    size_t len() const;  // 文本长度


    // 支持的构造来源
    AutoStr(int / float / double / ...); // 数值 → UTF-8
    AutoStr(const StrU8& v);             // UTF-8 直拷
    AutoStr(const charW* v);             // W → UTF-8
    AutoStr(const StrW& v);              // W → UTF-8
    AutoStr(const char* v);              // A(U8) → UTF-8
    AutoStr(const Bytes& v);             // 字节集可视化预览

    // 隐式转换 (万能输出通道)
    operator const char*() const;        // 内部直接返回 str
    operator const StrU8&() const;
    operator const StrW&() const;        // 按需 W 缓存
    operator const charW*() const;       // 按需 W 缓存
    
    // 非常量指针隐式转换 (白易0.6+ 特性，支持 Win32 API 直接传参)
    operator char*() const;
    operator charW*() const;

    // C++20 支持
#ifdef __cpp_char8_t
    operator const char8_t*() const { return (const char8_t*)str.bytes.buf; }
    AutoStr(const char8_t* v);
#endif

    // 流式拼接 (修改自身)
    template<typename T> AutoStr& operator<<(const T& v);
};

// 自动拼接全局运算符
inline AutoStr operator+(const c_AutoStr& a, const AutoStr& b);

using c_AutoStr = const AutoStr&; // 极其重要的别名，通用 API 接收此参数。
```

### 6.1 任意类型的格式化扩展支持（白易 0.6+ 新架构）

有关自定义结构体、第三方系统结构体的格式化扩展方法与使用演示，请参见：[12.4 任意类型的格式化扩展支持（白易 0.6+ 新架构）](#124-任意类型的格式化扩展支持白易-06-新架构)。

### 6.2 通用文本编码传参类型 (StrX，白易 0.6+ 新架构)

在设计高级文件处理或系统扩展 API 时，如果每次都要按“1+1+1”原则手写 A 版和 W 版双接口，不仅繁琐，还会导致头文件严重膨胀。
`StrX`（常引用别名为 `c_StrX`）就是为了解决这个痛点而诞生的**“双接口折中替代方案”**。它继承自 `AutoStr`，但**完全剔除了万能模板构造函数**。

- **核心作用**：它既能享受 `AutoStr` 带来的跨编码（窄/宽字符/UTF-8）全自动转换、局部零拷贝的便利，又能在**编译期严格限制**调用方只能传入文本相关的类型（`StrA`、`StrW`、`const char*`、`const charW*` 等）。这防止了像 `c_AutoStr` 那样来者不拒，把 `RECT` 或数组也照单全收导致的类型安全问题。
- **设计指引**：在文本/系统扩展函数 API 设计中，**但凡涉及文件路径相关，或者是不影响性能的高级功能函数参数，均推荐使用 `StrX` / `c_StrX` 进行参数设计**。
- **极简接口**：从此，你的 API 只需要定义一个 `c_StrX` 版本的函数，就能完美兼容调用方的任何文本输入，内部按需转为所需编码。
- **默认参数支持**：支持安全地使用空字符串作为默认参数，如 `c_StrX 目录名 = ""`。
- **混合编码拼接支持**：重载了 `inline StrX operator+(c_StrX a, c_StrX b)`，支持不同编码/类型的文本直接使用 `+` 拼接。例如，可以直接编写 `"让W版字符串" + strw + "可直接拼接A版字符串"`，同理，A版字符串也可以直接这样混合拼接，框架会自动处理转码与合并。
  > [!NOTE]
  > **重载优先级与隐式转换**：依据 C++ 的重载决策规则，精确匹配的优先级最高。对于已有完全类型匹配的拼接（例如 `StrW + StrW`、`StrA + StrA`、`StrA + const char*` 等），编译器会优先匹配原生的精确重载，而不会走此重载；只有在没有精确匹配的混合拼接场景下，隐式转换机制（隐式转换总是次之）才会生效，匹配到 `StrX` 进行拼接。

#### 💡 典型应用场景：路径传递

```cpp
// 严格要求传入文本类型，传入其他结构体会直接引发编译错误
StrX 取临时文件名(c_StrX 目录名 = "") {
	const charW* dir = 目录名; // 直接隐式强转出符合 Win32 API 要求的平台指针
	charW tempPath[MAX_PATH] = { 0 };

	if (!dir[0]) {
		GetTempPathW(MAX_PATH, tempPath);
		dir = tempPath;
	}

	charW tempFile[MAX_PATH] = { 0 };
	if (GetTempFileNameW(dir, L"TMP", 0, tempFile)) {
		return StrW(tempFile); // 也可以直接返回并隐式向上转换为 StrX
	}
	return ""; // 返回空字符串也完全合法
}
```

---

## 7. 全局工具函数与编码转换

### 7.1 格式化 (sprintF)

```cpp
StrA sprintF(const char* format, ...);    // 公开接口，返回 UTF-8 (StrA)
StrW sprintF<W>(const charW* format, ...); // 半公开接口，返回 UTF-16 (StrW)
```

### 7.2 数值 ↔ 字符串

```cpp
// 1. 数值转字符串 (ToStr 系列)
StrA ToStr(int / uint / int64 / uint64 / float / double); // 转 UTF-8
StrW ToStr<W>(int / ...);                                 // 转 UTF-16
StrPlat ToStrPlat(const T& a);  // 平台版（Win32:W, Linux:A）

// 2. 字符串转数值 (ToInt 系列)
int    ToInt(const char* / const charW*);
int64  ToLong(const char* / const charW*);
double ToDouble(const char* / const charW*);
// ... short, float 同理
```

### 7.3 字节集可视化 (jzjj)

用于调试输出，将 `Bytes` 内容展开为十进制或十六进制：

```cpp
StrA jzjj(const Bytes& bin, bool endWithNewline = false); // 输出 "Bytes:3{1,2,3}"
StrW jzjj<W>(const Bytes& bin, ...);

StrA jzjj16(const Bytes& bin, ...); // 输出 "{01 02 FF}"
StrW jzjj16<W>(const Bytes& bin, ...);
```

### 7.4 任意类型转 Bytes (ToBytes / ArrayBytes)

```cpp
template<typename T> Bytes ToBytes(const T& a); // 拷贝 POD 内存
Bytes ToBytes(const char* a); // 给 char / charW 提供了直接拷贝版（不含 \0）
Bytes ToBytes(const StrA& / StrW&); // 拷贝字符串内部 bytes

// 从字面量数组全引构建
template <typename T, int N> Bytes ArrayBytes(T(&arr)[N]);
```

### 7.5 字符编码转换

#### Win32 版（基于 MultiByteToWideChar/WideCharToMultiByte）

```cpp
StrW GBKtoW(const StrA& str);    // GBK → UTF-16
StrA WtoGBK(const StrW& wstr);   // UTF-16 → GBK
StrU8 WtoU8(const StrW& wstr);   // UTF-16 → UTF-8
StrW  U8toW(const StrU8& str);   // UTF-8 → UTF-16
StrU8 GBKtoU8(const StrA& str);  // GBK → UTF-8
StrA  U8toGBK(const StrU8& str); // UTF-8 → GBK
```

#### Linux 版（基于 iconv）

与Win32版具有相同的函数签名，内部使用 `iconv_convert()` 实现。

> ⚠️ **Linux 上的 `AtoU8` / `U8toA` 别名**：Linux 平台默认认为 "A 版" 即 UTF-8，故 `AtoU8` / `U8toA` 宏直接返回原字符串（无需转换），相关宏在 Linux 中定义为直接返回。

#### 转码工具函数别名 (UTF-8优先设计)

由于约定了 **A版字面量 == U8版**，框架内部的转码宏已重定向：

```cpp
// 当定义了 _BE_CHARSETUTF8 宏时（BECore.h已定义）
#define AtoW  U8toW    // A(U8) -> UTF-16
#define WtoA  WtoU8    // UTF-16 -> A(U8)
#define AtoU8(str) str  // A -> U8（直接返回，等价）
#define U8toA(str) str  // U8 -> A（直接返回，等价）

// 历史兼容（Win32且未定义_BE_CHARSETUTF8时）
#define AtoW  GBKtoW    // GBK -> UTF-16
#define WtoA  WtoGBK    // UTF-16 -> GBK
#define AtoU8 GBKtoU8   // GBK -> UTF-8
#define U8toA U8toGBK   // UTF-8 -> GBK
```

> **历史设计与设计哲学**：
> 传统的 `GBKtoW`、`WtoGBK` 等函数依然保留，但在框架开发中应优先使用上述宏以保持跨平台一致性。
> 白易框架通过 `_BE_CHARSETUTF8` 宏确立UTF-8为首要字符集，不再随系统语言 `CP_ACP` 变化，实现A版全平台编码语义的高度统一。

---

## 8. Arraybe 类 — 静态栈/动态堆数组

```cpp
template<typename T_ELE, int STACK_CAP = 0>
class Arraybe {
public:
    T_ELE* pArr;   // 容器指针
    int    count;  // 当前元素个数
};
```

### 8.1 栈预分配 (STACK_CAP)

当 `STACK_CAP > 0` 时，Arraybe 在栈上预分配一块 `sizeof(T_ELE) * STACK_CAP` 的缓冲区。如果元素个数不超过 `STACK_CAP`，所有数据都在栈上，**零堆分配**。超过后自动切换到堆。

```cpp
Arraybe<BR, 4> arr;            // 栈上预留4个 BR 的空间
Split(BR("1,2,3"), BR(","), arr);  // 3个元素，全部在栈上
```

### 8.2 构造

```cpp
Arraybe(int initCount = 0, int capacity = 0, int increment = 5);
Arraybe(std::initializer_list<T_ELE> arr, int increment = 5);
Arraybe(const Arraybe& arr);     // 深拷贝
Arraybe(Arraybe&& arr);          // 浅拷贝+转移所有权
```

### 8.3 主要方法

```cpp
void push(const T_ELE& elem);      // 拷贝添加
void push(T_ELE&& elem);           // 移动添加
bool pop(T_ELE& elem);             // 取出并删除最后一个
bool pop();                        // 仅删除最后一个
T_ELE& top();                      // 取最后一个（空时返回静态空元素）
bool del(int index, int n = 1);    // 删除指定位置
bool insert(int index, const T_ELE& elem);
void reset(int n = 0, bool retainContent = false, bool shrinkCapacity = false);
void clear();                      // 等价 reset(0)
int  find(const T_ELE& elem);      // 线性查找，返回 -1 or 索引
```

### 8.4 容量控制

```cpp
int  _capacity();                  // 获取当前容量
void _capacity(int newCapacity);   // 设定到目标容量(扩或缩)
void _shrink();                    // 收缩到 count
void _hookExpand(FnHookCap fn);    // hook 扩容时机
void _hookDelete(FnHookCap fn);    // hook 删除时机（可实现自动缩容策略）
```

### 8.5 Range-for 支持

```cpp
Arraybe<StrA> arr;
for (auto& item : arr) {
    // ...
}
```

### 8.6 自动输出

`Arraybe<T>` 有 `operator AutoStr()`，只要 `T` 定义了 `operator AutoStr()`，就能直接 `print(arr)`：

```
Array:3{Point{1,2},Point{3,4},Point{5,6}}
```

对 StrA/StrW 元素会自动加引号：`Array:3{"a","b","c"}`

### 8.7 ArrayBytes工具函数

```cpp
/**从任意字面量数组构造字节集引用**/
template <typename T, int N>
inline Bytes ArrayBytes(T(&arr)[N])
{
	return Bytes((void*)arr, BRef, sizeof(T)*N);
}

/**从任意Arraybe数组构造字节集引用**/
template<typename T_ELE, int STACK_CAP>
inline Bytes ArrayBytes(const Arraybe<T_ELE, STACK_CAP>& arr)
{
	return Bytes(arr.pArr, BRef, arr.count*sizeof(T_ELE));
}
```
**返回对 pArr 内存的引用（不拷贝）。**

> ⚠️ **类型限制警告**
> 提取 `Arraybe` 连续内存的原始字节形式，**仅在元素为基础算术类型（如 `int, byte, float` 等）或简单 POD 结构体时才有意义**，通常用于快速写入文件或网络封包下发。
> 如果你的 `Arraybe` 里装的是 `StrW`、`Bytes` 甚至带有虚函数的复杂对象，提取出来的这段连续字节里全是它们内部的"动态内存指针（地址值）"，这对于网络传输、序列化是完全无效的废物数据（俗称浅拷贝残骸）。必须明确区分"物理内存序列"与"逻辑数据序列"。

---

### 8.8 数组元素类型约定与底层设计思想

基于手动内存管理与底层 `realloc` 的架构考量，`Arraybe` 内部有着高级的内存搬运机制。关于 `push` 的无分配移动语义优化，以及针对元素要求类型属于 `trivially relocatable`（即 POD-like 约定）深度展开，**为了篇幅和逻辑连贯，已统一移步至第 13 章专题讲解内存管理的重点讲述，请参阅 👉 [13.5 数组元素类型约定与底层设计思想](#135-数组元素类型约定与底层设计思想)**。

---



## 9. Split 与 SplitV — 分割函数

### 9.1 Split — 分割函数

```cpp
template<typename T, typename T_STR, typename T_SPLIT, int STACK_CAP>
Arraybe<T, STACK_CAP>& Split(
    const T_STR& str,
    const T_SPLIT& split,
    Arraybe<T, STACK_CAP>& arr,
    bool arrKeepCapacity = false
);
```

#### 支持的类型组合

| T | str 类型 | split 类型 | 说明 |
|---|---------|-----------|------|
| `StrA` | `StrA` / `const char*` | `StrA` / `const char*` | 窄字符串分割 |
| `StrW` | `StrW` / `const charW*` | `StrW` / `const charW*` | 宽字符串分割 |
| `Bytes` | `Bytes` | `BR` / `Bytes` | 字节集分割（**拷贝**模式） |
| `BR` | `Bytes` | `BR` / `Bytes` | 字节集分割（**引用**模式，零拷贝） |

#### 示例

```cpp
// 字符串分割
Arraybe<StrA> arr;
Split<StrA>("a,b,c", ",", arr);
// arr: {"a", "b", "c"}

// 字节集分割（拷贝）—— 修改子分片不影响原数据
Arraybe<Bytes> arr;
Split(src, BR("-"), arr);

// 字节集分割（引用）—— 子分片引用原数据，零拷贝
Arraybe<BR> arr;
Split(src, BR("-"), arr);
```

### 9.2 SplitV — 惰性分割视图

**不创建数组**，在 range-for 中按需产生每个分片（BR 引用），可随时 `break`：

```cpp
template<typename T>
_SplitView<T> SplitV(const T& str, const T& split);
```

#### 示例

```cpp
// 惰性遍历，找到第3个后 break
StrA src = "a:bb:ccc:dd:eeeeeee";
int i = 0;
for (BR v : SplitV<StrA>(src, ":")) {
    printf("分片%d: ...\n", i++);
    if (i == 3) break;  // 只处理前3个
}
```

> **注意**：`SplitV` 产生的 `BR` 不含空终止符。如需打印，要么手动加 `\0`（需确保源字符串可写），要么用 `ToStr(v)` 构造新字符串。

---

## 10. HashTbe 类 — 哈希表 (K:V)

`HashTbe<K, V>` 是一个高性能的哈希映射容器，其核心设计参考了 **Python 3 (CPython)** 的字典实现方案：采用 **FNV-1a 哈希算法** 与 **开放寻址（带冲突扰动链）** 策略实现。

> **💡 底层原理**：它使用的是 Python 风格的线性同余探测公式：`index = (index * 5 + 1 + perturb) & mask`。这种“扰动链”机制能有效利用哈希值的高位比特，在发生碰撞时能极快地跳跃到其他槽位，显著降低了长探测链产生的风险。

### 10.1 核心特性

- **高性能映射**：支持 O(1) 平均复杂度的查找、插入与删除。
- **开放寻址法**：内部使用 `State { EMPTY, OCCUPIED, DELETED }` 标记节点状态，不产生额外的链表指针开销。
- **自动扩容**：当装载因子达到阈值（约 0.66）时，自动触发 `rehash` 翻倍扩容并重新排列元素。
- **智能哈希适配**：内置对基础数值、`char*`、`StrA`、`StrW`、`StrU8` 以及 POD 类型的哈希计算支持。

### 10.2 构造与读写

```cpp
HashTbe<StrA, int> map;

// 1. [] 运算符（最常用）：读写一体，若键不存在则创建默认值
map["Apple"] = 10;
int val = map["Apple"];

// 2. set 方法
map.set("Orange", 20);

// 3. find 方法：返回指针，若不存在返回 nullptr
if (int* p = map.find("Apple")) {
    // *p = ...
}

// 4. del 方法：返回是否删除成功
map.del("Apple");
```

### 10.3 自动输出支持

只要 `K` 和 `V` 都支持 `AutoStr` 输出，`HashTbe` 即可直接参与打印：

```cpp
HashTbe<int, StrA> idNames;
idNames[1] = "Alice";
idNames[2] = "Bob";

be::print(idNames); // 输出：{1:Alice, 2:Bob}
```

### 10.4 遍历 (Range-for)

`HashTbe` 支持标准的 `Iterator` 遍历，可以使用 C++11 的 `range-for` 语法。遍历时返回的是 `Node&`，通过该引用可访问 `key` 和 `value` 成员：

```cpp
HashTbe<StrA, int> scores;
scores["Alice"] = 95;
scores["Bob"] = 80;

// 遍历返回 Node&，包含 key, value, state
for (auto& node : scores) {
    be::print("Name:", node.key, "Age:", node.value);
}
```

> **注意**：遍历器会自动过滤掉内部的 `EMPTY`（空）和 `DELETED`（墓碑）状态，仅输出有效数据。

### 10.5 自定义 Hash 计算 (针对自定义结构体)

`HashTbe` 默认对基础数值、字符串类型及 POD 结构体提供了内置支持。如果键（Key）是复杂的自定义结构体，或者你需要基于特定字段进行哈希，可以通过在 `namespace be` 下重载 `_hash_i_calc` 来实现：

```cpp
struct UserKey {
    int id;
    StrA group;

    // 1. 必须重载 operator== 供哈希表进行冲突判定
    bool operator==(const UserKey& other) const {
        return id == other.id && group == other.group;
    }
};

// 2. 在 be 命名空间下重载哈希计算函数
namespace be {
    inline int _hash_i_calc(const UserKey& key, uint mask, uint* firstHash = nullptr) {
        // 使用内置的 fHash_FNV1a 进行组合计算
        uint h = (uint)key.id;
        h = fHash_FNV1a(key.group._buf(), key.group.bytes.size); 
        
        // 必须回写原始 hash 值以供内部扰动探测链使用
        if (firstHash) *firstHash = h;

        // 返回掩码运算后的索引位置
        return h & mask;
    }
}
```

> **💡 典型应用**：`Bytes`（字节集）作为键时的哈希计算就是通过这种方式实现的，它确保了哈希值取决于字节集的内容，而非指针地址。

---

## 11. NilOpt — 可空类型

`NilOpt<T>`（中文别名 `可空<T>`）是白易核心库提供的轻量级“可空”可选值容器（类似于 C++17 中的 `std::optional`）。常用于指示某个参数或返回值是否有效、未提供或为空。空值状态由全局常量 `nil`（中文别名 `空`，类型为 `Nil` / `空型`）表示。

### 11.1 核心设计与偏特化

`NilOpt` 针对不同的传值和传引用场景，提供了三套精妙的特化实现：

#### 1. 基础传值版本 `NilOpt<T>`
内部存储 `T val` 和布尔标记 `has`：
```cpp
template<typename T>
class NilOpt {
public:
    T val; 
    bool has;
    
    NilOpt() : has(false) {}
    NilOpt(Nil) : has(false) {}
    NilOpt(const T& v) : val(v), has(true) {}
    
    // 针对窄/宽/UTF-8 字符指针的特例构造（如果是 nullptr 则置为空状态）
    NilOpt(const char* s) : has(false) {
        if constexpr (be::sameT<T, StrA> || be::sameT<T, StrU8> || be::sameT<T, BR>) {
            has = (s != nullptr); if (has) val = (T)s;
        }
    }
    NilOpt(const charW* s) : has(false) {
        if constexpr (be::sameT<T, StrW>) {
            has = (s != nullptr); if (has) val = s;
        }
    }
    
    operator T&() { return val; }
    operator const T&() const { return val; }
    bool operator ==(Nil) const { return !has; }
    bool operator !=(Nil) const { return has; }
    NilOpt& operator=(const T& v) { has = true; val = v; return *this; }
    void SET(const T& v, bool has_) { has = has_, val = v; }
    const T& OR(const T& default_value) const { return has ? val : default_value; }
    T* operator&() { return &val; }
};
```

#### 2. 传引用版本 `NilOpt<T&>`
内部仅存储一个指针 `T* p`。当它被初始化为 `nil` 时，指针指向 `nullptr`。主要应用于**函数的可选 out 出参**。当调用者不需要接收该出参数据时，可直接传入 `nil`，从而免去外部声明无用临时变量的麻烦，实现按需输出。
```cpp
template<typename T>
class NilOpt<T&> {
public:
    T* p;
    NilOpt() : p(nullptr) {}
    NilOpt(Nil) : p(nullptr) {}
    NilOpt(T& v) : p(&v) {}
    
    operator T&() const { return *p; }
    bool operator==(Nil) const { return p == nullptr; }
    bool operator!=(Nil) const { return p != nullptr; }
    T& OR(T& default_value) { return p ? *p : default_value; }
    T* operator&() { return p; }
    NilOpt& operator=(const T& value) {
        if (p) { *p = value; } else { p = (T*)&value; }
        return *this;
    }
};
```

#### 3. 传常量引用版本 `NilOpt<const T&>` (双态持有特化版)
主要服务于 `c_StrU8`/`c_StrA`/`c_StrW` 等及其 `const`字符串指针引用别名的传参。采用**双态持有（Dual-state holder）**的设计：
- **如果是实参传值**（如已有的 `const StrA&` 实体对象），内部**仅保存其指针**，从机制上彻底实现**零拷贝、零内存分配**；
- **如果是字符串指针构造**（仅 `const char* / const charW*`，适用于 `T` 能够接收其赋值的类型），则利用内部 `val` 进行存储并绑定，以支持直接转换到 `StrA` 等实体容器类型进行存储。
  > 💡 **注**：对于 `const` 字符串指针，`StrA` / `StrW` 等实体容器的单参构造函数本身就是**赋引用构造**（直接绑定指针，`capacity_ = 0`），因此即使利用 `val` 进行了转换，底层也完全不会发生内存拷贝和堆分配开销。


##### 临时值（右值）传递与生命周期安全性
当 `NilOpt<const T&>` 用于函数形参时，**它可以安全地接收临时对象（右值，例如函数返回的 `StrA` 临时串），而不需要专门去写右值引用（`T&&`）重载进行移动拷贝**。原因如下：
1. **C++ 临时对象生命周期规则**：当一个临时对象作为实参被传递给函数参数时，它的生命周期将延伸到包含该函数调用的“完整表达式（Full-expression）”结束。也就是说，该临时对象只会在函数执行完毕并返回后（分号处）才被析构。
2. **形参的安全引用**：只要 `NilOpt` 作为形参且只在函数执行期间被读取，它内部存下的临时对象地址 `p = &v` 就是 100% 有效的安全地址，绝不会发生提前析构和悬空。

##### 完整版实现代码：
```cpp
template<typename T>
class NilOpt<const T&> {
public:
	const T* p;   // 指向实际数据的指针（永远不为 nullptr，默认指向 val）
	T val;        // 仅当传入临时变量或字面量时用作内部存储
	bool has;

	NilOpt() : p(&val), has(false) {}
	NilOpt(Nil) : p(&val), has(false) {}
	
	// 1. 传入已有的常量引用：零拷贝绑定指针
	NilOpt(const T& v) : p(&v), has(true) {}

	// 2. 传入字面量指针：利用内部 val 延长生命周期，并重定向 p
	NilOpt(const char* s) : p(&val) {
		has = (s != nullptr); if (has) val = s;
	}
	NilOpt(const charW* s) : p(&val) {
		has = (s != nullptr); if (has) val = s;
	}

	// 3. 自定义拷贝与移动构造/赋值：防止 p 在对象转移后仍指向旧对象的 val 产生悬空
	NilOpt(const NilOpt& other) : val(other.val), has(other.has) {
		p = (other.p == &other.val) ? &val : other.p;
	}
	NilOpt(NilOpt&& other) noexcept : val((T&&)other.val), has(other.has) {
		p = (other.p == &other.val) ? &val : other.p;
		other.has = false;
		other.p = &other.val;
	}

	operator const T&() const { return *p; }
	bool operator ==(Nil) const { return !has; }
	bool operator !=(Nil) const { return has; }
	NilOpt& operator=(const T& v) { p = &v; has = true; return *this; }
	
	NilOpt& operator=(const NilOpt& other) {
		if (this != &other) {
			val = other.val;
			has = other.has;
			p = (other.p == &other.val) ? &val : other.p;
		}
		return *this;
	}
	NilOpt& operator=(NilOpt&& other) noexcept {
		if (this != &other) {
			val = (T&&)other.val;
			has = other.has;
			p = (other.p == &other.val) ? &val : other.p;
			other.has = false;
			other.p = &other.val;
		}
		return *this;
	}

	const T& OR(const T& default_value) const { return has ? *p : default_value; }
	const T* operator&() const { return p; }
	const T* operator&() { return p; }
};
```

### 11.2 核心操作与使用示例

- **状态判断**：直接与全局的 `nil`（或者在中文核心下的别名 `空`）进行 `==` 或 `!=` 比较，或者检查它的状态（如 `p != nullptr` 或 `has` 标记）。
- **隐式转换与降级取值**：支持直接隐式转换回 `T&` 或 `const T&`；或通过 `.OR(defaultValue)` 提供降级兜底方案。
- **获取指针**：重载了 `operator&()`，可直接通过 `&opt` 取出内部承载的值或实体的指针。

```cpp
// 1. 基础传值与默认值示例
NilOpt<int> port = nil; 
if (port == nil) {
    int actualPort = port.OR(8080); // 结果为 8080
}

port = 443;
if (port != nil) {
    int actualPort = port;          // 隐式转换为 int，结果为 443
}

// 2. 指针判空隐式构造（安全防崩溃）
const char* rawPath = nullptr;
NilOpt<StrA> path = rawPath;       // has=false, 内部不会触发 StrA 构造
if (path == nil) {
    be::print("路径为空！");
}


// 3. 传引用特化（可选 out 出参演示）
bool tryParseInt(const StrA& text, int& result, NilOpt<StrA&> outError = nil) {
    if (text == "invalid") {
        if (outError != nil) {
            // 直接赋值会隐式修改传入的外部变量
            outError = "格式非法，无法解析为整数"; 
        }
        return false;
    }
    result = 123; // 假设解析成功
    return true;
}
void callParse() {
    int val = 0;
    StrA errMsg;

    // (A) 需要获取错误信息：传入接收变量 errMsg。解析失败时，内部会直接写入 errMsg
    if (!tryParseInt("invalid", val, errMsg)) {
        be::print("错误详情:", errMsg);
    }

    // (B) 忽略错误信息：直接传入 nil。函数内部检测到空后不执行写操作，也省去了外部声明无用变量的开销
    if (!tryParseInt("123", val, nil)) {
        be::print("转换成功，值为:", val);
    }
}

// 4. 传常量引用特化（函数定义与传参演示）
void writeLog(const StrA& message, NilOpt<const StrA&> tag = nil) {
    if (tag != nil) {
        // tag 可直接隐式转换为 const StrA& 进行常规操作
        be::print("[" + (const StrA&)tag + "] " + message);
    } else {
        be::print(message);
    }
}

void callLogs() {
    StrA myTag = "SYSTEM";

    // (A) 传入已有的 StrA 实体对象：
    //     由于是 NilOpt(const T&)，双态持有在此状态下仅保存 &myTag 指针（零拷贝，零分配）
    writeLog("服务启动成功", myTag);

    // (B) 传入 const char* 字符串字面量：
    //     直接调用 NilOpt(const char*) 隐式转换构造，将 StrA 的引用绑定至内部 val 存储区中。
    //     （注：由于 StrA 自身的单参构造即为引用构造，因此该转换依然完全是零拷贝的）
    writeLog("连接数据库失败", "ERROR");

    // (C) 传入 nil / 空：
    //     未传递可选参数
    writeLog("普通通知信息", nil);
}
```

### 中文化别名

```cpp
// 在中文核心.hpp 中定义
template<typename T> using 可空 = NilOpt<T>;
using 空型 = Nil;
#define 空 nil
```




### 为什么 NilOpt 没有被放进 be:: 命名空间？

从传统 C++ 库的开发规范来看，诸如 `NilOpt`、`nil` 这样名字极短的修饰符非常容易被诸如 OS X 等其他环境库所污染，通常的解法是把它们包裹进特定的命名空间中（如 `be::NilOpt`）。

白易不仅仅是一个普通工具库，更是**一套意向在 C++ 顶层搭建起的底层原语面向新易语言的系统**（旨在平替 C++ 繁冗的 STL，拥抱极简编程体验）。在这个视界中，`Bytes`、`StrW`、`Arraybe` 乃至 `NilOpt` 这类容器修饰类，都被视作如同 `int`、`float` 一样自然的基础内置类型（作者我愿称作二级基础数据类型或二等公民类型）。

因此，它们理应享受到 **“全局声明免包装”** 的次高待遇。试想，如果每次声明个空传参都要写成 `be::NilOpt<int>`，无疑会破坏掉这种如原生语言般丝滑干练的编码体验，且在 IDE 上看到的参数鼠标悬停提示或代码补全建议也会是一大长串扎眼的命名空间前缀。

相对的，**对于存在巨大泛滥重名风险、或者是偏向具体功能执行性质的动作函数或系统**（比如即将在下一章登场的万能输出体系 `be::print` 和 `be::cout`），才会被真正严格且自觉收拢到 `be::` 命名空间内部，以保证不和 C 语言默认拥挤的名称体系相冲突。

### 还因为IDE 智能悬停与辅助函数参数的注释级生成特例

`可空<T>`（或 `NilOpt<T>`）属于白易框架底层内置的特殊的标识类型。在配套的 IDE 插件生态（例如鼠标悬停查看函数的参数提示时）中，**仅对此类型提供唯一的代码解析与生成特例渲染：它会在参数名后自动附带上 `<可空>` 标记，即生成的注释呈现为 `变量名<可空>`。**

而其他所有的常规开发类型，默认均按照去掉前端具体类型、保留 `变量名 [=是否有默认值] 用户注释` 的清爽格式进行提取和生成。

> **示例**：
> 如底层函数的注释所写 `@param 句柄<可空> 窗口句柄模态载入`
> 对应了形参 `可空<HWND> 句柄 = 空` 的完美映射解析。
> 这种专门拔高的 IDE 级特例待遇，能在极大简化用户阅读代码时的类型噪音的前提下，极其精准、不容忽视地向调用者传达出该参数“可选、不需要时可填空”的核心业务指引。



---

## 12. be::print / be::cout — 输出系统

定义在 `beout.hpp`，基于 `AutoStrW` 实现万能输出。

### 中文化别名

```cpp
// 在中文核心.h 中
#define 打印输出 be::print
#define 调试输出 be::debug
```

### 12.1 控制台输出 (be::print / be::cout)

对于屏幕控制台打印，白易提供了两种等效的风格供开发者选择：

```cpp
// 1. 变参格式化风格（无需手工加分隔符，自动换行）
be::print("Hello", 123, myPoint);
// 自动输出：Hello | 123 | Point{10,20}\n

// 2. 传统 C++ 流式风格
be::cout << "Hello" << " | " << 123 << " | " << myPoint << be::endl;
```

### 12.2 VS 调试窗口输出 (be::debug / be::dout)

同样的双风格也完全映射到了 Visual Studio 环境的“输出”调试面板中（对应 `OutputDebugStringW` 封装）：

```cpp
// 1. 变参格式化风格
be::debug("Debug:", 123, myPoint);

// 2. 传统 C++ 流式风格
be::dout << "Debug:" << myPoint << be::endl;
```

### 12.3 统一万能路由的基本工作原理（白易 0.6+ ）

在白易 0.6+ 版本中，输出系统的架构得到了前所未有的精简与统一。
所有通过 `be::print` 变参传递、或是通过 `be::cout <<` 流式输出的值，最终都只会汇聚到唯一的一个类型上：**`AutoStr`**。

#### 1. 隐式转换万能桥接
当你写下 `be::cout << pt` 时，编译器会自动查找如何将自定义类型 `Point` 输出。
因为 `be::cout` 只接受 `const AutoStr&`，编译器会触发 `AutoStr` 的万能模板构造函数：
`template <typename T> AutoStr(const T& v)`

#### 2. ADL 与 `__AutoStr__魔术方法`
在AutoStr这个构造函数内部有个万能类型构造，除了开头的模板约束声明外，只有极简的一行`__AutoStr__(v)`调用代码：

	template <typename T, typename = be::enable_ifT<!be::is_AutoStr_builtinT<T>>>
	AutoStr(const T& v) {
		(StrU8&)str = __AutoStr__(v).str;
	}
由于没有指定命名空间（原先这个是写进be::的），C++ 强大的 **ADL（参数依赖查找，Argument-Dependent Lookup）** 机制会生效。编译器会像雷达一样，全自动扫描全局命名空间以及类型 `T` 所在的命名空间（也能完美利用上类内部写成friend方法注入），寻找匹配的 `__AutoStr__` 重载，并将格式化后的数据无缝写入到当前的 `AutoStr` 实例中。

> **💡 命名空间与编译优化**：
> 如果用户有自己的命名空间（而后里边再定义自己的类时），**则完全可以自己把 `__AutoStr__(v)` 重载函数直接写在自定义结构体所在的命名空间中**（编译器会通过 ADL 机制更快自动识别到它，无需强行写在全局命名空间）。这使得代码更加模块化，避免了全局作用域的污染。

#### 3. 极简的“必要 SFINAE”保障安全

在白易 0.6+ 新架构中，虽然我们彻底废弃了类内 `operator AutoStr`，杜绝了绝大多数的二义性冲突，但为了防止 C++ 模板过分贪婪地匹配基础类型，底层依然保留了一个**极简且绝对必要的 SFINAE 约束**：
`!be::is_AutoStr_builtinT<T>`

这道约束作为白名单，限制万能模板构造函数**仅放行用户自定义的复合类型**，而安全排除所有常规基础类型（整型、浮点、指针、以及系统内置字符串类等）。

如果去掉这个必要的 SFINAE 约束，将会引发以下灾难性后果：
1. **整型提升（Numeric Promotion）被劫持**：
   当传入 `short` 等类型时，编译器为了实现完美匹配，会绕过内置的 `AutoStr(int)` 构造函数直接进入万能模板，这会导致其进入 `AutoStr(short) -> __AutoStr__(short) -> AutoStr(short)` 的无限递归循环，最终导致编译期报错（模板实例化深度超限）或运行期直接爆栈崩溃。
2. **非常量（non-const）拷贝构造被劫持**：
   当尝试拷贝一个非常量 `AutoStr` 对象时（如 `AutoStr a; AutoStr b(a);`），编译器会认为万能模板 `AutoStr(AutoStr&)` 比默认的拷贝构造函数 `AutoStr(const AutoStr&)` 匹配度更高，从而强行走万能模板，导致自我拷贝直接死锁。



### 12.4 任意类型的格式化扩展支持（白易 0.6+ ）

> **⚠️ 白易 0.6+ 核心变更（必读）**：
> 彻底**废弃并禁止**在自定义类内部书写 `operator AutoStr()` 隐式类型转换函数！如果残留此写法，编译器将报“二义性”或“找不到重载”等硬性错误。

现在，无论是**自定义结构体**还是第三方库的**内置结构体**（如 Win32 的 `RECT`），只要你想让它能够被自动化流式输出、打印，或者是隐式转入 `AutoStr` 进行参数传递，**做法极其简单统一**：
只需实现 `__AutoStr__` 重载函数即可，底层会利用 C++ 的 ADL（参数依赖查找）自动匹配。

> [!TIP]
> 无论是写在类外（同命名空间下，如“方式 A”），还是通过 `friend` 写在类内（如“方式 B”），**两者在机制上完全等同**，**起作用的本质是命名空间（C++编译器里类内部其实还不算正式的名称空间）**。`friend` 在这里的核心作用仅仅是作为语法糖，告诉编译器“请把这个非成员函数向外注册（注入）到该类所在的命名空间中”，两者最终都是依靠在所属命名空间内被 ADL 查找匹配的。（注意：不可使用 `static` 静态成员函数，因为 `static` 仍属于类作用域，ADL 机制不会去类内部查找）

```cpp
// 1. 对于你自己的类如果有命名空间的话下面做法会使得编译查找速度更佳：
namespace MyProject {
    // 【方式 A】外部同命名空间定义法：
    struct Point {
        int x, y;
    };
    // 编译器在解析 MyProject::Point 时，会通过 ADL 机制首选在 MyProject 命名空间内查找该函数
    StrX __AutoStr__(const Point& pt) {
        return sprintF("Point(%d, %d)", pt.x, pt.y);
    }

    // 【方式 B】类内友元定义法（与方式 A 机制完全等同）：
    struct Point2D {
        int x, y;
        
        // 声明为友元并直接在类内实现：它本质是一个非成员函数，会被自动向外注册到 MyProject 命名空间中
        friend AutoStr __AutoStr__(const Point2D& pt) {
            return sprintF("Point2D(%d, %d)", pt.x, pt.y);
        }
    };
}

// 2. 对于无特定命名空间或第三方的系统类（只能写在全局命名空间）：
StrX __AutoStr__(const RECT& r) {
    return sprintF("{ left:%d, top:%d, right:%d, bottom:%d }",
                   r.left, r.top, r.right, r.bottom);
}

// ======================== 使用演示 ========================

// 测试 1：直接作为参数隐式转换（自动触发万能模板）
MyProject::Point pt = { 1, 2 };
AutoStr str_pt = pt;

// 测试 2：直接输出
RECT rc = { 0, 0, 800, 600 };
be::print("当前窗口的坐标为：", rc);
be::cout << "点坐标为：" << pt << "\n";

// 测试 3：套娃式隐式支持！当你把它们塞进 Arraybe 或 HashTbe 时，它们也会被完美解析！
Arraybe<MyProject::Point> arr = { {1,2}, {3,4} };
be::print(arr); // 输出：Array:2{Point(1, 2),Point(3, 4)}
```

只要按如上方式提供全局 `__AutoStr__` 后，该类型立即获得以下终极能力：
- `be::print(obj)` — 控制台智能输出
- `be::cout << obj` — 流式输出
- `be::dout << obj` — VS 调试输出窗口打印
- 任何含有它的 `Arraybe<T>` 和 `HashTbe<K,V>` 的自适应层叠输出
- **作为函数参数** — 可无缝投入任何接收 `c_AutoStr`（即 `const AutoStr&`）的函数：

```cpp
int 信息框(c_AutoStr 内容, c_AutoStr 标题 = "提示：", int 类型 = MB_OK);

// 以下全部合法：
信息框("纯文本");           // char* → AutoStr
信息框(L"宽文本");          // charW* → AutoStr
信息框(pt);                 // 自定义结构体 Point → AutoStr
信息框(12345);              // int → AutoStr
```

## 13. 内存管理约定

### 13.1 引用 vs 拷贝

Bytes、StrA、StrW、StrU8 均具备引用和拷贝两套内存管理方案

| 操作 | 引用（capacity_=0） | 拷贝（capacity_>0） |
|------|---------------------|---------------------|
| Bytes构造自字面量 | `Bytes b = {1,2,3}` | `Bytes b(ptr, len)` |
| StrA 构造自单参指针 | `StrA s = "hello"` | `StrA s("hello", 5)` |
| 析构 | 不 free | free(buf) |
| 内容追加 | 自动malloc切换堆模式 | 容量足够 或 realloc扩容 |

#### 常量指针与常量数组引用构造的“零堆拷贝”与对 SSO 缺失的完美代偿

白易的一大核心内存设计是：**常量指针和常量数组引用在初次构造时，默认采用“引用构造”模式（引用绑定）**。
* **对于 `Bytes`**：`Bytes(const byte(&arr)[N])`（常量字节数组）和使用 `BRef` 标签的引用构造，均直接绑定外部内存，零堆分配，零数据拷贝。
* **对于 `StrA/W`**：传入常量字符串指针或字面量时（例如 `StrA s = "hello";`），**默认不分配任何堆内存，直接建立只读绑定（`capacity_ = 0`）**。

这一“零堆拷贝”的引用构造设计在很大程度上**完美代偿了没有 SSO（短字符串优化）的初次构造性能损失**。
C++ 标准库中 SSO 的核心目的是避免小字符串初次构造时昂贵的堆内存分配开销。在白易体系下，只要传入常量指针或字面量，初次构造便是**绝对的零堆分配、零内存拷贝**，开销极低（等同于两个指针大小 of 浅绑定）。只有当对象在后续被修改（如 `append`）时，才会自动触发 **COA（添时复制）** 切换为拥有模式（申请堆内存并拷贝）。这不仅在只读流转场景中性能更胜一筹，还保持了物理内存布局的绝对整洁统一。

### 13.2 移动语义

`Bytes` 对象移动后该源对象的 `capacity_` 被置0（变成引用模式），但 `buf` 和 `size` **未清零**。这是安全的，因为析构只检查 `capacity_`!=0才真正进行析构；

`Arraybe` 对象由于不存在引用方案，移动后该源对象的 pArr 被置为空指针，而其析构检查pArr；

`Arraybe.push` 也提供了移动语义重载，详细内容请参考 [13.5.2 push 的移动语义重载](#1352-push-的移动语义重载)。

### 13.3 Arraybe 的动态缩放机制

在长生命周期的业务（如常驻服务或重度缓存）中，标准容器（如 `std::vector`）扩容容易收缩难，甚至面临**内存“只吃不吐”**的痛点。很多时候，C++ 程序员不得不手工去搞一些诸如“跟空容器 `swap`”或显式调用 `shrink_to_fit()` 的丑陋 trick 来对峙极其死板的内建内存管家。

而白易的 `Arraybe` 在保持极简性能的同时，底层创新预置了一套极其优雅且**完全由用户高度自定义收放策略的“触发器式”内存自动缩放机制**。出于避免高频重分配产生性能抖动的最佳实践，这套基于函数钩子（Hook）的回调机制**默认不开启**。

如果你想精确掌管阵地的粮草水位，只需装配对应的 Hook 即可接管全局：

#### 1. 自定义收缩策略 (`_hookDelete`)

正如前文所提，在长生命周期中遭遇过数据洪峰而被撑大的动态数组，事后如果你删除了其中大量的闲置元素，物理内存默认是死霸不退的。此时你可以极为轻量地安装起手 `_hookDelete` 以接管防空载收缩大权：

```cpp
void 数组收缩接管演示() {
    Arraybe<int> arr(0, 32); 

    // 核心：基于 lambda 的高度自定义防“空载”策略
    arr._hookDelete([](Arraybe<int>& self){
        int cap = self._capacity(); // 获取当前被撑大的真实物理容量
        // 策略判定：如果总容量仍大于 10 的基础上，
        // 探测到实际存活的元素数连物理容量的一半都不到，
        // 则认为此时处于大量空闲气泡期，立刻触发紧缩动作
        if (cap > 10 && cap > self.count * 2){
            self._shrink(); // 底层同样调用极高效的 realloc 向操作系统回缴余闲内存
        }
    });

    for (int i = 0; i < 6; ++i) {
        arr.pop(); // 每次删除发生时，都会毫厘不差地评估上述收缩防洪策略！
    }
}
```

#### 2. 自定义扩容策略 (`_hookExpand`)

**Arraybe 默认自带克制化线性步进保障**：与专门用于乱拼接的字节级别容器（`Bytes / StrA/W`）采用了极其贪婪的按倍数激进增长算法不同（详见 [13.4 阶梯式倍数暴涨](#134-为什么-bytesstr-没有带有栈缓存的模板参数)），`Arraybe` 作为结构化组件（装载的往往是几十、上百字节的深层实体对象），其扩容天生默认维护着基于 `increment` 的克制化**线性增长策略**。这能确保即便是承载诸如窗口组件大列表等复杂业务模型时，也不会因为极其鲁莽的倍数乘法而动辄凭空吞噬掉几 MB 甚至十几 MB 的无关物理内存空泡。并在遇到真有批量的 `push` 需求时，同样可以使用带步长指定的重定义容量接口将其一次性喂饱。

但即便如此，如果你的该数组在极特殊的业务流中正处于一次性能狂暴期，需要抛弃线性的斯文做派，你完全可以装配 `_hookExpand` 直接接管扩容大权：

```cpp
// 自定义容量扩张策略函数（普通静态函数）
void CustomExpandHook(Arraybe<int>& self) {
    // 1. 制定你的野蛮策略（例如：原容量直接乘以 2 倍暴涨）
    int currentCap = self._capacity();
    int newCap = (currentCap == 0) ? 10 : (currentCap * 2); 

    // 2. 落实扩容方案（调用底层的 realloc）
    self._capacity(newCap);
}

void 数组扩容接管演示() {
    Arraybe<int> arr;
    arr._hookExpand(CustomExpandHook); // 装配干预钩子
    // 随后持续无脑 push 即可，全权由你的函数决定何时以何种规模暴涨！
}
```

> 🤔 **对照解惑：那为什么同为底层容器的 `Bytes`、`StrA/W` 等“二级基础类型”不提供基于 Hook 的收缩机制呢？**
>
> 1. **严苛的 ABI 锁死与内存膨胀惩罚**：如第 13.4 节所述，`Bytes` 和字符串是为了高频跨语言和充当 C 语言层级通讯协议而生的。如果在它们内部署一个像 `_hookDelete` 这样的函数指针甚至 Lambda 捕获块，**会直接生生把每个小小字符串实体撑大至少 8 到 32 个字节**，且彻底丧失纯内存平移透明度（跨语言 FFI 解析闭包简直是灾难）。
> 2. **生命周期维度的截然不同**：`Arraybe` 作为“重型容器大本营”，往往肩负着长生命周期的任务调度或持久缓存，起起落落间会产生极其可怕的闲置对象黑洞。而 `Bytes/Str` 则是不折不扣的“一次性快递盒（ ephemeral payload ）”：几乎都是短线中极其粗暴地“一路狂接狂拼 -> 用完 -> 销毁”。即使业务中途想要截取或截断其中的部分内容，也是极便宜地去更改一下 `count` 和打个 `\0` 即可搞定，在此情境下去为几十、几百个微末字节调用昂贵的 `realloc` 进行操作系统级别的碎内存缩倍缴回，简直是本末倒置的负优化。

---

### 13.4 为什么 Bytes/Str 没有带有栈缓存的模板参数？

与 `Arraybe` 提供模板级的栈区预分配容量（`STACK_CAP`）完全不同，`Bytes` 和 `StrA/W` 系列除了引用缓冲区设计外**绝无任何模板形式的内置小数据栈缓存（如常见 STL 中的 SSO 机制）**。除了严肃的**跨模块、跨进程乃至跨语言场景下的 ABI 兼容性考量**外，更重要的还**为了与 `Arraybe` 的底层哲学设计产生联动**：

- **绝对一致的二进制布局与无运行时虚表的极简内存模型**：如果字符串带上了类似 `<int StackSize>` 这样的模板参数来提供栈内存缓存，或者包含了不可预见的隐藏 SSO 分支机制，它均会导致类对象的 `sizeof` 大小和内部数据结构排布充满极大的不确定性。当它被作为基础的数据传递介质频繁穿越不同的 DLL/so 或者不同的编译器甚至不同的语言环境时，这是绝对灾难性的。不仅如此，`Bytes/Str` 系列在设计上**绝不包含任何虚函数（连抽象类继承都没有，直接杜绝了虚表指针 `vptr` 的存在）**。这种彻底打消 C++ 重型特性的设计，使得它们如同纯粹的 C 语言原生结构体那般，内存尺寸及布局模型永远统一的物理透明可控。
- **与 Arraybe 联动中的“自引用指针之坑”与“物理平移白名单”**：
  在 `Arraybe` 0.5+ 版本中，虽然已全面支持泛化类型构造搬移（**若支持 `noexcept` 移动构造则调用移动构造搬移，否则退化为拷贝构造搬移**），但为了压榨出极限性能，`Arraybe` 内部优先检测 **`be::is_trivially_relocatable`** 标记：
  * **可平凡重定位 (Trivially Relocatable)**：直接触发底层最激进的 `realloc` / `memmove` 纯字节内存平移，完全抛弃构造/析构步骤，速度极快。
  * **非平凡类型**：降级调用上述构造/移动构造搬移。
  
  试想 `std::string` 等常见 STL 类型的 SSO（短字符串优化）机制：它将短字符串直接“嵌”在对象内部空间的 `char buf[16]` 数组里，同时其内部用于管理的 `char* ptr` 还会向内指向这个对象自身的 `buf`（形成自引用）。**一旦将带有此类设计（自引用指针）的容器丢进 `Arraybe`，在数组扩容发生内存绝对位置的平移后，如果使用最快的纯字节平移（realloc/memcpy/memmove），新对象内部的 `ptr` 参数仍然顽固地死指着原来旧对象地址上被释放掉的旧 `buf` ！这会直接导致极其危险的系统崩溃和内存析构悬垂。**
  由于 `Bytes`、`StrA` 和 `StrW` 在设计上彻底摒弃了 SSO 内置小数据缓存，仅持有一个指向外部堆内存的指针，不存在任何自引用结构，这让它们可以非常安全地声明在 `be::is_trivially_relocatable` 物理平移白名单中。从而确保了它们在被 `Arraybe` 频繁搬运时，永远能绕过复杂的构造/析构层，直达最快、性能最高的 `realloc`/纯字节平移动作。
- **与 Arraybe 本身的定位截然不同**：`Arraybe` 作为数组容器，本就是被设计为在这套封闭生态内部流转和管理动态元素的。对于很多场景下元素起步少但动态增删高频产生的情形，允许你在早期利用它的泛型参数预留一块专属的（`stack_buf`）栈内存承载（即 `STACK_CAP`），从而规避掉海量碎片的堆内存分配开销。而**为了能激活针对该数组装载类型的栈缓存特性支持，那就必须要把它做成模板级容器，好让编译器在编译期给出元素类型并自动推导预留其在栈上具体的长度**。总之，数组容器本就是生态内部处理流转用的重型机，无需像字符串这类信息传媒一样，频繁且严峻地需要跨语言、跨框架的系统边界去维持原样格式。
- **自身内置的高效贪婪扩容机制**：虽然没有栈级别的起步缓存容纳小数据，但这完全**不意味着**会造成性能瓶颈。在白易体系中，底层针对不同容器的生命周期特征设计了截然不同但皆极高效的动态感知预分配算法：
  - **`Bytes / StrA/W` 的阶梯式倍数激进暴涨**：因为字符串往往是极其高频的“短线重击拼接”，它们在常态下每次装不下时，会直接触发 **`原容量 × 1.5`** （即`capacity_ += max(addSize, capacity_ >> 1)`）倍的激进增涨；而当容器尺寸越过极重负载阈值（**`capacity_ >= 32MB`**）时，则克制地切回 **`原容量 × 1.25`**（即 `+= max(addSize, capacity_ >> 2)`） 倍平滑渐进增长，以便缓解大块连续堆内存请求失败的极端压力。这种机制足以抹平丧失局部小栈缓存所带来的痛感。



---



### 13.5 数组元素类型约定与底层设计思想

`Arraybe` 的内存管理与 `std::vector` 不完全相同，理解这一点是正确使用它的关键。

#### 13.5.1 Placement New / Placement Delete 方案

`Arraybe` **并不使用常规的 `new T` / `delete T`**。内存分配（`malloc`/`realloc`/`free`）与对象生命周期（构造/析构）是完全分离的两件事：

```text
内存层：  malloc ──────────────────────────── free
                ↑                         ↑
对象层：  placement new (构造)   ~T_ELE() (析构)
```

- **构造**：在已分配的原始内存地址上就地调用构造函数：
  ```cpp
  new(&pArr[count++]) T_ELE(elem);            // 拷贝构造
  new(&pArr[count++]) T_ELE(std::move(elem)); // 移动构造
  ```
- **析构**：显式调用析构函数，但**不释放内存**（内存由容器统一管理）：
  ```cpp
  pArr[i].~T_ELE();  // 只析构元素，不 free
  ```

这是手动内存管理容器的标准惯用法，这部分与 `std::vector` 内部原理相同。



#### 13.5.2 push 的移动语义重载

`push` 提供两个重载，对元素类型有实质的性能差异：

```cpp
void push(const T_ELE& elem);   // 拷贝构造：malloc 新内存 + memcpy 内容
void push(T_ELE&& elem);        // 移动构造：仅转移指针所有权，无额外分配
```

对于 `Bytes`/`StrA`/`StrW` 这类"指针容器"，移动构造只做一次指针赋值和 `capacity_` 置零（标记源对象不再拥有内存），完全避免了堆分配和内容拷贝：

```cpp
Arraybe<StrW> arr;
StrW s = _W("hello world");

// 拷贝版：在槽位就地构造，内部 malloc 并 memcpy 字符数据
arr.push(s);                      // s 保持不变

// 移动版：在槽位就地移动构造，转移 s 的堆指针，s 变为空串
arr.push(std::move(s));           // s 之后不可再使用其内容

// 临时对象（右值）自动触发移动重载，无需手写 std::move
arr.push(StrW(_W("hello world"))); // ← 走 T_ELE&& 重载
arr.push(AtoW(someStr));           // ← 函数返回值也是右值，走移动
```

> **规则**：当元素是通过函数返回值或 `std::move` 产生时，编译器会自动选择 `push(T_ELE&&)`，无需手动干预。只有在明确需要保留原变量时才用 `push(T_ELE&)` 的拷贝版本。



#### 13.5.3 Trivially Relocatable (可平凡重定位) 与白名单机制

为了在保证安全的前提下压榨出极限性能，`Arraybe` 内部引入了基于 C++ 原生宏与标准前瞻的 **`be::is_trivially_relocatable`** 特征。当你向 `Arraybe` 投入元素时，它会进行编译期决议：
- **如果判定为“可平凡重定位”**：直接触发 `realloc` / `memmove` / `memcpy` 进行内存级别的纯字节极速搬移。
- **如果不满足**：它将退回标准的 C++ 做法，即分配新内存后，使用 `placement new` 逐个调用拷贝/移动构造函数，并在完成后分别调用旧对象的析构函数，同时**在此分支内提供了基本的异常安全拦截（防 UB 空洞化截断）**。

在这里，我们必须清晰地认知以下三个 C++ 历史中有关内存布局的概念差异：

| 概念名称 | 限定条件与特征 | 在 BECore 视角的意义 |
|---------|---------------|----------------------|
| **POD** (Plain Old Data) | 最古老严苛的标准。既要是标准布局（无虚函数，无各色访问控制），又要是平凡类型（不能有任何自定义构造、拷贝或析构函数）。如 C 语言的 `struct` 和 `int`。 | 过于陈旧严苛。哪怕只是为了方便初始化加了个空构造函数，也会失去 POD 资格。 |
| **trivially_copyable**<br />(可平凡拷贝的) | 现代 C++ 针对内存拷贝放宽了标准。只要没有自定义析构、拷贝/移动函数，且没有虚函数即可。允许包含其他平凡类型。 | 这是 C++ 原生宏判断基础类型的基石（对应的内置宏是 `__is_trivially_copyable`）。但 `Bytes` 等带有自定义析构的类无法通过该检测。 |
| **trivially_relocatable**<br/>(可平凡重定位的) | **正在写入 C++26 标准的现代机制**。即便类拥有自定义析构或拷贝构造，只要**将其字节原样复制到新地址后，直接放弃旧对象（绝不调用旧对象的析构），若对象语义依旧完全成立**，即视为可平凡重定位。 | **这正是白易最核心需要的！** `Bytes`/`StrA` 的析构是释放堆指针，拷贝它们不仅耗时还会指向同一个堆；但在“搬家”时，我们直接把地址复制过去且不再析构源对象，这就完美符合。 |

> **⚠️ 为什么编译器无法直接检测“可平凡重定位”？（自引用指针陷阱）**
> 你可能会问，既然 `StrA` 的指针只是指向堆区，直接平移不就行了吗？为什么编译器它默认不认可呢？
> 因为在很多现代 STL 容器实现中（如 `std::string` 著名的 **SSO 短字符串优化**），其内部不仅有静态的 `char buf[16]`，还有用于管理的**对象内部指针 `char* ptr`**。在短串时，这个 `ptr` **直接指向了该对象自身的堆栈结构内 `buf`（自引用）**！
> 如果编译器对这种携带内部自引用指针的对象进行了无脑的 `memcpy` 字节平移，平移之后，新对象里的 `ptr` 还是**死死咬着旧对象 `buf` 的老地址**。当老地址被覆盖或挪作他用时，程序当场崩溃。
> 编译器根本没有能力去深入剖析你自定义析构逻辑里的指针到底是“指向外部安全堆内存”还是“存在自引用重叠”。因此，它采取了“一刀切”法则：**只要你写了析构或拷贝函数，我就认定你不敢被平移！**

**白名单声明机制：**
正因为此，为了解决这个痛点，在 `Arraybe.hpp` 顶部，我们直接使用 traits 偏特化开了官方白名单通道，为你知根知底的安全对象颁发免检通行证，绕过编译器的死板机制使其直达底层内存字节层面的搬运。

```cpp
namespace be {
    template<typename T> struct is_trivially_relocatable { 
        static constexpr bool value = __is_trivially_copyable(T); // 默认拦截
    };
    // 发放免检白名单，向编译器起誓它们绝对不存在内部自引用重叠！
    template<> struct is_trivially_relocatable<Bytes> { static constexpr bool value = true; };
    template<> struct is_trivially_relocatable<StrA>  { static constexpr bool value = true; };
    template<> struct is_trivially_relocatable<StrW>  { static constexpr bool value = true; };
    template<> struct is_trivially_relocatable<StrU8> { static constexpr bool value = true; };
}
```
**BECore 自身的所有类型（Bytes/StrA/StrW/StrU8/BR）均已加入此（POD-like）trivially relocatable 物理拷贝安全白名单。** 如果未来接入 C++26，我们只需将其直接桥接至标准提供的官方验证接口中即可。



#### 13.5.4 数组搬迁降级机制：从 memmove 到无异常的移动构造再到降级为拷贝构造的接管异常兜底的最后防线

如果投入 `Arraybe` 的元素是已被认证的 `is_trivially_relocatable`，则容器在内部发生搬移（扩容搬家、插入挪位、删除补位）时都只会执行一条指令：**以极致性能直接 `realloc` 或 `memmove` 进行纯内存块的复制平移**，全程绝不触发任何构造与析构。

然而，**一旦放入的是未经认证的非平凡复杂对象（比如未知的第三方重量级实体类）**，直接的底层移位会因为它们内部暗藏的自引用或严格生命周期而导致 UB 悬垂。针对这种情况，`Arraybe` 设计了一套**自动降解且极其完备的安全保防机制**：

1. **第一防线（无异常保证移动的优先选择）**：  
   在进入常规的面向元素构造阶段前，`Arraybe` 会借助 `be::nothrow_movable<T>::is`（判断是否具有 `noexcept` 的移动构造）甄别该对象。若是，则使用安全的**移动构造**逐一接手元素并伴随对旧实体的就地析构清理。由于该操作受静态检查保证绝对不会抛出意外，因此这一步能在不损耗原有一丝数据的情况下完成最高效的深层平移。

2. **第二防线（拷贝回退与基本异常安全兜底）**：  
   如果对象的开发者连最基本的 `noexcept` 都不肯给出（此时意味着它的构造过程有高度崩溃/溢出抛出的雷区），`Arraybe` 将别无选择地**彻底退化为深拷贝构造**。
   但深层拷贝随时可能中途抛异常。为此，白易针对最核心的三大搬移场景绘制了精确的“断尾求生”异常隔离机制（所有操作都裹挟在被极其严密设计的 `try...catch` 和修正回退网中）：

---

##### 边界场景 1：[插入元素中面临异常击穿]

由于在已有队列中插入元素时，必须先将索引右侧的群体**自右向左依次**（即从数组最尾端开始倒序遍历）平移目标位，由于偏移产生**内重叠**：

```text
【移位方向：往右侧走】（由于为防覆盖采用倒序遍历拷贝，所以移动成功的数据总是分布在最右侧，红区代表已经清空的源遗体）

[安全区(不变)]  [腾空的目标位]  [待移走的源数据]
      ||             /-----------/         |
      ||            / 异常爆破点           |
      ███████▒▒▒▒▒▒■■■■■■■■■■■■■■■■■■■■■■■■■
                 (抛出异常前因安全设计已原位析构的壳)
```

**兜底策略：**
由于内部是从队列最尾端开始倒序拷贝并在原位执行对应析构的，这意味着当某个复杂对象在中途抛出异常时，**处于发飙点向右一直延展至队尾的数据，都已经被向右完好复制并位移存放，但是被挖空的源位置留下了大量已被显式原位析构（`.~T_ELE()`）的非法空壳。**
由于中场产生了断裂型黑洞，为了确保你下一次退栈再对此 `Arraybe` 清理调取时不会踩到这种虚无的雷，捕获异常后我们将立即执行壮烈截断：
- 析构所有在刚才这个灾难瞬间已经成功建立起的新位拷贝（图中 `■` 代表的区域以及右侧的延伸）。
- **直接将整个大数组的有效 `count` 强制截断缩减为发灾点即本欲插入处左侧的距离（图中 `█` 代表的安全前序区）。**
- 异常外抛。此战损意味着原本右侧所有即便未波及的幸存数据也必须无奈舍弃以换安全（以防数组中留有黑洞斑连）。

---

##### 边界场景 2：[删除元素中遭受异常击穿]

与右挪相比，将右侧群体向左整体平移以填补被删除数据的空洞（**自左向右正序操作**），**不会发生覆盖源数据的内存重叠危险**。但拷贝过程仍藏危机：

```text
【移位方向：往左侧填】（正序填坑，右侧都是还未动过刀的规矩老数据，而坑坑洼洼出现在前端填补区处）

[目标填坑区]       [源数据待移区]
<-----/                 |                 |
     / 异常爆破点       |                 |
▒▒▒▒■■■■■■■■■■■■███████████████████████████
(废弃)已转移失败带残缺   还没来得及移动的原好数据
```

**兜底策略：**
当左平移在正序拷贝某段对象时抛出异常：
- 那些刚被拖运到左面新坑但还未安顿好的那批（图中 `■` 区块）将当即就地清理析构处理。
- 那些待在安全旧家原址还未被转移指令惊动的数据（图中 `█` 区块）也将被严密地连坐析构。
- **容器的 `count` 将被干净利落地削减至原本将被删除元素的起跑线处**（图中 `▒` 之前那完好无缺的区域），使得之后外界的访问与释放一切正常。
- 异常外抛。

---

##### 边界场景 3：[扩容搬家迁徙中途发生异常抛掷]

在栈满溢出或是堆内存重新分配 `malloc` 新片区时，所发生的是**全安界的跨区独立拷贝**：

```text
【移位方向：旧长屋 跨界拷贝至 新豪宅】

[旧址] █████████████████████████████ (毫无瓜葛，原样存留待命)

[新址] ▒▒▒▒▒▒▒▒▒▒▒■■■■■■■■■■■■....... (正在分配安放构造中)
                  | 
               产生异常
```

**兜底策略：**
这大概是所有情况中遭遇致命抛出时最能全身而退云淡风轻的一种结构：
- 因为拷贝首度在互不干扰的全新堆区进行，如果在构造第 N 个时发生了爆破抛出异常。
- 兜底异常块只需原路去逐个收拾从 0 开始建到 N 为止的半成品序列进行分别析构清理（将图中已放置的 `▒` 击毙销毁）。
- 释放此次新屋申请到的废弃堆指针 `free`，**并保持原宿主的 `count` 与 `pArr` （纯血旧址阵地）丝毫不变！**
- 这次相当于刚迈出去的脚收了回来并且当作啥都没发生，老住户没有蒙受任何损失直接将异常向外传递。



#### 13.5.5 白易极简构造哲学：不在构造函数中抛出异常

随着框架的不断演进，更多不确定的第三方重量级实体（或外来复杂模板类）可能被装填入 `Arraybe`。我们很难假设外部代码都能遵循白易极致严苛的无异常约定。如果某个第三方对象在 `Arraybe` 插入或扩容搬运的中途抛出了异常（例如深拷贝时内存分配失败），若没有我们前述 13.5.4 小节中精心构筑的分级防御体系，容器将面临致命溃烂：
- 已被强行移出并在原位调用过析构 `.~T_ELE()` 的失效空壳，会变成致命的黑洞横亘在数据队列中间；
- 当异常退栈并引发容器被动销毁时，全局析构的遍历一旦踩入这些黑洞，就会引发无可挽回的未定义行为（UB：如双重析构或悬野指针崩溃）。

为此，`Arraybe` 凭借**“从 `memmove` 白名单极速平移 → 到无异常移动构造 (`nothrow_move`) 安全接管 → 再到退化为深拷贝并辅以 `try-catch` 状态截断兜底”**这套三级降级防线，牢牢封锁了越界崩溃的命门。对于最坏情况下的拷贝抛掷兜底，我们主动舍弃了类似 `std::vector` 那样庞杂且极度剥削性能的**“原始状态精准回滚（强异常安全保证）”**，转而采用干脆利落的**状态保底截断（基本异常安全保证）**：迅速切除受异常污染的残缺网段，将容器及时拦截并封存至已平移安全且连续的剩余阵列状态。此举以极小的数据切割代价彻底剿灭了系统级连环溃崩的隐患。

这套严丝合缝的阵列护甲，便是我们全量接纳外部泛化类型时的底气与屏障。

但反观白易**自属的二级基本数据类型（如内置的 Bytes、StrA/W 及等效受控对象）**，它们不仅从不抛掷，也绝不担忧在容器搬运中有任何失败的风险。由于这类核心组件完全超脱了逐一析构与重构的性能禁锢，它们的迁徙实际上就处在最高效的**纯字节平移层面（这种毫无赘余的物理连续块拷贝操作，能毫无阻碍地被编译器与系统底层识别，从而触发极为彻底的 SIMD 向量化指令加速和 CPU 缓存段极限优化）**。而在标准的 `std::vector` 中，为了死保那苛刻的“强异常安全”，系统不得不引入铺天盖地的 `noexcept` 侦测：只要类构造有一丁点抛掷嫌疑，容器就会像惊弓之鸟般放弃高效的移动语义，被迫退化为沉重的安全查验式深拷贝。

正是基于这两者在运行期展现出的巨大算力与调度代价鸿沟，我们需要在此**重申白易所主推的核心哲学思想设计：绝不提倡在构造函数中执行业务逻辑并阻断控制流。**

**业务失败 ≠ 对象实例作废（保留重试权）：**  
C++ 传统的 RAII 思想提倡*“构造失败即抛异常自动释放资源，构造成功的对象必定有效（维持不变量）”*（比如一个文件对象在构造时若文件不存在，则直接抛出异常，而不是给出一个半残废的对象）。其潜台词是：“一旦伴随的业务行为（如打开文件、连接TCP）失败，这个对象本身也就沦为了废品，必须销毁释放”。但这实际上直接扼杀了程序简单灵活的**动作重试**余地，也会带来一层层突如其来的心智负担。

在白易原生的核心思想看来，大多数类对象应当仅仅只是一个“状态载体”或“参数结构”。例如一个经典的重型 `TCP客户（套接字）` 对象，它的构造仅仅只是安放好“目标IP”和“端口”。如果后续的 `连接()` 等业务动作失败了，**该类对象结构本身依然是完备可用的**，使用者完全可以在不销毁该对象的上下文中稍作等待，再次调用它的 `连接()` 方法进行补救测试。没有任何理由因为一次前线动作失败，就在机制上抛出异常让整个躯体结构当场阵亡。

因此，如果秉承极简风格，使用明确的返回状态码或结构体去表达业务过程，并把你的类退化为毫无悬念的简单资源把柄组合：
> **白易极简构造的最高理想：构造函数绝不越权，只负责快速装填参数与安全清零，永不在其生命起始阶段直接抛出异常诱发管线停摆。**



#### 13.5.6 与 std::vector 的核心区别对照

| 维度 | `std::vector` | `Arraybe` |
|------|--------------|----------|
| 默认扩容机制 | 逐元素移动/拷贝机制 | `realloc` / 字节平移机制（基于白名单判定） |
| noexcept 束缚 | 极重（`move_if_noexcept` 退化深拷贝） | 几无（有则走免抛出移动，否则走 `try` 基本防护） |
| 途中异常安全 | 强保证（慢速精准原状回滚） | 基本保证（快速断尾保全、防内存悬空致溃） |
| 推荐元素要求 | 任意类型 | 鼓励原生 / POD-like / 已发白名单核心对象 |



---

## 14. 已知设计要点与陷阱

### 14.1 initializer_list 字面量魔法

使用字面量字节集构造 如：`Bytes b = {1,2,3} `会在栈中分配内存、这里b只是其栈内存的引用

```cpp
Bytes(std::initializer_list<byte> bytes){
    static volatile byte _trap = *bytes.begin();  // 制造静态逃逸假象
    _ref(bytes.begin(), bytes.size());
}
```

`static volatile` 的 `_trap` 是为了防止 Release 优化掉 `initializer_list` 底层数组的内存——否则编译器可能认为数据无副作用而不分配，导致 `_ref` 引用到完全没在编译期安排产生的栈内存（读者可试着把该行魔法去掉在Release版本下看运行结果数据是否正常）

### 14.2 字符串成员 `bytes.buf` 永不为 NULL

白易的字符串类（`StrA`、`StrW`、`StrU8`）在设计上强制设置内部缓冲区指针 `bytes.buf` 始终指向一个有效的内存地址。

- **空状态重定向**：无论是默认构造 `StrA()`、显式 `reset(0)` 还是移动后的源对象，其内部 `bytes.buf` 均会自动指向对应的空字符串字面量（`""` 或 `_W("")`），而不是 `nullptr`。
- **调用安全**：这种设计确保了将白易字符串对象直接传给绝大多数 C/C++ API（如 `Win32 API`、`strcmp` 等）时是相对安全的，消除了外部判空检查的心智负担。（当然在Win32API下NULL和""的语义有时还不太一样，这需开发者自知其场景）
- **💡 判空提示**：请直接使用 `if (!str)` 或 `if (str)` 判断对象是否为空（它底层已重载安全的布尔转换映射到长度判断）。

```cpp
StrA s; 
char* p = s; // p 会指向 ""，且 *p == '\0'，绝对不是 NULL

StrA s2 = std::move(s);
// s 现在处于 moved-from 状态，但其 s.bytes.buf 依然被重置为了 "" 指针
```

### 14.3 StrA/StrW：单参 `const` 指针为“赋引用”，其余构造均为拷贝

核心规则（已在源码内标注）：
- **`StrA(const char* s)` / `StrW(const charW* s)`**：单参 `const` 指针构造为**引用**（`capacity_==0`）
- **除上述以外的构造方式**：均为**拷贝构造**（含 `char* / charW*` 指针、带长度参数、拷贝构造等）

```cpp
StrA s = "hello";  // 引用！不拷贝！capacity_=0
s += " world";     // _append 检测到 capacity_==0，malloc+自动拷贝，s变为独立堆内存
```

这是一个**性能优化**：常量字面量大多是只读的，引用能避免不必要的内存分配与拷贝。

#### ⚠️ 警惕“const 裸指针”导致的悬垂引用

由于单参 `const char* / const charW*` 是**引用语义**，当你把**生命周期不安全**的指针以 `const` 形式传入时会悬垂：

```cpp
// ❌ 危险：const 指针走引用，离开作用域后悬垂
StrA Bad() {
    char buf[64];
    sprintf(buf, "tmp");
    const char* p = buf;
    return StrA(p); // 引用 buf，函数结束即悬垂
}

// ✅ 安全：char* 构造是拷贝（会 strlen 并复制）
StrA Ok1() {
    char buf[64];
    sprintf(buf, "tmp");
    return StrA(buf); // 走 StrA(char*), 深拷贝
}

// ✅ 安全：显式拷贝（推荐给 const 指针）
StrA Ok2(const char* p) {
    return StrA(p, strlen(p)); // 或者：StrA s; s = p;
}
```

**结论与最佳实践**：
- **字面量 `"hello"`**：走引用（安全，常量区静态存储）
- **非 const 可变数组/指针 (`char* / charW*`)**：走拷贝（安全）
- **`const char* / const charW*` 指针**：默认走引用（可能悬垂）  
  若需要拷贝，请用 **带长度构造 **或 **单参的To开头函数**`ToStr()` 或 `WToStr()`、`ToStrPlat()`  

#### ⚠️ 警惕三目表达式中临时变量的指针悬垂

**这个也是一个极其隐蔽且危险的陷阱。** 当三目表达式 `?:` 的分支中产生了临时的容器对象（如 `Bytes`、`StrW`），而你用**指针类型**去接收结果时，临时对象在整个表达式求值完毕后立即析构，导致指针悬垂！

```cpp
// ❌ 危险！指针悬垂！
StrA a = "world";
char* s = a==NULL ? "" : ("hello"+a);
printf(s);  // 💥 s 指向的 临时对象已析构，内存已释放！
```

下面是安全做法：

```cpp
// ✅ 正确：用 Bytes/StrW 等容器类型接收
StrA a = "world";
StrA s = a==NULL ? "" : ("hello"+a);
printf(s);  // 安全：s 在作用域结束前不会析构
```

**原理**：用容器类型接收时，临时对象在析构前就已经通过拷贝构造（或移动构造）将数据转移给了接收变量。在 BytesStr.hpp 2.0 中引入了移动构造后，这里实际走的是移动构造（`Bytes(Bytes&&)`）

**规则总结**：**永远不要用裸指针（`byte*`、`char*`、`wchar_t*`）去接收可能产生临时容器对象的表达式结果。** 用 `Bytes`/`StrA`/`StrW` 接收即可。

### 14.4 ⚠️ 警惕临时对象的链式方法调用（以及 BA/BW 解决方案）

**这是引入移动构造后产生的隐蔽缺陷。** 当函数返回一个**临时对象**实体（如 `Bytes`），哪怕它支持移动语义，你若对其链式调用返回引用的成员函数（原先设计有 `_SL(-1)`），其结果在赋值之后会引用一个**已析构的临时对象**。

```cpp
Bytes make() { return Bytes("hello", 6); } // 返回临时对象

// ❌ 错误：链式调用 _SL
// make() 返回临时对象A -> _SL 返回引用B(指向A的buf)
// 语句结束 -> A 析构(释放buf) -> b 持有的引用B 悬垂！
Bytes b = make()._SL(1);
```

因为 `_SL` 返回的是字节集在Tag层面的引用（Ref），并不拥有内存，但 `_SL` 它已经显式把右值再次转为了常规(左)值这里把右值类型的传递性给抹灭，且拥有内存的临时对象 在表达式结束后就销毁了，最终b拿到的只是即将销毁的那个临时对象的引用版字面量（因C++17强制RVO会在b上就地构造返回对象）。

**解决方案：使用 `BA` 或 `BW` **

这两个 helper 函数专门用于接管临时对象的所有权或借用左值缓冲区并做尾部截断（去空终止）：

- `BA(Bytes&& b)` / `BA(Bytes& b)`：移动/引用接管 b 的内存，并将 size 减 1（去 ANSI 空终止）
- `BW(Bytes&& b)` / `BW(Bytes& b)`：移动/引用接管 b 的内存，并将 size 减 2（去 Unicode 空终止）

```cpp
// BA/BW 真实实现：
inline Bytes BA(Bytes&& b) { return Bytes((Bytes&&)b, b.size ? b.size-1 : 0); } // 移动构造+截断（安全接管）
inline Bytes BA(Bytes& b)  { return Bytes(b, BRef, b.size ? b.size-1 : 0); }     // 引用构造+截断（非深拷贝！）
inline Bytes BW(Bytes&& b) { return Bytes((Bytes&&)b, b.size ? b.size-2 : 0); } // 移动构造+截断（安全接管）
inline Bytes BW(Bytes& b)  { return Bytes(b, BRef, b.size ? b.size-2 : 0); }     // 引用构造+截断（非深拷贝！）

// ✅ 正确：BA 接管了 make() 返回的临时对象的所有权，防止其析构释放
Bytes b = BA(make());

// ⚠️ 警告：对于左值（非临时对象），BA/BW 仅执行引用构造（浅拷贝，capacity_ = 0），绝非深拷贝！
Bytes origin = xxx;
Bytes copy = BA(origin); // 仅引用 origin 的缓冲区并去掉末尾 1 字节。若 origin 析构，copy 会变成悬垂引用！
```

**规则**：当需要接收一个返回 Bytes 实体的函数并去除其空终止时，**必须**包裹在 `BA(...)` 或 `BW(...)` 中，而不要链式调用 `_SL`。

### 14.5 ⚠️ 警惕 printf/scanf 类变参函数的直接传参

**千万不要在变参函数（如 `printf`、`sprintF`）中直接传入 `StrA` 或 `StrW` 对象！**

C 风格的 `...` 变参不会触发 C++ 的 `operator T()` 隐式类型转换。如果你直接传入对象，编译器会把整个对象结构（buf指针+size+capacity）作为参数压入栈中，`printf` 却只把它当成一个指针来读取，（放到结尾还没什么大问题）但如果参数放中间则参数错位则后续读取错误，严重时导致崩溃！

```cpp
StrA s = "hello";

// ❌ 错误：s 是对象，不会自动转为 char*
printf("%s", s);   // 💥 崩溃！

// ✅ 正确：显式强转（推荐）
printf("%s", (char*)s);

// ✅ 正确：调用 _buf()
printf("%s", s._buf());

// ✅ 正确：sprintF/FW 同样需要强转
sprintF("Val: %s", (char*)s);
```

而在 `be::print` 或 `operator<<` 等 C++ 模板函数中，由于有类型推导和重载，可以直接传 `s`。




### 14.6 字符串类手动预分配内存后的 size 修正 

当你使用 `StrA/StrW` 的构造函数（如 `StrA(size_t len)`）或 `reset(len)` 进行**字符数预分配**时，对象的 `bytes.size` 会被初始化为该长度，除了若内部你自己有手动填充空终止外，后续结束时必须手动修正 `bytes.size`。

这是因为 `StrA/StrW` 的高级行为（如拼接 `+=`、获取长度 `len()`、转码等）完全依赖内部的 `bytes.size`，而不是实时扫描 `\0`。
### 14.7 内存布局等价性与零拷贝强转

由于 `StrA` / `StrW` 内部唯一的成员就是 `Bytes bytes;`，没有任何虚函数，也没有其他字段。这意味着**字节集与字符串的物理内存模型是绝对 100% 一致的**。

#### 14.7.1 极致性能的零拷贝传参
如果你有一个巨大的 `Bytes`，需要投入到一个参数要求为 `const StrA&` 的字符串业务函数中（比如查找、替换），你**不应该**去调用诸如 `ToStr(bytes)` 或者重新构造 `StrA(bytes)`，因为这会触发一次完全没有必要的深拷贝。

正确且高效的姿势是**直接通过 C 风格强转或者引用转换**：

```cpp
Bytes t = 读入文件(L"t.txt");

// ❌ 错误：触发了拷贝构造，白白浪费内存和性能
StrA a = 子文本替换(StrA((char*)t.buf, t.size), ...);

// ✅ 神级操作：直接强转引用，骗过编译器，完全零拷贝！
StrA a = 子文本替换((StrA&)t, ...);
```
这对于底层做 IO 读取和纯数据清洗时能带来数量级的性能飞跃！

> ⚠️ **安全警告**：由于白易内置的文本操作函数（如查找、替换等）是基于 `bytes.size` 进行逻辑处理的，因此对不含空终止的原始字节集进行强转是安全的。
> 但如果你将这种强转后的对象传递给需要**隐式转换为 `char*` / `charW*` 指针**的场景（如 Win32 API 或 `printf`），你必须确保原始字节集末尾**已经手动包含了空终止符**，否则会引发越界读取的灾难。

#### 14.7.2 核心技巧：结合 BSA 包装非常规字符集（如 GBK）
在采用 UTF-8 优先的架构中，如果你需要在代码里硬编码几个其他编码（如 GBK）的汉字去参与搜索或替换，可以直接写出该汉字对应的 GBK 字节码，并通过 `BSA` 包装。这里可以通过模板参数展示 `false` (默认) 和 `true` 两种版本：

```cpp
Bytes t = 读入文件(L"t.txt");

// 1. 默认模式（PADZERO = false）：字节集尾部已含空终止符（如末尾的 0）
// { 202, 199, 0 } 是 GBK 下 "不" (202, 199) 的字节编码加上 1 字节的空终止
// BSA(bytes) 会接管内存并直接将 size 减 1，得到 2 字节长度的 StrA 实例，零分配零拷贝。
StrA a = 子文本替换((StrA&)t, BSA({ 202, 199, 0 }), U8toGBK("是"));

// 2. 填充模式（PADZERO = true）：字节集尾部不含空终止符（无末尾 0）
// { 202, 199 } 是 GBK 下 "不" 的 2 字节纯数据编码
// BSA<true>(bytes) 会先在尾部追加 0，再做递减，将其转化为带有空终止的 2 字节 StrA 实例。
// ⚠️ 性能警告：由于 { 202, 199 } 字面量默认为只读的“引用模式”（capacity_ == 0），
// 对其追加 0 时会触发添时复制（COA）机制，从而进行堆内存申请与数据拷贝。
StrA a2 = 子文本替换((StrA&)t, BSA<true>({ 202, 199 }), U8toGBK("是"));
```

> 🚫 **关于空终止与 PADZERO 模板选择及性能代价**：
> - **使用 `BSA`（默认 `BSA<false>`）**时，传入的字节集**必须手动包含空终止符**。因为默认会将 `size` 减 1（或减 2），如果原始数据不包含 `\0`，不仅会导致原字符长度无故截断，而且在隐式转换为 `char*` 指针使用时，会因缺少空终止而引发未定义的越界内存读取。不过此模式在传入 `{ 202, 199, 0 }` 等字面量或已有空终止的 Bytes 时，是 **100% 零堆分配、零内存拷贝** 的极致性能。
> - **使用 `BSA<true>`**时，传入的字节集**无需包含 `\0`**。函数在内部（如果容量足够）则会先为字节集缓冲区追加填充 `\0` 空终止，之后自减原版缓冲区，而最后返回其浅拷贝。
>   > ⚠️ **堆分配开销警告**：如果传入的源字节集处于**引用模式**（如字面量 `{ 202, 199 }`、外部静态/栈内存引用等，其 `capacity_ == 0`）或剩余空闲容量不足以容纳空终止符，追加 `\0` 动作将**强制触发添时复制 (COA) 扩容机制，在堆上申请新内存并进行整段数据拷贝**。因此，在对性能要求极高的热点循环中，应优先在字面量尾部手动写好 `0` 并使用默认的 `BSA` 零拷贝版。


