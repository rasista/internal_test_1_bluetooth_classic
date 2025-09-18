#common.cmake file
set(BTC $ENV{WORKSPACE_PATH})

set(CMAKE_CONFIGURATION_TYPES
    release
    debug
)

set(PY_UTILS "${BTC}/ut_frameworks/py_utils")
set(BTC_SRC "${BTC}/src")
set(LLC "${BTC}/src/lc/llc")
set(RISCVSEQ_INTERFACE "${BTC}/src/lc/llc/riscvseq_interface")
set(ULC "${BTC}/src/lc/ulc")
set(SCHEDULER "${BTC}/src/lc/ulc/scheduler")
set(UTILS "${BTC}/src/utils")
set(RAIL_LIBINTF "${BTC}/src/lc/rail_libintf")
set(TOOLS "${BTC}/tools")
set(C_SRC "${BTC}/ut_frameworks/simpy_framework/c_src")
set(JANSSON "${BTC}/ut_frameworks/c_utils/json_libs/jansson-2.13")
