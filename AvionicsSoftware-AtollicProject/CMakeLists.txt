cmake_minimum_required(VERSION 3.5.1)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(EABI_PREFIX arm-none-eabi)

if ($ENV{CLION_IDE})
    if (WIN32)
        message("OS DETECTED: " "OS WINDOWS")
        message(WARNING "WARNING: The project is being compiled from CLion IDE on Windows!
            MAKE SURE to set up CLION properly.
            The configurations include: Toolchains such as MinGW make, gcc and g++.
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
             g) wait till it is finished, but make sure the build is successful

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


            NOTE: MAKE SURE TO EDIT \"EABI_PREFIX\" VARIABLE BASED ON WHICH IDE YOU ARE USING:  AtollicTrue Studio or STM32CubeIDE
            \"EABI_PREFIX\" for AtollicTrue Studio MUST BE: \"arm-atollic-eabi\"
            \"EABI_PREFIX\" for STM32CubeIDE       MUST BE: \"arm-none-eabi\""

            )

    elseif(UNIX)
        message("OS DETECTED: " "OS Linux")
        message(WARNING "WARNING: The project is being compiled from CLion IDE.
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

                NOTE: MAKE SURE TO EDIT \"EABI_PREFIX\" VARIABLE BASED ON WHICH IDE YOU ARE USING:  AtollicTrue Studio or STM32CubeIDE
                \"EABI_PREFIX\" for AtollicTrue Studio MUST BE: \"arm-atollic-eabi\"
                \"EABI_PREFIX\" for STM32CubeIDE       MUST BE: \"arm-none-eabi\""
                )
    endif()
endif()



IF(WIN32)
    set(CMAKE_C_COMPILER     ${EABI_PREFIX}-gcc)     # absolute path needs to be set accordingly
    set(CMAKE_CXX_COMPILER   ${EABI_PREFIX}-g++)     # absolute path needs to be set accordingly
    set(AS                   ${EABI_PREFIX}-gcc-as)  # absolute path needs to be set accordingly
    set(AR                   ${EABI_PREFIX}-gcc-ar)  # absolute path needs to be set accordingly
    set(OBJCOPY              ${EABI_PREFIX}-objcopy) # absolute path needs to be set accordingly
    set(OBJDUMP              ${EABI_PREFIX}-objdump) # absolute path needs to be set accordingly
    set(SIZE                 ${EABI_PREFIX}-size)    # absolute path needs to be set accordingly
    set(GDB                  ${EABI_PREFIX}-gdb)     # absolute path needs to be set accordingly
elseif(UNIX)
    include(CMakeForceCompiler)

    set(ARM_TOOLS_DIR           "/opt/st/stm32cubeide_1.0.2/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.7-2018-q2-update.linux64_1.0.0.201904181610/tools/bin")
    set(CMAKE_C_COMPILER_WORKS  1)
    set(CMAKE_C_COMPILER        "${ARM_TOOLS_DIR}/${EABI_PREFIX}-gcc")  # absolute path needs to be set accordingly
    set(CMAKE_CXX_COMPILER      "${ARM_TOOLS_DIR}/${EABI_PREFIX}-g++")  # absolute path needs to be set accordingly

    CMAKE_FORCE_C_COMPILER  (${CMAKE_C_COMPILER}   GNU)                 # absolute path needs to be set accordingly
    CMAKE_FORCE_CXX_COMPILER(${CMAKE_CXX_COMPILER} GNU)                 # absolute path needs to be set accordingly

    set(AS                   ${ARM_TOOLS_DIR}/${EABI_PREFIX}-gcc-as)    # absolute path needs to be set accordingly
    set(AR                   ${ARM_TOOLS_DIR}/${EABI_PREFIX}-gcc-ar)    # absolute path needs to be set accordingly
    set(OBJCOPY              ${ARM_TOOLS_DIR}/${EABI_PREFIX}-objcopy)   # absolute path needs to be set accordingly
    set(OBJDUMP              ${ARM_TOOLS_DIR}/${EABI_PREFIX}-objdump)   # absolute path needs to be set accordingly
    set(SIZE                 ${ARM_TOOLS_DIR}/${EABI_PREFIX}-size)      # absolute path needs to be set accordingly
    set(GDB                  "${ARM_TOOLS_DIR}/${EABI_PREFIX}-gdb")     # absolute path needs to be set accordingly
endif()

if (WIN32)
    SET(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32F401RE_FLASH.ld)
    SET(CMAKE_EXE_LINKER_FLAGS "-Wl,-gc-sections -T ${LINKER_SCRIPT}")
    SET(CMAKE_EXE_LINKER_FLAGS_INIT "-Wmissing-prototypes -Wmissing-declarations -Wl,-gc-sections,--print-memory-usage -T $${LINKER_SCRIPT}")
endif()

project(avionics)

enable_language(ASM)
enable_language(C)

if (UNIX)
    SET(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32F401RE_FLASH.ld)
    SET(CMAKE_EXE_LINKER_FLAGS "-Wl,-gc-sections -T ${LINKER_SCRIPT}")
    SET(CMAKE_EXE_LINKER_FLAGS_INIT "-Wmissing-prototypes -Wmissing-declarations -Wl,-gc-sections,--print-memory-usage -T $${LINKER_SCRIPT}")
endif()

# Add -O0 to remove optimizations when using gcc
IF(CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")
ENDIF(CMAKE_COMPILER_IS_GNUCC)

add_definitions(-DSTM32F401xE)
add_definitions(-DUSE_HAL_DRIVER)
add_definitions("-D__weak=__attribute__((weak))")
add_definitions("-D__packed=__attribute__((__packed__))")


set(CMAKE_C_FLAGS "-g -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=gnu11 \
                   -specs=nano.specs -fstack-usage -fdata-sections -fdata-sections"
        )

SET(CMAKE_CXX_FLAGS "-g -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=c++11 \
                    -specs=nano.specs -fstack-usage -fdata-sections -fdata-sections"
        )


set(SYS_CALLS_SRC syscalls.c)

add_library(CMSIS_LIB
        Src/system_stm32f4xx.c
        Src/stm32f4xx_hal_msp.c
        Src/stm32f4xx_hal_timebase_tim.c
        Src/stm32f4xx_it.c
        startup/startup_stm32f401xe.s
        )

add_library(RTOS_LIB
        Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/cmsis_os.c
        Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c
        Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c
        Middlewares/Third_Party/FreeRTOS/Source/croutine.c
        Middlewares/Third_Party/FreeRTOS/Source/event_groups.c
        Middlewares/Third_Party/FreeRTOS/Source/list.c
        Middlewares/Third_Party/FreeRTOS/Source/queue.c
        Middlewares/Third_Party/FreeRTOS/Source/tasks.c
        Middlewares/Third_Party/FreeRTOS/Source/timers.c
        )

file(GLOB_RECURSE USER_SRC "Src/*.c")
file(GLOB_RECURSE HAL_SRC "Drivers/STM32F4xx_HAL_Driver/Src/*.c")


include_directories(Drivers/STM32F4xx_HAL_Driver/Inc)
include_directories(Drivers/CMSIS/Include)
include_directories(Drivers/CMSIS/Device/ST/STM32F4xx/Include)
include_directories(Src)
include_directories(Inc)
include_directories(Middlewares/Third_Party/FreeRTOS/Source)
include_directories(Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS)
include_directories(Middlewares/Third_Party/FreeRTOS/Source/include)
include_directories(Middlewares/Third_Party/FreeRTOS/Source/portable)
include_directories(Middlewares/Third_Party/FreeRTOS/Source/portable/GCC)
include_directories(Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F)
include_directories(Drivers/BMI08x-Sensor-API)
include_directories(Drivers/CMSIS/Device/ST/STM32F4xx/Include)
include_directories(Drivers/CMSIS/Include)
include_directories(Drivers/STM32F4xx_HAL_Driver/Inc)

add_executable(${PROJECT_NAME}.elf
        ${USER_SRC}
        ${HAL_SRC}
        ${SYS_CALLS_SRC}
        ${LINKER_SCRIPT}
        )

target_link_libraries(${PROJECT_NAME}.elf CMSIS_LIB RTOS_LIB)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.map")
set(HEX_FILE ${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.bin)

set_target_properties(${CMAKE_PROJECT_NAME}.elf PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE} \nBuilding ${BIN_FILE}")

add_custom_target(upload
        ${GDB} -iex "target remote tcp:localhost:3333"
        -iex "monitor program $<TARGET_FILE:${PROJECT_NAME}.elf>"
        -iex "monitor reset init"
        -iex "disconnect" -iex "quit")