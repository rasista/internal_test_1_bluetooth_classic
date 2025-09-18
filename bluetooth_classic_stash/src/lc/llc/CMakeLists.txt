cmake_minimum_required(VERSION 3.16)
include($ENV{WORKSPACE_PATH}/cmake/common.cmake)
include($ENV{WORKSPACE_PATH}/cmake/RailDepends.cmake)

message("ToolchainFile used for ${CMAKE_CURRENT_SOURCE_DIR}: ${CMAKE_TOOLCHAIN_FILE}")

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake/AutoGenTargets.cmake)
include(${UTILS}/debug/sl_btdm_logging/Cmake/debug_functions.cmake)
project(btc_llc C)

set(BTC_LLC ${CMAKE_CURRENT_SOURCE_DIR})



rail_dependencies(${RAIL_CHIP_NAME})

add_library(btc_llc)

target_compile_options(btc_llc PRIVATE -Os -g)


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Os -fstack-usage -Werror -DLLC_SEQUENCER")
#The Cmake either builds for a library or a executable based on the COMPILATION flag
#if compilation is set to cross, the build is for a executable, else it is for a library

if("${COMPILATION}" STREQUAL "CROSS")
    target_sources(btc_llc PRIVATE
        device_management/btc_rail_intf.c
    )

    # Include sub_folder for riscvseq_interface
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/riscvseq_interface
        ${CMAKE_CURRENT_BINARY_DIR}/riscvseq_interface)
elseif("${COMPILATION}" STREQUAL "LLC_LIB_FPGA")
    target_sources(btc_llc PRIVATE
        device_management/btc_rail_intf.c
    )
endif()


# THIS FLAG IS FOR JENKINS and is the default flag
if(DEFINED ENV{COMPILE_TEST_MODE} AND "$ENV{COMPILE_TEST_MODE}" STREQUAL "1")
    target_sources(btc_llc PRIVATE
        btc_test_mode/test_mode.c
        device_management/btc_mem_mgmt.c
    )
    target_compile_options(btc_llc PRIVATE -DCOMPILE_TEST_MODE)
endif()

if(DEFINED ENV{RUN_TEST_MODE} AND "$ENV{RUN_TEST_MODE}" STREQUAL "1")
    target_compile_options(btc_llc PRIVATE -DRUN_TEST_MODE)
endif()

if(DEFINED ENV{SET_BT_SLOT_WIDTH})
    target_compile_options(btc_llc PRIVATE -DBT_SLOT_WIDTH=$ENV{SET_BT_SLOT_WIDTH})
endif()

if(BUILD_STATIC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
endif()

#include riscseq_shared_vars.cmake present in $ENV{GSDK}/platform/radio/rail_lib/build/riscvseq_interface_lib

# no inclusion of test_mode.c file here so if the TEST_MODE=0 the above conditions are all bypassed and test_mode.c is not included
target_sources(btc_llc PRIVATE
    device_management/btc_device.c
    # device_management/btc_mem_mgmt.c
    device_management/btc_rail.c
    inquiry/inquiry.c
    inquiry_scan/inquiry_scan.c
    page/page.c
    page_scan/page_scan.c
    decoder/message_decode.c
    acl_central/acl_central.c
    acl_common/acl_common.c
    acl_peripheral/acl_peripheral.c
    esco_peripheral/esco_peripheral.c
    esco_central/esco_central.c
    roleswitch_peripheral/roleswitch_peripheral.c
    arqn_seqn/arqn_seqn_check.c
    roleswitch_central/roleswitch_central.c
    lpw_scheduler/lpw_scheduler.c
    lpw_hss/lpw_to_host_interface.c
    debug_logs/debug_logs.c
    #${RAIL_LIBINTF}/host_to_lpw_interface.c
    ${UTILS}/state_machine/state_machine.c
    ${UTILS}/queue/btc_queue.c
    ${UTILS}/debug/debug.c
    ${UTILS}/plt_deps/btc_plt_deps.c
)

target_include_directories(btc_llc PUBLIC
    device_management
    inquiry
    inquiry_scan
    page
    page_scan
    decoder
    lpw_hss
    acl_central
    acl_peripheral
    esco_peripheral
    esco_central
    acl_common
    roleswitch_peripheral
    arqn_seqn
    roleswitch_central
    lpw_scheduler
    riscvseq_interface
    debug_logs
    btc_test_mode
    ${UTILS}/state_machine
    ${UTILS}/include
    ${UTILS}/queue
    ${UTILS}/debug
    ${UTILS}/pkb_mgmt
    ${UTILS}/plt_deps
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}
    ${RAIL_LIBINTF}
    ${RAIL_LIBINTF}/data_path
    ${RAIL_LIBINTF}/autogen
    ${SCHEDULER}
    $ENV{GSDK}/platform/radio/rail_lib/common
    $ENV{GSDK}/platform/common/inc
    $ENV{GSDK}/platform/radio/rail_lib/chip/efr32/sixg3xx
    $ENV{GSDK}/platform/rvlib/inc
    $ENV{GSDK}/platform/radio/rail_lib/protocol/ble
    $ENV{GSDK}/platform/radio/rail_lib/protocol/btc
    $ENV{GSDK}/platform/radio/rail_lib/chip/efr32/seq_s3
    #${BTC_SRC}
    ${CHIP_SEQ_INCLUDE_PATH}
)

# List of all files generated through Jinja templates in form of 3-tuples
# Strictly follow format where first item as output file, second argument is template and third argument is configuration file
set(AUTOGEN_SM_RECIPES)
list (APPEND AUTOGEN_SM_RECIPES inquiry_sm_autogen.c ${UTILS}/state_machine/sm_autogen.c.j2 ${CMAKE_CURRENT_SOURCE_DIR}/inquiry/inquiry_config.yaml)
list (APPEND AUTOGEN_SM_RECIPES inquiry_sm_autogen.h ${UTILS}/state_machine/sm_autogen.h.j2 ${CMAKE_CURRENT_SOURCE_DIR}/inquiry/inquiry_config.yaml)
list (APPEND AUTOGEN_SM_RECIPES inquiry_scan_sm_autogen.c ${UTILS}/state_machine/sm_autogen.c.j2 ${CMAKE_CURRENT_SOURCE_DIR}/inquiry_scan/inquiry_scan_config.yaml)
list (APPEND AUTOGEN_SM_RECIPES inquiry_scan_sm_autogen.h ${UTILS}/state_machine/sm_autogen.h.j2 ${CMAKE_CURRENT_SOURCE_DIR}/inquiry_scan/inquiry_scan_config.yaml)
list (APPEND AUTOGEN_SM_RECIPES page_sm_autogen.c ${UTILS}/state_machine/sm_autogen.c.j2 ${CMAKE_CURRENT_SOURCE_DIR}/page/page_config.yaml)
list (APPEND AUTOGEN_SM_RECIPES page_sm_autogen.h ${UTILS}/state_machine/sm_autogen.h.j2 ${CMAKE_CURRENT_SOURCE_DIR}/page/page_config.yaml)
list (APPEND AUTOGEN_SM_RECIPES page_scan_sm_autogen.c ${UTILS}/state_machine/sm_autogen.c.j2 ${CMAKE_CURRENT_SOURCE_DIR}/page_scan/page_scan_config.yaml)
list (APPEND AUTOGEN_SM_RECIPES page_scan_sm_autogen.h ${UTILS}/state_machine/sm_autogen.h.j2 ${CMAKE_CURRENT_SOURCE_DIR}/page_scan/page_scan_config.yaml)
list (APPEND AUTOGEN_SM_RECIPES acl_central_sm_autogen.c ${UTILS}/state_machine/sm_autogen.c.j2 ${CMAKE_CURRENT_SOURCE_DIR}/acl_central/acl_central_config.yaml)
list (APPEND AUTOGEN_SM_RECIPES acl_central_sm_autogen.h ${UTILS}/state_machine/sm_autogen.h.j2 ${CMAKE_CURRENT_SOURCE_DIR}/acl_central/acl_central_config.yaml)
list (APPEND AUTOGEN_SM_RECIPES acl_peripheral_sm_autogen.c ${UTILS}/state_machine/sm_autogen.c.j2 ${CMAKE_CURRENT_SOURCE_DIR}/acl_peripheral/acl_peripheral_config.yaml)
list (APPEND AUTOGEN_SM_RECIPES acl_peripheral_sm_autogen.h ${UTILS}/state_machine/sm_autogen.h.j2 ${CMAKE_CURRENT_SOURCE_DIR}/acl_peripheral/acl_peripheral_config.yaml)
list (APPEND AUTOGEN_SM_RECIPES esco_peripheral_sm_autogen.c ${UTILS}/state_machine/sm_autogen.c.j2 ${CMAKE_CURRENT_SOURCE_DIR}/esco_peripheral/esco_peripheral_config.yaml)
list (APPEND AUTOGEN_SM_RECIPES esco_peripheral_sm_autogen.h ${UTILS}/state_machine/sm_autogen.h.j2 ${CMAKE_CURRENT_SOURCE_DIR}/esco_peripheral/esco_peripheral_config.yaml)
list (APPEND AUTOGEN_SM_RECIPES esco_central_sm_autogen.c ${UTILS}/state_machine/sm_autogen.c.j2 ${CMAKE_CURRENT_SOURCE_DIR}/esco_central/esco_central_config.yaml)
list (APPEND AUTOGEN_SM_RECIPES esco_central_sm_autogen.h ${UTILS}/state_machine/sm_autogen.h.j2 ${CMAKE_CURRENT_SOURCE_DIR}/esco_central/esco_central_config.yaml)
list (APPEND AUTOGEN_SM_RECIPES roleswitch_peripheral_sm_autogen.c ${UTILS}/state_machine/sm_autogen.c.j2 ${CMAKE_CURRENT_SOURCE_DIR}/roleswitch_peripheral/roleswitch_peripheral_config.yaml)
list (APPEND AUTOGEN_SM_RECIPES roleswitch_peripheral_sm_autogen.h ${UTILS}/state_machine/sm_autogen.h.j2 ${CMAKE_CURRENT_SOURCE_DIR}/roleswitch_peripheral/roleswitch_peripheral_config.yaml)
list (APPEND AUTOGEN_SM_RECIPES roleswitch_central_sm_autogen.c ${UTILS}/state_machine/sm_autogen.c.j2 ${CMAKE_CURRENT_SOURCE_DIR}/roleswitch_central/roleswitch_central_config.yaml)
list (APPEND AUTOGEN_SM_RECIPES roleswitch_central_sm_autogen.h ${UTILS}/state_machine/sm_autogen.h.j2 ${CMAKE_CURRENT_SOURCE_DIR}/roleswitch_central/roleswitch_central_config.yaml)
list (APPEND AUTOGEN_SM_RECIPES test_mode_sm_autogen.c ${UTILS}/state_machine/sm_autogen.c.j2 ${CMAKE_CURRENT_SOURCE_DIR}/btc_test_mode/btc_test_mode_config.yaml)
list (APPEND AUTOGEN_SM_RECIPES test_mode_sm_autogen.h ${UTILS}/state_machine/sm_autogen.h.j2 ${CMAKE_CURRENT_SOURCE_DIR}/btc_test_mode/btc_test_mode_config.yaml)

set(AUTOGEN_RECIPES)

list (APPEND AUTOGEN_RECIPES 
    procedure_triggers_autogen.h 
    ${CMAKE_CURRENT_SOURCE_DIR}/device_management/procedure_triggers_autogen.h.j2 
    ${CMAKE_CURRENT_SOURCE_DIR}/device_management/procedure_triggers.yaml
    )

list (APPEND AUTOGEN_RECIPES 
    sl_btc_seq_interface.h 
    ${UTILS}/user_intf/user_intf.h.j2 
    ${CMAKE_CURRENT_SOURCE_DIR}/device_management/lpw_user_intf.yaml
    )

math(EXPR need_sm_config "1")
add_autogen_targets(btc_llc AUTOGEN_SM_RECIPES ${need_sm_config})
math(EXPR need_sm_config "0")
add_autogen_targets(btc_llc AUTOGEN_RECIPES ${need_sm_config})

target_compile_options(btc_llc PUBLIC
                                -nodefaultlibs 
                                -nostartfiles
                                -fno-delete-null-pointer-checks
                                -fdata-sections
                                -ffunction-sections
                                -fomit-frame-pointer
                                -std=c99
                                -D${CHIP_CONFIG}
                                -DRAIL_INTERNAL_BUILD
                                -DBTC_CLOCK_PERIOD_US=$ENV{SET_BT_SLOT_WIDTH}
                            )

                        
function(write_target_source_files_to_json target output_file)
    # Filter the sources to include only .c files
    file(WRITE ${output_file} "{\n  \"user_seq_apps\": [\n    {\n      \"app_name\": \"BTC\",\n      \"app_code_size_allocation_bytes\": 32352,\n      \"app_data_allocation_bytes\": 4512\n    }\n  ]\n}\n")
endfunction()

# Call the custom function to write the source files for btc_llc to a JSON file 
write_target_source_files_to_json(btc_llc "${RISCVSEQ_INTERFACE}/riscvseq_link_ll_libs.json")
#intallation
install(TARGETS btc_llc DESTINATION ${CMAKE_INSTALL_PREFIX}/$<CONFIG>)

sl_btdm_logging_autogen_header_file(btc_llc
    sl_debug_btc_llc_autogen.h
)
sl_btdm_logging_add_fast_logging_producer(btc_llc)

