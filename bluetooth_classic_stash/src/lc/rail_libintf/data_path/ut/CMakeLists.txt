cmake_minimum_required(VERSION 3.10)
project(data_path_test C)

include($ENV{WORKSPACE_PATH}/cmake/common.cmake)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Use UTILS_PATH from cache or default
if(NOT DEFINED UTILS_PATH)
  set(UTILS_PATH ${CMAKE_SOURCE_DIR}/src/lc/llc/utils)
endif()

# Only add real source directories, not inc
include_directories(
    ../.. # src/lc/rail_libintf
    ${UTILS_PATH}/queue
    ${UTILS_PATH}/pkb_mgmt
    ${UTILS_PATH}/plt_deps
    ${UTILS_PATH}/debug
    ${UTILS_PATH}/access_code_gen
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${RISCVSEQ_INTERFACE}/release
    ${CMAKE_SOURCE_DIR}/ut_frameworks/c_utils/linux/threads
)

# Source files
set(SRC_FILES
    data_path_test.c
    ../data_path.c
    ${UTILS_PATH}/debug/debug.c
    ${UTILS_PATH}/plt_deps/btc_plt_deps.c
    ../../../../../ut_frameworks/c_utils/linux/threads/thread_protection.c
)

add_executable(data_path_test ${SRC_FILES})

target_compile_options(data_path_test PRIVATE -Wall -Wextra -Werror -DLINUX_UTILS)
