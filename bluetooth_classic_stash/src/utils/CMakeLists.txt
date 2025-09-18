cmake_minimum_required(VERSION 3.16)
if(POLICY CMP0070)
    cmake_policy(SET CMP0070 NEW)
endif()
include($ENV{WORKSPACE_PATH}/cmake/common.cmake)
include($ENV{WORKSPACE_PATH}/cmake/RailDepends.cmake)
message("ToolchainFile used for ${CMAKE_CURRENT_SOURCE_DIR}: ${CMAKE_TOOLCHAIN_FILE}")

project(btc_common C)
set(UTILS ${CMAKE_CURRENT_SOURCE_DIR})
rail_dependencies(${CHIP_NAME})

add_library(btc_common)

target_sources(btc_common PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/debug/debug.c
    ${CMAKE_CURRENT_SOURCE_DIR}/pkb_mgmt/pkb_mgmt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/queue/btc_queue.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../btc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/access_code_gen/access_code_gen.c
)
target_include_directories(btc_common PUBLIC
    ${UTILS}/debug/
    ${UTILS}/queue/
    ${UTILS}/pkb_mgmt/
    ${UTILS}/plt_deps/
    ${UTILS}/access_code_gen/
    ${UTILS}/../lc/ulc/ut/linapps/btc_virtual_controller/   #cleanup, shouldn't be here
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    $ENV{GSDK}/platform/radio/rail_lib/common
    $ENV{GSDK}/platform/common/inc
    $ENV{GSDK}/platform/radio/rail_lib/chip/efr32/sixg3xx
    $ENV{GSDK}/platform/rvlib/inc
    $ENV{GSDK}/platform/radio/rail_lib/protocol/ble
    $ENV{GSDK}/platform/radio/rail_lib/chip/efr32/seq_s3
    ${CHIP_SEQ_INCLUDE_PATH}
)

target_compile_options(btc_common PUBLIC
                             -D${CHIP_CONFIG}
                             -g
                             -Os
                        )

install(TARGETS btc_common DESTINATION ${CMAKE_INSTALL_PREFIX}/$<CONFIG>)