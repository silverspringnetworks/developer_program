set(CMAKE_SYSTEM_NAME           Generic)
set(CMAKE_SYSTEM_VERSION        none)
set(CMAKE_SYSTEM_PROCESSOR      arm)

# Typical GNU install on Windows 10
SET(CMAKE_FIND_ROOT_PATH        "C:/GNU/10 2020-q4-major")
# Typical GNU install on Ubuntu
#SET(CMAKE_FIND_ROOT_PATH        "/var/opt/gcc-arm-none-eabi-10-2020-q4-major/bin")

# specify cross compilers and tools
set(tools                       ${CMAKE_FIND_ROOT_PATH}/bin)
# Windows 10 tool file extension
set(extension                   ".exe")
# Ubuntu tool file extension
#set(extension                   "")

SET(CMAKE_C_COMPILER_WORKS      1)
SET(CMAKE_C_COMPILER            ${tools}/arm-none-eabi-gcc${extension})
SET(CMAKE_CXX_COMPILER_WORKS    1)
SET(CMAKE_CXX_COMPILER          ${tools}/arm-none-eabi-g++${extension})
set(CMAKE_ASM_COMPILER          ${tools}/arm-none-eabi-gcc${extension})
set(CMAKE_AR                    ${tools}/arm-none-eabi-ar${extension})
set(CMAKE_LINKER                ${tools}/arm-none-eabi-ld${extension})
set(CMAKE_OBJCOPY               ${tools}/arm-none-eabi-objcopy${extension})
set(CMAKE_OBJDUMP               ${tools}/arm-none-eabi-objdump${extension})
set(SIZE                        ${tools}/arm-none-eabi-size${extension})
SET(CMAKE_ARM_COMPILATION       1)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
