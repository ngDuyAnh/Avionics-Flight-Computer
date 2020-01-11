
. ./PSConfigFile.ps1

If ($MINGW_ENV_PATH -eq "")
{
    Write-Error "Oops! Seems like you have not run \"python process-user-configurations.py\" first!"
    exit
}

If ($CLION_PATH -eq "")
{
    Write-Error "Oops! Seems like you have not run \"python process-user-configurations.py\" first!"
    exit
}

If ($CLION_CMAKE_PATH -eq "")
{
    Write-Error "Oops! Seems like you have not run \"python process-user-configurations.py\" first!"
    exit
}

If ($ARM_TOOLS_PATH -eq "")
{
    Write-Error "Oops! Seems like you have not run \"python process-user-configurations.py\" first!"
    exit
}

SETX ARM_DEV_ENV "$MINGW_ENV_PATH;$CLION_PATH;$CLION_CMAKE_PATH;$ARM_TOOLS_PATH"
$Reg = "Registry::HKLM\System\CurrentControlSet\Control\Session Manager\Environment"
$OldPath = (Get-ItemProperty -Path "$Reg" -Name PATH).Path
SETX Path "$OldPath%ARM_DEV_ENV%"

New-Item -ItemType File $profile -Force
Clear-Content -Path $profile
"New-Alias make C:\dev\env\bin\mingw32-make.exe" | Add-Content  $profile
"function go_to_avionics {set-location $SOURCE_PATH}" | Add-Content  $profile
"New-Alias avionics go_to_avionics" | Add-Content  $profile