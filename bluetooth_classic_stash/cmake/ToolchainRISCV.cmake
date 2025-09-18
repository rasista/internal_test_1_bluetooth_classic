# this file needs to be included before calling project()

set(PATH_GCC "/opt/gcc_riscv/bin")
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER ${PATH_GCC}/riscv32-unknown-elf-gcc${EXESUFFIX})
set(CMAKE_AR ${PATH_GCC}/riscv32-unknown-elf-gcc-ar${EXESUFFIX})
set(CMAKE_RANLIB ${PATH_GCC}/riscv32-unknown-elf-gcc-ranlib)
set(CMAKE_C_COMPILER_FORCED TRUE)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Os -fstack-usage -Werror")
set(CMAKE_ASM_FLAGS "${PLATFORM_FLAGS} ")
set(CMAKE_C_LINK_FLAGS "${PLATFORM_FLAGS} -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/build.map")

set(CMAKE_EXECUTABLE_SUFFIX ".elf")

# This is needed to stop CMake's CXX compiler test failure
set(CMAKE_CXX_LINK_FLAGS "--specs=nosys.specs")

SET(ELFTOOL ${PATH_GCC}riscv32-unknown-elf-objcopy${EXESUFFIX})
