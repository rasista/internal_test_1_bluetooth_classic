#!/bin/bash
if [ -z $SIM_PATH ] ; then
export SIM_PATH="/home/src/ut_frameworks/simpy_framework"
fi
if [ -z $BUILD_PATH ] ; then
export BUILD_PATH='build'
fi
display_usage() {
	echo "Usage: $0 [-c config file name]
			 [-t test case file name] 
			 [-s simulation time] 
			 [-f output filename for simulation output (Default : stdout)]"
	
	echo "Sample : sh simulation_run.sh -c testcase_configs/s3_btc_dev.yaml -t tc/Page.txt -s 10000"
}


while getopts "c:t:s:f:h" opt; do
  case $opt in
	  h)
	  	display_usage
		exit 1
	  	;;
	  c)
		  arg_cfg="$OPTARG"
		  ;;
	  t)
          arg_cmds="$OPTARG"
		  ;;
	  s)
          arg_sim="$OPTARG"
		  ;;
	  f)
          arg_out="$OPTARG"
		  ;;

	  *)
		  echo "Invalid option: -$OPTARG requires an argument." >&2
		  display_usage
		  exit 1
		  ;;
	  :)
		  echo "Option -$OPTARG requires an argument." >&2
		  display_usage
		  exit 1
		  ;;
  esac
done

if [ -z $arg_cmds ] || [ -z $arg_sim ]; then
	echo "Provide proper input"
	display_usage
	exit 1
fi

if [ -z $arg_cfg]; then
    python3 $SIM_PATH/sim.py -cmds ../$arg_cmds -sim $arg_sim
	exit 
fi

if [ -z $arg_out ];
then
    python3 $SIM_PATH/sim.py -cfg ../$arg_cfg -cmds ../$arg_cmds -sim $arg_sim 
else
    python3 $SIM_PATH/sim.py -cfg ../$arg_cfg -cmds ../$arg_cmds -sim $arg_sim -out ../$arg_out
fi

