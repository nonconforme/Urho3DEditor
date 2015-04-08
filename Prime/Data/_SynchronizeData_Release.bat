set targetfolder="../_bin/Release/GameData\"
set sourcefolder="GameData"

cd /d %targetfolder%
for /F "delims=" %%i in ('dir /b') do (rmdir "%%i" /s/q || del "%%i" /s/q)
cd /d "%~dp0"
xcopy /e /d /c /y %sourcefolder% %targetfolder%