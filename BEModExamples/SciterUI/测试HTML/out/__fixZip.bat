@echo off
chcp 65001 >nul
title BE-ZIP 资源解密还原工具

if "%~1"=="" (
    echo.
    echo ============================================================
    echo [提示] 请将要修复的 .zip 文件直接【拖放】到本 BAT 脚本图标上！
    echo ============================================================
    echo.
    pause
    exit /b
)

:loop
if "%~1"=="" goto end

set "TARGET_PATH=%~f1"
echo 正在还原修复: "%~1" ...

powershell -NoProfile -ExecutionPolicy Bypass -Command "$p = $env:TARGET_PATH; if (Test-Path -LiteralPath $p) { $b = [System.IO.File]::ReadAllBytes($p); $cd = [BitConverter]::ToUInt32($b, $b.Length - 6); $b[0] = $b[0] -bxor 0xA5; for ($i = $cd; $i -lt ($b.Length - 6); $i++) { $b[$i] = $b[$i] -bxor 0xA5 }; [System.IO.File]::WriteAllBytes($p, $b); Write-Host ' -> [成功] 已完成还原修复！' -ForegroundColor Green } else { Write-Host ' -> [错误] 找不到该文件。' -ForegroundColor Red }"

shift
goto loop

:end
echo.
echo ============================================================
echo  所有拖入的文件已处理完毕！
echo ============================================================
pause
