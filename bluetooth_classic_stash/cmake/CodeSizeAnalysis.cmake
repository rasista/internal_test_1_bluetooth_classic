# Get the path to the virtual environment relative to the CMakeLists.txt
set(VENV_PATH "$ENV{BTC}/code_size_analysis_venv")

# Function to run code size analysis using a Python virtual environment
# Now accepts a MERGE_ID argument for merge-to-main tracking
function(run_code_size_analysis TARGET MAP_FILE STACK_NAME TARGET_PART COMPILER OUTPUT_FILE MERGE_ID)
    # Use the globally defined VENV_PATH
    # Check if the virtual environment exists
    if(NOT EXISTS "${VENV_PATH}")
        message(WARNING "Virtual environment at ${VENV_PATH} does not exist. Code size analysis will not be performed.")
        return()
    endif()
    
    # Determine the activation script based on the OS
    if(WIN32)
        set(ACTIVATE_CMD "${VENV_PATH}/Scripts/activate")
    else()
        set(ACTIVATE_CMD "${VENV_PATH}/bin/activate")
    endif()
    # Set branch and merge id for CLI
    set(BRANCH_NAME "$ENV{BRANCH_NAME}")
    set(BUILD_NUMBER "${MERGE_ID}")
    # Create the full command to activate the venv and run the analyzer
    # Using proper CMake list operations to avoid backslash issues
    set(CMD_STRING "source ${ACTIVATE_CMD} && code_size_analyzer_cli")
    set(CMD_STRING "${CMD_STRING} --map_file ${MAP_FILE}")
    set(CMD_STRING "${CMD_STRING} --stack_name ${STACK_NAME}")
    set(CMD_STRING "${CMD_STRING} --target_part ${TARGET_PART}")
    set(CMD_STRING "${CMD_STRING} --target_board ${TARGET_PART}")
    set(CMD_STRING "${CMD_STRING} --app_name ${TARGET}")
    set(CMD_STRING "${CMD_STRING} --branch_name ${BRANCH_NAME}")
    set(CMD_STRING "${CMD_STRING} --build_number ${BUILD_NUMBER}")
    set(CMD_STRING "${CMD_STRING} --compiler ${COMPILER}")
    set(CMD_STRING "${CMD_STRING} --output_file ${OUTPUT_FILE}")
    set(CMD_STRING "${CMD_STRING} --store_results True")
    set(CMD_STRING "${CMD_STRING} --verify_ssl False")
    
    add_custom_target(
        ${TARGET}_code_size_analysis
        COMMAND ${CMAKE_COMMAND} -E echo "Running code size analysis on ${MAP_FILE} after building ${TARGET}"
        COMMAND bash -c "${CMD_STRING}"
        COMMENT "Running code size analysis on ${MAP_FILE} after building ${TARGET}"
        VERBATIM
    )

    message(STATUS "Code size analysis added as post-build step for target: ${TARGET} ${MAP_FILE} (MERGE_ID=${MERGE_ID})")
endfunction()

# Example usage:
# run_code_size_analysis(
#     my_target                        # Target to attach the analysis to
#     "chip-efr32-lighting-example.out.map"
#     "matter"
#     "efr32mg1p232f256gm48"
#     "gcc"
#     "chip-efr32-lighting-example.json"
# )
