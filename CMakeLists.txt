cmake_minimum_required(VERSION 3.5.1)
include(CMakeForceCompiler)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)

# Attolic ARM-Tools
#set(ARM_TOOLS_DIR       "/opt/Atollic_TrueSTUDIO_for_STM32_x86_64_9.3.0/ARMTools/bin")
#set(EABI_PREFIX         "arm-atollic-eabi")

# STM32Cube ARM-Tools
set(ARM_TOOLS_DIR       "/opt/st/stm32cubeide_1.0.2/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.7-2018-q2-update.linux64_1.0.0.201904181610/tools/bin")
set(EABI_PREFIX         "arm-none-eabi")

set(CMAKE_C_COMPILER   "${ARM_TOOLS_DIR}")
set(CMAKE_CXX_COMPILER "${ARM_TOOLS_DIR}")

#### specify the cross compiler
CMAKE_FORCE_C_COMPILER  (${ARM_TOOLS_DIR}/${EABI_PREFIX}-gcc GNU) # absolute path needs to be set accordingly
CMAKE_FORCE_CXX_COMPILER(${ARM_TOOLS_DIR}/${EABI_PREFIX}-g++ GNU) # absolute path needs to be set accordingly

project(avionics C CXX ASM)
enable_language(ASM)

SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(AS                          ${ARM_TOOLS_DIR}/${EABI_PREFIX}-gcc-as)  # absolute path needs to be set accordingly
set(AR                          ${ARM_TOOLS_DIR}/${EABI_PREFIX}-gcc-ar)  # absolute path needs to be set accordingly
set(OBJCOPY                     ${ARM_TOOLS_DIR}/${EABI_PREFIX}-objcopy) # absolute path needs to be set accordingly
set(OBJDUMP                     ${ARM_TOOLS_DIR}/${EABI_PREFIX}-objdump) # absolute path needs to be set accordingly
set(SIZE                        ${ARM_TOOLS_DIR}/${EABI_PREFIX}-size)    # absolute path needs to be set accordingly

SET(CMAKE_EXE_LINKER_FLAGS_INIT "-Wmissing-prototypes -Wmissing-declarations -Wl,-gc-sections,--print-memory-usage -T $${LINKER_SCRIPT}")

SET(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32F401RE_FLASH.ld)
SET(CMAKE_EXE_LINKER_FLAGS "-Wl,-gc-sections -T ${LINKER_SCRIPT}")

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

add_custom_target(UPLOAD
        ${ARM_TOOLS_DIR}/${EABI_PREFIX}-gdb -iex "target remote tcp:localhost:3333"
        -iex "monitor program $<TARGET_FILE:${PROJECT_NAME}.elf>"
        -iex "monitor reset init"
        -iex "disconnect" -iex "quit")