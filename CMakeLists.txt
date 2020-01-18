cmake_minimum_required(VERSION 3.5.1)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_VERBOSE_MAKEFILE ON)

project(avionics NONE)
find_package (Python 3.0 REQUIRED)
execute_process(COMMAND python generator.py WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

if(EXISTS "${CMAKE_SOURCE_DIR}/generated/CMakeConfigFile.cmake")
    include(${CMAKE_SOURCE_DIR}/generated/CMakeConfigFile.cmake)
else()
    message(FATAL_ERROR "Cannot generate project: please, first run \"python process-user-configurations.py\"")
endif()

if(UNIX)
    include(CMakeForceCompiler)
    set(CMAKE_C_COMPILER_WORKS                                     1)
    CMAKE_FORCE_C_COMPILER  (${ARM_TOOLS_DIR}/${EABI_PREFIX}-gcc GNU)   # absolute path needs to be set accordingly
    CMAKE_FORCE_CXX_COMPILER(${ARM_TOOLS_DIR}/${EABI_PREFIX}-g++ GNU)   # absolute path needs to be set accordingly
    set(AS                   ${ARM_TOOLS_DIR}/${EABI_PREFIX}-gcc-as )   # absolute path needs to be set accordingly
    set(AR                   ${ARM_TOOLS_DIR}/${EABI_PREFIX}-gcc-ar )   # absolute path needs to be set accordingly
    set(OBJCOPY              ${ARM_TOOLS_DIR}/${EABI_PREFIX}-objcopy)   # absolute path needs to be set accordingly
    set(OBJDUMP              ${ARM_TOOLS_DIR}/${EABI_PREFIX}-objdump)   # absolute path needs to be set accordingly
    set(SIZE                 ${ARM_TOOLS_DIR}/${EABI_PREFIX}-size   )   # absolute path needs to be set accordingly
    set(GDB                  ${ARM_TOOLS_DIR}/${EABI_PREFIX}-gdb    )   # absolute path needs to be set accordingly
else()
    set(CMAKE_C_COMPILER     ${EABI_PREFIX}-gcc                     )   # absolute path needs to be set accordingly
    set(CMAKE_CXX_COMPILER   ${EABI_PREFIX}-g++                     )   # absolute path needs to be set accordingly
    set(AS                   ${EABI_PREFIX}-gcc-as                  )   # absolute path needs to be set accordingly
    set(AR                   ${EABI_PREFIX}-gcc-ar                  )   # absolute path needs to be set accordingly
    set(OBJCOPY              ${EABI_PREFIX}-objcopy                 )   # absolute path needs to be set accordingly
    set(OBJDUMP              ${EABI_PREFIX}-objdump                 )   # absolute path needs to be set accordingly
    set(SIZE                 ${EABI_PREFIX}-size                    )   # absolute path needs to be set accordingly
    set(GDB                  ${EABI_PREFIX}-gdb                     )   # absolute path needs to be set accordingly
endif()

project(avionics)
enable_language(ASM)
enable_language(C)
enable_language(CXX)

SET(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32F401RE_FLASH.ld)
SET(CMAKE_EXE_LINKER_FLAGS "-Wl,-gc-sections -T ${LINKER_SCRIPT}")
SET(CMAKE_EXE_LINKER_FLAGS_INIT "-Wmissing-prototypes -Wmissing-declarations -Wl,-gc-sections,--print-memory-usage -T $${LINKER_SCRIPT}")

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


set(SYS_CALLS_SRC sys/syscalls.c)

add_library(CMSIS_LIB
        Drivers/CMSIS/Device/ST/STM32F4xx/Source/system_stm32f4xx.c
        Drivers/CMSIS/Device/ST/STM32F4xx/Source/stm32f4xx_hal_msp.c
        Drivers/CMSIS/Device/ST/STM32F4xx/Source/stm32f4xx_hal_timebase_tim.c
        Drivers/CMSIS/Device/ST/STM32F4xx/Source/stm32f4xx_it.c
        Startup/startup_stm32f401xe.s
        )

add_library(RTOS_LIB
        Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/cmsis_os.c
        Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c
        Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_1.c
        Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c
        Middlewares/Third_Party/FreeRTOS/Source/croutine.c
        Middlewares/Third_Party/FreeRTOS/Source/event_groups.c
        Middlewares/Third_Party/FreeRTOS/Source/list.c
        Middlewares/Third_Party/FreeRTOS/Source/queue.c
        Middlewares/Third_Party/FreeRTOS/Source/tasks.c
        Middlewares/Third_Party/FreeRTOS/Source/timers.c
        )

add_library(SPIFFS
        Src/fs/params_test.h
        Src/fs/spiffs.h
        Src/fs/spiffs_cache.c
        Src/fs/spiffs_check.c
        Src/fs/spiffs_config.h
        Src/fs/spiffs_gc.c
        Src/fs/spiffs_hydrogen.c
        Src/fs/spiffs_nucleus.c
        Src/fs/spiffs_nucleus.h
        Src/fs/fs.h
        Src/fs/fs.c
        )

file(GLOB_RECURSE USER_SRC "Src/*.c")
file(GLOB_RECURSE HAL_SRC "Drivers/STM32F4xx_HAL_Driver/Src/*.c")


include_directories(Drivers/STM32F4xx_HAL_Driver/Inc)
include_directories(Drivers/CMSIS/Include)
include_directories(Drivers/CMSIS/Device/ST/STM32F4xx/Include)
include_directories(Src)
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

target_link_libraries(${PROJECT_NAME}.elf CMSIS_LIB RTOS_LIB -lm)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_SOURCE_DIR}/cmake-build-debug/${PROJECT_NAME}.map")
set(HEX_FILE ${PROJECT_SOURCE_DIR}/cmake-build-debug/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_SOURCE_DIR}/cmake-build-debug/${PROJECT_NAME}.bin)

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