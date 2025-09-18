export CHIP_NAME = sixg300xilpwh72000fpga
WITH_COMPONENTS = emdk1001_lpwh_fpga,rail_lib_singleprotocol_debug
export COMPILE_TEST_MODE=1
export RUN_TEST_MODE=0
export SET_BT_SLOT_WIDTH=625
export FREERTOS=0
TOOLCHAIN = toolchain_gcc
WITHOUT_COMPONENTS =
LITE_CONTROLLER_PATH = $(WORKSPACE_PATH)/src/lc/llc/ut/lite_controller
RAIL_LIB_PATH = $(GSDK)/platform/radio/rail_lib
GENERATOR_TIMEOUT_SECONDS = 360
BUILD_DIR = $(WORKSPACE_PATH)/build
LIB_DIR = $(WORKSPACE_PATH)/lib
INC_DIR = $(WORKSPACE_PATH)/inc
LLC_DIR = $(WORKSPACE_PATH)/src/lc/llc
LLC_AUTOGEN_DIR = $(LLC_DIR)/autogen
LLC_BUILD_DIR = $(LLC_DIR)/build
LLC_LIB_DIR = $(LLC_DIR)/lib
LLC_BUILD_CONFIG_DIR = $(LLC_DIR)/build_config
LITE_CONTROLLER_BUILD_DIR = $(LITE_CONTROLLER_PATH)/build
RISCVSEQ_INTF_SEQ_DIR = $(LLC_DIR)/riscvseq_interface/seq
COMPONENTS = component/bluetooth_classic_rail_interface.slcc
RAIL_LIBINTF_AUTOGEN_DIR = $(WORKSPACE_PATH)/src/lc/rail_libintf/autogen

# Check if the CHIP_NAME is set to a valid value
ifeq ($(CHIP_NAME),sixg300xilpwh72000fpga)
    # Valid CHIP_NAME, do nothing
else ifeq ($(CHIP_NAME),sixg300xilpwh74000fpga)
    # Valid CHIP_NAME, do nothing
else
    $(error Invalid CHIP_NAME: "$(CHIP_NAME)". Valid values are "sixg300xilpwh72000fpga" or "sixg300xilpwh74000fpga")
endif

ifeq ($(CHIP_NAME),sixg300xilpwh74000fpga)
	WITH_COMPONENTS = emdk1001_lpwh74000_fpga,rail_lib_singleprotocol_debug
endif

ifeq ($(FREERTOS),1)
  WITH_COMPONENTS := $(WITH_COMPONENTS),freertos,freertos_heap_4
endif


default: build_sdk

trust:
	@slc signature trust \
		--sdk $(GSDK)

lib_export:
	@echo "SLED lib export"
	@cd $(WORKSPACE_PATH) && sled lib export $(COMPONENTS)
	
configure_cmake:
	@echo "configuring cmake of LLC for RUN_TEST_MODE=$(RUN_TEST_MODE) and COMPILE_TEST_MODE=$(COMPILE_TEST_MODE) and SET_BT_SLOT_WIDTH=$(SET_BT_SLOT_WIDTH)" 
	cd $(LLC_DIR) && cmake -S $(LLC_DIR) -B $(LLC_BUILD_CONFIG_DIR) -DRUN_TEST_MODE=$(RUN_TEST_MODE) -DCOMPILE_TEST_MODE=$(COMPILE_TEST_MODE) -DSET_BT_SLOT_WIDTH=$(SET_BT_SLOT_WIDTH) -DRAIL_CHIP_NAME=$(CHIP_NAME)



# Define the hss_build target
make_btc_hss:
ifeq ($(CHIP_NAME),sixg300xilpwh72000fpga)
	@echo "Building hss for sixg300xilpwh72000fpga"
	cmake --workflow --preset cortex/m33
else ifeq ($(CHIP_NAME),sixg300xilpwh74000fpga)
	@echo "Building hss for sixg300xilpwh74000fpga"
	cmake --workflow --preset cortex/m55
endif


make_rail_lib:
	@echo "Building rail_lib..."
ifeq ($(CHIP_NAME),sixg300xilpwh72000fpga)
	@cd $(RAIL_LIB_PATH) && make librail_sixg300xilpwh72000fpga_gcc_debug RAIL_LIB_DEV=1 -j "CFLAGS=-DBTC_CLOCK_PERIOD_US=$(SET_BT_SLOT_WIDTH)"
else ifeq ($(CHIP_NAME),sixg300xilpwh74000fpga)
	@cd $(RAIL_LIB_PATH) && make librail_sixg300xilpwh74000fpga_gcc_debug RAIL_LIB_DEV=1 -j "CFLAGS=-DBTC_CLOCK_PERIOD_US=$(SET_BT_SLOT_WIDTH)"
endif

# Define the slc_generate target
make_lite_controller:
	@echo "Configuring SDK"
	@slc configuration --sdk $(GSDK) 
	@sled slc generate \
        --export-destination $(LITE_CONTROLLER_PATH)/build \
        --generator-timeout $(GENERATOR_TIMEOUT_SECONDS) \
        --project-file $(PROJECT_NAME).slcp \
        --toolchain $(TOOLCHAIN) \
        --with=$(WITH_COMPONENTS)
	
	cd $(LITE_CONTROLLER_PATH)/build && \
    make -f btc_lite_controller_app.Makefile -j
	

# Define the build_sdk target that depends on trust, btc_build, and slc_generate
build_sdk: trust lib_export configure_cmake make_btc_hss make_rail_lib make_lite_controller clear_env 
	@echo "Compilation complete."

clean_rail_lib:
	@echo "cleaning rail_lib"
	@cd $(RAIL_LIB_PATH) && make clean

clean: clean_rail_lib
	@echo "Cleaning lpw_build , hss_build , lite_contoller_build"
	@rm -rf $(BUILD_DIR)
	@rm -rf $(LIB_DIR)
	@rm -rf $(INC_DIR)
	@rm -rf $(LLC_BUILD_DIR)
	@rm -rf $(LLC_LIB_DIR)
	@rm -rf $(LLC_AUTOGEN_DIR)
	@rm -rf $(LITE_CONTROLLER_BUILD_DIR)
	@rm -rf $(LLC_BUILD_CONFIG_DIR)
	@rm -rf $(RISCVSEQ_INTF_SEQ_DIR)
	@rm -rf $(RAIL_LIBINTF_AUTOGEN_DIR)

clear_env:
	@echo "Clear env vars"
	@unset RUN_TEST_MODE
	@unset COMPILE_TEST_MODE
	@unset SET_BT_SLOT_WIDTH
	@unset FREERTOS
	@unset CHIP_NAME