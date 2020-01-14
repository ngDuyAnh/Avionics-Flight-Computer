# redirect stderr into stdout
$p = &{python -V} 2>&1
# check if an ErrorRecord was returned
$version = if($p -is [System.Management.Automation.ErrorRecord])
{
    # grab the version string from the error message

    Write-Warning "Oops! Seems liek you do not have python installed in your system!"
}
else
{
    Write-Information "Found Python: $p"
    python process-user-configurations.py
}

if (!(Test-Path generated\PSConfigFile.ps1)) {
    Write-Warning "Oops! Seems like you have not run python process-user-configurations.py first!"
    Exit
}

. ./generated/PSConfigFile.ps1


If ($MINGW_ENV_PATH -eq "" -Or $CLION_PATH -eq "" -Or $CLION_CMAKE_PATH -eq "" -Or $ARM_TOOLS_DIR -eq "")
{
    Write-Warning "Oops! Seems like you have not run python process-user-configurations.py first!"
    exit
}

SETX ARM_DEV_ENV "$MINGW_ENV_PATH;$CLION_PATH;$CLION_CMAKE_PATH;$ARM_TOOLS_DIR"
$Reg = "Registry::HKLM\System\CurrentControlSet\Control\Session Manager\Environment"
$OldPath = (Get-ItemProperty -Path "$Reg" -Name PATH).Path
SETX Path "$OldPath%ARM_DEV_ENV%"

$BUILD_FILE = $SOURCE_PATH +'/REBUILD.ps1'

New-Item -ItemType File $profile -Force
Clear-Content -Path $profile
"New-Alias make C:\dev\env\bin\mingw32-make.exe" | Add-Content  $profile
"function go_to_avionics {set-location $SOURCE_PATH}" | Add-Content  $profile
"function _avionics_build {$BUILD_FILE}" | Add-Content  $profile
"New-Alias avionics go_to_avionics" | Add-Content  $profile

$BUILD_FILE
Start-Process powershell -Argument '-noexit -command _avionics_build'
Stop-Process -Id $PID