include($ENV{WORKSPACE_PATH}/cmake/common.cmake)
function(rail_dependencies CHIP_NAME)
    set(DEVICE_DIR "$ENV{GSDK}/platform/Device")

    if("${CHIP_NAME}" STREQUAL "sixg300xilpwh74000fpga")
        set(CHIP_SEQ_INCLUDE_PATH "${DEVICE_DIR}/SiliconLabs-Internal/SIXG300XILPWH74000_SEQUENCER/Include")
        set(CHIP_CONFIG "SIXG300XILPWH74000XFULL")
    elseif("${CHIP_NAME}" STREQUAL "sixg300xilpwh72000fpga")
        set(CHIP_SEQ_INCLUDE_PATH "${DEVICE_DIR}/SiliconLabs-Internal/SIXG300XILPWH72000_SEQUENCER/Include")
        set(CHIP_CONFIG "SIXG300XILPWH72000XFULL")
    else()
        message(FATAL_ERROR "Unknown CHIP_NAME: ${CHIP_NAME}")
    endif()

  
    set(CHIP_SEQ_INCLUDE_PATH "${CHIP_SEQ_INCLUDE_PATH}" PARENT_SCOPE)
    set(CHIP_CONFIG "${CHIP_CONFIG}" PARENT_SCOPE)
endfunction()


function(call_seq_preset CHIP_NAME TARGET_NAME)
    set(SEQ_BTC_FW ${LLC}/lib/release/generic_seq_btc.c)
    set(SEQ_BTC_FW_MARKER ${SEQ_BTC_FW}.marker)


    if("${CHIP_NAME}" STREQUAL "sixg300xilpwh74000fpga")
        add_custom_command(
            OUTPUT ${SEQ_BTC_FW_MARKER} ${SEQ_BTC_FW}
            COMMAND cd ${LLC} && ${CMAKE_COMMAND} --workflow --preset gcc/sixg300xilpwh74000fpga-lpw0
            COMMAND touch ${SEQ_BTC_FW_MARKER}
            COMMENT "Running custom CMake workflow to build LLC and generate ${SEQ_BTC_FW}"
        )
    elseif("${CHIP_NAME}" STREQUAL "sixg300xilpwh72000fpga")
        add_custom_command(
            OUTPUT ${SEQ_BTC_FW_MARKER} ${SEQ_BTC_FW}
            COMMAND cd ${LLC} && ${CMAKE_COMMAND} --workflow --preset gcc/sixg300xilpwh72000fpga-lpw0
            COMMAND touch ${SEQ_BTC_FW_MARKER}
            COMMENT "Running custom CMake workflow to build LLC and generate ${SEQ_BTC_FW}"
        )
    endif()

    add_custom_target(SEQ_BTC_FW_BUILD_MARKER DEPENDS ${SEQ_BTC_FW_MARKER})
    add_dependencies(${TARGET_NAME} SEQ_BTC_FW_BUILD_MARKER)

    add_custom_command(
        TARGET ${TARGET_NAME} POST_BUILD
        COMMAND rm ${SEQ_BTC_FW_MARKER}
    )
endfunction()

