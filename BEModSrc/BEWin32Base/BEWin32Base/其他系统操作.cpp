#include "stdafx.h"

定义_枚举型(MB, int,
	确认 = MB_OK,
	确认取消 = MB_OKCANCEL,
	放弃重试忽略 = MB_ABORTRETRYIGNORE,
	是否取消 = MB_YESNOCANCEL,
	是否 = MB_YESNO,
	重试取消 = MB_RETRYCANCEL,
	取消重试继续 = MB_CANCELTRYCONTINUE,

	错误 = MB_ICONERROR,
	警告 = MB_ICONWARNING,
	询问 = MB_ICONQUESTION,
	提示 = MB_ICONINFORMATION,
	叹号 = MB_ICONEXCLAMATION,

	第一按钮 = MB_DEFBUTTON1,
	第二按钮 = MB_DEFBUTTON2,
	第三按钮 = MB_DEFBUTTON3,
	第四按钮 = MB_DEFBUTTON4,

	ID确认 = IDOK,
	ID取消 = IDCANCEL,
	ID放弃 = IDABORT,
	ID重试 = IDRETRY,
	ID忽略 = IDIGNORE,
	ID是 = IDYES,
	ID否 = IDNO,
	ID继续 = IDCONTINUE
);

/**全局简单封装至MessageBox
 * @param 内容
 * @param 标题="提示："
 * @param 类型=MB_OK MessageBox中原本"MB_"常量或使用MB::白易内置枚举型
 * @param 句柄<可空> 窗口句柄模态载入
 * @return 参考MessageBox返回按钮类型
 */
int 信息框(c_AutoStr 内容, c_AutoStr 标题 = "提示：", MB 类型 = MB_OK, 可空<HWND> 句柄 = 空)
{
	HWND hWnd = (句柄 != 空) ? (HWND)句柄 : GetActiveWindow();
	return MessageBoxW(hWnd, 内容, 标题, 类型);
}

/**运行程序或打开文件
 * @param 文件名 欲运行的程序EXE名、任意后缀文件名、或 http网址链接
 * @param 命令参数=_W("") 运行程序时的命令行参数
 * @param 是否等待=false 是否等待程序运行完毕
 * @param 被运行程序窗口显示方式=2 1隐藏窗口；2普通激活(默认)；3最小化激活；
	4最大化激活；5普通不激活；6最小化不激活
 * @param 是否以管理员权限=false
 * @return
 */
bool 运行(c_StrX 文件名, c_StrX 命令参数 = "", bool 是否等待 = false,
	int 被运行程序窗口显示方式 = 2, bool 是否以管理员权限 = false)
{
	auto _toWinShow = [](int e) {
		switch (e) {
		case 1: return SW_HIDE; case 2: return SW_SHOWNORMAL;
		case 3: return SW_SHOWMINIMIZED; case 4: return SW_SHOWMAXIMIZED;
		case 5: return SW_SHOWNOACTIVATE; case 6: return SW_SHOWMINNOACTIVE;
		default: return SW_SHOWNORMAL;
		}
	};
	SHELLEXECUTEINFOW sei = { sizeof(sei) };
	sei.fMask = 是否等待 ? SEE_MASK_NOCLOSEPROCESS : 0;
	sei.lpVerb = 是否以管理员权限 ? _W("runas") : NULL;
	sei.lpFile = 文件名;
	sei.lpParameters = !命令参数 ? NULL : (charW*)命令参数;
	sei.nShow = _toWinShow(被运行程序窗口显示方式);

	/*这里该WindowsAPI的规则：
	  当 是否等待 为 假(false) 时：fMask 为 0。此时 ShellExecuteExW 不会返回进程句柄，
	  即sei.hProcess 会保持为 NULL，自然也就不存在关闭的需求。
	  当 是否等待 为 真(true) 时：我们需要等待进程结束，显式传入了 SEE_MASK_NOCLOSEPROCESS
	  这告诉系统：“请把进程句柄给我，我待会要用”。此时系统会分配句柄并填入 sei.hProcess。
	  我们在 WaitForSingleObject 之后调用了 CloseHandle，正确地关闭了它。
	*/
	if (ShellExecuteExW(&sei)) {
		if (是否等待 && sei.hProcess) {
			WaitForSingleObject(sei.hProcess, INFINITE);
			CloseHandle(sei.hProcess);
		}
		return true;
	}
	return false;
}

/**取剪辑板文本
 * @return 返回存放于当前 Windows 系统剪辑板中的文本。
 */
StrX 取剪辑板文本() {
	if (!OpenClipboard(NULL)) return "";
	HANDLE hData = GetClipboardData(CF_UNICODETEXT);
	if (hData == NULL) {
		CloseClipboard();
		return "";
	}
	StrW res = (charW*)GlobalLock(hData);
	GlobalUnlock(hData);
	CloseClipboard();
	return res;
}

/**置剪辑板文本
 * @param 文本 准备置入剪辑板的文本
 * @return 成功返回真，失败返回假
 */
bool 置剪辑板文本(c_StrX 文本) {
	if (!OpenClipboard(NULL)) return false;
	EmptyClipboard();
	size_t size = (文本.str.len() + 1) * sizeof(charW);
	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, size);
	if (hGlobal == NULL) {
		CloseClipboard();
		return false;
	}
	memcpy(GlobalLock(hGlobal), (const charW*)文本, size);
	GlobalUnlock(hGlobal);
	if (SetClipboardData(CF_UNICODETEXT, hGlobal) == NULL) {
		GlobalFree(hGlobal);
		CloseClipboard();
		return false;
	}
	CloseClipboard();
	return true;
}

/**剪辑板中可有文本
 * @return 如果当前 Windows 系统剪辑板中有文本数据，则返回真
 */
bool 剪辑板中可有文本() {
	return IsClipboardFormatAvailable(CF_UNICODETEXT)
		|| IsClipboardFormatAvailable(CF_TEXT);
}

/**清除剪辑板
 * 清除当前 Windows 系统剪辑板中的所有数据。
 */
void 清除剪辑板() {
	if (OpenClipboard(NULL)) {
		EmptyClipboard();
		CloseClipboard();
	}
}

//下面这些移步至 BEWin32UI （含DPI适配）
//int 取屏幕宽度() { return GetSystemMetrics(SM_CXSCREEN); }
//int 取屏幕高度() { return GetSystemMetrics(SM_CYSCREEN); }
//int 取鼠标水平位置() {
//	POINT pt;
//	GetCursorPos(&pt);
//	return pt.x;
//}
//int 取鼠标垂直位置() {
//	POINT pt;
//	GetCursorPos(&pt);
//	return pt.y;
//}

/**鸣叫
 * @param type=MB_OK 参考"MB_"开头常量
 */
void 鸣叫(int type = MB_OK) { MessageBeep(type); }

int 取启动时间() { return (int)GetTickCount(); }

namespace {
HCURSOR g_hWaitCursorPrev = NULL;
}

/**置等待鼠标
 * 本命令设置现行鼠标的形状为沙漏形，用作在即将长时间执行程序前提示操作者。
 */
void 置等待鼠标() {
	HCURSOR hWait = LoadCursorW(NULL, IDC_WAIT);
	g_hWaitCursorPrev = SetCursor(hWait);
}

/**恢复鼠标
 * 本命令恢复现行鼠标的原有形状，用作与“置等待鼠标”命令配对使用。
 */
void 恢复鼠标() {
	if (g_hWaitCursorPrev) {
		SetCursor(g_hWaitCursorPrev);
		g_hWaitCursorPrev = NULL;
	}
}

/**本命令暂停当前程序的运行并等待指定的时间（毫秒）。
 * @param 欲等待的毫秒
 */
void 延时(int 欲等待的毫秒) { Sleep(欲等待的毫秒); }

StrW 取绝对路径(c_StrW path);

/**读配置项
 * @param 配置文件名
 * @param 节名称
 * @param 配置项名称
 * @param 默认文本
 * @return 读取指定配置文件中指定项目的文本内容。
 */
StrX 读配置项(c_StrX 配置文件名, c_StrX 节名称, c_StrX 配置项名称, c_StrX 默认文本) {
	StrW absPath = 取绝对路径(配置文件名);
	DWORD size = 2048;
	StrW buf(size);
	while (true) {
		DWORD ret = GetPrivateProfileStringW(节名称, 配置项名称, 默认文本, (charW*)buf, size, absPath);
		if (ret < size - 1) {
			buf.reset(ret);
			return StrX(buf);
		}
		size *= 2;
		buf.reset(size);
	}
}

/**写配置项
 * @param 配置文件名
 * @param 节名称
 * @param 配置项名称<可空> 如果参数值被省略(nil)，则删除指定节及其下的所有配置项。
 * @param 欲写入值<可空> 如果参数值被省略(nil)，则删除所指定配置项。
 * @return 成功返回真，失败返回假。
 */
bool 写配置项(c_StrX 配置文件名, c_StrX 节名称, NilOpt<c_StrX> 配置项名称 = nil,
	NilOpt<c_StrX> 欲写入值 = nil) {
	const charW* lpKey = (配置项名称 != nil) ? (const charW*)配置项名称.val : NULL;
	const charW* lpString = (欲写入值 != nil) ? (const charW*)欲写入值.val : NULL;
	return WritePrivateProfileStringW(节名称, lpKey, lpString, 取绝对路径(配置文件名)) != 0;
}

/**取配置节名
 * @param 配置文件名
 * @return 返回指定配置文件中所有已有节名的文本数组。
 */
Arraybe<StrX> 取配置节名(c_StrX 配置文件名) {
	Arraybe<StrX> res;
	StrW absPath = 取绝对路径(配置文件名);
	DWORD size = 8192;
	StrW buf(size);
	while (true) {
		DWORD ret = GetPrivateProfileSectionNamesW((charW*)buf, size, absPath);
		if (ret < size - 2) {
			charW* p = (charW*)buf;
			while (p < (charW*)buf + ret && *p) {
				res.push(StrX(p));
				p += wcslen(p) + 1;
			}
			break;
		}
		size *= 2;
		buf.reset(size);
	}
	return res;
}

/**取配置项名
 * @param 配置文件名
 * @param 节名称
 * @return 返回指定配置文件中指定节下所有已有配置项名的文本数组。
 */
Arraybe<StrX> 取配置项名(c_StrX 配置文件名, c_StrX 节名称) {
	Arraybe<StrX> res;
	StrW absPath = 取绝对路径(配置文件名);
	DWORD size = 8192;
	StrW buf(size);
	while (true) {
		DWORD ret = GetPrivateProfileStringW(节名称, NULL, L"", (charW*)buf, size, absPath);
		if (ret < size - 2) {
			charW* p = (charW*)buf;
			while (p < (charW*)buf + ret && *p) {
				res.push(StrX(p));
				p += wcslen(p) + 1;
			}
			break;
		}
		size *= 2;
		buf.reset(size);
	}
	return res;
}


/**多文件对话框
 * @param 标题
 * @param 过滤器 格式："文本文件（*.txt）|*.txt"
 * @param 初始过滤器
 * @param 初始目录
 * @param 不改变目录
 * @param 父窗口<可空> 默认为GetActiveWindow()
 * @return 返回用户所选择或输入后的结果文本数组。
 */
Arraybe<StrW> 多文件对话框(c_StrX 标题, c_StrX 过滤器, int 初始过滤器,
	c_StrX 初始目录 = "", bool 不改变目录 = true, NilOpt<HWND> 父窗口 = nil)
{
	Arraybe<StrW> res;
	OPENFILENAMEW ofn = { sizeof(ofn) };
	charW szFile[32768] = { 0 };
	ofn.hwndOwner = (父窗口 != nil) ? (HWND)父窗口 : GetActiveWindow();
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = 32768;

	StrW wFilter = 过滤器;
	for (size_t i = 0; i < wFilter.len(); ++i) {
		if (wFilter.at(i) == L'|') wFilter.at(i) = L'\0';
	}
	wFilter.bytes.append(2);
	((charW*)wFilter._buf())[wFilter.len() - 1] = L'\0';

	ofn.lpstrFilter = wFilter;
	ofn.nFilterIndex = 初始过滤器 + 1;
	ofn.lpstrTitle = 标题;
	ofn.lpstrInitialDir = 初始目录 ? (const charW*)初始目录 : NULL;
	ofn.Flags = OFN_ALLOWMULTISELECT | OFN_EXPLORER | OFN_FILEMUSTEXIST;
	if (不改变目录) ofn.Flags |= OFN_NOCHANGEDIR;

	if (GetOpenFileNameW(&ofn)) {
		charW* p = szFile;
		StrW dir = p;
		p += wcslen(p) + 1;
		if (*p == 0) {
			res.push(dir);
		} else {
			while (*p) {
				StrW fullPath = dir;
				if (fullPath.bytes.size > 0) {
					charW lastChar = fullPath.at(-1);
					if (lastChar != L'\\' && lastChar != L'/') fullPath += _W("\\");
				}
				fullPath += p;
				res.push(fullPath);
				p += wcslen(p) + 1;
			}
		}
	}
	return res;
}

typedef NTSTATUS(WINAPI* RtlGetVersion_PTR)(void*);

/**取系统版本
 * @param 系统版本 <参考 可空> 返回系统名称文本，如 "Windows 10"
 * @param 内核NT版本 <参考 可空> 返回内核版本，如 "10.0"
 * @return 返回操作系统内部版本号 (BuildVersion)
 */
int 取系统版本(NilOpt<StrA&> 系统版本 = nil, NilOpt<StrA&> 内核NT版本 = nil)
{
	OSVERSIONINFOEXA osvi = { sizeof(osvi) };
	HMODULE hNtdll = LoadLibraryA("ntdll.dll");
	RtlGetVersion_PTR pRtlGetVersion = hNtdll ?
		(RtlGetVersion_PTR)GetProcAddress(hNtdll, "RtlGetVersion") : nullptr;

	bool isRtlGetVersionSuccess = false;
	if (pRtlGetVersion) {
		isRtlGetVersionSuccess = (pRtlGetVersion(&osvi) == 0);
	}

	StrA sysName = "未知系统";
	StrA ntVerStr;

	if (!isRtlGetVersionSuccess) {
		// RtlGetVersion 失败，走降级逻辑 (p == 0)
#pragma warning(push)
#pragma warning(disable: 4996)
		GetVersionExA((OSVERSIONINFOA*)&osvi);
#pragma warning(pop)
		if (hNtdll) FreeLibrary(hNtdll);

		ntVerStr = sprintF("%d.%d", osvi.dwMajorVersion, osvi.dwMinorVersion);
		if (ntVerStr == "5.0") sysName = "Windows 2000";
		else if (ntVerStr == "5.1") sysName = "Windows XP";
		else if (ntVerStr == "5.2") sysName = "Windows Server 2003";
		else if (ntVerStr == "6.0") sysName = "Windows Vista";
		else if (ntVerStr == "6.1") sysName = "Windows 7";
		else if (ntVerStr == "6.2") sysName = "Windows 8";

		if (内核NT版本 != nil) 内核NT版本 = ntVerStr;
		if (系统版本 != nil) 系统版本 = sysName;
		return (int)osvi.dwBuildNumber;
	}

	if (hNtdll) FreeLibrary(hNtdll);

	// RtlGetVersion 成功逻辑
	ntVerStr = sprintF("%d.%d", osvi.dwMajorVersion, osvi.dwMinorVersion);
	if (ntVerStr == "5.1") sysName = "Windows XP";
	else if (ntVerStr == "5.2") sysName = "Windows Server 2003";
	else if (ntVerStr == "6.0") sysName = "Windows Vista";
	else if (ntVerStr == "6.1") sysName = "Windows 7";
	else if (ntVerStr == "6.2") sysName = "Windows 8";
	else if (osvi.dwMajorVersion == 10) {
		if (osvi.dwBuildNumber >= 22000) sysName = "Windows 11";
		else if (osvi.dwBuildNumber >= 10240) sysName = "Windows 10";
	}

	if (内核NT版本 != nil) 内核NT版本 = ntVerStr;
	if (系统版本 != nil) 系统版本 = sysName;

	return (int)osvi.dwBuildNumber;
}

/**创建线程
 * @param 子程序指针
DWORD WINAPI 线程函数(LPVOID param) {
	return 0;
}
 * @param 参数
 * @param 句柄_<可空>
 * @return 返回线程ID
 */
DWORD 创建线程(PTHREAD_START_ROUTINE 子程序指针, void* 参数 = 0, 可空<HANDLE&> 句柄 = 空)
{
	DWORD tid = 0;
	HANDLE h = CreateThread(NULL, 0, 子程序指针, 参数, 0, &tid);
	if (h == NULL) { return 0; }
	if (句柄 == 空) {
		CloseHandle(h);
	} else {
		句柄 = h;
	}
	return tid;
}
#define 启动线程 创建线程

/**创建一个程序进程(成功返回进程ID,失败返回0)
 * @param 程序路径 欲创建进程的执行路径
 * @param 命令行=nil 附加上程序路径后的命令行参数如“a b c”三个命令行
 * @param 运行目录=nil 通常留空,为该程序的目录，特殊情况下使用
 * @param 隐藏窗口=false
 * @param 进程结构=nil 接收进程结构信息的变量
 * @return 成功返回进程ID,失败返回0
 */
int 创建进程(c_StrX 程序路径, c_StrX 命令行 = "", c_StrX 运行目录 = "",
	bool 隐藏窗口 = false, 可空<PROCESS_INFORMATION&> 进程结构 = nil)
{
	StrW dir = (!运行目录) ? 路径_去文件名<W>(程序路径) : 运行目录;
	charW* lpCurrentDirectory = !dir ? NULL : (charW*)dir;

	STARTUPINFOW si = { sizeof(si) };
	if (隐藏窗口) {
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;
	}

	// 拼接命令行，把程序路径放在第一个参数，用双引号包裹以防路径含有空格
	StrW cmdLine = L"\"" + 程序路径.w() + L"\"";
	if (命令行) {
		cmdLine += L" " + 命令行.w();
	}

	PROCESS_INFORMATION pi = { 0 };
	PROCESS_INFORMATION* lppi = (进程结构 != nil) ? 进程结构.p : &pi;

	if (CreateProcessW(NULL, cmdLine, NULL, NULL, FALSE, 0, NULL, lpCurrentDirectory, &si, lppi)) {
		DWORD dwProcessId = lppi->dwProcessId;
		if (进程结构 == nil) {
			CloseHandle(lppi->hProcess);
			CloseHandle(lppi->hThread);
		}
		return (int)dwProcessId;
	}
	return 0;
}

HashTbe<int, be::function<int>> g_DelayCallMap;

void CALLBACK __DelayCallTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
	KillTimer(hwnd, idEvent);
	be::function<int>* funcPtr = g_DelayCallMap.find((int)idEvent);
	if (funcPtr && *funcPtr) {
		be::function<int> func = (be::function<int>&&)*funcPtr;
		g_DelayCallMap.del((int)idEvent);
		func(0);
	}
}

/**推迟调用子程序
 * @tparam 互斥=false 是否互斥。为真时若ID已在等待则忽略；为假时若ID已在等待则重置（重载）时钟。
 * @param 推迟时间 延时调用的毫秒数
 * @param 任意子程序指针 传入的回调函数或Lambda表达式
 * @param ID=空 指定由本函数之前返回的ID进行互斥/重置检查
 * @param ...参数 传递给子程序的可变参数
 * @return 定时器ID，失败返回0
 */
template<bool 互斥 = false, typename Func, typename... Args>
int 推迟调用子程序(int 推迟时间, Func 任意子程序指针, 可空<int> ID = 空, Args... args)
{
	if (ID != 空) {
		int oldId = (int)ID;
		if (g_DelayCallMap.find(oldId)) {
			if constexpr (互斥) {
				return oldId; // 互斥模式：直接返回原 ID
			} else {
				// 重置模式：杀掉旧时钟并从映射表中移除，后续会重新创建
				KillTimer(NULL, oldId);
				g_DelayCallMap.del(oldId);
			}
		}
	}

	UINT_PTR osTimerId = SetTimer(NULL, 0, 推迟时间, __DelayCallTimerProc);
	if (osTimerId) {
		g_DelayCallMap[(int)osTimerId] = [=](int) mutable {
			任意子程序指针(args...);
		};
		return (int)osTimerId;
	}
	return 0;
}