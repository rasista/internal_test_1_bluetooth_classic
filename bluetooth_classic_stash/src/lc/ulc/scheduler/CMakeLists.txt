cmake_minimum_required(VERSION 3.16)
include($ENV{WORKSPACE_PATH}/cmake/common.cmake)

message("ToolchainFile used for ${CMAKE_CURRENT_SOURCE_DIR}: ${CMAKE_TOOLCHAIN_FILE}")
include(${CMAKE_CURRENT_SOURCE_DIR}/../../../../cmake/AutoGenTargets.cmake)
project(btc_usched C)

# Optimize for size
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os")

add_library(btc_usched)
# build-static option
if(BUILD_STATIC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
endif()
set(BT C_SIMPY "$ENV{BTC_SIMPY}")
set(BTsC_EVENT_LOOP "$ENV{BTC_EVENT_LOOP}")
set(BTC_UTILS "$ENV{BTC_UTILS}")
set(BTC_THREADS "$ENV{BTC_THREADS}")

add_dependencies(btc_usched btc_hal)

target_sources(btc_usched PRIVATE
    usched_common_role.c
    usched_acl.c
    usched_pi.c
    usched_pi_scan.c
    usched_port.c
    usched.c
    ${ULC}/event_loop/event_loop.c
)
target_include_directories(btc_usched PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/
    ${UTILS}/debug
    ${ULC}/event_loop/
    ${UTILS}/plt_deps
    ${ULC}/../../
    # ${BTC_SRC}   
    ${ULC}
    ${ULC}/hci
    ${ULC}/host_interface
    ${UTILS}/queue
    ${UTILS}/pkb_mgmt    
    ${RAIL_LIBINTF}
    ${RAIL_LIBINTF}/autogen
)

# List of all files generated through Jinja templates in form of 3-tuples
# Strictly follow format where first item as output file, second argument is template and third argument is configuration file

set(AUTOGEN_RECIPES)

 list (APPEND AUTOGEN_RECIPES 
     usched_api.h 
     ${CMAKE_CURRENT_SOURCE_DIR}/usched_api.h.j2 
     ${CMAKE_CURRENT_SOURCE_DIR}/usched_api.yaml
     )

 list (APPEND AUTOGEN_RECIPES 
     usched_role_autogen.h 
     ${CMAKE_CURRENT_SOURCE_DIR}/usched_role_autogen.h.j2
     ${CMAKE_CURRENT_SOURCE_DIR}/usched_api.yaml
     )

math(EXPR need_sm_config "0")
add_autogen_targets(btc_usched AUTOGEN_RECIPES ${need_sm_config})

#install target
install(TARGETS btc_usched DESTINATION ${CMAKE_INSTALL_PREFIX}/$<CONFIG>)
