cmake_minimum_required(VERSION 3.16)
project(simpy_intf)

add_library(simpy_intf
    command_parser.c
    simulation_interface.c
    root_simulation_interface.c
    )

target_include_directories(simpy_intf PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)