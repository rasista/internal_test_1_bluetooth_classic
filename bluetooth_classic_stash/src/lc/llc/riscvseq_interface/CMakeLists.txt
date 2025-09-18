include($ENV{BTC}/cmake/CodeSizeAnalysis.cmake)
cmake_minimum_required(VERSION 3.16)
include($ENV{BTC}/cmake/common.cmake)
message("ToolchainFile used for ${CMAKE_CURRENT_SOURCE_DIR}: ${CMAKE_TOOLCHAIN_FILE}")


project(btc_riscvseq C)

add_executable(btc_riscvseq)

# Optimize for size
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os")

#Add a dummy main.c file to the executable
target_sources(btc_riscvseq PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/main.c)
include($ENV{GSDK}/platform/radio/rail_lib/build/riscvseq_interface_lib/riscvseq_shared_vars.cmake)

# target_include_directories(btc_riscvseq PRIVATE "$ENV{GSDK}/platform/radio/rail_lib/common")
#Add a subdirectory for riscv_seq_interface_library, This will build the library and link it to the executable
add_subdirectory($ENV{GSDK}/platform/radio/rail_lib/build/riscvseq_interface_lib
    ${CMAKE_CURRENT_BINARY_DIR}/riscvseq_interface_lib)
#Link the riscvseq_interface_lib to the executable
target_link_libraries(btc_riscvseq PRIVATE ${RISCVSEQ_INTERFACE_LIB})
#Add the riscvseq_interface_lib as a dependency to the executable,
#this will ensure that the library is built before the executable
add_dependencies(btc_riscvseq ${RISCVSEQ_INTERFACE_LIB})

#Link the btc_llc library to the executable
target_link_libraries(btc_riscvseq PRIVATE btc_llc)
#Add the btc_llc library as a dependency to the executable,
#this will ensure that the library is built before the executable
add_dependencies(btc_riscvseq btc_llc)


riscvseq_add_btc_depedencies(btc_riscvseq)

target_compile_options(btc_riscvseq PRIVATE -g)

#below is just a workaround right now to ensure that page and inquiry related symbols are all present in compilation.
#Need to identify a better entry point.
target_link_options(btc_riscvseq PRIVATE -Wl,--undefined=rail_user_btc_int)
target_link_options(btc_riscvseq PRIVATE "-Wl,-Map=${CMAKE_CURRENT_SOURCE_DIR}/seq/${RAIL_CHIP_NAME}/$<CONFIG>/riscvseq_${RAIL_CHIP_NAME}.map")





# Link RISCV sequencer standard libs
riscvseq_gen_app_linker_script(btc_riscvseq  
                           ${CMAKE_CURRENT_SOURCE_DIR}/riscvseq_link_ll_libs.json 
                           ${CMAKE_CURRENT_SOURCE_DIR}/seq/${RAIL_CHIP_NAME}/$<CONFIG>/riscvseq_${RAIL_CHIP_NAME}.ld
                    )

set(riscvseq_app_bin_files "")
set(riscvseq_app_init_functions  "")

SET(bin_file                            "${CMAKE_CURRENT_SOURCE_DIR}/seq/${RAIL_CHIP_NAME}/$<CONFIG>/riscvseq_${RAIL_CHIP_NAME}.s37")
SET(output_c_file                       "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/generic_seq.c")
SET(app_output_c_file                   "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/generic_seq_btc.c")
SET(app_init_fn                         "rail_user_btc_int")
# Generate RISCV RAIL sequencer binary file
add_custom_command(
    TARGET btc_riscvseq POST_BUILD
    # Post Build artifacts
    COMMAND ${CMAKE_OBJCOPY} -j ".seq_vtable" -j ".btc_application_text"
        -j ".btc_application_data" --output-target=srec $<CONFIG>/btc_riscvseq ${bin_file}
    COMMAND python3 ${LLC}/riscvseq_interface/generate_address_map.py
        -e $<CONFIG>/btc_riscvseq -o ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/btc_riscvseq_address_map.h
        -t ${CMAKE_NM}
    COMMAND ${CMAKE_OBJDUMP} -S $<CONFIG>/btc_riscvseq > ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/btc_riscvseq.asm
    COMMENT "Generating RISCV Seq bin: ${bin_file}"
)
# add_custom_command(
#     TARGET btc_riscvseq POST_BUILD
#     # Post Build artifacts
#     COMMAND ${CMAKE_OBJCOPY} -j ".seq_vtable" -j ".btc_application_text"
#         -j ".btc_application_data" --output-target=srec $<CONFIG>/btc_riscvseq ${bin_file}
#     COMMENT "Generating RISCV Seq bin: ${bin_file}"
# )
# Generate RISCV RAIL sequencer and application .c
riscvseq_gen_bin_to_c_app(btc_riscvseq
              ${bin_file}
              ${app_output_c_file}
              ${app_init_fn})
# isntall the executable
install(TARGETS btc_riscvseq DESTINATION ${CMAKE_INSTALL_PREFIX}/$<CONFIG>)
#Install the generated files
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/seq/${RAIL_CHIP_NAME}/$<CONFIG>/riscvseq_${RAIL_CHIP_NAME}.map DESTINATION ${CMAKE_INSTALL_PREFIX}/$<CONFIG>)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/seq/${RAIL_CHIP_NAME}/$<CONFIG>/riscvseq_${RAIL_CHIP_NAME}.s37 DESTINATION ${CMAKE_INSTALL_PREFIX}/$<CONFIG>)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/generic_seq_btc.c DESTINATION ${CMAKE_INSTALL_PREFIX}/$<CONFIG>)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/btc_riscvseq.asm  DESTINATION ${CMAKE_INSTALL_PREFIX}/$<CONFIG>)
# install(FILES ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/generic_seq.c DESTINATION ${CMAKE_INSTALL_PREFIX}/$<CONFIG>)

# Set merge id for code size analysis
set(MERGE_ID "b$ENV{JENKINS_MERGE_ID}")

# Always add the code size analysis target so Jenkins can call it
run_code_size_analysis(
    btc_riscvseq
    "${CMAKE_INSTALL_PREFIX}/$<CONFIG>/riscvseq_${RAIL_CHIP_NAME}.map"
    "btc_riscvseq"
    "${RAIL_CHIP_NAME}"
    "gcc" 
    "${CMAKE_CURRENT_BINARY_DIR}/btc_riscvseq_code_size_analysis.json"
    "${MERGE_ID}"
)