Example of the file user-configurations.yaml:
NOTE: You must create this file in the same directory as 'CONFIGURE.ps1'. The locations will vary depending on your installation.
The file locations should have no spaces in the name. It is suggested to follow a similar directory structure as is listed below.

```
MINGW_ENV_PATH:         C:/dev/env/bin
CLION_PATH:             C:/dev/opt/clion/bin
CLION_CMAKE_PATH:       C:/dev/opt/clion/bin/cmake/win/bin
ARM_TOOLS_DIR:          C:/dev/opt/cubeide/STM32CubeIDE/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-arm-embedded.7-2018-q2-update.win32_1.0.0.201904081647/tools/bin
SOURCE_PATH:            C:/dev/projects/Avionics-Flight-Computer/AvionicsSoftware-AtollicProject
```


FOR WINDOWS:
```
MAKE SURE to set up CLION properly.
The configurations include: Toolchains such as MinGW make, gcc and g++, and OpenOCD.
Install STM32CubeIDE or AtttolicTrue Studio to install ARM build toolchains

If you use STM32CubeIDE:          {STM32CubeIDE_LOCATION}/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-arm-embedded.7-2018-q2-update.win32_{CHOOSE_LATEST_VERSION}/tools/bin
If you use AttolicTrueStudio:     {AttolicTrueStudioIDE_LOCATION}/ARMTools/bin

In order CLion to compile this project on Windows machine you need to:
1. Setup your environment:
 a) install MinGW environment, either follow the link https://sourceforge.net/projects/mingw-w64 or just search for MinGW-w64 - for 32 and 64 bit Windows
 b) setup the packages you need in the mingw manager (usually they are already checked for you), you will need packages such as gcc, g++, gdb, make, cmake, etc.
2. Setup the System and the System Environment Variables:
 a) Control Panel -> System And Security -> System -> Advanced System Settings -> Environment Variables [OR] My Computer -> Properties -> Advanced -> Environment Variables
 b) Add your ARM Build Toolchains binaries folder to your path (e.g. {AttolicTrueStudioIDE_LOCATION}/ARMTools/bin)
 c) Add CLion's CMake path to the system environment variables (e.g. C:/Program Files/JetBrains/CLion 2019.3.2/bin/cmake/win/bin)
 NOTE: You can either create your own variable where you would add all the development related pathes or just add it straight to the variable named \"PATH\"
 d) Create a \"make\" alias for Windows PowerShell. Search for PowerShell open it with administrator rights, then type the following:
    >> Set-ExecutionPolicy RemoteSigned (if asking confirm with \"Y\")
    >> echo $profile (hit Enter, you should receive a path e.g. C:/Users/xxxxxxx/Documents/WindowsPowerShell/Microsoft.PowerShell_profile.ps1)
       Create the path and the file if it does not exist, then open it and type:
       New-Alias make YOUR_PATH_TO_mingw32-make.exe (e.g. C:\\dev\\env\\bin\\mingw32-make.exe)
       Save file and close it
 e) open PowerShell and go to the project directory and type:
    ./build.ps1
 f) wait till it is finished, but make sure the build is successful
 g) Execute CLion from PowerShell!


3. Setup CLion Toolchains:
 a) Go to File -> Settings -> Build, Execution, Deployment -> Toolchains, let CLion detect the existing environments. then make some modifications to it
    Then for the first field \"Environment\" you specify mingw root folder (e.g. C:/Program Files/mingw-w64/x86_64-7.2.0-posix-seh-rt_v5-rev1/mingw64)
    then CLion will try to find \"Make\", \"C Compiler\" and \"CXX Compiler\" for you automatically.
 b) Change CLion CMake Genration Path from default \"cmake-build-debug\" to \"build\":
    File -> Settings -> Build, Execution, Deployment -> CMake -> \"Generation path\"
 c) Debugger. In order to enable CLion debugger we need to download \"OpenOCD\"
    OpenOCD     - https://gnutoolchains.com/arm-eabi/openocd/

    File -> Settings -> Build, Execution, Deployment -> Embedded Development.
    Specify OpenOCD Location        : (e.g. C:\\dev\\opt\\openocd\\bin\\openocd.exe)

    Lastly, do:
    Run -> Debug... -> Edit Configurations -> Add -> OpenOCD Download & Run
    Then press \"Assist\" and choose the configuration file that corresponds to the STM32 board that is used in the project
```


FOR LINUX

```
MAKE SURE to set up CLION Toolchains properly.
Install STM32CubeIDE or AtttolicTrue Studio to install ARM build toolchains
The location of the ARM Build Toolchains depends on the OS you are running CLion on:
For Linux OS, if you use STM32CubeIDE:            {STM32CubeIDE_LOCATION}/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.7-2018-q2-update.linux64_{CHOOSE_LATEST_VERSION}/tools/bin
For Linux OS, if you use AttolicTrueStudio:       {AttolicTrueStudioIDE_LOCATION}/ARMTools/bin
Setup CLion Toolchains:
a) Go to File -> Settings -> Build, Execution, Deployment -> Toolchains, let CLion detect the existing environments.
b) Change the values of the \"C Compiler\" and \"CXX Compiler\" to the corresponding ARM Build Toolchains gcc and g++.
c) Change CLion CMake Genration Path from default \"cmake-build-debug\" to \"build\":
   File -> Settings -> Build, Execution, Deployment -> CMake -> \"Generation path\"

d) Debugger. In order to enable CLion debugger we need to download \"OpenOCD\"
   OpenOCD     - https://gnutoolchains.com/arm-eabi/openocd/

   File -> Settings -> Build, Execution, Deployment -> Embedded Development.
   Specify OpenOCD Location        : (e.g. \\openocd\\bin\\openocd.exe)

   Lastly, do:
   Run -> Debug... -> Edit Configurations -> Add -> OpenOCD Download & Run
   Then press \"Assist\" and choose the configuration file that corresponds to the STM32 board that is used in the project
```