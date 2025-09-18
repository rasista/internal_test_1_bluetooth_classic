#The CmakeLists file builds a library for the fast logging producer having  sl_btdm_fast_logging_write.c as source file
#The library needs ../include in its include directories

cmake_minimum_required(VERSION 3.22)
# Add the source in local directory
project(fast_logging_producer C)
#set optimization flags ro fastest execution just for this file, without impacting the overall project Cflags
set(CMAKE_C_FLAGS "-g")
add_library(fast_logging_producer sl_btdm_fast_logging_write.c)
target_include_directories(fast_logging_producer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)