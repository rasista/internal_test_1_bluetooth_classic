#!/bin/bash
#set -e
readonly FILE_FOLDER_SEARCH_PATH="../../Simulation_Framework_V2/tc"
readonly PYTHON_SIMULATION_FILE_PATH="../../Simulation_Framework_V2"
readonly CWD_RELATIVE_TO_PYTHON="../lpw_ss/build"
readonly C_ELF_FILE="bt_lpw.elf"
arg_a=""
arg_t=""
gcov_arg=.
make=0
out_file="coverage_stats.txt"
declare -a arg_i=()
declare -a files=()
declare -a exit_status=()

display_usage() {
	echo "Usage: $0 [-f OUTPUT FILENAME OF COVERAGE REPORT(default:coverage_stats.txt)][-a OPTIONAL_DIRECTORY_NAME] [-i OPTIONAL_FILE_NAME] [-m MAKE_MAKE_CLEAN(default: don't run)] -t SIMULATION_TIME [-g COVERAGE REPORT DIRECTORY(default: All files under btc/]"
  echo "For multiple files use -i multiple time"
}

while getopts ":a:i:t:g:f:m" opt; do
  case $opt in
	  a)
		  arg_a="$OPTARG"
		  ;;
	  i)
		  arg_i+=("$OPTARG")
		  ;;
	  t)
		  arg_t=("$OPTARG")
		  ;;
	  g)
		  arg_gcov=${OPTARG}
		  ;;
	  m)
		  make=1
		  ;;
	  f)
		  out_file=${OPTARG}
		  touch $out_file
		  ;;

	  \?)
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

if [ -z "$arg_t" ]; then
  echo "Simulation Time is required." >&2
  display_usage
  exit 1
fi

if [[ -z $arg_a && ${#arg_i[@]} -eq 0 ]]; then
  echo "At least one of the -a or -i flags is required." >&2
  display_usage
  exit 1
fi

if [[ -n $arg_a ]]; then
  echo "Running all the testcases from the directory : $arg_a"
  files+=("$FILE_FOLDER_SEARCH_PATH/$arg_a"/*)
fi

if [[ ${#arg_i[@]} -gt 0 ]]; then
  for arg in "${arg_i[@]}"; do
    echo "Running the file : $arg"
    files+=($arg)
  done
fi

if [ $make -eq 1 ]; then
    make clean
    make sim_test=true
fi

j=0
for i in "${files[@]}";
do
    echo -e "Current Test-case :  $i \n"
    gcno_files=$(find ../btc/$arg_gcov/ -name  *.gcno)
    
    file_c=$(basename $i)
    ./${C_ELF_FILE} $file_c >/dev/null & 
    cd $PYTHON_SIMULATION_FILE_PATH
    python3 sim.py -cfg testcase_configs/s3_btc_dev.yaml -cmds "tc/$i" -out $i -sim $arg_t >/dev/null
    
    last_exit_code=$?
    exit_status[j]=$last_exit_code
    j=$((j+1))
    cd $CWD_RELATIVE_TO_PYTHON
    touch ${out_file}
    >${out_file}
    for file in $gcno_files; 
    do
	    gcov $file>> ${out_file}
    done
    echo -e "\n\nTestCase $j ended\n\n">>${out_file}
done

echo -e "\n\nResults of Testing :"
array_size=${#exit_status[@]}
count=0
for((i=0;i<array_size;i++)); do
  file_name=${files[i]}
  file_name=$(basename $file_name)
  if [ ${exit_status[i]} -eq 1 ]; then
    echo -e "$file_name : ${RED}Failed ✘"
  else
    echo -e "$file_name : ${GREEN}Passed ✔"
    count=$((count+1))
  fi
done

echo -e "\n$count test-cases passed out of $array_size\n"
