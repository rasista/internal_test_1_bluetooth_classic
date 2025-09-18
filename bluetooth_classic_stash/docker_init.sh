arg_user="sudo"
arg_curr_path="/home"
arg_docker_path="/home"
arg_enable_build=0
arg_gsdk_path=""
display_usage() {
	echo "Usage: $0 [-u root user name. Default user: $arg_user]
			 [-c current path to take in docker. Default_path : $arg_curr_path] 
			 [-d docker path. Default_docker_path: $arg_docker_path] 
			 [-h help]"
	
	echo "Sample : sh docker_init.sh -u sudo -c ./ -d /home/src"
}


while getopts "u:c:d:g:bh" opt; do
  case $opt in
    h)
	  	display_usage
		exit 1
	  	;;
    u)
        arg_user="$OPTARG"
        ;;
    c)
        arg_curr_path="$OPTARG"
        ;;
    d)
        arg_docker_path="$OPTARG"
        ;;
    b)
        arg_enable_build=1
        ;;
    g)
        arg_gsdk_path="$OPTARG"
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

cd tools/script/uncrustify
sh install-hooks.sh
cd ../../../

#if build is enabled, build image, else run straightaway
if [ "$arg_enable_build" -eq 1 ]; then
    $arg_user docker build . -t classic_image 
fi

#if arg_gsdk_path is not empty, map gsdk_path to /home/<gsdk_path> and $PWD to /home/classic-btc
if [ -n "$arg_gsdk_path" ]; then
    $arg_user docker run -e "SIM_PATH=$PWD/ut_frameworks/simpy_framework/" -e "PYTHONPATH=$PWD/ut_frameworks/utf:$PWD/ut_frameworks/utf/utf_lib_btdm" -e "HARNESS_DIR=$PWD/ut_frameworks/utf/config/" -e "HARNESS_NAME=example_harness.yaml" -e "WORKSPACE_PATH=$PWD" -e "BUILD_PATH=$PWD/bin/" -v $PWD:/home/classic-bluetooth -v $arg_gsdk_path:/home/gsdk -it classic_image
    exit 0
else
    $arg_user docker run -e "SIM_PATH=$PWD/ut_frameworks/simpy_framework/" -e "PYTHONPATH=$PWD/ut_frameworks/utf:$PWD/ut_frameworks/utf/utf_lib_btdm" -e "HARNESS_DIR=$PWD/ut_frameworks/utf/config/" -e "HARNESS_NAME=example_harness.yaml" -e "WORKSPACE_PATH=$PWD" -e "BUILD_PATH=$PWD/bin/" -v $arg_curr_path:$arg_docker_path -it classic_image
    exit 0
fi
