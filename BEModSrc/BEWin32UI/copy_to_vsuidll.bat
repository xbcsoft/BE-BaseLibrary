@echo off
setlocal

set "SRC_DIR=%~dp0BEWin32UI\VSUIDll"
set "DST_DIR=%~dp0..\..\VSUIDll"

if not exist "%SRC_DIR%\BEWin32UI.dll" (
    echo [错误] 找不到源文件: "%SRC_DIR%\BEWin32UI.dll"
    pause
    exit /b 1
)

if not exist "%DST_DIR%" (
    mkdir "%DST_DIR%"
)

echo 正在复制 BEWin32UI.dll 到 "%DST_DIR%" ...
copy /y "%SRC_DIR%\BEWin32UI.dll" "%DST_DIR%\BEWin32UI.dll"

if exist "%SRC_DIR%\BEWin32UI.pdb" (
    echo 正在同步复制 BEWin32UI.pdb ...
    copy /y "%SRC_DIR%\BEWin32UI.pdb" "%DST_DIR%\BEWin32UI.pdb"
)

if %ERRORLEVEL% equ 0 (
    echo.
    echo [成功] 文件复制完成！
) else (
    echo.
    echo [失败] 复制过程中出错，错误代码：%ERRORLEVEL%
	 pause
)