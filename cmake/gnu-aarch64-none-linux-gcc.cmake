set(CMAKE_SYSTEM_NAME               Linux)
set(CMAKE_SYSTEM_PROCESSOR          aarch64)

# Some default GCC settings
# aarch64-linux-gnu- must be part of path environment
set(TOOLCHAIN_PREFIX                aarch64-none-linux-gnu-)

message("$ENV{AARCH64_LINUX_GNU_SYSROOT}")

SET(CMAKE_ASM_FLAGS "")
SET(CMAKE_C_FLAGS "-march=armv8-a --sysroot=$ENV{AARCH64_LINUX_GNU_SYSROOT}")
SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
SET(CMAKE_EXE_LINKER_FLAGS "--sysroot=$ENV{AARCH64_LINUX_GNU_SYSROOT}")

# Define compiler settings
set(CMAKE_C_COMPILER                ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_ASM_COMPILER              ${CMAKE_C_COMPILER})
set(CMAKE_CXX_COMPILER              ${TOOLCHAIN_PREFIX}g++)
set(CMAKE_OBJCOPY                   ${TOOLCHAIN_PREFIX}objcopy)
set(CMAKE_SIZE                      ${TOOLCHAIN_PREFIX}size)

set(CMAKE_EXECUTABLE_SUFFIX_ASM     ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C       ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX     ".elf")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)