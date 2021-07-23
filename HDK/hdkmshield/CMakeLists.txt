# For Bare Metal Generic Build
cmake_minimum_required (VERSION 3.16)
set (CMAKE_TOOLCHAIN_FILE "toolchain-gnu.cmake")


#Uncomment for hardware floating point
#SET(FPU_FLAGS "-mfloat-abi=hard -mfpu=fpv4-sp-d16")
#add_definitions(-DARM_MATH_CM4 -DARM_MATH_MATRIX_CHECK -DARM_MATH_ROUNDING -D__FPU_PRESENT=1)

#Uncomment for software floating point
#SET(FPU_FLAGS "-mfloat-abi=soft")
SET(FPU_FLAGS "")


SET (COMMON_FLAGS
        "-mcpu=cortex-m0plus ${FPU_FLAGS} -mthumb -g3 -Os -w -mfloat-abi=soft -ffunction-sections -fdata-sections -fno-use-cxa-atexit -MMD --param max-inline-insns-single=500")
SET (CMAKE_CXX_FLAGS_INIT
        "${COMMON_FLAGS} -nostdlib -std=gnu++11 -fno-threadsafe-statics -fno-rtti -fno-exceptions")
SET (CMAKE_C_FLAGS_INIT
        "${COMMON_FLAGS} -nostdlib --std=gnu11")

SET (CMAKE_EXE_LINKER_FLAGS_INIT
        "-Os -Wl,--cref -Wl,--check-sections -Wl,--gc-sections -save-temps --specs=nano.specs --specs=nosys.specs -Wl,--gc-sections -Wl,--unresolved-symbols=report-all -Wl,--warn-common -Wl,--warn-section-align")


PROJECT (hdkmshield C CXX ASM)
set (CMAKE_CXX_STANDARD 11)

file (GLOB_RECURSE SOURCES "adafruit/*.*" Libraries/*.* SensorAPI/*.* Sensors/*.* mshield.cpp)


add_compile_definitions (DEBUG
        F_CPU=48000000L
        ARDUINO=10813
        ARDUINO_SAMD_ZERO
        ARDUINO_ARCH_SAMD
        __SAMD21G18A__
        ARDUINO_SAMD_ZERO
        ARM_MATH_CM0PLUS
        ADAFRUIT_METRO_M0_EXPRESS
        _ATMEL_BIGENDIAN_
        USB_VID=0x239A
        USB_PID=0x8013
        USBCON
        USB_CONFIG_POWER=100 USB_MANUFACTURER="Adafruit"
        USB_PRODUCT="Metro M0 Express" )

include_directories (
        adafruit/hardware/samd/1.5.7/cores/arduino/Adafruit_TinyUSB_Core
        adafruit/hardware/samd/1.5.7/cores/arduino/Adafruit_TinyUSB_Core/tinyusb/src
        CMSIS-Atmel/1.2.0/CMSIS/Device/ATMEL
        CMSIS-Atmel/1.2.0/CMSIS/Device/ATMEL/samd21/include
        CMSIS-Atmel/1.2.0/CMSIS/Device/ATMEL/samd21/include/component
        adafruit/hardware/samd/1.5.7/cores/arduino
        adafruit/hardware/samd/1.5.7/variants/metro_m0
        adafruit/hardware/samd/1.5.7/cores/arduino/avr
        CMSIS/4.5.0/CMSIS/Include
        Libraries/Adafruit_Unified_Sensor
        Libraries/DHT_sensor_library
        Libraries/RTCZero
        SensorAPI
        Sensors)

SET (CMAKE_PREFIX_PATH
        ${CMAKE_FIND_ROOT_PATH}/lib/gcc/arm-none-eabi/10.2.1/thumb/v6-m/nofp ;
        ${CMAKE_FIND_ROOT_PATH}/arm-none-eabi/lib/thumb/v6-m/nofp)

SET (LINKER_SCRIPT
        ${CMAKE_SOURCE_DIR}/adafruit/hardware/samd/1.5.7/variants/metro_m0/linker_scripts/gcc/flash_with_bootloader.ld)

add_executable (${PROJECT_NAME}.elf ${SOURCES} adafruit/hardware/samd/1.5.7/cores/arduino/avr/dtostrf.c ${LINKER_SCRIPT})

# Library order is important. Don't change the order.
target_link_libraries(${PROJECT_NAME}.elf
        "${CMAKE_FIND_ROOT_PATH}/arm-none-eabi/lib/thumb/v6-m/nofp/libc_nano.a"
        "${CMAKE_FIND_ROOT_PATH}/lib/gcc/arm-none-eabi/10.2.1/thumb/v6-m/nofp/libgcc.a"
        "${CMAKE_FIND_ROOT_PATH}/arm-none-eabi/lib/thumb/v6-m/nofp/libnosys.a")


set (CMAKE_EXE_LINKER_FLAGS
        "${CMAKE_EXE_LINKER_FLAGS_INIT} -T${LINKER_SCRIPT} -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map ")


set (HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set (BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

add_custom_command (TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMENT "Size ${PROJECT_NAME}.elf"
        COMMAND ${SIZE} -x -A $<TARGET_FILE:${PROJECT_NAME}.elf>)
