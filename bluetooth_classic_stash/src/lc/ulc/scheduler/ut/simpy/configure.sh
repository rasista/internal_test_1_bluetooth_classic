#!/bin/bash
set -e
export BTC_TOOLS=$PWD/../../../../../../tools
export BTC_SIMPY=$PWD/../../../../../../ut_frameworks/simpy_framework/c_src
export BTC_JSON_LIBS=$PWD/../../../../../../ut_frameworks/c_utils/json_libs
export BTC_USCHED_SRC=$PWD/../../
export BTC_BUILD=$WORKSPACE_PATH/bin/
export BTC_ULC=$WORKSPACE_PATH/src/lc/ulc
export BTC_EVENT_LOOP=$WORKSPACE_PATH/src/lc/ulc/event_loop/
export BTC_UTILS=$WORKSPACE_PATH/src/utils/
export BTC_THREADS=$WORKSPACE_PATH/ut_frameworks/c_utils/linux/threads
export COMPILATION="NATIVE"
export FLAVOR="debug"
export CHIP="sixg301fpga"


# Set compiler flags
CMAKE_C_FLAGS="-fprofile-arcs -ftest-coverage -DLINUX_UTILS -Wall -Werror -g -O0"
# Check if compiler flags contain "-fprofile-arcs -ftest-coverage"
if [[ "$CMAKE_C_FLAGS" == "-fprofile-arcs -ftest-coverage" ]]; then
   BUILD_STATIC=true
fi

cmake -B $WORKSPACE_PATH/bin/ -DBTC_THREADS=$BTC_THREADS -DBUILD_STATIC=$BUILD_STATIC -DCMAKE_C_FLAGS="$CMAKE_C_FLAGS" -DTOOLS=$BTC_TOOLS -DSIMPY_SRC=$BTC_SIMPY -DBTC_USCHED_SRC=$BTC_USCHED_SRC -DBTC_BUILD=$BTC_BUILD -DULC=$BTC_ULC -DUTILS=$BTC_UTILS -DJSON_LIBS=$BTC_JSON_LIBS -DJANSSON_BUILD_DOCS=OFF -DCOMPILATION=$COMPILATION -DCHIP=$CHIP -DFLAVOR=$FLAVOR