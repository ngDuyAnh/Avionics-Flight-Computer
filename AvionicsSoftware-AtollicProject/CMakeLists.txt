cmake_minimum_required(VERSION 3.7)
INCLUDE(CMakeForceCompiler)
SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_VERSION 1)

set(CMAKE_C_COMPILER   "${CMAKE_SOURCE_DIR}/arm-tools-cube/bin")
set(CMAKE_CXX_COMPILER "${CMAKE_SOURCE_DIR}/arm-tools-cube/bin")

### specify the cross compiler
CMAKE_FORCE_C_COMPILER(${CMAKE_C_COMPILER}/arm-none-eabi-gcc     GNU)
CMAKE_FORCE_CXX_COMPILER(${CMAKE_CXX_COMPILER}/arm-none-eabi-g++ GNU)

#SET(COMMON_FLAGS "-mcpu=cortex-m4 -mthumb -mthumb-interwork -mfloat-abi=soft -ffunction-sections -fdata-sections -g -fno-common -fmessage-length=0")
SET(COMMON_FLAGS "-mcpu=cortex-m4 -mthumb -mthumb-interwork -mfloat-abi=hard -mfpu=fpv4-sp-d16 -ffunction-sections -fdata-sections -g -fno-common -fmessage-length=0")
SET(CMAKE_CXX_FLAGS "${COMMON_FLAGS} -std=c++11")
SET(CMAKE_C_FLAGS "${COMMON_FLAGS} -std=gnu99")
SET(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32F401RE_FLASH.ld)
SET(CMAKE_EXE_LINKER_FLAGS "-Wl,-gc-sections -T ${LINKER_SCRIPT}")


project(avionics C CXX ASM)
enable_language(ASM)

SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_VERBOSE_MAKEFILE ON)


set(AS                          ${CMAKE_SOURCE_DIR}/arm-tools-cube/bin/arm-none-eabi-as)
set(AR                          ${CMAKE_SOURCE_DIR}/arm-tools-cube/bin/arm-none-eabi-ar)
set(OBJCOPY                     ${CMAKE_SOURCE_DIR}/arm-tools-cube/bin/arm-none-eabi-objcopy)
set(OBJDUMP                     ${CMAKE_SOURCE_DIR}/arm-tools-cube/bin/arm-none-eabi-objdump)
set(SIZE                        ${CMAKE_SOURCE_DIR}/arm-tools-cube/bin/arm-none-eabi-size)


#SET(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32F401RE_FLASH.ld)

SET(CMAKE_C_FLAGS "-mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=gnu11")
#
SET(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,-gc-sections,--print-memory-usage -T $${LINKER_SCRIPT}")
SET(CMAKE_EXE_LINKER_FLAGS "-Wl,-gc-sections -T ${LINKER_SCRIPT}")

add_definitions(-DSTM32F401xE)
add_definitions(-DUSE_HAL_DRIVER)
add_definitions("-D__weak=__attribute__((weak))")
add_definitions("-D__packed=__attribute__((__packed__))")

set(CMAKE_C_FLAGS "-mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=gnu11")

set(SYS_CALLS_SRC syscalls.c)

add_library(CMSIS_LIB
            Drivers/CMSIS/Device/ST/STM32F4xx/Source/system_stm32f4xx.c
            Drivers/CMSIS/Device/ST/STM32F4xx/Source/stm32f4xx_hal_msp.c
            Drivers/CMSIS/Device/ST/STM32F4xx/Source/stm32f4xx_hal_timebase_tim.c
            Drivers/CMSIS/Device/ST/STM32F4xx/Source/stm32f4xx_it.c
            startup/startup_stm32f401xe.s
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

file(GLOB_RECURSE USER_SRC "Src/*.c")
file(GLOB_RECURSE HAL_SRC "Drivers/STM32F4xx_HAL_Driver/Src/*.c")


include_directories(Inc)
include_directories(Drivers/STM32F4xx_HAL_Driver/Inc)
include_directories(Drivers/CMSIS/Include)
include_directories(Drivers/CMSIS/Device/ST/STM32F4xx/Include)

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

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE} \nBuilding ${BIN_FILE}")

add_custom_target(UPLOAD
        ${CMAKE_SOURCE_DIR}/arm-tools-cube/bin/arm-none-eabi-gdb -iex "target remote tcp:localhost:3333"
        -iex "monitor program $<TARGET_FILE:${PROJECT_NAME}.elf>"
        -iex "monitor reset init"
        -iex "disconnect" -iex "quit")