cmake_minimum_required(VERSION 3.0)
include($ENV{WORKSPACE_PATH}/cmake/common.cmake)



message("ToolchainFile used for ${CMAKE_CURRENT_SOURCE_DIR}: ${CMAKE_TOOLCHAIN_FILE}")

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake/AutoGenTargets.cmake)

project(btc_ulc C)

set(BTC_ULC ${CMAKE_CURRENT_SOURCE_DIR})

# Optimize for size
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage -fPIC -g -fstack-usage -Os -DULC_FW")

add_library(btc_ulc)


if (NOT PYTEST_MODE)
  add_dependencies(btc_ulc btc_hal)
endif()

set(HCI_JINJA_FILES ${ULC}/hci/jinja_files)
set(HCI_YAML ${ULC}/hci/yaml_files)
set(HCI_UTILS ${PY_UTILS})

if (PYTEST_MODE)
    target_sources(btc_ulc PRIVATE
        ${RAIL_LIBINTF}/host_to_lpw_interface.c
    )
endif()

target_sources(btc_ulc PRIVATE
    ${ULC}/ulc.c
    ${ULC}/event_loop/event_loop.c
    ${ULC}/hci/hci.c
    ${ULC}/hci/btc_hci_rx_pkt_handlers.c
    ${ULC}/host_interface/host_interface.c
    ${ULC}/hci/command_handlers/vendor_specific_handlers.c
    ${ULC}/hci/command_handlers/hci_cmd_controller_bb_group.c
    ${ULC}/hci/command_handlers/hci_cmd_info_params_group.c
    ${ULC}/hci/command_handlers/hci_cmd_link_control_group.c
    ${ULC}/hci/command_handlers/hci_cmd_link_policy_group.c
    ${ULC}/hci/command_handlers/hci_cmd_status_params_group.c
    ${ULC}/hci/command_handlers/hci_cmd_testing_group.c
    ${ULC}/hci/btc_process_hci_cmd.c
    ${ULC}/hci/btc_hci_events.c
    ${ULC}/dm/btc_dm_authentication.c
    ${ULC}/dm/btc_dm.c
    ${ULC}/dm/btc_dm_simple_secure_pairing.c
    ${ULC}/dm/btc_dm_synchronous_conn.c
    ${ULC}/dm/btc_dm_sniff.c
    ${ULC}/lmp/btc_lmp.c
    ${ULC}/lmp/lmp.c
    ${ULC}/lmp/btc_key_mgmt_algos.c
    ${ULC}/lmp/lmp_rx.c
    ${ULC}/lmp/lmp_power_control/btc_lmp_power_control.c
    ${ULC}/lmp/lmp_synchronous_conn/btc_lmp_synchronous_conn.c
    ${ULC}/lmp/lmp_qos/btc_lmp_qos.c
    ${ULC}/lmp/lmp_afh_classification/lmp_afh_classification.c
    ${ULC}/lmp/lmp_authentication/btc_lmp_legacy_authentication.c
    ${ULC}/lmp/lmp_authentication/btc_lmp_simple_secure_pairing.c
    ${ULC}/lmp/lmp_test_mode/btc_lmp_rx_test_mode.c
    ${ULC}/lmp/lmp_test_mode/btc_lmp_test_mode.c
    ${ULC}/lmp/lmp_test_mode/btc_lmp_tx_test_mode.c
    ${ULC}/lmp/lmp_sniff/btc_lmp_sniff.c
    ${SCHEDULER}/usched_pi.c
    ${SCHEDULER}/usched_port.c
    ${SCHEDULER}/usched.c
    ${SCHEDULER}/usched_common_role.c
    ${SCHEDULER}/usched_pi_scan.c
)
target_include_directories(btc_ulc PUBLIC
    ${ULC}/../../
    ${ULC}
    ${ULC}/event_loop/
    ${ULC}/hci
    ${ULC}/dm
    ${ULC}/host_interface
    ${ULC}/scheduler
    ${ULC}/lmp
    ${ULC}/lmp/lmp_authentication
    ${ULC}/lmp/lmp_afh_classification
    ${ULC}/lmp/lmp_sniff
    ${ULC}/lmp/lmp_power_control
    ${ULC}/lmp/lmp_qos
    ${ULC}/lmp/lmp_synchronous_conn
    ${ULC}/lmp/lmp_test_mode
    ${UTILS}/debug/
    ${UTILS}/queue/
    ${UTILS}/pkb_mgmt/
    ${UTILS}/plt_deps/
    ${UTILS}/access_code_gen/
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}
    ${RAIL_LIBINTF}
    ${RAIL_LIBINTF}/autogen    # TODO: Need to see how to remove autogen folder in include path
    ${SCHEDULER}/autogen
)

# List of all files generated through Jinja templates in form of 3-tuples
# Strictly follow format where first item as output file, second argument is template and third argument is configuration file

set(AUTOGEN_RECIPES)

list (APPEND AUTOGEN_RECIPES
    lmp_tx_rx_handlers_autogen.h
    ${ULC}/lmp/jinja_files/lmp_tx_rx_handlers.h.j2
    ${ULC}/lmp/lmp.yaml
    )

list (APPEND AUTOGEN_RECIPES
    lmp_tx_rx_handlers_lut_autogen.c
    ${ULC}/lmp/jinja_files/lmp_tx_rx_handlers_lut.c.j2
    ${ULC}/lmp/lmp.yaml
    )


list (APPEND AUTOGEN_RECIPES
    hci_intf_autogen.py
    ${HCI_UTILS}/hci_classes/hci_intf.py.j2
    ${HCI_YAML}/hci_cmds.yaml
    )

list (APPEND AUTOGEN_RECIPES
    hci_common_api_autogen.py
    ${HCI_UTILS}/hci_classes/hci_common_api.py.j2
    ${HCI_YAML}/hci_cmds.yaml
)

list (APPEND AUTOGEN_RECIPES
    callback_handlers_def_autogen.py
    ${HCI_UTILS}/hci_classes/callback_handlers_def.py.j2
    ${HCI_YAML}/hci_events.yaml
)

list (APPEND AUTOGEN_RECIPES
    hci_event_vars_autogen.py
    ${HCI_UTILS}/hci_classes/hci_event_vars.py.j2
    ${HCI_YAML}/hci_events.yaml
)

list (APPEND AUTOGEN_RECIPES
     hci_event_callback_autogen.py
     ${HCI_UTILS}/hci_classes/hci_call_back.py.j2
     ${HCI_YAML}/hci_events.yaml
     )

list (APPEND AUTOGEN_RECIPES
    hci_lut_autogen.c
    ${HCI_JINJA_FILES}/hci_lut.c.j2
    ${HCI_YAML}/hci_cmds.yaml
)

list (APPEND AUTOGEN_RECIPES
    hci_handlers_autogen.h
    ${HCI_JINJA_FILES}/hci_handlers.h.j2
    ${HCI_YAML}/hci_cmds.yaml
) 

list (APPEND AUTOGEN_RECIPES
    hci_cmd_get_defines_autogen.h
    ${HCI_JINJA_FILES}/hci_cmd_get_defines.h.j2
    ${HCI_YAML}/hci_cmds.yaml
) 

list (APPEND AUTOGEN_RECIPES
    hci_cmd_set_defines_complete_event_autogen.h
    ${HCI_JINJA_FILES}/hci_cmd_set_defines_complete_event.h.j2
    ${HCI_YAML}/hci_cmds.yaml
)

list (APPEND AUTOGEN_RECIPES
    hci_cmd_set_defines_status_event_autogen.h
    ${HCI_JINJA_FILES}/hci_cmd_set_defines_status_event.h.j2
    ${HCI_YAML}/hci_cmds.yaml
)

list (APPEND AUTOGEN_RECIPES
    hci_events_ptl_autogen.h
    ${HCI_JINJA_FILES}/hci_events_ptl.h.j2
    ${HCI_YAML}/hci_events.yaml
)

list (APPEND AUTOGEN_RECIPES
    hci_events_set_defines_autogen.h
    ${HCI_JINJA_FILES}/hci_events_set_defines.h.j2
    ${HCI_YAML}/hci_events.yaml
)

list (APPEND AUTOGEN_RECIPES
    lmp_set_defines_autogen.h
    ${ULC}/lmp/jinja_files/lmp_set_defines.h.j2
    ${ULC}/lmp/lmp.yaml
)

list (APPEND AUTOGEN_RECIPES
    lmp_get_defines_autogen.h
    ${ULC}/lmp/jinja_files/lmp_get_defines.h.j2
    ${ULC}/lmp/lmp.yaml
)

# list (APPEND AUTOGEN_RECIPES
#     hci_cmd_definitions_autogen.c
#     ${HCI_JINJA_FILES}/hci_cmd_definitions.c.j2
#     ${HCI_YAML}/hci_cmds.yaml
# )

math(EXPR need_sm_config "0")
add_autogen_targets(btc_ulc AUTOGEN_RECIPES ${need_sm_config})

#install target
install(TARGETS btc_ulc DESTINATION ${CMAKE_INSTALL_PREFIX}/$<CONFIG>)
