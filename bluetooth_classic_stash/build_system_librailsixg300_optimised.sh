#!/bin/bash

DEFAULT_USER=${SUDO_USER:-$(whoami)}

set -e

# Use the existing value of RUN_TEST_MODE if set, otherwise default to 0
export RUN_TEST_MODE=${RUN_TEST_MODE:-0}
export COMPILE_TEST_MODE=1
export SET_BT_SLOT_WIDTH=${SET_BT_SLOT_WIDTH:-625}
BUILD_RAIL_LIB=1

while getopts "r:c:b:s:h" opt; do
  case $opt in
    h)
        echo "Usage: $0 [-r RUN_TEST_MODE] [-c COMPILE_TEST_MODE] [-b BUILD_RAIL_LIB] [-s SET_BT_SLOT_WIDTH] {clean_bt_classic|clean_lite_controller|clean_rail_lib|clean_all|build}"
        exit 1
        ;;
    r)
        RUN_TEST_MODE="$OPTARG"
        ;;
    c)
        COMPILE_TEST_MODE="$OPTARG"
        ;;
    b)
        BUILD_RAIL_LIB="$OPTARG"
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

# Cleaning bt_classic
clean_bt_classic() {
    echo "Cleaning bt_classic..."
    cd /home/$DEFAULT_USER/gsdk/protocol/bluetooth_classic/
    rm -rf build/ libs/ inc/
    cd src/lc/llc/
    rm -rf build/ libs/ autogen/ build_config/ riscvseq_interface/seq
    cd -
}

# Cleaning rail_lib
clean_rail_lib() {
    echo "Cleaning rail_lib..."
    cd /home/$DEFAULT_USER/gsdk/platform/radio/rail_lib/
    make clean
}

# Cleaning lite_controller
clean_lite_controller() {
    echo "Cleaning lite_controller..."
    cd /home/$DEFAULT_USER/gsdk/protocol/bluetooth_classic/src/lc/llc/ut/lite_controller/
    rm -rf build/
}

# Building stage
build() {

    echo "Building bt_classic..."
    echo "RUN_TEST_MODE=$RUN_TEST_MODE"
    echo "SET_BT_SLOT_WIDTH=$SET_BT_SLOT_WIDTH"
    cd /home/$DEFAULT_USER/gsdk/protocol/bluetooth_classic/src/lc/llc/
    cmake -S . -B ./build_config -DCOMPILE_TEST_MODE=$COMPILE_TEST_MODE -DRUN_TEST_MODE=$RUN_TEST_MODE -DSET_BT_SLOT_WIDTH=$SET_BT_SLOT_WIDTH
    cd /home/$DEFAULT_USER/gsdk/protocol/bluetooth_classic/
    sled lib export component/bluetooth_classic_rail_interface.slcc
    cmake --workflow --preset cortex/m33

    if [ "$BUILD_RAIL_LIB" = "1" ]; then
        echo "Building rail_lib..."
        cd /home/$DEFAULT_USER/gsdk/platform/radio/rail_lib/
        make librail_sixg300xilpwh72000fpga_gcc_debug RAIL_LIB_DEV=1 -j
    else
        echo "Skipping rail_lib build..."
    fi

    echo "Building lite_controller..."
    cd /home/$DEFAULT_USER/gsdk/protocol/bluetooth_classic/src/lc/llc/ut/lite_controller/
    slc configuration --sdk /home/$DEFAULT_USER/gsdk
    sled slc generate --export-destination build/ --generator-timeout 360 --project-file lite_controller.slcp --toolchain toolchain_gcc --with emdk1001_lpwh_fpga,rail_lib_singleprotocol_debug
    cd build/
    make -f btc_lite_controller_app.Makefile -j
}

# Parse command-line arguments
shift $((OPTIND -1))
case "$1" in
    clean_bt_classic)
        clean_bt_classic
        ;;
    clean_rail_lib)
        clean_rail_lib
        ;;
    clean_lite_controller)
        clean_lite_controller
        ;;
    clean_all)
        clean_bt_classic
        clean_rail_lib
        clean_lite_controller
        ;;
    build)
        build
        ;;
    *)
        echo "Usage: $0 [-r RUN_TEST_MODE] [-c COMPILE_TEST_MODE] [-b BUILD_RAIL_LIB] [-s SET_BT_SLOT_WIDTH] {clean_bt_classic|clean_lite_controller|clean_rail_lib|clean_all|build}"
        exit 1
        ;;
esac

# Reset the environment variables to their default values
unset RUN_TEST_MODE
unset COMPILE_TEST_MODE
unset SET_BT_SLOT_WIDTH