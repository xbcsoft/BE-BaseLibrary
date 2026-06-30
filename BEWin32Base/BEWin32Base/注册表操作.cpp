#include "stdafx.h"

/** 注册表类
 * 封装了常用的 Windows 注册表读写与管理功能
 */
class EXP 注册表
{
public:
	HKEY hKey = NULL; // 当前打开的注册表句柄

	注册表() : hKey(NULL) {}

	~注册表() {
		关闭项();
	}

	// 内部辅助：处理 @ 符号，将其转为 NULL 以访问默认值
	static const charW* _GetRealName(const charW* 项名称) {
		if (项名称 && 项名称[0] == L'@' && 项名称[1] == L'\0') return nullptr;
		return 项名称;
	}

	/** 打开注册表项
	 * @param 根键 HKEY_CLASSES_ROOT, HKEY_LOCAL_MACHINE, HKEY_CURRENT_USER 等
	 * @param 子项 路径字符串
	 * @param 仅读取 是否以只读方式打开，默认为 false (读写)
	 * @return 成功返回 true
	 */
	bool 打开项(HKEY 根键, const charW* 子项, bool 仅读取 = false) {
		关闭项();
		REGSAM samDesired = 仅读取 ? KEY_READ : KEY_ALL_ACCESS;
		return RegOpenKeyExW(根键, 子项, 0, samDesired, &hKey) == ERROR_SUCCESS;
	}

	/** 创建或打开注册表项
	 * @param 根键 HKEY_CLASSES_ROOT, HKEY_LOCAL_MACHINE, HKEY_CURRENT_USER 等
	 * @param 子项 路径字符串
	 * @return 成功返回 true
	 */
	bool 创建项(HKEY 根键, const charW* 子项) {
		关闭项();
		return RegCreateKeyExW(根键, 子项, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL) == ERROR_SUCCESS;
	}

	/** 关闭当前打开的项 */
	bool 关闭项() {
		if (hKey) {
			LONG ret = RegCloseKey(hKey);
			hKey = NULL;
			return ret == ERROR_SUCCESS;
		}
		return false;
	}

	StrW 读文本值(const charW* 项名称) {
		if (!hKey) return StrW();
		const charW* realName = _GetRealName(项名称);
		DWORD type = 0;
		DWORD size = 0;
		if (RegQueryValueExW(hKey, realName, NULL, &type, NULL, &size) != ERROR_SUCCESS) return StrW();
		if (type != REG_SZ && type != REG_EXPAND_SZ) return StrW();

		StrW ret;
		ret.reset(size / 2); // size 是字节数，含空终止
		if (RegQueryValueExW(hKey, realName, NULL, &type, (LPBYTE)ret._buf(), &size) == ERROR_SUCCESS) {
			if (size >= 2) ret.bytes.size = size - 2;
			else ret.bytes.size = 0;
			return ret;
		}
		return StrW();
	}

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
	uint 读整数值(const charW* 项名称) {
		if (!hKey) return 0;
		const charW* realName = _GetRealName(项名称);
		DWORD type = 0;
		DWORD val = 0;
		DWORD size = sizeof(DWORD);
		if (RegQueryValueExW(hKey, realName, NULL, &type, (LPBYTE)&val, &size) == ERROR_SUCCESS) {
			if (type == REG_DWORD) return (uint)val;
		}
		return 0;
	}

	/** 读取字节集值 (REG_BINARY) */
	Bytes 读字节集值(const charW* 项名称) {
		if (!hKey) return Bytes();
		const charW* realName = _GetRealName(项名称);
		DWORD type = 0;
		DWORD size = 0;
		if (RegQueryValueExW(hKey, realName, NULL, &type, NULL, &size) != ERROR_SUCCESS) return Bytes();
		if (type != REG_BINARY) return Bytes();

		Bytes ret(size);
		if (RegQueryValueExW(hKey, realName, NULL, &type, (LPBYTE)ret.buf, &size) == ERROR_SUCCESS) {
			ret.size = size;
			return ret;
		}
		return Bytes();
	}

	/** 删除指定的键值 */
	bool 删除键(const charW* 项名称) {
		if (!hKey) return false;
		return RegDeleteValueW(hKey, _GetRealName(项名称)) == ERROR_SUCCESS;
	}

	/** 删除子项 */
	bool 删除项(const charW* 子项) {
		if (!hKey) return false;
		return RegDeleteTreeW(hKey, 子项) == ERROR_SUCCESS;
	}

	/** 判断子项是否存在 */
	bool 子项是否存在(const charW* 子项) {
		if (!hKey) return false;
		HKEY hTemp = NULL;
		if (RegOpenKeyExW(hKey, 子项, 0, KEY_READ, &hTemp) == ERROR_SUCCESS) {
			RegCloseKey(hTemp);
			return true;
		}
		return false;
	}

	/** 判断子键是否存在 (支持 @ 表示默认值) */
	bool 子键是否存在(const charW* 键名称) {
		if (!hKey) return false;
		const charW* realName = _GetRealName(键名称);
		return RegQueryValueExW(hKey, realName, NULL, NULL, NULL, NULL) == ERROR_SUCCESS;
	}

	bool _获取子项(int 索引, StrW& 子项名称) {
		if (!hKey) return false;
		WCHAR name[256];
		DWORD nameLen = 256;
		if (RegEnumKeyExW(hKey, 索引, name, &nameLen, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {
			子项名称 = name;
			return true;
		}
		return false;
	}

	/**枚举所有子项 (通过回调)
	 * @param 项名 签名: bool(*)(StrW& 名) 返回 false 则停止
	 */
	void 枚举子项(bool(*回调)(StrW& 项名)) {
		if (!回调) return;
		StrW name;
		for (int i = 0; _获取子项(i, name); ++i) {
			if (!回调(name)) break;
		}
	}

	bool _获取子键(int 索引, StrW& 值名称, uint& 类型) {
		if (!hKey) return false;
		WCHAR name[256];
		DWORD nameLen = 256;
		DWORD type = 0;
		if (RegEnumValueW(hKey, 索引, name, &nameLen, NULL, &type, NULL, NULL) == ERROR_SUCCESS) {
			值名称 = name;
			类型 = (uint)type;
			return true;
		}
		return false;
	}

	/**枚举所有键名 (通过回调)
	 * @param 键名 签名: bool(*)(StrW& 名, uint 类型) 返回 false 则停止
	 */
	void 枚举键名(bool(*回调)(StrW& 键名, uint 类型)) {
		if (!回调) return;
		StrW name; uint type;
		for (int i = 0; _获取子键(i, name, type); ++i) {
			if (!回调(name, type)) break;
		}
	}
};


#pragma region 全局便捷函数

bool _SplitRegPath(const charW* 全路径, HKEY& out根键, StrW& out子项, StrW& out项名称) {
	if (!全路径) return false;
	const charW* firstSep = 全路径;
	while (*firstSep && *firstSep != L'\\' && *firstSep != L'/') firstSep++;
	if (!*firstSep) return false; // 必须至少有一个分隔符（根键\项名）

	StrW rootName(全路径, firstSep - 全路径);
	const charW* lastSep = firstSep;
	const charW* p = firstSep + 1;
	while (*p) {
		if (*p == L'\\' || *p == L'/') lastSep = p;
		p++;
	}

	if (lastSep == firstSep) {
		out子项 = _W("");
		out项名称 = firstSep + 1;
	} else {
		out子项 = StrW(firstSep + 1, lastSep - firstSep - 1);
		out项名称 = lastSep + 1;
	}

	if (rootName == _W("HKEY_CURRENT_USER") || rootName == _W("HKCU")) out根键 = HKEY_CURRENT_USER;
	else if (rootName == _W("HKEY_LOCAL_MACHINE") || rootName == _W("HKLM")) out根键 = HKEY_LOCAL_MACHINE;
	else if (rootName == _W("HKEY_CLASSES_ROOT") || rootName == _W("HKCR")) out根键 = HKEY_CLASSES_ROOT;
	else if (rootName == _W("HKEY_USERS") || rootName == _W("HKU")) out根键 = HKEY_USERS;
	else if (rootName == _W("HKEY_CURRENT_CONFIG") || rootName == _W("HKCC")) out根键 = HKEY_CURRENT_CONFIG;
	else return false;
	return true;
}

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

uint 读注册表整数(const charW* 全路径) {
	HKEY 根键; StrW 子项, 项名;
	if (!_SplitRegPath(全路径, 根键, 子项, 项名)) return 0;
	注册表 reg;
	if (reg.打开项(根键, 子项, true)) {
		return reg.读整数值(项名);
	}
	return 0;
}

StrW 读注册表文本(const charW* 全路径) {
	HKEY 根键; StrW 子项, 项名;
	if (!_SplitRegPath(全路径, 根键, 子项, 项名)) return StrW();
	注册表 reg;
	if (reg.打开项(根键, 子项, true)) {
		return reg.读文本值(项名);
	}
	return StrW();
}

Bytes 读注册表字节集(const charW* 全路径) {
	HKEY 根键; StrW 子项, 项名;
	if (!_SplitRegPath(全路径, 根键, 子项, 项名)) return Bytes();
	注册表 reg;
	if (reg.打开项(根键, 子项, true)) {
		return reg.读字节集值(项名);
	}
	return Bytes();
}

bool 注册表是否存在(const charW* 全路径) {
	HKEY 根键; StrW 子项, 项名;
	if (!_SplitRegPath(全路径, 根键, 子项, 项名)) return false;

	注册表 reg;
	if (reg.打开项(根键, 子项, true)) {
		// 1. 判断是否为子项
		if (reg.子项是否存在(项名)) return true;
		// 2. 判断是否为子键
		if (reg.子键是否存在(项名)) return true;
	} else if (子项.len() == 0) {
		// 处理根键直属项的情况
		reg.hKey = 根键;
		bool exists = (reg.子项是否存在(项名)) || reg.子键是否存在(项名);
		reg.hKey = NULL; // 别物理关闭根键
		return exists;
	}

	return false;
}

bool 删除注册表(const charW* 全路径) {
	HKEY 根键; StrW 子项, 项名;
	if (!_SplitRegPath(全路径, 根键, 子项, 项名)) return false;

	注册表 reg;
	if (reg.打开项(根键, 子项, false)) {
		if (reg.子项是否存在(项名)) return reg.删除项(项名);
		if (reg.子键是否存在(项名)) return reg.删除键(项名);
	} else if (子项.len() == 0) {
		reg.hKey = 根键;
		bool ret = false;
		if (reg.子项是否存在(项名)) ret = reg.删除项(项名);
		else if (reg.子键是否存在(项名)) ret = reg.删除键(项名);
		reg.hKey = NULL;
		return ret;
	}
	return false;
}

bool Wow64重定向_禁用(PVOID* oldValue = nullptr)
{
	PVOID dummy;
	return Wow64DisableWow64FsRedirection(oldValue ? oldValue : &dummy) != 0;
}

bool Wow64重定向_启用(PVOID* oldValue = nullptr)
{
	PVOID dummy;
	return Wow64RevertWow64FsRedirection(oldValue ? oldValue : &dummy) != 0;
}
#pragma endregion
