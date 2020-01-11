@ECHO OFF
rd /s /q "build" > nul 2>&1
rd /s /q "bin" > nul 2>&1
del CMakeConfigFile.cmake
del CMDConfigFile.cmd
del PSConfigFile.ps1