##### Build System for Bluetooth Classic using common.mk 

This repository contains the build system for the Bluetooth Classic project. The build system is defined in the `common.mk` Makefile, which includes various targets for building, cleaning, and managing the project.

## Prerequisites

- Ensure that you have the necessary tools installed, such as `cmake`, `make`, and the required toolchains.
- Set the environment variables `WORKSPACE_PATH` and `GSDK` to the appropriate paths in your system.

## Targets

### Default Target

- `default`: Builds the lite_controller. This is the default target.

### Build Targets

- `make_btc_lpw`: Runs the cmake build with the specified preset to build lpw.
- `make_rail_lib`: Builds the rail_lib.
- `make_lite_controller`: genrates the lite controller application.
- `build_sdk`: Compiles the project by running the  `make_lpw_build`, `make_hss_build`  , `make_rail_lib` and `make_lite_controller` targets.
### Clean Targets

- `clean_bt_classic`: Cleans the bt_classic build system.
- `clean_rail_lib`: Cleans the rail_lib build system.

### Help Target

- `help`: Displays the available targets and their descriptions.

## Usage
 


### Building the Project

To build the project(with test_mode.c but won't reflect in map file), you can use the following commands:

```sh 
make 
```

To build the project without test_mode.c file 
```sh
make COMPILE_TEST_MODE=0
```

To build the project with test_mode.c and will be reflected in map file
```sh
make RUN_TEST_MODE=1
```

To build the project with freertos_os_kernel

```sh 
make FREERTOS=1
```
To build the project for 74k 

```sh 
make CHIP_NAME=sixg300xilpwh74000fpga
```

### cleaning the targets

```sh 

make clean_all
make clean_rail_lib

```

### help 

```sh 
make help 
```


####################################################################################


### Build System for Bluetooth Classic using build_system_librailsixg300_optimised.sh 


This repository contains a build script for the Bluetooth Classic project. The script is designed to clean and build various components of the project, including `bt_classic`, `rail_lib`, and `lite_controller`.

## Prerequisites

- Ensure that you have the necessary tools installed, such as `cmake`, `make`, and the required toolchains.
- Set the environment variable `DEFAULT_USER` to the appropriate user if not running as the current user.

## Script Usage

The script provides several commands to clean and build different parts of the project. You can run the script with the following commands:

### Cleaning Targets

- `clean_bt_classic`: Cleans the `bt_classic` build system.
- `clean_rail_lib`: Cleans the `rail_lib` build system.
- `clean_lite_controller`: Cleans the `lite_controller` build system.
- `clean_all`: Cleans all components (`bt_classic`, `rail_lib`, and `lite_controller`).

### Build Target

- `build`: Builds the `bt_classic` and `lite_controller` components. Optionally builds the `rail_lib` component if the `BUILD_RAIL_LIB` environment variable is set.

### Help

- If an invalid command is provided, the script will display the usage information.

## Usage

### Cleaning

To clean the `bt_classic` build system:
```sh
./build_system_librailsixg300_optimised.sh clean_bt_classic
```

To clean the `rail_lib` build system:
```sh
./build_system_librailsixg300_optimised.sh clean_rail_lib
```

To clean the `lite_cotroller` build_system:
```sh  
./build_system_librailsixg300_optimised.sh clean_lite_controller
```
To clean all componensts:
```sh 
./build_system_librailsixg300_optimised.sh clean_all
```

### BUILDING
TO build our sdk in accordance with jenkins:
```sh
./build_system_librailsixg300_optimised.sh build
```
TO build our sdk with COMPILE_TEST_MODE=OFF : 
```sh
./build_system_librailsixg300_optimised.sh -c 0 build
```

TO build our sdk with RUN_TEST_MODE=ON and inclusion of function calls  of it in map file 
```sh
RUN_TEST_MODE=1 ./build_system_librailsixg300_optimised.sh build
```

### HELP
```sh
./build_system_librailsixg300_optimised.sh -h
```


### Build system for bluetooth_classic using build_system_librailsixg300_jenkins.sh 


#### Building 
To build our sdk with TEST_MODE=1
```sh
./build_system_librailsixg300_jenkins.sh 
```

To build our sdk with TEST_MODE=0
```sh 
./build_system_librailsixg300_jenkins.sh -c 0 
```

