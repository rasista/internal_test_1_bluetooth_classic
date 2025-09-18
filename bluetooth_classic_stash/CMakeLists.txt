#Cmake file creates a project called btc_sdk_extension
#The comprise of a install target, which invokes different install targets in the subdirectories
#The subdirectories are the different frameworks that are part of the btc_sdk_extension project
#The install target is used to install the btc_sdk_extension project

cmake_minimum_required(VERSION 3.25)
cmake_policy(SET CMP0114 NEW)

include(cmake/common.cmake)
project(btc_sdk_extension C)


# Optimize for size
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os")

separate_arguments(CMAKE_BTC_EXTENSION_ARGS_LIST UNIX_COMMAND ${BTC_ARGS})

# Function to add subdirectory with its own toolchain file
function(add_subdirectory_with_toolchain external_project dir toolchain_file)
    ExternalProject_Add(${external_project}
        SOURCE_DIR ${dir}
        BINARY_DIR ${BTC}/${BTC_BUILD_PATH}/${external_project}
        CMAKE_ARGS --preset=gcc/x301  -DCMAKE_TOOLCHAIN_FILE=${toolchain_file} -DCMAKE_POLICY_DEFAULT_CMP0114=NEW
        BUILD_ALWAYS 1
    )
    #Adding a custom target to clean the external project
    add_custom_target(${external_project}-clean
        COMMAND ${CMAKE_COMMAND} --build ${BTC}/${BTC_BUILD_PATH}/${external_project} --target clean
    
    )
    add_dependencies(clean_sdk ${external_project}-clean)
endfunction()

# Add subdirectories with their respective toolchain files
add_custom_target(clean_sdk)

add_subdirectory(${ULC}/ btc_ulc)
add_subdirectory(${UTILS}/ btc_common)
add_subdirectory(${SCHEDULER}/ btc_usched)
add_subdirectory(${RAIL_LIBINTF}/ btc_hal)
add_custom_target(btc_sdk_extension ALL
    DEPENDS btc_ulc btc_common btc_usched btc_hal
)
