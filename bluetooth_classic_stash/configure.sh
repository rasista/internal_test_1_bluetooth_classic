#!/bin/bash
#The configure file is used to configure the Cmake file for the project. It Takes
# the -b <build path_prefix>(defualt prefix build), -c<chip>(default si353) and -t <compilation toolchain>(default cross)
#as the input arguments. It then traverses all the folders in the project and creates a string with format -D<base_folder | caps> = <path>
#It then calls the cmake command with the build path and the string created above as the arguments.
#The script also sets the default values for the build path, chip and compilation toolchain.


BTC_BUILD_PATH="build"
COMPILATION="cross"
CHIP="sixg301fpga"
FLAVOR="release"

#parse arguments
while getopts "b:t:c:g:" opt; do
  case $opt in
    b)
      BTC_BUILD_PATH=$OPTARG
      ;;
    t)
      COMPILATION=$OPTARG
      ;;
    c)
      CHIP=$OPTARG
      ;;
    g) 
      FLAVOR=$OPTARG
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      ;;
  esac
done



#Add $Compilation and $Chip to the build variables in upper case
COMPILATION_UPPER=$(echo $COMPILATION | tr '[:lower:]' '[:upper:]')
BTC_BUILD_PATH=$BTC_BUILD_PATH"_"$CHIP"_"$COMPILATION
BTC_INSTALL_PREFIX="${BTC}/libs/${CHIP}/${COMPILATION}/${FLAVOR}/"
ARGS="-DCHIP=$CHIP"
ARGS="$ARGS -DCOMPILATION=$COMPILATION_UPPER"
ARGS="$ARGS -DFLAVOR=$FLAVOR"
ARGS="$ARGS -DCMAKE_INSTALL_PREFIX=$BTC_INSTALL_PREFIX"
ARGS="$ARGS -DBTC_BUILD_PATH=$BTC_BUILD_PATH"
BTC_ARGS="$ARGS"
echo "BTC_ARGS=$BTC_ARGS"
cmake -B $BTC_BUILD_PATH $BTC_ARGS -DBTC_ARGS="$BTC_ARGS"
