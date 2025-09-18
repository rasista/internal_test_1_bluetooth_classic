#!/bin/bash

DEFAULT_USER=${SUDO_USER:-$(whoami)}

# Step 1: Clean and build
cd /home/$DEFAULT_USER/gsdk/platform/radio/rail_lib/
make clean
make librail_sixg300xilpwh72000fpga_gcc_debug  RAIL_LIB_DEV=1 FPGA_BUILD=1 -j 


cd /home/$DEFAULT_USER/gsdk/protocol/bluetooth_classic/
# Step 2: Remove directories and run cmake
rm -rf build/
rm -rf libs/
rm -rf inc/
cd src/lc/llc/
rm -rf build/
rm -rf libs/
cd -
cmake --workflow --preset cortex/m55

# step 4:move to rail_lib for railtest command and build
cd /home/$DEFAULT_USER/gsdk/platform/radio/rail_lib/
make clean
make railtest_sixg301fpga_gcc_debug RAIL_LIB_DEV=1 FPGA_BUILD=1 -j 


# Step 3: Navigate to lite_controller directory
cd  /home/$DEFAULT_USER/gsdk/protocol/bluetooth_classic/src/lc/llc/ut/lite_controller/

# Step 4: Remove build directory
rm -rf build/

# Step 5: Run slc configuration
slc configuration --sdk /home/$DEFAULT_USER/gsdk

# Step 6: Generate project with slc
# slc generate --export-destination build/ --generator-timeout 360 --project-file lite_controller.slcp --toolchain toolchain_gcc --with emdk1001_lpwh_fpga,rail_lib_singleprotocol_debug --without radio_config_internal
slc generate --export-destination build/ --generator-timeout 360 --project-file lite_controller.slcp --toolchain toolchain_gcc --with emdk1001_rainier_fpga,radio_config_internal_manual,rail_lib_singleprotocol_debug --without radio_config_internal

#step 7: Navigate to the build directory
cd build/

# Step 8: Build the project
make -f btc_lite_controller_app.Makefile -j



###### hex flashing fine and teraterm responding , hex size 237KB