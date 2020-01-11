@ECHO OFF

call CMDConfigFile.cmd

IF !MINGW_ENV_PATH!=="" GOTO:EOF
IF !CLION_PATH!=="" GOTO:EOF
IF !CLION_CMAKE_PATH!=="" GOTO:EOF
IF !ARM_TOOLS_PATH!=="" GOTO:EOF

SETX ARM_DEV_ENV "%MINGW_ENV_PATH%;%CLION_PATH%;%CLION_CMAKE_PATH%;%ARM_TOOLS_PATH%"
FOR /F "tokens=2* delims= " %%f IN ('reg query "HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\Environment" /v Path ^| findstr /i path') DO SET OLD_SYSTEM_PATH=%%g
SETX Path "%OLD_SYSTEM_PATH%;%%ARM_DEV_ENV%%"

ECHO ^@echo off > startup.bat
ECHO doskey make=%MINGW_ENV_PATH%\mingw32-make.exe >> startup.bat
ECHO doskey avionics=cd %SOURCE_PATH% >> startup.bat
ECHO doskey go_to_avionics=cd %SOURCE_PATH% >> startup.bat

reg add "HKCU\Software\Microsoft\Command Processor" /v AutoRun /t REG_EXPAND_SZ /d "%SOURCE_PATH%\startup.bat" /f

