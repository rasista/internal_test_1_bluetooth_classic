#The CmakeLists file builds a library for the fast logging consumer having  sl_btdm_fast_logging_read.c as source file
#The library needs ../include in its include directories

cmake_minimum_required(VERSION 3.22)
# Add the source in local directory
project(fast_logging_consumer C)
add_library(fast_logging_consumer 
    sl_btdm_fast_logging_read.c
    ../logging_consumer/sl_btdm_logging_emit.c)
target_include_directories(fast_logging_consumer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)