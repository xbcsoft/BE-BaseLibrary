#include "stdafx.h"
#include <shlobj.h>

StrW _取命令行(Arraybe<StrW>& 命令行数组) {
	命令行数组.clear();
	int argc = 0; LPCWSTR cmd = GetCommandLineW();
	LPWSTR* argv = CommandLineToArgvW(cmd, &argc);
	if (argv) {
		//i从1开始不包括自身
		for (int i = 1; i < argc; i++) {
			StrW s;
			s = argv[i];
			命令行数组.push(s);
		}
		LocalFree(argv);
	}

	bool inQuotes = false;
	while (*cmd) {
		if (*cmd == L'"') inQuotes = !inQuotes;
		else if (*cmd == L' ' && !inQuotes) break;
		cmd++;
	}
	while (*cmd == L' ') cmd++;

	StrW ret;
	ret = cmd;
	return ret;
}
template<class StrT = W>
StrW 取命令行(Arraybe<StrW>& 命令行数组) { return _取命令行(命令行数组); }

StrA 取命令行(Arraybe<StrA>& 命令行数组) {
	命令行数组.clear();
	Arraybe<StrW> arrW;
	StrW retW = _取命令行(arrW);
	for (auto& wStr : arrW) {
		命令行数组.push(WtoU8(wStr));
	}
	return WtoU8(retW);
}

StrX 取运行目录() {
	static StrW cache;
	if (cache.len() > 0) return cache;
	wchar_t path[MAX_PATH];
	GetModuleFileNameW(NULL, path, MAX_PATH);
	wchar_t* lastSlash = wcsrchr(path, L'\\');
	if (lastSlash) {
		*lastSlash = L'\0';
	}
	cache = path;
	return cache;
}

StrX 取当前目录() {
	wchar_t path[MAX_PATH];
	DWORD res = GetCurrentDirectoryW(MAX_PATH, path);
	if (res > 0 && res < MAX_PATH) {
		StrW ret;
		ret = path;
		return ret;
	}
	return StrW();
}

bool 置当前目录(c_StrX 新当前目录) {
	return SetCurrentDirectoryW((LPCWSTR)新当前目录) != 0;
}

StrW 取模块路径(void* 模块句柄 = NULL) {
	wchar_t path[MAX_PATH];
	GetModuleFileNameW((HMODULE)模块句柄, path, MAX_PATH);
	StrW ret;
	ret = path;
	return ret;
}

StrX 取执行文件名() {
	static StrW cache;
	if (cache.len() > 0) return cache;
	wchar_t path[MAX_PATH];
	GetModuleFileNameW(NULL, path, MAX_PATH);
	wchar_t* lastSlash = wcsrchr(path, L'\\');
	if (lastSlash) cache = lastSlash + 1;
	return cache;
}

/**读环境变量
 * @param 环境变量名称
 * @param 环境类别=2 0系统级，1用户级，2进程级
 * @return
 */
StrX 读环境变量(c_StrX 环境变量名称, int 环境类别 = 2) {
	if (环境类别 == 0 || 环境类别 == 1) {
		HKEY hKey = (环境类别 == 0) ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER;
		LPCWSTR subKey = (环境类别 == 0) ? L"System\\CurrentControlSet\\Control\\Session Manager\\Environment" : L"Environment";
		HKEY hKeyOpen;
		if (RegOpenKeyExW(hKey, subKey, 0, KEY_QUERY_VALUE, &hKeyOpen) == ERROR_SUCCESS) {
			WCHAR buf[32768] = { 0 };
			DWORD type = REG_SZ;
			DWORD size = sizeof(buf);
			if (RegQueryValueExW(hKeyOpen, (LPCWSTR)环境变量名称, NULL, &type, (LPBYTE)buf, &size) == ERROR_SUCCESS) {
				RegCloseKey(hKeyOpen);
				StrW ret;
				ret = buf;
				return ret;
			}
			RegCloseKey(hKeyOpen);
		}
		return "";
	}
	// Fallback process env
	wchar_t pBuf[32768];
	DWORD res = GetEnvironmentVariableW((LPCWSTR)环境变量名称, pBuf, 32768);
	if (res > 0 && res < 32768) {
		StrW ret;
		ret = pBuf;
		return ret;
	}
	return "";
}

/**写环境变量
 * @param 环境变量名称
 * @param 欲写入内容
 * @param 环境类别=2 0系统级，1用户级，2进程级
 * @return
 */
bool 写环境变量(c_StrX 环境变量名称, c_StrX 欲写入内容, int 环境类别 = 2) {
	if (环境类别 == 0 || 环境类别 == 1) {
		HKEY hKey = (环境类别 == 0) ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER;
		LPCWSTR subKey = (环境类别 == 0) ? L"System\\CurrentControlSet\\Control\\Session Manager\\Environment" : L"Environment";
		HKEY hKeyOpen;
		if (RegOpenKeyExW(hKey, subKey, 0, KEY_SET_VALUE, &hKeyOpen) == ERROR_SUCCESS) {
			bool ok = false;
			if (!欲写入内容) {
				ok = (RegDeleteValueW(hKeyOpen, (LPCWSTR)环境变量名称) == ERROR_SUCCESS) || (GetLastError() == ERROR_FILE_NOT_FOUND);
			} else {
				ok = (RegSetValueExW(hKeyOpen, (LPCWSTR)环境变量名称, 0, REG_SZ, (const BYTE*)((LPCWSTR)欲写入内容), (DWORD)((欲写入内容.len() + 1) * sizeof(WCHAR))) == ERROR_SUCCESS);
			}
			RegCloseKey(hKeyOpen);
			// Update Process Env
			SetEnvironmentVariableW((LPCWSTR)环境变量名称, !欲写入内容 ? NULL : (LPCWSTR)欲写入内容);

			DWORD_PTR res;
			SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"Environment", SMTO_ABORTIFHUNG, 1000, &res);
			return ok;
		}
		return false;
	}
	return SetEnvironmentVariableW((LPCWSTR)环境变量名称, !欲写入内容 ? NULL : (LPCWSTR)欲写入内容) != 0;
}

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
StrX 取特定目录(int 欲获取目录类型 = 目录::系统桌面) {
	wchar_t path[MAX_PATH] = { 0 };
	bool ok = false;

	if (欲获取目录类型 == 0) {
		欲获取目录类型 = 3;
	}

	if (欲获取目录类型 >= 1 && 欲获取目录类型 <= 8) {
		int csidls[] = { 5, 6, 16, 20, 11, 2, 7, 26 };
		int csidl = csidls[欲获取目录类型 - 1];
		if (SHGetSpecialFolderPathW(NULL, path, csidl, FALSE)) {
			ok = true;
		}
	}
	else if (欲获取目录类型 == 9) {
		if (GetWindowsDirectoryW(path, MAX_PATH) > 0) {
			ok = true;
		}
	}
	else if (欲获取目录类型 == 10) {
		if (GetSystemDirectoryW(path, MAX_PATH) > 0) {
			ok = true;
		}
	}
	else if (欲获取目录类型 == 11) {
		if (GetTempPathW(MAX_PATH, path) > 0) {
			ok = true;
		}
	}
	else if (欲获取目录类型 == 12) {
		if (SHGetSpecialFolderPathW(NULL, path, 32, FALSE)) {
			ok = true;
		}
	}

	if (ok) {
		int len = (int)wcslen(path);
		if (len > 0 && (path[len - 1] == L'\\' || path[len - 1] == L'/')) {
			path[len - 1] = L'\0';
		}
		StrW ret;
		ret = path;
		return ret;
	}
	return "";
}
