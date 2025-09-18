#!/bin/bash

DEFAULT_USER=${SUDO_USER:-$(whoami)}

export COMPILE_TEST_MODE=1
export RUN_TEST_MODE=0
export SET_BT_SLOT_WIDTH=625
export CHIP_NAME="sixg300xilpwh72000fpga"

while getopts "c:r:s:h" opt; do
  case $opt in
    h)
        echo "Usage: $0 [-c COMPILE_TEST_MODE] [-r RUN_TEST_MODE] [-s SET_BT_SLOT_WIDTH]"
        exit 1
        ;;
    c)
        COMPILE_TEST_MODE="$OPTARG"
        ;;
    r) 
        RUN_TEST_MODE="$OPTARG"
        ;;
    s)
        SET_BT_SLOT_WIDTH="$OPTARG"
        ;;
    *)
        echo "Invalid option: -$OPTARG requires an argument." >&2
        exit 1
        ;;
    :)
        echo "Option -$OPTARG requires an argument." >&2
        exit 1
        ;;
  esac
done


set -e
# cd /home/$DEFAULT_USER/gsdk/protocol/bluetooth_classic/
# Step 2: Remove directories and run cmake
rm -rf build/
rm -rf lib/
rm -rf inc/
cd src/lc/llc/
rm -rf build/
rm -rf lib/
rm -rf autogen/
rm -rf build_config/
rm -rf riscvseq_interface/seq
cd ut/lite_controller/
rm -rf build/
cd ${BTC}/src/lc/llc
cd ${BTC}/src/lc/rail_libintf/
rm -rf autogen/
cd ${BTC}/src/lc/llc
cd ${BTC}/../../platform/radio/rail_lib 
make clean
cd ${BTC}/src/lc/llc

echo "COMPILE_TEST_MODE: $COMPILE_TEST_MODE"
echo "RUN_TEST_MODE: $RUN_TEST_MODE"
echo "SET_BT_SLOT_WIDTH: $SET_BT_SLOT_WIDTH"
cmake -S . -B ./build_config -DCOMPILE_TEST_MODE=$COMPILE_TEST_MODE -DRUN_TEST_MODE=$RUN_TEST_MODE -DSET_BT_SLOT_WIDTH=$SET_BT_SLOT_WIDTH -DRAIL_CHIP_NAME=$CHIP_NAME

cd ${BTC}
sled lib export component/bluetooth_classic_rail_interface.slcc
cmake --workflow --preset cortex/m33

# step 4:move to rail_lib for railtest command and build
cd ${BTC}/../../platform/radio/rail_lib/
make librail_sixg300xilpwh72000fpga_gcc_debug RAIL_LIB_DEV=1 -j 

# Step 3: Navigate to lite_controller directory
cd ${BTC}/src/lc/llc/ut/lite_controller/


# Step 5: Run slc configuration
slc configuration --sdk ${BTC}/../../

# Step 6: Generate project with slc
sled slc generate --export-destination build/ --generator-timeout 360 --project-file lite_controller.slcp --toolchain toolchain_gcc --with emdk1001_lpwh_fpga,rail_lib_singleprotocol_debug

#step 7: Navigate to the build directory
cd build/

# Step 8: Build the project
make -f btc_lite_controller_app.Makefile -j
unset COMPILE_TEST_MODE
unset RUN_TEST_MODE
unset SET_BT_SLOT_WIDTH
unset CHIP_NAME