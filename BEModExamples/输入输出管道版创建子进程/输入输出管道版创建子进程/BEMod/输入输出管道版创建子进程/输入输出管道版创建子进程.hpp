#pragma once
#include "stdafx.h"

void 处理事件();

template <bool EnableMsgLoop = false>
bool 创建子进程_读写管道(
	c_StrA 文件名, 可空<c_StrA> 命令行 = 空, 可空<c_StrA> 运行目录 = 空,
	可空<StrA&> 输出buf = 空,
	void(*fn进程创建完毕)(PROCESS_INFORMATION&, HANDLE) = nullptr,
	void(*fn回调输出)(const StrA&) = nullptr,
	HANDLE* phStdinWrite = nullptr
) {
	static LONG pipeCount = 0;
	char szPipeName[MAX_PATH];
	sprintf_s(szPipeName, "\\\\.\\pipe\\BE_SubOut_%d_%ld", GetCurrentProcessId(), InterlockedIncrement(&pipeCount));

	// 1. 创建重叠 I/O 命名管道的 stdout 读写两端
	HANDLE hChildStdoutRead = CreateNamedPipeA(
		szPipeName, PIPE_ACCESS_INBOUND | FILE_FLAG_OVERLAPPED,
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
		1, 65536, 65536, 0, NULL
	);
	if (hChildStdoutRead == INVALID_HANDLE_VALUE) return false;

	SECURITY_ATTRIBUTES saAttr{ sizeof(saAttr), NULL, TRUE };
	HANDLE hChildStdoutWrite = CreateFileA(szPipeName, FILE_WRITE_DATA | SYNCHRONIZE, 0, &saAttr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hChildStdoutWrite == INVALID_HANDLE_VALUE) {
		CloseHandle(hChildStdoutRead);
		return false;
	}

	// 2. 创建 stdin 管道并设置写端不可继承
	HANDLE hChildStdinRead = NULL, hChildStdinWrite = NULL;
	if (!CreatePipe(&hChildStdinRead, &hChildStdinWrite, &saAttr, 0)) {
		CloseHandle(hChildStdoutRead); CloseHandle(hChildStdoutWrite);
		return false;
	}
	SetHandleInformation(hChildStdinWrite, HANDLE_FLAG_INHERIT, 0);

	// 3. 准备启动参数与命令行（首参数使用双引号包裹）
	STARTUPINFOW siStartInfo{ sizeof(STARTUPINFOW) };
	siStartInfo.hStdError = hChildStdoutWrite;
	siStartInfo.hStdOutput = hChildStdoutWrite;
	siStartInfo.hStdInput = hChildStdinRead;
	siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

	PROCESS_INFORMATION piProcInfo{};

	StrA fullCmdLine = (文件名.bytes.size > 0 && 文件名[0] == '"') ? 文件名 : "\"" + 文件名 + "\"";
	if (命令行 != 空 && ((c_StrA)命令行).bytes.size > 0) {
		fullCmdLine += " " + (c_StrA)命令行;
	}

	StrW cmdLineW = AtoW(fullCmdLine);

	StrW currentDirW = (运行目录 != 空) ? AtoW((c_StrA)运行目录) : StrW();
	const wchar_t* szCurrentDirectory = (运行目录 != 空) ? (const wchar_t*)currentDirW : nullptr;

	// 4. 启动子进程（lpApplicationName 设为 NULL 以支持系统从 PATH 中搜索）
	BOOL bSuccess = CreateProcessW(
		NULL, (charW*)cmdLineW, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL,
		szCurrentDirectory, &siStartInfo, &piProcInfo
	);

	CloseHandle(hChildStdoutWrite);
	CloseHandle(hChildStdinRead);

	if (!bSuccess) {
		CloseHandle(hChildStdoutRead); CloseHandle(hChildStdinWrite);
		return false;
	}

	// 5. 进程创建回调与输入管道管理
	if (fn进程创建完毕 != nullptr) {
		fn进程创建完毕(piProcInfo, hChildStdinWrite);
	}
	if (phStdinWrite != nullptr) {
		*phStdinWrite = hChildStdinWrite;
	} else {
		if (hChildStdinWrite != NULL) {
			CloseHandle(hChildStdinWrite);
		}
	}

	// 6. 异步读取事件循环配置
	HANDLE hReadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (hReadEvent == NULL) {
		CloseHandle(piProcInfo.hProcess); CloseHandle(piProcInfo.hThread); CloseHandle(hChildStdoutRead);
		return false;
	}

	OVERLAPPED overlapped{};
	overlapped.hEvent = hReadEvent;

	bool bRunning = true, bReadPending = false, bEOF = false;
	char readBuf[4096];
	DWORD dwRead = 0;

	// Lambda 表达式：统一并且安全地处理数据输出
	auto handleOutput = [&](DWORD len) {
		if (len > 0) {
			readBuf[len] = '\0';
			StrA utf8Str(readBuf, len);
			if (输出buf != 空) {
				StrA& outRef = 输出buf;
				outRef += utf8Str;
			}
			if (fn回调输出 != nullptr) {
				fn回调输出(utf8Str);
			}
		}
	};

	while (bRunning || bReadPending) {
		MSG msg;
		if (PeekMessage(&msg, NULL, WM_QUIT, WM_QUIT, PM_NOREMOVE)) {
			break;
		}
		if (!bReadPending && !bEOF) {
			overlapped = {};
			overlapped.hEvent = hReadEvent;
			ResetEvent(hReadEvent);

			BOOL bReadSuccess = ReadFile(hChildStdoutRead, readBuf, sizeof(readBuf) - 1, &dwRead, &overlapped);
			if (bReadSuccess) {
				if (dwRead > 0) handleOutput(dwRead);
				else bEOF = true;
			} else {
				if (GetLastError() == ERROR_IO_PENDING) bReadPending = true;
				else bEOF = true;
			}
		}

		DWORD dwWaitCount = 0;
		HANDLE waitHandles[2];
		if (bRunning) waitHandles[dwWaitCount++] = piProcInfo.hProcess;
		if (bReadPending) waitHandles[dwWaitCount++] = hReadEvent;

		if (dwWaitCount == 0) break;

		DWORD dwWaitResult = 0;
		if constexpr (EnableMsgLoop) {
			dwWaitResult = MsgWaitForMultipleObjects(dwWaitCount, waitHandles, FALSE, INFINITE, QS_ALLINPUT);
		} else {
			dwWaitResult = WaitForMultipleObjects(dwWaitCount, waitHandles, FALSE, INFINITE);
		}

		if (dwWaitResult == WAIT_FAILED) break;

		if constexpr (EnableMsgLoop) {
			if (dwWaitResult == WAIT_OBJECT_0 + dwWaitCount) {
				处理事件();
				continue;
			}
		}

		DWORD signaledIndex = dwWaitResult - WAIT_OBJECT_0;
		if (signaledIndex < dwWaitCount) {
			HANDLE signaledHandle = waitHandles[signaledIndex];
			if (signaledHandle == piProcInfo.hProcess) {
				bRunning = false;
			} else if (signaledHandle == hReadEvent) {
				bReadPending = false;
				DWORD dwTransferred = 0;
				if (GetOverlappedResult(hChildStdoutRead, &overlapped, &dwTransferred, FALSE)) {
					if (dwTransferred > 0) handleOutput(dwTransferred);
					else bEOF = true;
				} else {
					bEOF = true;
				}
			}
		}
	}

	CloseHandle(hReadEvent);
	CloseHandle(piProcInfo.hProcess);
	CloseHandle(piProcInfo.hThread);
	CloseHandle(hChildStdoutRead);

	return true;
}
inline bool 管道_写入(HANDLE hStdinWrite, AutoS txt) {
	if (!hStdinWrite) return false;
	StrA writeData = txt.str + "\n";
	DWORD dwWritten = 0;
	return WriteFile(hStdinWrite, writeData._buf(), writeData.len(), &dwWritten, NULL) != 0;
}

