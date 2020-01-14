@ECHO OFF

set "$py=0"
call:construct

for /f "delims=" %%a in ('python #.py ^| findstr "2"') do set "$py=2"
for /f "delims=" %%a in ('python #.py ^| findstr "3"') do set "$py=3"
del #.py
goto:%$py%

echo Python is not installed or python's path Path is not in the %%$path%% env. var
goto:EOF

:2
echo Found Python 2
python process-user-configurations.py
PAUSE 2
goto:configure

:3
echo Found Python 3
python process-user-configurations.py
PAUSE 2
goto:configure

:construct
echo import sys; print('{0[0]}.{0[1]}'.format(sys.version_info^)^) >#.py
goto:configure


:configure

if not exist generated/CMDConfigFile.cmd (
    GOTO:oops
)

call generated/CMDConfigFile.cmd

IF !MINGW_ENV_PATH!=="" GOTO:oops
IF !CLION_PATH!=="" GOTO:oops
IF !CLION_CMAKE_PATH!=="" GOTO:oops
IF !ARM_TOOLS_DIR!=="" GOTO:oops

SETX ARM_DEV_ENV "%MINGW_ENV_PATH%;%CLION_PATH%;%CLION_CMAKE_PATH%;%ARM_TOOLS_DIR%"
FOR /F "tokens=2* delims= " %%f IN ('reg query "HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\Environment" /v Path ^| findstr /i path') DO SET OLD_SYSTEM_PATH=%%g
SETX Path "%OLD_SYSTEM_PATH%;%%ARM_DEV_ENV%%"

doskey make=%MINGW_ENV_PATH%\mingw32-make.exe
doskey cmake=%CLION_CMAKE_PATH%\cmake.exe
doskey mingw32-make=%MINGW_ENV_PATH%\mingw32-make.exe
doskey avionics=cd %SOURCE_PATH%
doskey go_to_avionics=cd %SOURCE_PATH%

ECHO ^@echo off > generated\startup.cmd
ECHO doskey make=%MINGW_ENV_PATH%\mingw32-make.exe >> generated\startup.cmd
ECHO doskey cmake=%CLION_CMAKE_PATH%\cmake.exe >> generated\startup.cmd
ECHO doskey mingw32-make=%MINGW_ENV_PATH%\mingw32-make.exe >> generated\startup.cmd
ECHO doskey avionics=cd %SOURCE_PATH% >> generated\startup.cmd
ECHO doskey go_to_avionics=cd %SOURCE_PATH% >> generated\startup.cmd

reg add "HKCU\Software\Microsoft\Command Processor" /v AutoRun /t REG_EXPAND_SZ /d "%SOURCE_PATH%\generated\startup.cmd" /f

GOTO:EOF

:oops
echo "Oops! Seems like you have not run python process-user-configurations.py first!"
GOTO:OEF

