set -e
export BTC_UTILS=$PWD/../../../../utils
export BTC_TOOLS=$PWD/../../../../../tools
export BTC_SIMPY=$PWD/../../../../../ut_frameworks/simpy_framework/c_src
export BTC_JSON_LIBS=$PWD/../../../../../ut_frameworks/c_utils/json_libs
export BTC_LLL_SRC=$PWD/../../
export BTC_BUILD=$WORKSPACE_PATH/bin/
export BTC_LLC_SRC_LPW=$PWD/../../lpw_hss
export UT_JINJA_FILES=$PWD/ut_jinja_templates/hss_cmd
export PYTHON_JINJA_FILES=$PWD/../../../../../ut_frameworks/py_utils
export COMPILATION="NATIVE"
export FLAVOR="debug"
export RAIL_CHIP_NAME="sixg300xilpwh72000fpga"



# Set compiler flags
CMAKE_C_FLAGS="-fprofile-arcs -ftest-coverage -Wall -Werror -g -O0"
BUILD_STATIC=true


cmake -B $WORKSPACE_PATH/bin/ -DBUILD_STATIC=$BUILD_STATIC -DCMAKE_C_FLAGS="$CMAKE_C_FLAGS" -DUTILS=$BTC_UTILS -DTOOLS=$BTC_TOOLS -DSIMPY_SRC=$BTC_SIMPY -DBTC_LLL_SRC=$BTC_LLL_SRC -DBTC_BUILD=$BTC_BUILD -DJSON_LIBS=$BTC_JSON_LIBS -DUT_JINJA_FILES=$UT_JINJA_FILES -DPYTHON_JINJA_FILES=$PYTHON_JINJA_FILES -DBTC_LLC_SRC_LPW=$BTC_LLC_SRC_LPW -DJANSSON_BUILD_DOCS=OFF -DCOMPILATION=$COMPILATION -DFLAVOR=$FLAVOR -DRAIL_CHIP_NAME=$RAIL_CHIP_NAME