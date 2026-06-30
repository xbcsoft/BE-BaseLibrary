@echo off

set "CURRENT_PATH=%~dp0"
if "%CURRENT_PATH:~-1%"=="\" set "CURRENT_PATH=%CURRENT_PATH:~0,-1%"

echo Adding "%CURRENT_PATH%" to User PATH environment variable

set "USER_PATH="
for /f "usebackq tokens=2,*" %%A in (`reg query HKCU\Environment /v Path 2^>nul`) do (
    set "USER_PATH=%%B"
)

echo ;%USER_PATH%; | find /i ";%CURRENT_PATH%;" > nul
if %errorlevel% neq 0 (
    if defined USER_PATH (
        setx PATH "%USER_PATH%;%CURRENT_PATH%"
    ) else (
        setx PATH "%CURRENT_PATH%"
    )
    echo Successfully added to User PATH.
) else (
    echo Already exists in User PATH.
)
echo Setting InstallPath to "%CURRENT_PATH%" in HKCU\Software\xbcsoft\BE
reg add "HKCU\Software\xbcsoft\BE" /v "InstallPath" /t REG_SZ /d "%CURRENT_PATH%" /f

if %errorlevel% neq 0 (
    echo Failed to update the registry.
    exit /b %errorlevel%
)

echo Registry updated successfully.

echo Launching VSIX installer and waiting for it to finish...
powershell -NoProfile -Command ^
    "& { $process = Start-Process -FilePath (Resolve-Path 'BEVC++.vsix') -Wait -PassThru; exit $process.ExitCode }"

if %errorlevel% equ 0 (
    echo VSIX installer finished successfully.
	 pause
) else (
    echo VSIX installer returned error %errorlevel%.
    exit /b %errorlevel%
)