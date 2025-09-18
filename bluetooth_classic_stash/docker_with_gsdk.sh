#!/bin/bash
#The shell script is used to run docker image
#the input options are as follows:
#-i <image_name> (default is classic_image)
#-g <path to gsdk repo> (mandatory)
#The script maps path_to_gsd_repo to /home/$USER/gsdk and $PWD to /home/$USER/bluetooth_classic_extension in docker environment


IMAGE="classic_image"
GSDK_PATH=""
DEFAULT_USER=${SUDO_USER:-$(whoami)}
while getopts u:i:g: option
do
case "${option}"
in
i) IMAGE=${OPTARG};;
g) GSDK_PATH=${OPTARG};;
esac
done

if [ -z "$GSDK_PATH" ]
then    
    echo "Please provide path to gsdk repo"
    exit 1
fi

#Set $GSDK to /home/$DEFAULT_USER/$GSDK_PATH
ENV_GSDK="GSDK=/home/$DEFAULT_USER/gsdk"
ENV_BTC="BTC=/home/$DEFAULT_USER/gsdk/protocol/bluetooth_classic"
ENV_CLS="/home/$DEFAULT_USER/gsdk/protocol/bluetooth_classic"
WORKSPACE_PATH="WORKSPACE_PATH=/home/$DEFAULT_USER/gsdk/protocol/bluetooth_classic"
#excute commands by default on starting docker
CMD="git config --global --add safe.directory /home/$DEFAULT_USER/gsdk && slc signature trust --sdk \"/home/$DEFAULT_USER/gsdk/\""
docker run -it --rm --net=host \
    --env=$ENV_GSDK \
    --env=$ENV_BTC  \
    --env=$WORKSPACE_PATH \
    --env="SIM_PATH=$ENV_CLS/ut_frameworks/simpy_framework/" \
    --env="PYTHONPATH=$ENV_CLS/ut_frameworks/utf/utf_lib_btdm" \
    --env="HARNESS_DIR=$ENV_CLS/ut_frameworks/utf/config/" \
    --env="HARNESS_NAME=example_harness.yaml" \
    --volume="$GSDK_PATH:/home/$DEFAULT_USER/gsdk" \
    --volume="$PWD:/home/$DEFAULT_USER/gsdk/protocol/bluetooth_classic" \
    --entrypoint /bin/bash $IMAGE -c "$CMD && bash"
