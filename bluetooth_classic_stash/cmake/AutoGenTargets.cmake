function(add_autogen_targets target list need_sm_config)
    # Get the length of the Autogen recipes
    set(autogen_recipes ${${list}})
    list(LENGTH autogen_recipes counter)
    math(EXPR index "0")
    while(${index} LESS ${counter})
        math(EXPR out_index "${index} + 0")
        math(EXPR template_index "${index} + 1")
        math(EXPR config_index "${index} + 2")
        list(GET autogen_recipes ${out_index} output_file_name)
        list(GET autogen_recipes ${template_index} template_file)
        list(GET autogen_recipes ${config_index} config_file)
        #placing output file in CMAKE_CURREN_SOURCE_DIR/autogen
        set(output_file "${CMAKE_CURRENT_SOURCE_DIR}/autogen/${output_file_name}")
        #Adding autogen_file to source_list
        target_sources(${target} PRIVATE ${output_file})
        #Actual command in make file which autogenerates the source file
        if (${need_sm_config} MATCHES "1")
            string(REPLACE "/" "_" config_target "${config_file}")
            add_custom_command(
                OUTPUT ${output_file}
                COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/tmp_files
                COMMAND python3 ${TOOLS}/generate_sm_details.py -i ${config_file} -o ${CMAKE_CURRENT_BINARY_DIR}/tmp_files/${config_target} -b ${CMAKE_CURRENT_SOURCE_DIR}
                COMMAND python3 ${TOOLS}/jinja_renderer.py -o ${output_file_name} -t ${template_file} -y ${CMAKE_CURRENT_BINARY_DIR}/tmp_files/${config_target}
                COMMAND mkdir -p ${CMAKE_CURRENT_SOURCE_DIR}/autogen
                COMMAND cp ${output_file_name} ${output_file}
                DEPENDS ${config_file} ${template_file}
            )
        else()
            add_custom_command(
                OUTPUT ${output_file}
                COMMAND python3 ${TOOLS}/jinja_renderer.py -o ${output_file_name} -t ${template_file} -y ${config_file}
                COMMAND mkdir -p ${CMAKE_CURRENT_SOURCE_DIR}/autogen
                COMMAND cp -f ${output_file_name} ${output_file}
                DEPENDS ${config_file} ${template_file}
            )
        endif()
        # Create a custom target that depends on the output of the custom command
        add_custom_target(${output_file_name} DEPENDS ${output_file})
        #Adding dependency to make to ensure file is gnerated
        add_dependencies(${target} ${output_file_name})
        target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/autogen) 
        math(EXPR index "${index} + 3")
    endwhile()
endfunction()

