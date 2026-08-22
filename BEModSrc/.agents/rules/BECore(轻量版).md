---
trigger: always_on
---

# BECore 核心库0.6 — AI Skills 参考

> [!IMPORTANT]
> **UTF-8 编码约定**：白易框架在编译层面已设定 `/utf-8` 选项，**`StrA` 的字面量编码始终为 UTF-8**。这确保了窄字符串在 Win32 与 Linux 平台间具有统一的编码语义，开发者应始终视 `StrA` 为 UTF-8 字符串进行处理。

#### 常引用类型别名设定 (c_系列)

为了使函数参数声明更加整洁、统一，白易核心库正式推荐采用带有 `c_` 前缀的别名来代替传统的 `const T&` 常引用写法：
- **`c_Bytes`**：= `const Bytes&`
- **`c_StrA`**：= `const StrA&`
- **`c_StrW`**：= `const StrW&`
- **`c_StrX`**：= `const StrX&`
- **`c_StrU8`**：= `const StrU8&`
- **`c_Arraybe`**：= `const Arraybe&`
- **`c_AutoStr`**：= `const AutoStr&`，以保持与其他基础容器常引用 `c_` 前缀别名的命名规范完全一致。

### 1. 字节集容器 (Bytes)

`Bytes` 是白易底层核心的数据存储容器，StrA、StrW 等皆是基于其内存布局的语义封装。
- **核心属性**：通过 **`size`** 属性获取当前字节长度。
- **判断为空**：直接使用 `if (!bytes)` 即可进行快速且安全的判空（等价于size==0）
- **容量保底机制**：底层 `_reset(n)` 分配内存时，实际会申请 `n + 2` 字节容量。这为 `StrA` (1字节 `\0`) 和 `StrW` (2字节 `\0`) 提供了无需二次分配即可原位填充空终止符的机制。
- **内存所有权**：内部通过 `capacity_` 区分模式：`capacity_ == 0` 为**引用模式**（析构时不释放 `buf`）；`capacity_ > 0` 为**拥有模式**（析构自动释放）。
- **字面量支持**：直接用 `{1,2,3}` 作为 `initializer_list` 初始化，默认为**引用模式**（存在栈中零堆申请）。
- **字节数组构造 (重载模版)**：
  - `Bytes(byte(&arr)[N])`：对**非 const** 字节数组执行**拷贝构造**（申请内存并深拷贝）。
  - `Bytes(const byte(&arr)[N])`：对**const** 字节数组执行**引用构造**（直接绑定为引用模式，零堆拷贝）。
- **引用构造 (三参数)**：可通过 `Bytes(ptr, BRef, size)` 构造一个**引用模式**的字节集，其中 `BRef` 是全局定义的 `RefTag` 标签，用于显式声明不进行内存拷贝。
- **拼凑构造 (四参数)**：可通过 `Bytes(ptr1, size1, ptr2, size2)` 构造一个**拥有模式**的字节集，它会自动分配 `size1 + size2` 的空间并将两个数据块依次拷贝进去，常用于报文头部与体的快速合并。
- **安全保障 (COA)**：一旦发生修改（如 `append`），若当前是引用模式会自动触发添时复制 (COA / Copy-on-Append) ，转换为拥有模式并在堆上分配内存。
- **空构造与 NULL 安全**：`Bytes()` 默认构造后 `buf` 为 `nullptr`，`size` 为 0。在使用 `byte*` 转换前应使用 `if (bytes)` 检查。这与 `StrA/StrW` 不同（字符串类即便为空也保证额外指向 `""`）。
- **重置与分配 (`reset`)**：
  - `reset(n)`：释放旧内存并重新分配 `n` 字节（实际分配 `n+2`），设置 `size = n`。
  - `reset(p, n)`：重置并从指针 `p` 拷贝 `n` 字节。
  - `reset(byte, n)`：重置并填充 `n` 个指定字节。
- **追加扩展 (`append`)**：
  - 支持多种重载：指针 `append(p, n)`、字节集 `append(bytes)`、引用 `append(BR)` 等。
- **内存拷贝 (`copyFrom`)**：用于对**已分配**的内存进行局部或整体覆盖，**不改变**当前 `size`。
  - `copyFrom(p, n)`：从起始位置拷贝 `n` 字节。
  - `copyFrom(offset, p, n)`：从指定偏移处开始拷贝。
  - 支持直接传入 `Bytes` 或 `BR` 对象。
- **手动引用 (`ref`)**：`ref(p, n)` 可手动将现有 `Bytes` 对象切换为引用模式，指向外部内存 `p`。
- **指针转换**：可以直接隐式转 `byte*` 或通过 `.buf` 获取。注意：仅在 `size > 0` 时指针才保证有效。



### 2. 字符串体系 (StrA / StrW)

常规运算符操作均支持：`+`、`=`、`+=`，提供 `operator char*` / `operator charW*` 隐式转换。

#### 核心区别：字符数与字节数

- **`StrA`**：`len()` == `bytes.size`（字符数即字节数）。
- **`StrW`**：`len()` 返回字符数，`bytes.size` 返回字节数（字符数 × 2）。
- **公开的API**：`reset(n)` 的参数是**字符数**。内存拷贝操作（如 memcpy）请一律使用 `.bytes.size`。

#### 隐式转换与变参陷阱

- **标准传参**：如`const char* p = str `（这里str为StrA），可无缝传递给 Win32 API、`fopen` 等需要参数是字符串指针类型的函数。
- **⚠️变参陷阱**：在 `printf` / `sprintF` 等变参函数中**绝对不可**直接传对象，必须显式强转：`(char*)str` 或 `(charW*)str`。
- **永不为 NULL与保证空终止**：即便是空构造内部指针始终安全指向 `""`，这依赖于底层Bytes在初始分配时自动预留至多2字节清零，而后期的所有字符串内部扩充函数都会满足此约定，天然满足 Win32 要求 单/双 `\0` 结尾的 API。

#### 内存安全性（引用 vs 拷贝）
- **引用语义**：`StrA("hello")`（单参常量指针）默认**不分配内存**，直接引用。若修改（如追加内容）会自动 COA (添时复制)。
  > **风险**：切勿传入栈上临时 `const char*`，离开作用域会导致悬垂指针。
- **拷贝语义**：传入非 `const` 指针 `char*`、带长度的构造 `StrA(p, len)`、或调用 `ToStr()` 均为深拷贝。

#### 快速布尔判空
对 `Bytes`、`StrA`、`StrW`、`StrX` 均已重载 `explicit operator bool`，直接使用 `if (obj)` 或 `if (!obj)` 即可完成快速且语义准确的判空（其语义等价于判断其缓冲区size是否不为0）。




### 3. 字节集引用与高阶技巧 (BR/SR/BA/BW/BSA/BSW)

#### 引用与所有权
用于在函数间高效传递内存分片（Slices）而无需拷贝的只读引用类型.
- **`BR` (Bytes::Ref)**：字节集引用。默认**不含**空终止符（如 `BR("abc").size == 3`）。适用于字节集拼接、分割符、文本比较等纯数据场景。
- **`SR` (String Ref)**：这是一个包装函数它返回的是 **`BR`** 类型，是对字符串的字节集带空终止字节的引用。通过 `SR()` 创建，其 `size` 包含空终止符（如 `SR("abc").size == 4`）。
- **`BA` / `BW`**：对字节集进行浅引用去除其空终止，返回Bytes容器。（如 `Bytes a = BA(sprintF("..."));`）。注意提醒：**严禁**对临时对象链式调用 `_buf()` 或 `_SL()`，但这里BA/BW是安全的、可接管其生命周期。
- **`BSA` / `BSW`**：将 `Bytes` 对象提升为安全真实的字符串对象 (`StrA` / `StrW`)。
  - **场景 1：直接调用 `BSA(bytes)` (默认)**：适用于原始字节流**已包含**空终止符。会自动递减 1 (A) 或 2 (W) 字节的 `size` 以符合字符串长度语义。
  - **场景 2：模板调用 `BSA<true>(bytes)`**：适用于原始字节流**不含**空终止符（如纯数据内存）。会自动先填充空终止字节后再递减长度（此操作**不影响源字节集的原始 size**，仅在必要时扩容并在原始长度的数据后添增 `\0`），随后将该数据区进行引用安全地赋予给到返回的字符串（无数据区拷贝）。
  - **`_BSA` / `_BSW` (指针包装引用)**：`_BSA(const void* p, size_t size)` / `_BSW(const void* p, size_t size)` 用于将任意指针+大小直接包装为 `StrA`/`StrW` 的零拷贝引用。
    > [!WARNING]
    > **不保证空终止符**：包装得到的字符串类**完全不保证以 `\0` 结尾**！虽然可以安全传入框架内置的基于字节长度（`len()`）处理的文本操作，但若隐式强转为指针传给 Win32 API 或 C 风格字符串函数，极易引发越界读取甚至崩溃。

#### 内存模型一致性带来的强转安全便利性 (Zero-Copy Casting)
由于 `StrA` 和 `StrW` 内部唯一的成员即是 `Bytes bytes;`，它们的**内存模型与 `Bytes` 是完全等价的**。

**神级强转带来的性能提升**：当你有一份读取自文件或网络的 `Bytes`，需要调用诸如 `子文本替换(const StrA&, ...)` 这样的白易内置字符串操作时，**完全不需要**调用任何 `ToStr` 转换函数（那会引发拷贝），你只需直接强转：`(StrA&)bytes` 或 `(StrW&)bytes`，这能极大地消除冗余的二次内存分配 and 拷贝，得到极致性能。

> ⚠️ **安全警告**：由于白易内置的文本操作函数（如查找、替换等）是基于 `bytes.size` 进行逻辑处理的，因此对不含空终止的原始字节集进行强转实质是安全的。
> 但如果你将这种强转后的对象传递给需要**隐式转换为 `char*` / `charW*` 指针**的场景（如 Win32 API 或 `printf`），你必须确保原始字节集末尾**已经手动包含了空终止符**，否则会引发越界读取的灾难。



### 4. 万能字符串中转 (AutoStr)

`AutoStr`（全局常引用别名 `c_AutoStr`）是框架核心的格式化/中转通道，采用 **UTF-8 优先**。

- **统一传参与输出**：将函数参数声明为 `c_AutoStr`，调用者可无缝传入基础类型、字符串字面量或自定义结构体。同时支持通过 `be::print(var)` 变参及流式 `be::cout << var` 进行万能打印。
- **扩展支持自定义类型**：**严禁**在自定义类内重载 `operator AutoStr()`！若要让你的类型支持格式化，只需提供一个同命名空间下的 `__AutoStr__` 函数（利用 ADL 参数依赖查找）。
- **快速判空**：支持直接使用 `if (!obj)` 或 `if (obj)` 快速判断内容是否为空。

#### 💡 自定义类型格式化示例：

```cpp
struct Point { int x, y; };

// 既然你要自己接管该类型的输出，你必须为其定义一个重载（并且请勿包含在namespace be中）
StrX __AutoStr__(const Point& pt) {
    return sprintF("{x:%d, y:%d}", pt.x, pt.y);
}

void main() {
    Point pt = { 10, 20 };
    be::print(pt); // 输出: {x:10, y:20}
}

// 更强大的是，它可以被嵌套在其他容器中自动递归格式化
Arraybe<Point> arr;
arr.push(pt);
be::print(arr); // 输出: Array:1{{X:10, Y:20}}
```




### 5. 通用文本编码传参类型 (StrX)

`StrX`（常引用别名 `c_StrX`）继承自 `AutoStr`，同样具备跨编码自动转码缓存的能力，但它**约束了万能模板的构造函数**，这是避免手写 A/W 双版本接口的**极简替代方案**。

- **类型安全防御**：编译期**严格限制**只能传入纯文本（`StrA/StrW/char*`等）。绝不会像 `c_AutoStr` 那样来者不拒，把 `RECT` 或数组也照单全收。
- **设计指引**：在文本/系统扩展函数 API 设计中，**但凡涉及文件路径相关，或者是不影响性能的高级功能函数参数，均推荐使用 `c_StrX` 进行参数设计**。
- **极简与默认值**：你的 API 仅需定义一个 `c_StrX` 参数即可兼容一切文本输入，且支持使用空字符串作为默认参数（如 `c_StrX 目录名 = ""`）。
- **快速判空**：继承自 `AutoStr`，同样支持直接使用 `if (!obj)` 或 `if (obj)` 快速判断内容是否为空。
- **混合编码拼接支持**：重载了 `inline StrX operator+(c_StrX a, c_StrX b)`，支持不同编码/类型的文本直接使用 `+` 拼接。例如，可以直接编写 `"让W版字符串" + strw + "可直接拼接A版字符串"`，同理，A版字符串也可以直接这样混合拼接，框架会自动处理转码与合并。

#### 💡 典型应用场景

```cpp
// 严格要求传入文本类型，传入其他结构体会直接引发编译错误
StrX 取临时文件名(c_StrX 目录名 = "") {
	const charW* dir = 目录名; // 直接隐式强转出符合 Win32 API 要求的平台指针
	charW tempPath[MAX_PATH] = { 0 };

	if (!dir[0]) {            // 这里能直接写是因为白易字符串内部保证不会空指针
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



### 6. 泛型数组 (Arraybe)

- **栈预分配**：支持 `Arraybe<T, STACK_CAP=0>`。例如 `Arraybe<BR, 4>` 时，不超过 4 个元素则完全在栈上运行，**零堆分配**开销。（STACK_CAP可缺省为0则初始在堆上分配）

- **初始构造**：`Arraybe(int initCount = 0, int capacity = 0, int increment = 5)` 

  ​                   `Arraybe(std::initializer_list<T_ELE> arr, int increment = 5)` 

- **常用方法**：`push()`, `pop()`, `clear()`, `insert()`, `del()`, `find()`；获取成员数使用 **`count`** 属性。

- **遍历支持**：支持 `for (auto& item : arr)` 现代 C++ 的 Range-for 遍历

- **文本分割**：`Split(src, BR(","), arr)`；或无数组开销的惰性遍历 `for(BR v : SplitV<StrA>(src, ","))`。

#### 💡 基础使用示例

```cpp
Arraybe<StrA> list;
list.push("Apple");
list.push("Banana");

// 获取数量
print("Count:", list.count);

// 插入与删除
list.insert(1, "Orange");
list.del(0); // 删除第一个元素 ("Apple")

// 遍历
for (auto& fruit : list) {
    print("Fruit:", fruit);
}
```



### 7. 哈希映射容器 (HashTbe)

- **性能卓越**：基于伪随机变种的开放寻址法的极速映射容器，核心算法源于 Python 字典（Dict）。
- **支持泛型键**：键（Key）可以是 `StrA`, `StrW`, `int`, `Bytes` 甚至 `BR` 视图（如 `map[BR("Token")] = value`）。
- **常用操作**：
  - **读写**：`map[key] = value`
  - **查找**：`ValueT* p = map.find(key)`，未找到返回 `nullptr`。
  - **删除**：`map.del(key)`，删除成功返回 `true`。
  - **属性**：通过 `map.count` 属性获取元素个数，`map._capacity()` 获取当前容量。
  - **空间整理与缩容**：`map.compact()` 整理墓碑槽位，释放空闲空间。
- **遍历支持**：支持 现代 C++ 的 Range-for 遍历，遍历迭代器为包含 `key` 和 `value` 的节点。


#### 💡 基础使用示例

```cpp
HashTbe<StrA, int> scores;
scores["Alice"] = 95;
scores["Bob"] = 80;

// 查找
if (int* p = scores.find("Alice")) {
    print("Alice score:", *p);
}

// 删除
scores.del("Bob");

// 遍历
for (auto& node : scores) {
    print(node.key, "->", node.value);
}
```




### 8. 可空值与引用容器 (NilOpt / 可空)

白易核心提供的轻量级中文别名为“可空”类的可选容器（类似 `std::optional`），表示值是否存在。空值状态用全局常量 `nil`（中文别名 `空`）表示。

- **应用场景 1：可选的函数参数**
  当函数参数可以不传或传空时使用。可直接与 `nil` / `空` 比较，或者判断是否具有有效值。
  ```cpp
  void log(int msg, 可空<int> tag = nil) {
      if (tag != 空) {
          // 处理带 tag 的逻辑（当中可直接隐式转为 int& 使用）
      }
  }
  ```
- **应用场景 2：可选的出/入参 (引用/常量引用传递)**
  - **`NilOpt<T&>`**：用于传递可空的非常量引用作为出参。若调用者不需要该参数，传入 `nil` 即可。
  - **`NilOpt<const T&>`**：用于常量引用参数。采用**双态持有**设计：若传入已有的 `const T&` 实参则仅保存其指针（零拷贝）；若是字符串指针（仅 `const char* / const charW*`，限支持构造/赋值的 `T` 类型），则利用内部 `val` 存储并绑定，以支持直接转换到 `StrA` 等实体容器类型进行存储（注：对 `const` 字符串指针，`StrA` 等实体本身的单参构造即为引用构造，不发生内存拷贝）。
- **核心操作快捷方式**：
  - **判断**：直接用 `== nil` 或 `!= nil`。
  - **取值**：可直接隐式转换为原类型（如 `T&`），或使用重载的 `operator&()` 获取内部指针。
  - **兜底**：使用 `opt.OR(default_val)` 在值为空时返回默认兜底值。





------

### 附录：

#### BytesStr.hpp 提供的转码工具函数速查

```cpp
#define AtoW  U8toW    // A(U8) -> UTF-16
#define WtoA  WtoU8    // UTF-16 -> A(U8)
#define AtoU8(str) str  // A -> U8（直接返回，等价）
#define U8toA(str) str  // U8 -> A（直接返回，等价）

StrW GBKtoW(const StrA& str);    // GBK → UTF-16
StrA WtoGBK(const StrW& wstr);   // UTF-16 → GBK
StrU8 WtoU8(const StrW& wstr);   // UTF-16 → UTF-8
StrW  U8toW(const StrU8& str);   // UTF-8 → UTF-16
StrU8 GBKtoU8(const StrA& str);  // GBK → UTF-8
StrA  U8toGBK(const StrU8& str); // UTF-8 → GBK

/**平台默认字符串类型：Windows 默认 StrW，Linux 默认 StrA**/
#ifdef _WIN32
using StrPlat = StrW;
#else
using StrPlat = StrA;
#endif

StrPlat U8ToPlat(const StrU8& str){
#ifdef _WIN32
	return U8toW(str);
#else
	return str;
#endif
}
StrU8 PlatToU8(const StrPlat& str)
{
#ifdef _WIN32
	return WtoU8(str);
#else
	return str;
#endif
}

// ----------------------------------------------------
// 【万能数值与文本互转系列】 (包含基于平台和编码的分流)
// ----------------------------------------------------

// 1. 数值或字节集 -> 文本拷贝
StrA ToStr(int / float / double / const Bytes& / ...);      // 转为 StrA (UTF-8)
template<class T=W> StrW ToStr(int / float / ...);          // 转为 StrW (需显式带 <W> 触发)
template<typename T> StrPlat ToStrPlat(const T& a);         // 智能转为当前平台格式 (Win32->W, Linux->A)

// 2. 文本 -> 数值解析 (支持传入 char* 或 charW* 隐式转换)
int ToInt(const char* s);       // 文本 -> int
short ToShort(const char* s);   // 文本 -> short
int64 ToLong(const char* s);    // 文本 -> int64
float ToFloat(const char* s);   // 文本 -> float
double ToDouble(const char* s); // 文本 -> double

// 3. 强转字节集拷贝
Bytes ToBytes(const char* / StrA / StrW / Bytes...); // 任意文本/内存强拷为独立拥有模式的 Bytes 容器
```

> **注意**：传统的 `GBKtoW` / `WtoGBK` 系列函数依然存在，但已不再通过 `AtoW` 等别名引出，仅用于显式处理旧版 GBK 数据的特殊场景。

> **设计哲学**：白易C++框架现在强制约定 **A版字面量 == 字节兼容U8版**。
>
> 1. 过去 Win32 上的 A 版通常绑定到 GBK，但白易为了跨平台一致性，现在通过编译器设置（/utf-8配置）和运行时初始化（`SetConsoleOutputCP`），使 `StrA` 具备了以 UTF-8 作为硬编码编译的条件。
> 2. 这意味着开发者编写 A版`取文本长度("中文")` 时，内部处理的就是 UTF-8 字节流，无需再考虑 GBK 转换的琐事。