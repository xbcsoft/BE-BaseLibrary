#include "stdafx.h"

class EXP 文件读写
{
public:
	bool 不存在则创建 = true;
	文件读写(bool 必须存在 = true) {
		不存在则创建 = 必须存在;
	}

	HANDLE hFile = 0;

	/**打开文件
	 * @param 文件名
	 * @param 打开方式=3 1读，2写，3读写。
	 * @param 共享方式=3 1允许别的进程读，2允许别的进程写，3=1+2
	 * @return
	 */
	bool 打开文件(c_StrX 文件名, int 打开方式 = 3, int 共享方式 = 3) {
		if (hFile && hFile != INVALID_HANDLE_VALUE) 关闭文件();

		switch (打开方式) {
		case 1: 打开方式 = GENERIC_READ; break;
		case 2: 打开方式 = GENERIC_WRITE; break;
		default:打开方式 = GENERIC_READ | GENERIC_WRITE; break;
		}

		switch (共享方式) {
		case 1: 共享方式 = FILE_SHARE_READ; break;
		case 2: 共享方式 = FILE_SHARE_WRITE; break;
		default:共享方式 = FILE_SHARE_READ | FILE_SHARE_WRITE; break;
		}

		DWORD dwCreation = 不存在则创建 ? OPEN_EXISTING : OPEN_ALWAYS;

		hFile = CreateFileW(文件名, 打开方式, 共享方式, NULL, dwCreation, FILE_ATTRIBUTE_NORMAL, NULL);
		return hFile != INVALID_HANDLE_VALUE;
	}

	int64 获取当前位置() {
		if (!hFile || hFile == INVALID_HANDLE_VALUE) return 0;
		LARGE_INTEGER offset = { 0 };
		LARGE_INTEGER newPos = { 0 };
		if (SetFilePointerEx(hFile, offset, &newPos, FILE_CURRENT)) {
			return newPos.QuadPart;
		}
		return 0;
	}
	int64 取文件大小() {
		if (!hFile || hFile == INVALID_HANDLE_VALUE) return 0;
		LARGE_INTEGER liSize;
		if (GetFileSizeEx(hFile, &liSize)) {
			return liSize.QuadPart;
		}
		return 0;
	}

	/**移动读写位置
	 * @param 起始位置=0 0=开头，1=当前，2=结尾
	 * @param 移动距离=0
	 * @return
	 */
	bool 移动读写位置(int 起始位置 = 0, int64 移动距离 = 0) {
		if (!hFile || hFile == INVALID_HANDLE_VALUE) return false;

		DWORD moveMethod = FILE_BEGIN;
		if (起始位置 == 1) moveMethod = FILE_CURRENT;
		else if (起始位置 == 2) moveMethod = FILE_END;

		LARGE_INTEGER liDistance;
		liDistance.QuadPart = 移动距离;
		return SetFilePointerEx(hFile, liDistance, NULL, moveMethod) != FALSE;
	}
	Bytes 读入字节集(int 读入长度 = 0, bool 是否不偏移 = false) {
		if (!hFile || hFile == INVALID_HANDLE_VALUE) return Bytes();
		if (读入长度 <= 0) {
			LARGE_INTEGER liSize;
			if (GetFileSizeEx(hFile, &liSize)) {
				读入长度 = (int)liSize.QuadPart;
			}
		}
		if (读入长度 <= 0) return Bytes();

		Bytes 文件数据(读入长度);

		int64 oldPos = 0;
		if (是否不偏移) {
			oldPos = 获取当前位置();
		}

		DWORD dwRead = 0;
		if (!ReadFile(hFile, 文件数据.buf, 读入长度, &dwRead, NULL)) {
			return Bytes();
		}

		if (是否不偏移) {
			移动读写位置(0, oldPos);
		}

		文件数据.size = dwRead;
		return 文件数据;
	}
	bool 写出字节集(const Bytes& 文件数据, bool 是否不偏移 = false) {
		if (!hFile || hFile == INVALID_HANDLE_VALUE) return false;
		if (!文件数据.buf || 文件数据.size == 0) return true;

		int64 oldPos = 0;
		if (是否不偏移) {
			oldPos = 获取当前位置();
		}

		DWORD dwWritten = 0;
		BOOL bRet = WriteFile(hFile, 文件数据.buf, (DWORD)文件数据.size, &dwWritten, NULL);

		if (是否不偏移) {
			移动读写位置(0, oldPos);
		}

		return bRet != FALSE;
	}
	bool 关闭文件() {
		if (hFile && hFile != INVALID_HANDLE_VALUE) {
			BOOL r = CloseHandle(hFile);
			hFile = 0;
			return r != FALSE;
		}
		return false;
	}
	void 当前位置截断文件() {
		if (!hFile || hFile == INVALID_HANDLE_VALUE) return;
		SetEndOfFile(hFile);
	}

	/**从当前位置开始删除指定数量的字节
	 * @param 删的字节数
	 * @return
	 */
	bool 删除字节集(int64 删的字节数) {
		if (删的字节数 <= 0) return false;

		int64 oldPos = 获取当前位置();
		移动读写位置(2, 0); // 移到末尾
		int64 fileLen = 获取当前位置();

		if (oldPos >= fileLen) {
			移动读写位置(0, oldPos);
			return false;
		}

		int64 srcPos = oldPos + 删的字节数;
		if (srcPos >= fileLen) {
			// 删除的内容等于或超过剩余内容，直接在这里截断即可
			移动读写位置(0, oldPos);
			当前位置截断文件();
			return true;
		}

		// 剩余需要平移的数据长度
		int64 moveBytes = fileLen - srcPos;
		int bufferSize = 65536; // 64KB 块读取缓冲

		int64 currDst = oldPos;
		int64 currSrc = srcPos;

		while (moveBytes > 0) {
			int readLen = (int)(moveBytes > bufferSize ? bufferSize : moveBytes);

			移动读写位置(0, currSrc);
			Bytes dat = 读入字节集(readLen);
			if (dat.size == 0) break; // 防止死循环

			移动读写位置(0, currDst);
			写出字节集(dat);

			currSrc += dat.size;
			currDst += dat.size;
			moveBytes -= dat.size;
		}

		// 截去尾端残缺的多余内容
		移动读写位置(0, currDst);
		当前位置截断文件();

		// 重新还原光标位置到被删除的缝隙处
		移动读写位置(0, oldPos);
		return true;
	}
	void 文件扩容(int64 扩容大小) {
		if (!hFile || hFile == INVALID_HANDLE_VALUE) return;
		移动读写位置(2, 扩容大小);
		SetEndOfFile(hFile);
	}
	~文件读写() {
		关闭文件();
	}
};

Bytes 读入文件(c_StrX 文件路径) {
	文件读写 f;
	if (f.打开文件(文件路径, 1, 1)) {
		return f.读入字节集();
	}
	return Bytes();
}

bool 写到文件(c_StrX 文件路径, const Bytes& dat) {
	文件读写 f(false);
	if (f.打开文件(文件路径, 2)) {
		bool ret = f.写出字节集(dat);
		if (ret) f.当前位置截断文件();
		return ret;
	}
	return false;
}

/**将相对路径转换到绝对路径（当然自身已经是绝对路径的不影响）
 * @param path 相对路径
 * @return
 */
StrW 取绝对路径(c_StrW path) {
	charW fullPath[MAX_PATH] = { 0 };

	// GetFullPathName 会将相对路径转换为绝对路径
	DWORD ret = GetFullPathNameW(
		path,     // 输入路径
		MAX_PATH,         // 缓冲区大小
		fullPath,         // 输出绝对路径
		nullptr           // 可选指针，接收文件名部分
	);

	if (ret == 0 || ret >= MAX_PATH) {
		// 出错或路径太长
		return L"";
	}

	return fullPath;
}

bool 判断是否为相对路径(c_StrW path) {
	// 在 Windows 下：
	// - 以 "\" 或者 "C:\" 开头的是绝对路径
	// - 其他大多数都是相对路径
	if (!path) return true;

	if ((path.len() >= 2 && path[1] == ':') ||
		(path.len() >= 2 && path[0] == '\\' && path[1] == '\\')) {
		return false; // 绝对路径
	}
	return true; // 相对路径
}