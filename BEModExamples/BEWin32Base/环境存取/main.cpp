#include "stdafx.h"
using namespace be;

#define ASSERT(expr) \
    if (!(expr)) { \
        printf("FAIL: %s (%s:%d)\n", #expr, __FILE__, __LINE__); \
    } else { \
        printf("PASS: %s\n", #expr); \
    }

void TestEnv() {
	print("--- Testing Env Operations ---");

	数组<StrW> cmdArgs;
	StrW cmdLine = 取命令行(cmdArgs);
	print("Command Line: ", cmdLine);
	for (int i = 0; i < 取数组成员数(cmdArgs); i++) {
		print("Arg[", i, "]: ", cmdArgs[i]);
	}

	StrW runDir = 取运行目录();
	print("Run Dir: ", runDir);
	ASSERT(runDir.len() > 0);

	StrW exeName = 取执行文件名();
	print("Exe Name: ", exeName);
	ASSERT(exeName.len() > 0);

	StrW modPath = 取模块路径();
	print("Module Path: ", modPath);
	ASSERT(modPath.len() > 0);

	StrW curDir = 取当前目录();
	print("Current Dir: ", curDir);
	ASSERT(curDir.len() > 0);

	bool setDirOk = 置当前目录(runDir);
	ASSERT(setDirOk);
	ASSERT((StrW)取当前目录() == runDir);

	bool setOk = 写环境变量(_T("TEST_ENV_VAR"), _T("Hello BECore"));
	ASSERT(setOk);

	StrW envVal = 读环境变量(_T("TEST_ENV_VAR"));
	ASSERT(envVal == _T("Hello BECore"));

	setOk = 写环境变量(_T("TEST_ENV_VAR"), _T(""));
	ASSERT(setOk);

	envVal = 读环境变量(_T("TEST_ENV_VAR"));
	ASSERT(envVal.len() == 0);
	print("All Env operations tests passed.");

	{
		StrW envVal = 读环境变量(_T("JAVA_HOME"));
		print(envVal);
		写环境变量(_T("JAVA_HOME"), _T("my_java"));

		envVal = 读环境变量(_T("JAVA_HOME"));
		print(envVal);

		envVal = 读环境变量(_T("JAVA_HOME"), 1); //读用户级
		print(envVal);
	}
}

int main()
{
	TestEnv();
	
	return 0;
}