#Cmake for testing btc_virtual_controller
#Add minimum version of cmake

cmake_minimum_required(VERSION 3.5)

cmake_policy(SET CMP0076 NEW)
#Add project name
include($ENV{WORKSPACE_PATH}/cmake/common.cmake)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage -fPIC -g -Os -fstack-usage -Werror -DSCHED_SIM=1 -DLINUX_UTILS -DSIM=1")

if (${PROCESSOR} MATCHES "RISCV")
    include(${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../cmake/ToolchainRISCV.cmake)
elseif(${PROCESSOR} MATCHES "ARM")
    include(${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../cmake/ToolchainARMGCC.cmake)
endif()
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os -DSCHED_SIM")

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../cmake/AutoGenTargets.cmake)

project(btc_virtual_controller C)
#generate executeable file
add_executable(btc_virtual_controller)
#Add library
add_subdirectory(${ULC}/ btc_ulc_bin)
target_link_libraries(btc_virtual_controller btc_ulc)
add_dependencies(btc_virtual_controller btc_ulc)

add_subdirectory(${UTILS}/ btc_common_bin)
target_link_libraries(btc_virtual_controller btc_common)
add_dependencies(btc_virtual_controller btc_common)

target_link_libraries(btc_virtual_controller pthread)

#Add source files to the project
target_sources(btc_virtual_controller PRIVATE
        main.c
        sl_hci_packet.c
        tx_rx_handling_for_virtual_link.c
		ssp_sw_algos.c
        sw_ecdh.c
        ${LINUX_UTILS}/threads/thread.c
        ${LINUX_UTILS}/threads/thread_protection.c
        ${LINUX_UTILS}/uart/uart.c
        ${LINUX_UTILS}/virtual_link/virtual_link.c
        ${LINUX_UTILS}/sokets/tcp.c
        ${ULC}/../../btc.c
        ${ULC}/../../utils/plt_deps/btc_plt_deps.c
    )


target_include_directories(btc_virtual_controller PUBLIC
    ${LINUX_UTILS}/threads
    ${LINUX_UTILS}/uart
    ${LINUX_UTILS}/virtual_link
    ${LINUX_UTILS}/sokets
    ${ULC}/
    ${ULC}/../../
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${ULC}/ut/linapps/btc_virtual_controller
    ${ULC}/../../utils/plt_deps
)
