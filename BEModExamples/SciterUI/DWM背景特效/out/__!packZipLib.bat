@echo off
cd /d "%~dp0"
set FILES='index.html','css','js'
set "TARGET_ZIP=res.zip"

echo 正在打包并混淆 %TARGET_ZIP% ...
powershell -NoProfile -ExecutionPolicy Bypass -Command "Compress-Archive -Path %FILES% -DestinationPath '%TARGET_ZIP%' -Force; $b = [System.IO.File]::ReadAllBytes('%TARGET_ZIP%'); $cd = [BitConverter]::ToUInt32($b, $b.Length - 6); $b[0] = $b[0] -bxor 0xA5; for ($i = $cd; $i -lt ($b.Length - 6); $i++) { $b[$i] = $b[$i] -bxor 0xA5 }; [System.IO.File]::WriteAllBytes('%TARGET_ZIP%', $b);"
powershell -command "Start-Sleep -Milliseconds 100"
echo 开始编译到lib库 ...
bin2Lib.exe -cl "%~dp0..\BEMod\R.stb"
