cmake_minimum_required(VERSION 3.0)

if(POLICY CMP0070)
    cmake_policy(SET CMP0070 NEW)
endif()
include($ENV{WORKSPACE_PATH}/cmake/common.cmake)
include($ENV{WORKSPACE_PATH}/cmake/RailDepends.cmake)

message("ToolchainFile used for ${CMAKE_CURRENT_SOURCE_DIR}: ${CMAKE_TOOLCHAIN_FILE}")

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake/AutoGenTargets.cmake)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} $ENV{GSDK}/cmake/module)
include(sled OPTIONAL RESULT_VARIABLE sled_FOUND)

project(btc_hal C)
set(LIBRAIL_INTF ${CMAKE_CURRENT_SOURCE_DIR})

rail_dependencies(${CHIP_NAME})

add_library(btc_hal)


call_seq_preset(${CHIP_NAME} btc_hal)


target_sources(btc_hal PRIVATE
    #${CMAKE_CURRENT_SOURCE_DIR}/librail_intf.c #ToDO: Add the following files after adding librail_intf.c and librail_intf.h in the PWD
    ${CMAKE_CURRENT_SOURCE_DIR}/data_path/data_path.c
    ${CMAKE_CURRENT_SOURCE_DIR}/host_to_lpw_interface.c    # install(TARGETS btc_ulc DESTINATION ${CMAKE_INSTALL_PREFIX})
    ${CMAKE_CURRENT_SOURCE_DIR}/lpw_to_host_mbox_interface.c
    ${LLC}/lib/release/generic_seq_btc.c
    ${UTILS}/../btc.c
    ${UTILS}/plt_deps/btc_plt_deps.c
    ${UTILS}/queue/btc_queue.c
    ${UTILS}/pkb_mgmt/pkb_mgmt.c
    ${UTILS}/access_code_gen/access_code_gen.c

)


target_include_directories(btc_hal PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    # $ENV{GSDK}/platform/common/inc
    # $ENV{GSDK}/platform/radio/rail_lib/common
    # $ENV{GSDK}/platform/radio/rail_lib/chip/efr32/sixg3xx
    # $ENV{GSDK}/platform/rvlib/inc
    # $ENV{GSDK}/platform/radio/rail_lib/protocol/ble
    # $ENV{GSDK}/platform/radio/rail_lib/chip/efr32/seq_s3
    # ${sixg300xilpwh72000_CHIP_SEQ_INCLUDE_PATH}
    #ToDO: Add the following directories after adding librail_intf.c and librail_intf.h in the PWD
    ${ULC}/hci                    # to clear the error dependency due to hci.h, btc_dev.h
    ${ULC}/host_interface
    ${UTILS}/debug                # to clear the error dependency due to debug.h
    ${UTILS}/pkb_mgmt             # to clear the error dependency due to pkb_mgmt.h
    ${UTILS}/queue                # to clear the error dependency due to btc_queue.h
    ${UTILS}/plt_deps            # to clear the error dependency due to btc_plt_deps.h
    ${UTILS}/access_code_gen      # to clear the error dependency due to access_code_gen.h
    ${ULC}                        # to clear the error dependency due to ulc.h
    ${ULC}/event_loop             # to clear the error dependency due to event_loop.h
    ${BTC_SRC}                 # to clear the error dependency due to btc.h   
    ${SCHEDULER}
    $ENV{GSDK}/platform/radio/rail_lib/common
    $ENV{GSDK}/platform/common/inc
    $ENV{GSDK}/platform/radio/rail_lib/chip/efr32/sixg3xx
    $ENV{GSDK}/platform/rvlib/inc
    $ENV{GSDK}/platform/radio/rail_lib/protocol/ble
    $ENV{GSDK}/platform/radio/rail_lib/chip/efr32/seq_s3
    ${CMAKE_CURRENT_SOURCE_DIR}/data_path
    ${CHIP_SEQ_INCLUDE_PATH}
)

set(AUTOGEN_RECIPES)

list (APPEND AUTOGEN_RECIPES 
    sl_btc_seq_interface.h 
    ${UTILS}/user_intf/user_intf.h.j2 
    ${LLC}/device_management/lpw_user_intf.yaml
    )

list (APPEND AUTOGEN_RECIPES 
    procedure_role_autogen.h 
    ${SCHEDULER}/usched_role_autogen.h.j2
    ${SCHEDULER}/usched_api.yaml
    )

list (APPEND AUTOGEN_RECIPES 
    sl_structs_btc_seq_interface.h 
    ${UTILS}/user_intf/struct_user_intf.h.j2 
    ${LLC}/device_management/lpw_user_intf.yaml
    )


math(EXPR need_sm_config "0")
add_autogen_targets(btc_hal AUTOGEN_RECIPES ${need_sm_config})
find_package(SLCC QUIET MODULE REQUIRED COMPONENTS rail_lib rail_seq_user_app_info_internal+device_generic_family_sixx300+fpga)
message("SLCC_COMPONENTS: ${SLCC_COMPONENTS}")

target_link_libraries(btc_hal PRIVATE ${SLCC_COMPONENTS})

if(sled_FOUND)
sled_target_set_slc_component_metadata(btc_hal 
    EXPORT TRUE
    COMPONENT bluetooth_classic_rail_interface
   )
else()
message(WARNING "sled not found")
endif()


#install target
install(TARGETS btc_hal DESTINATION ${CMAKE_INSTALL_PREFIX}/$<CONFIG>)

# Install headers preserving directory structure
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/sl_host_to_lpw_interface.h
              ${CMAKE_CURRENT_SOURCE_DIR}/host_to_lpw_interface.h
              ${CMAKE_CURRENT_SOURCE_DIR}/lpw_to_host_mbox_interface.h
              ${CMAKE_CURRENT_SOURCE_DIR}/autogen/sl_btc_seq_interface.h
              ${LLC}/build/riscvseq_interface/release/btc_riscvseq_address_map.h
              ${CMAKE_CURRENT_SOURCE_DIR}/autogen/sl_structs_btc_seq_interface.h
              ${UTILS}/pkb_mgmt/pkb_mgmt.h
              ${UTILS}/pkb_mgmt/pkb_mgmt_struct.h
              ${UTILS}/queue/btc_queue.h
              ${UTILS}/plt_deps/btc_plt_deps.h
              ${UTILS}/debug/debug.h
              ${UTILS}/../btc.h
              ${LLC}/../rail_libintf/stats.h
              ${CMAKE_CURRENT_SOURCE_DIR}/data_path/data_path.h
        DESTINATION "${INSTALL_PREFIX}")


target_compile_options(btc_hal PUBLIC
                                 -D${CHIP_CONFIG}
                                 -g
                                 -Os
                             )