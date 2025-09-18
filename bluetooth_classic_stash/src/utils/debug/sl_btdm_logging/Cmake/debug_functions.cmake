set(SL_BTDM_LOGGING "${CMAKE_CURRENT_LIST_DIR}/../")

if(DEFINED ENV{PYTHON3})
    set(PYTHON3 "$ENV{PYTHON3}")
else()
    set(PYTHON3 "python3")
endif()

function(parse_single_source_file_for_sl_debug target input_file header_file)
    set(parser_script "${SL_BTDM_LOGGING}/sl_btdm_log_parser.py")

    #strip path from the input file
    get_filename_component(input_file_base ${input_file} NAME)

    #get the path to the input file
    get_filename_component(input_file_path ${input_file} DIRECTORY)
    
    add_custom_command(
        OUTPUT ${input_file_base}.logdata.yaml
        COMMENT "Parsing ${input_file_base} for SL Log Entries"
        COMMAND ${PYTHON3} ${parser_script} -i ${input_file}
        COMMAND mv ${input_file_path}/${input_file_base}.logdata.yaml 
            ${CMAKE_CURRENT_BINARY_DIR}/${input_file_base}.logdata.yaml
        DEPENDS ${input_file}
    )
    add_custom_target(${target}_${input_file_base}_log_yaml DEPENDS ${input_file_base}.logdata.yaml)
    add_dependencies(${target}_generate_sl_debug_header_stage_1 ${target}_${input_file_base}_log_yaml)
endfunction()

set(DEBUG_CONFIGS_PATH "${CMAKE_CURRENT_LIST_DIR}/../sl_btdm_debug_configs.yaml")
function(gen_header_file_from_yaml target input_file output_file source_files)
    set(header_generation_script "${SL_BTDM_LOGGING}/sl_btdm_log_genrate_header.py")

    get_filename_component(output_file_base ${output_file} NAME)
    # Custom command to generate the header file
    add_custom_command(
        OUTPUT ${output_file_base}
        COMMENT "Generating header file"
        COMMAND ${PYTHON3} ${header_generation_script} -i ${input_file} -d ${DEBUG_CONFIGS_PATH} -c "SEQ_BTC_RAIL" -o ${output_file}
        DEPENDS ${source_files}
    )
    
    add_custom_target(${target}_generate_sl_debug_header_stage_2 DEPENDS ${output_file_base})
    add_dependencies(${target}_generate_sl_debug_header_stage_2 ${target}_generate_sl_debug_header_stage_1)
    add_dependencies(${target} ${target}_generate_sl_debug_header_stage_2)
endfunction()


function(parse_target_source_for_sl_debug target source_files list_parsed_files header_file)
    file(WRITE ${list_parsed_files} "")
    foreach(source_file IN LISTS source_files)
        #strip path from the input file
        get_filename_component(input_file_base ${source_file} NAME)
        file(APPEND ${list_parsed_files} "- ${CMAKE_CURRENT_BINARY_DIR}/${input_file_base}.logdata.yaml\n")
        parse_single_source_file_for_sl_debug(${target} ${source_file} ${header_file})
    endforeach()

endfunction()


function(sl_btdm_logging_autogen_header_file target header_file)
    # Get the source files for the target using get_target_property
    get_target_property(source_files ${target} SOURCES)
    #Add ${SL_BTDM_LOGGING}/include to the include directories
    target_include_directories(${target} PRIVATE ${SL_BTDM_LOGGING}/include)
    # Convert all source file paths to absolute paths
    set(absolute_source_files "")  # Initialize an empty list
    foreach(file IN LISTS source_files)
        get_filename_component(abs_file "${file}" ABSOLUTE)
        list(APPEND absolute_source_files "${abs_file}")
    endforeach()
    
    #Stage 1: Parse all source files for sl_debug
    add_custom_target(${target}_generate_sl_debug_header_stage_1)
    add_dependencies(${target} ${target}_generate_sl_debug_header_stage_1)
    
    #Yaml file comprising of all the parsed logdata.yaml files
    set(list_parsed_files "${CMAKE_CURRENT_BINARY_DIR}/${target}_list_parsed_files.yaml")
    
    #Stage 1 action: parse all source files for sl_debug
    parse_target_source_for_sl_debug(${target} "${absolute_source_files}" ${list_parsed_files} "${header_file}")
    
    #Stage 2: Generate header file from the parsed logdata.yaml files
    gen_header_file_from_yaml(${target} ${list_parsed_files} "${header_file}" "${absolute_source_files}")
endfunction()

#function to add {SL_BTDM_LOGGING}/fast_logging_producer/sl_btdm_fast_logging_write.c to the target
function(sl_btdm_logging_add_fast_logging_producer target)
    target_sources(${target} PRIVATE "${SL_BTDM_LOGGING}/fast_logging_producer/sl_btdm_fast_logging_write.c")
    target_sources(${target} PRIVATE "${SL_BTDM_LOGGING}/sl_btdm_fast_logging.c")
endfunction()