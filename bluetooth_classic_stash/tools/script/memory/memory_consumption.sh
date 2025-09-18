#!/bin/bash
 
if [ $# -ne 1 ]; then
   echo "Error: Library name not provided."  # error message if library name is not provided
   echo "Usage: $0 <library_name>"            
   exit 1
fi
 
# library name as an argument
LIBRARAY_NAME=$1
 
cd $WORKSPACE_PATH/src/lc/llc
sh configure.sh > /dev/null
cd $WORKSPACE_PATH/src/lc/llc/build
make clean; make > /dev/null
cd $WORKSPACE_PATH/src/lc/llc/build/release
 
# get the total
total_sizes=$( /opt/gcc_riscv/bin/riscv32-unknown-elf-size $LIBRARAY_NAME | awk 'NR > 1 {text += $1; data += $2; bss += $3; dec += $4} END {print "text = " text, "data = " data, "bss = " bss, "dec = " dec}' )
 
#for txt file only
{
   echo "Size Output:"
   /opt/gcc_riscv/bin/riscv32-unknown-elf-size $LIBRARAY_NAME
   echo "\nTotals for each column:"
   echo "$total_sizes"
   echo $WORKSPACE_PATH
} > $WORKSPACE_PATH/tools/memory_report.txt
 
cat $WORKSPACE_PATH/tools/memory_report.txt