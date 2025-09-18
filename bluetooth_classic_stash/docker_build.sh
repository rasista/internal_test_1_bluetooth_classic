#The Shell script is used to build a docker image for the project.
#The script takes below arguments:
# -i <image_name> (default: classic_image)
# -u <user> (defualt: sudo, possible option dzdo)
# -c (defualt: none, triggers clean build without cache)

#!/bin/bash

# Default values
IMAGE_NAME="classic_image"
CLEAN_BUILD=""
DEFAULT_USER=${SUDO_USER:-$(whoami)}
USER_UID=$(id -u "${DEFAULT_USER}")
USER_GID=$(id -g "${DEFAULT_USER}")

while getopts i:u:c:f flag
do
    case "${flag}" in
        i) IMAGE_NAME=${OPTARG};;
        u) USER=${OPTARG};;
        c) CLEAN_BUILD="--no-cache";;
        f) DOCKER_FILE=${OPTARG};;
    esac
done

echo "Building docker image: ${IMAGE_NAME}"
echo "Clean build: ${CLEAN_BUILD}"

sudo docker build ${DOCKER_FILE} ${CLEAN_BUILD} --build-arg RUN_ENV="local" --build-arg LOCAL_USER="${DEFAULT_USER}" --build-arg LOCAL_USER_UID="${USER_UID}" --build-arg LOCAL_USER_GID="${USER_GID}" -t "${IMAGE_NAME}" .
