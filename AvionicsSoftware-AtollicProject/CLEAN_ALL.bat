@echo off
call CLEAN.bat

REG delete "HKCU\Environment" /F /V ARM_DEV_ENV
REG delete "HKCU\Software\Microsoft\Command Processor" /F /V AutoRun

set removedir=%%ARM_DEV_ENV%%
FOR /F "tokens=2* delims= " %%f IN ('reg query "HKCU\Environment" /v Path ^| findstr /i path') DO SET _PATH=%%g

setlocal enableextensions enabledelayedexpansion
set _=!_PATH:%removedir%;=!
if "%_%" == "%_PATH%" set _=!_PATH:;%removedir%=!
endlocal & set _PATH=%_%
SETX Path "%_PATH%"

if exist CMDConfigFile.cmd (
    call CMDConfigFile.cmd
    del %SOURCE_PATH%\startup.cmd
)




