Set-ExecutionPolicy RemoteSigned -Force

$MINGW_ENV_PATH="C:\dev\env\bin"
$CLION_PATH="C:\dev\opt\clion\bin"
$CLION_CMAKE_PATH="C:\dev\opt\clion\bin\cmake\win\bin"
$ARM_TOOLS_PATH="C:\dev\opt\cubeide\STM32CubeIDE\plugins\com.st.stm32cube.ide.mcu.externaltools.gnu-arm-embedded.7-2018-q2-update.win32_1.0.0.201904081647\tools\bin"
$SOURCE_PATH="C:\dev\projects\Avionics-Flight-Computer\AvionicsSoftware-AtollicProject"

If ($MINGW_ENV_PATH -eq "")
{
    exit
}

If ($CLION_PATH -eq "")
{
    exit
}

If ($CLION_CMAKE_PATH -eq "")
{
    exit
}

If ($ARM_TOOLS_PATH -eq "")
{
    exit
}

SETX ARM_DEV_ENV "$MINGW_ENV_PATH;$CLION_PATH;$CLION_CMAKE_PATH;$ARM_TOOLS_PATH"
$Reg = "Registry::HKLM\System\CurrentControlSet\Control\Session Manager\Environment"
$OldPath = (Get-ItemProperty -Path "$Reg" -Name PATH).Path
SETX Path "$OldPath%ARM_DEV_ENV%"

New-Item -ItemType Directory  -Path $profile
Clear-Content -Path $profile
"New-Alias make C:\dev\env\bin\mingw32-make.exe" | Add-Content  $profile
"function go_to_avionics {set-location $SOURCE_PATH}" | Add-Content  $profile
"New-Alias avionics go_to_avionics" | Add-Content  $profile