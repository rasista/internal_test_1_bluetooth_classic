cmake_minimum_required(VERSION 3.16)
include($ENV{WORKSPACE_PATH}/cmake/common.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../../../../../cmake/AutoGenTargets.cmake)

project(btc_llc_ut_simpy)

add_executable(btc_llc_ut_simpy)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Os -fstack-usage -Werror -DSIM=1 -DLINUX_UTILS")

target_sources(btc_llc_ut_simpy PRIVATE
     rail_simulation.c
     host_lpw_interface.c
     ../../device_management/btc_rail.c
     ${BTC_LLL_SRC}/../../../ut_frameworks/c_utils/linux/threads/thread_protection.c
     )
     
add_subdirectory(${SIMPY_SRC}/ simpy_intf_binaries)
target_link_libraries(btc_llc_ut_simpy PRIVATE simpy_intf)
add_dependencies(btc_llc_ut_simpy simpy_intf)
 
add_subdirectory(${JSON_LIBS}/jansson-2.13/ jansson_binaries)
target_link_libraries(btc_llc_ut_simpy PRIVATE jansson)
add_dependencies(btc_llc_ut_simpy jansson)

add_subdirectory(${BTC_LLL_SRC}/ btc_llc_binaries)
target_link_libraries(btc_llc_ut_simpy PRIVATE btc_llc)
add_dependencies(btc_llc_ut_simpy btc_llc)

target_include_directories(btc_llc_ut_simpy PUBLIC
    ${BTC_LLL_SRC}/device_management
    ${BTC_LLL_SRC}/inquiry
    ${BTC_LLL_SRC}/inquiry_scan
    ${BTC_LLL_SRC}/page
    ${BTC_LLL_SRC}/page_scan
    ${BTC_LLL_SRC}/acl_central
    ${BTC_LLL_SRC}/acl_peripheral
    ${BTC_LLL_SRC}/esco_peripheral
    ${BTC_LLL_SRC}/esco_central
    ${BTC_LLL_SRC}/acl_common
    ${BTC_LLL_SRC}/roleswitch_peripheral
    ${BTC_LLL_SRC}/arqn_seqn
    ${BTC_LLL_SRC}/roleswitch_central
    ${UTILS}/state_machine
    ${BTC_LLL_SRC}/lpw_hss
    ${RAIL_LIBINTF}
    ${SIMPY_SRC}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(AUTOGEN_RECIPES)
list (APPEND AUTOGEN_RECIPES 
    cli_commands.c
    ${SIMPY_SRC}/command_template.j2 
    ${CMAKE_CURRENT_SOURCE_DIR}/llc_commands.yaml
    )

list (APPEND AUTOGEN_RECIPES 
    lpw_user_intf_json2c.c
    ${JSON_LIBS}/json2c.c.j2
    ${BTC_LLL_SRC}/device_management/lpw_user_intf.yaml
    )
list (APPEND AUTOGEN_RECIPES 
    hss_cmd_handler_autogen.h
    ${UT_JINJA_FILES}/hss_cmd_handler.h.j2
    ${BTC_LLL_SRC}/device_management/lpw_user_intf.yaml
    )
list (APPEND AUTOGEN_RECIPES 
    hss_cmd_handler_autogen.c
    ${UT_JINJA_FILES}/hss_cmd_handler.c.j2
    ${BTC_LLL_SRC}/device_management/lpw_user_intf.yaml
    )
list (APPEND AUTOGEN_RECIPES 
    lpw_intf_autogen.py
    ${PYTHON_JINJA_FILES}/user_intf.py.j2
    ${BTC_LLL_SRC}/device_management/lpw_user_intf.yaml
    )

math(EXPR need_sm_config "0")
add_autogen_targets(btc_llc_ut_simpy AUTOGEN_RECIPES ${need_sm_config})
