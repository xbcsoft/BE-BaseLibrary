#pragma once
#include "stdafx.h"
#include "Win32日期时间.h"
#include <shellapi.h> //for SHFileOperationW

int64 取文件大小(c_StrX 文件路径);

bool 置文件属性(c_StrX 文件路径, int 属性值);

// 内部辅助：将"A"、"C"等首字母转成标准的盘符根目录格式 "X:\\"
static bool _MakeDrivePath(const charW* driveStr, charW outPath[4]);

// 内部辅助：结合目录路径与源文件名（或源目录名），生成包含同名的全路径
static StrW _MakeDestPathWithSameName(const charW* 源路径, const charW* 目标目录);


/**取磁盘总空间
 * @param 磁盘驱动器字符 类似"A"、"C"等，只取用给定文本的第一个字符。
 * @return 成功返回以 1024 字节(KB)为单位的指定磁盘全部空间。如果失败返回-1。
 */
int64 取磁盘总空间(c_StrX 磁盘驱动器字符);

/**取磁盘剩余空间
 * @param 磁盘驱动器字符 类似"A"、"C"等，只取用给定文本的第一个字符
 * @return 成功返回以 1024 字节(KB)为单位的指定磁盘现行剩余空间。如果失败返回-1。
 */
int64 取磁盘剩余空间(c_StrX 磁盘驱动器字符);

/**取磁盘卷标
 * @param 磁盘驱动器字符 类似"A"、"C"等，只取用给定文本的第一个字符。
 * @return 返回指定磁盘的卷标文本。如果失败返回空文本。
 */
StrX 取磁盘卷标(c_StrX 磁盘驱动器字符);

/**置磁盘卷标
 * @param 磁盘驱动器字符 类似"A"、"C"等，只取用给定文本的第一个字符。省略则取缺省驱动器。
 * @param 欲置入的卷标文本 欲置入的卷标
 * @return 成功返回真，失败返回假。
 */
bool 置磁盘卷标(c_StrX 磁盘驱动器字符, c_StrX 欲置入的卷标文本);


/**创建目录
 * @param 欲创建的目录名称 只需要单级目录，若上级不存在会失败
 * @return 成功返回真，失败返回假。
 */
bool 创建目录(c_StrX 欲创建的目录名称);

/**删除目录
 * @param 欲删除的目录名称 将删除目录及其内部所有的子目录和下属文件
 * @return 成功返回真，失败返回假。
 */
bool 删除目录(c_StrX 欲删除的目录名称);

/**复制目录
 * @param 被复制的目录名称 源目录
 * @param 复制到的目录名称 目标目录
 * @return 成功返回真，失败返回假。
 */
bool 复制目录(c_StrX 被复制的目录名称, c_StrX 复制到的目录名称);

/**移动目录默认不支持覆盖目标已有的同名目录也不支持跨分区
 * @param 被移动的目录名称 源目录
 * @param 移动到的目录 目标目录
 * @param 允许跨分区且覆盖=false 默认假。开启跨分区将调用SH执行转移，若目标存在会直接强制合并/覆盖。由于MoveFile自身并不能在跨卷时移动非空目录。
 * @return 成功返回真，失败返回假。
 */
bool 移动目录(c_StrX 被移动的目录名称, c_StrX 移动到的目录, bool 允许跨分区且覆盖 = false);

/**复制文件
 * @param 被复制的文件名 源文件
 * @param 复制到的文件名 目标文件
 * @return 成功返回真，失败返回假。
 */
bool 复制文件(c_StrX 被复制的文件名, c_StrX 复制到的文件名);

/**移动文件默认不支持覆盖目标已有的文件也不支持跨分区
 * @param 被移动的文件 源文件
 * @param 移动到的文件 目标文件
 * @param 允许跨分区且覆盖=false 若目标存在会直接强制覆盖。
 * @return 成功返回真，失败返回假.
 */
bool 移动文件(c_StrX 被移动的文件, c_StrX 移动到的文件, bool 允许跨分区且覆盖 = false);

/**复制文件到目录
 * @param 被复制的文件名 源文件
 * @param 欲复制到的目录 目标目录
 * @return 成功返回真，失败返回假。
 */
bool 复制文件到目录(c_StrX 被复制的文件名, c_StrX 欲复制到的目录);

/**移动文件到目录默认不支持覆盖目标已有的文件
 * @param 被移动的文件 源文件
 * @param 欲移动到的目录 目标目录
 * @param 允许跨分区且覆盖=false 若目标存在会直接强制覆盖。
 * @return 成功返回真，失败返回假。
 */
bool 移动文件到目录(c_StrX 被移动的文件, c_StrX 欲移动到的目录, bool 允许跨分区且覆盖 = false);

/**复制目录到目录
 * @param 被复制的目录名称 源目录
 * @param 欲复制到的目录 目标目录
 * @return 成功返回真，失败返回假。
 */
bool 复制目录到目录(c_StrX 被复制的目录名称, c_StrX 欲复制到的目录);

/**移动目录到目录默认不支持覆盖目标已有的同名目录
 * @param 被移动的目录名称 源目录
 * @param 欲移动到的目录 目标目录
 * @param 允许跨分区且覆盖=false 默认假。开启跨分区将调用SH执行转移，若目标存在会直接强制合并/覆盖。
 * @return 成功返回真，失败返回假。
 */
bool 移动目录到目录(c_StrX 被移动的目录名称, c_StrX 欲移动到的目录, bool 允许跨分区且覆盖 = false);

/**删除文件
 * @param 欲删除的文件名 欲删除文件的全路径
 * @return 成功返回真，失败返回假。
 */
bool 删除文件(c_StrX 欲删除的文件名);


/**文件是否存在
 * @param 欲测试的文件名称
 * @return 真实存在则返回真，否则返回假。
 */
bool 文件是否存在(c_StrX 欲测试的文件名称);

/**取文件时间
 * @param 文件名 目标文件路径
 * @param 时间类型=0 0最后修改时间，1创建时间，2最后访问时间
 * @return 成功返回Windows文件(最后修改)时间戳即FILETIME整数形式；失败返回0
 */
日期时间型 取文件时间(c_StrX 文件名, int 时间类型 = 0);

/**取文件属性
 * @param 文件名 目标文件路径
 * @return 失败返回 -1；成功返回包含诸如 #只读文件(1) 掩码叠加的整数
 */
int 取文件属性(c_StrX 文件名);

/**取临时文件名
 * @param 目录名 指定建立在哪，若为 nullptr 或空文本将缺省使用系统默认缓存目录
 * @return 在目录内确定不存在的一条含有 '.TMP' 扩展名的全文件访问路径文本。
 */
StrX 取临时文件名(c_StrX 目录名 = "");

/**目录是否存在
 * @param 欲测试的目录名称
 * @return 真实存在则返回真，否则返回假。
 */
bool 目录是否存在(c_StrX 欲测试的目录名称);


#pragma region 寻找文件扩展部分

class 寻找文件
{
public:
	enum {
		只读文件 = FILE_ATTRIBUTE_READONLY,  // 1
		隐藏文件 = FILE_ATTRIBUTE_HIDDEN,    // 2
		系统文件 = FILE_ATTRIBUTE_SYSTEM,    // 4
		子目录 = FILE_ATTRIBUTE_DIRECTORY,   // 16
		存档文件 = FILE_ATTRIBUTE_ARCHIVE,   // 32
		正常文件 = FILE_ATTRIBUTE_NORMAL     // 128
	};

	HANDLE           hFind_ = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATAW findData_ = { 0 };
	int              attr_ = 0;

	寻找文件() = default;

	~寻找文件();

	// 禁止拷贝（持有内核句柄）
	寻找文件(const 寻找文件&) = delete;
	寻找文件& operator=(const 寻找文件&) = delete;

	// 允许移动
	寻找文件(寻找文件&& other);

	寻找文件& operator=(寻找文件&& other);

	bool _checkAttr(DWORD fileAttr) const;

	/**开始寻找  (首次调用，指定通配符路径与属性)
	 * @param 欲寻找的文件或目录名称 可含*、?通配符，例如 L"C:\\dir\\*.txt"
	 * @param 欲寻找文件的属性=0 例如 #子目录 等常量，默认隐藏/系统等文件会被忽略
	 * @return 返回首个匹配的文件名；若无匹配返回空文本。
	 */
	StrX 开始寻找(c_StrX 欲寻找的文件或目录名称, int 欲寻找文件的属性 = 0);

	/**下一个  (后续调用，继续检索下一个匹配项)
	 * @return 返回下一个匹配的文件名；若已全部检索完毕则返回空文本。
	 */
	StrX 下一个();

	/**关闭  (提前终止搜索并释放内核句柄，析构函数也会自动调用)
	 */
	void 关闭();
};

// 内部：统一递归实现
// 收集=false → 调用 回调，返回false终止; 收集=true → push进 结果数组
template<bool 带完整路径, bool 收集> inline
bool _枚举ImplBase(const StrW& base, bool 是否递归, int 类型,
	bool(*回调)(const StrW&), Arraybe<StrW>& 结果) {

	WIN32_FIND_DATAW fd = { 0 };
	HANDLE h = FindFirstFileW(base + _W("*"), &fd);
	if (h == INVALID_HANDLE_VALUE) return true;

	bool cont = true;
	do {
		bool isDir = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
		StrW name(fd.cFileName); // 引用构造，零拷贝

		if (isDir) {
			if (name == _W(".") || name == _W("..")) continue;
			if (是否递归) {
				cont = _枚举ImplBase<带完整路径, 收集>(base + name + _W("\\"), 是否递归, 类型, 回调, 结果);
				if (!cont) break;
			}
			if (类型 & FILE_ATTRIBUTE_DIRECTORY) {
				StrW item;
				if constexpr (带完整路径) item = base + name; else item = name;
				if constexpr (收集) { 结果.push(item); } else { cont = 回调(item); }
			}
		} else {
			if (类型 & FILE_ATTRIBUTE_NORMAL) {
				StrW item;
				if constexpr (带完整路径) item = base + name; else item = name;
				if constexpr (收集) { 结果.push(item); } else { cont = 回调(item); }
			}
		}
		if (!cont) break;
	} while (FindNextFileW(h, &fd));

	FindClose(h);
	return cont;
}

//白易中使用模板默认会完整保留定义在头文件中生成
//上边模板有再加inline关键字在白易中会被标记剔除
//而后这里还需要显式声明一下此模板
//并作模板显式实例化产生二进制符号

template<bool 带完整路径, bool 收集>
bool _枚举ImplBase(const StrW& base, bool 是否递归, int 类型,
	bool(*回调)(const StrW&), Arraybe<StrW>& 结果);

template bool _枚举ImplBase<false, false>(
	const StrW&, bool, int, bool(*)(const StrW&), Arraybe<StrW>&);

template bool _枚举ImplBase<false, true>(
	const StrW&, bool, int, bool(*)(const StrW&), Arraybe<StrW>&);

template bool _枚举ImplBase<true, false>(
	const StrW&, bool, int, bool(*)(const StrW&), Arraybe<StrW>&);

template bool _枚举ImplBase<true, true>(
	const StrW&, bool, int, bool(*)(const StrW&), Arraybe<StrW>&);


/**文件_枚举
 * @tparam 带完整路径=false 假→回调/数组存纯名称；真→回调/数组存完整路径
 * @tparam 收集=false       假→回调模式，回调返回 false 立即终止；真→收集模式，返回 Arraybe<StrW>
 * @param 目录 起始目录路径
 * @param 是否递归 为真时递归进入所有子目录
 * @param 寻找类型=空 #寻找文件::正常文件 | #寻找文件::子目录 | ... (若为空则默认为 正常文件)
 * @param 回调 bool(*)(const StrW&) — 收集=true 时可省略
 * @return 模板参数<收集>为真返回Arraybe<StrW>，否则返回void
 */
template<bool 带完整路径 = false, bool 收集 = false>
auto 文件_枚举(c_StrX 目录, bool 是否递归, 可空<int> 寻找类型 = 空, bool(*回调)(const StrW&) = nullptr) {
	int 类型 = 寻找类型.OR(寻找文件::正常文件);
	StrW base = 目录;
	if (base.len() > 0) { charW c = ((const charW*)base)[base.len()-1]; if (c != L'\\' && c != L'/') base += _W("\\"); }
	if constexpr (收集) {
		Arraybe<StrW> 结果;
		_枚举ImplBase<带完整路径, true>(base, 是否递归, 类型, nullptr, 结果);
		return 结果;
	} else {
		Arraybe<StrW> _; //回调模式不使用，编译器会优化掉
		_枚举ImplBase<带完整路径, false>(base, 是否递归, 类型, 回调, _);
	}
}
#pragma endregion
