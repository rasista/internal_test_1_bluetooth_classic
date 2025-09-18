#! /bin/bash
set -e
while getopts ":m" opt;do
	case $opt in
		m)
			make clean;make sim_test=true
			;;

	esac
done

#FILE_REM="$(find ../../ -name *.gcda)"
#for file in $FILE_REM;
#do
#	rm -rf $file
#done

>coverage_stats.txt
clear;./bt_lpw.elf
FILE_PATH="$(find ../btc/ -name  *.gcno)"
for file in $FILE_PATH;
do
	echo "$(gcov $file)">>coverage_stats.txt
	#echo "$file"
done


