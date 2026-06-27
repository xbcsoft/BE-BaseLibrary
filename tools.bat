@echo off
setlocal enabledelayedexpansion

:menu
echo =============================
echo 适用于白菜PHPMVC框架后期处理工具
echo 请选择功能：
echo.
echo  1. 输入字符串并计算 MD5 值
echo  2. 将当前脚本所在目录的 docs\ 下所有 .html 改为 .thtml
echo  0. 退出
echo =============================
set /p choice=请输入选项(0-2)： 

if "%choice%"=="1" goto calc_md5
if "%choice%"=="2" goto rename_html
if "%choice%"=="0" exit /b
echo 无效选项，请重试。
echo.
goto menu

:calc_md5
set /p "input=请输入要计算MD5的字符串： "
if "%input%"=="" (
    echo 不能为空！
    echo.
    pause
    goto menu
)
:: 使用 certutil 计算字符串 MD5
echo|set /p="%input%" >"%temp%\_md5_input.txt"
for /f "tokens=1-3" %%a in ('certutil -hashfile "%temp%\_md5_input.txt" MD5 ^| find /i /v "hash" ^| find /i /v ":"') do set "md5=%%a%%b%%c"
del "%temp%\_md5_input.txt" >nul 2>nul

echo.
echo 字符串：%input%
echo MD5：%md5%
echo.
pause
goto menu

:rename_html
set "target=%~dp0docs"
if not exist "%target%" (
    echo 未找到目录：%target%
    echo 请确认脚本所在目录下有 docs 文件夹。
    echo.
    pause
    goto menu
)

echo 正在将 "%target%" 下所有 .html 改为 .thtml ...
set /a count=0
for /r "%target%" %%f in (*.html) do (
    move /y "%%f" "%%~dpnf.thtml" >nul
    set /a count+=1
)
echo 共改名 !count! 个文件。
echo 操作完成！
echo.
pause
