#pragma once
#include "stdafx.h"

/** 注册表类
 * 封装了常用的 Windows 注册表读写与管理功能
 */
class 注册表
{
public:
	HKEY hKey = NULL; // 当前打开的注册表句柄

	注册表();

	~注册表();

	// 内部辅助：处理 @ 符号，将其转为 NULL 以访问默认值
	static const charW* _GetRealName(const charW* 项名称);

	/** 打开注册表项
	 * @param 根键 HKEY_CLASSES_ROOT, HKEY_LOCAL_MACHINE, HKEY_CURRENT_USER 等
	 * @param 子项 路径字符串
	 * @param 仅读取 是否以只读方式打开，默认为 false (读写)
	 * @return 成功返回 true
	 */
	bool 打开项(HKEY 根键, const charW* 子项, bool 仅读取 = false);

	/** 创建或打开注册表项
	 * @param 根键 HKEY_CLASSES_ROOT, HKEY_LOCAL_MACHINE, HKEY_CURRENT_USER 等
	 * @param 子项 路径字符串
	 * @return 成功返回 true
	 */
	bool 创建项(HKEY 根键, const charW* 子项);

	/** 关闭当前打开的项 */
	bool 关闭项();

	StrW 读文本值(const charW* 项名称);

	/** 写入键值 (自动识别类型)
	 * @param 键名称 键值名称
	 * @param 值 支持 const charW*, StrW, uint, Bytes
	 * @return 成功返回 true
	 */
	template<typename T>
	bool 写键值(const charW* 项名称, const T& 值) {
		if (!hKey) return false;
		const charW* realName = _GetRealName(项名称);
		if constexpr (be::same<T, const charW*> || be::same<T, charW*> || be::same<T, StrW>) {
			if (!值) return false;
			size_t len = strlenW(值);
			return RegSetValueExW(hKey, realName, 0, REG_SZ, (const BYTE*)(const charW*)值, (DWORD)(len + 1) * 2) == ERROR_SUCCESS;
		} else if constexpr (be::same<T, uint> || be::same<T, int>) {
			DWORD val = (DWORD)值;
			return RegSetValueExW(hKey, realName, 0, REG_DWORD, (const BYTE*)&val, sizeof(DWORD)) == ERROR_SUCCESS;
		} else if constexpr (be::same<T, Bytes>) {
			return RegSetValueExW(hKey, realName, 0, REG_BINARY, (const BYTE*)值.buf, (DWORD)值.size) == ERROR_SUCCESS;
		}
		return false;
	}

	/** 读取整数值 (REG_DWORD) */
	uint 读整数值(const charW* 项名称);

	/** 读取字节集值 (REG_BINARY) */
	Bytes 读字节集值(const charW* 项名称);

	/** 删除指定的键值 */
	bool 删除键(const charW* 项名称);

	/** 删除子项 */
	bool 删除项(const charW* 子项);

	/** 判断子项是否存在 */
	bool 子项是否存在(const charW* 子项);

	/** 判断子键是否存在 (支持 @ 表示默认值) */
	bool 子键是否存在(const charW* 键名称);

	bool _获取子项(int 索引, StrW& 子项名称);

	/**枚举所有子项 (通过回调)
	 * @param 项名 签名: bool(*)(StrW& 名) 返回 false 则停止
	 */
	void 枚举子项(bool(*回调)(StrW& 项名));

	bool _获取子键(int 索引, StrW& 值名称, uint& 类型);

	/**枚举所有键名 (通过回调)
	 * @param 键名 签名: bool(*)(StrW& 名, uint 类型) 返回 false 则停止
	 */
	void 枚举键名(bool(*回调)(StrW& 键名, uint 类型));
};


#pragma region 全局便捷函数

bool _SplitRegPath(const charW* 全路径, HKEY& out根键, StrW& out子项, StrW& out项名称);

/**写注册表值
 * @param 全路径 支持结尾"@"表示默认键
 * @param 值
 * @return
 */
template<typename T>
bool 写注册表值(const charW* 全路径, const T& 值) {
	HKEY 根键; StrW 子项, 项名;
	if (!_SplitRegPath(全路径, 根键, 子项, 项名)) return false;
	注册表 reg;
	if (reg.创建项(根键, 子项)) {
		return reg.写键值(项名, 值);
	}
	return false;
}

uint 读注册表整数(const charW* 全路径);

StrW 读注册表文本(const charW* 全路径);

Bytes 读注册表字节集(const charW* 全路径);

bool 注册表是否存在(const charW* 全路径);

bool 删除注册表(const charW* 全路径);

bool Wow64重定向_禁用(PVOID* oldValue = nullptr);

bool Wow64重定向_启用(PVOID* oldValue = nullptr);
#pragma endregion
