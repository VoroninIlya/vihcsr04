set(CMAKE_SYSTEM_NAME               Generic)
set(CMAKE_SYSTEM_PROCESSOR          arm)

# Some default GCC settings
# arm-none-eabi- must be part of path environment
set(TOOLCHAIN_PREFIX                arm-none-eabi-)

# This needs attention to properly set for used MCU
set(CPU_PARAMETERS "-mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard")

set(linkerscript                   ${PROJ_PATH}/linker.ld)

SET(CMAKE_ASM_FLAGS "")
SET(CMAKE_C_FLAGS "${CPU_PARAMETERS} -fdata-sections -ffunction-sections -Wl,--gc-sections ")
SET(CMAKE_CXX_FLAGS "-fno-rtti -fno-exceptions -fno-threadsafe-statics -Wall -Wextra -Wpedantic -Wno-unused-parameter -Og -g3 -ggdb")
SET(CMAKE_EXE_LINKER_FLAGS "${CPU_PARAMETERS} -T${linkerscript} -Wl,-Map=${CMAKE_PROJECT_NAME}.map --specs=nosys.specs 
  -u _printf_float -Wl,--start-group -lc -lm -lstdc++ -lsupc++ -Wl,--end-group -Wl,--print-memory-usage")

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