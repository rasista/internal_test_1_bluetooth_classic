cmake_minimum_required(VERSION 3.22)
# Add the source in local directory

include(..//Cmake/debug_functions.cmake)

project(logging_example C)
# Add cfalgs -g and -Werror -Wall to all targets
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Werror -Wall -DLINUX_UTILS")

#get the envoronement variable BTC
set(BTC $ENV{BTC})
set(LINUX ${BTC}/ut_frameworks/c_utils/linux)
set(PLT_DEPS ${BTC}/src/utils/plt_deps)

# Add the executable target
add_executable(logging_example)

#add -g amd -DLINUX to the compile flags
target_compile_options(logging_example PUBLIC -g -DLINUX)
# Add the generated file's directory to the include directories
include_directories(${CMAKE_BINARY_DIR})

target_sources(logging_example PUBLIC
    logging_example.c
    logging_example2.c
    main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../sl_btdm_fast_logging.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../fast_logging_producer/sl_btdm_fast_logging_write.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../fast_logging_consumer/sl_btdm_fast_logging_read.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../logging_consumer/sl_btdm_logging_emit.c
#    ${LINUX}/threads/thread.c
#    ${LINUX}/threads/thread_protection.c
    )

target_include_directories(logging_example PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${LINUX}/threads
    ${PLT_DEPS})

sl_btdm_logging_autogen_header_file(logging_example sl_btdm_log_autogen.h)
