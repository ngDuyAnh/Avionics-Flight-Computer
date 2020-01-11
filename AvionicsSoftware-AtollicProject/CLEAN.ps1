rm -r -fo .\build\ 2>&1 | out-null
rm -r -fo .\bin\ 2>&1 | out-null
Remove-Item -Path CMakeConfigFile.cmake
Remove-Item -Path CMDConfigFile.cmd
Remove-Item -Path PSConfigFile.ps1