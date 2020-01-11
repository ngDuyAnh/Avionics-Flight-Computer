@ECHO ON
SETLOCAL
set MINGW_ENV_PATH      =   "C:\dev\env\bin"
set CLION_PATH          =   "C:\dev\opt\clion\bin"
set CLION_CMAKE_PATH    =   "C:\dev\opt\clion\bin\cmake\win\bin"
set ARM_TOOLS_PATH      =   "C:\dev\opt\cubeide\STM32CubeIDE\plugins\com.st.stm32cube.ide.mcu.externaltools.gnu-arm-embedded.7-2018-q2-update.win32_1.0.0.201904081647\tools\bin"

IF %MINGW_ENV_PATH%==""        GOTO    :EOF
IF %CLION_PATH%==""            GOTO    :EOF
IF %CLION_CMAKE_PATH%==""      GOTO    :EOF
IF %ARM_TOOLS_PATH%==""        GOTO    :EOF

CALL ENVIRONMENT.BAT %MINGW_ENV_PATH% %CLION_PATH% %CLION_CMAKE_PATH% %ARM_TOOLS_PATH%
ENDLOCAL

