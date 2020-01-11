@echo off

SET /A ARGC=0
FOR %%x IN (%*) DO SET /A ARGC+=1
echo ARGUMENTS PROVIDED: %ARGC%

IF "%~4"=="" GOTO:usage
IF NOT "%~5"=="" GOTO:usage

SET MINGW_ENV_PATH=%1
SET CLION_PATH=%2
SET CLION_CMAKE_PATH=%3
SET ARM_TOOLS_PATH=%4

GOTO:addToARMVariable
goto:EOF

:addToARMVariable
    SETX ARM_DEV_ENV "%MINGW_ENV_PATH%;%CLION_PATH%;%CLION_CMAKE_PATH%;%ARM_TOOLS_PATH%;%ARM_DEV_ENV%"
    FOR /F "tokens=2* delims= " %%f IN ('reg query "HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\Environment" /v Path ^| findstr /i path') DO SET OLD_SYSTEM_PATH=%%g
    SETX Path "%OLD_SYSTEM_PATH%;%%ARM_DEV_ENV%%"
    exit /b

:usage
    echo usage()
    echo THERE MUST BE EXACTLY 4 ARGUMENTS
    @echo Usage: %0 ^<MINGW_ENV_PATH^> ^<CLION_PATH^> ^<CLION_CMAKE_PATH^> ^<ARM_TOOLS_PATH^>
    GOTO:EOF
    exit /b
