#!/bin/bash

IMG="fs-os"                    # Image and container name for docker
RUN_FLAGS="--privileged --device=/dev/kvm --rm --name $IMG"   # Flags for ./docker.sh run

# TODO:
# --rm -i -e "DISPLAY=:0" -v /tmp/.X11-unix:/tmp/.X11-unix --name fs-os fs-os

# First check if there are no arguments
if [[ $# == 0 ]]; then
    echo "No arguments detected! Use '$0 --help' to see the help!"
    exit 1
fi

# Check the commands that will only be executed once, in this case help
if [[ " $* " == *" help "* || " $* " == *" --help "* ]]; then
    echo "Showing help for docker.sh"
    echo "  ./docker.sh help   | Show this help"
    echo "  ./docker.sh stop   | Stop the docker container ($IMG)"
    echo "  ./docker.sh clean  | Remove the docker image (The container is auto-removed by default)"
    echo "  ./docker.sh build  | Build the docker container ($IMG:latest)"
    echo "  ./docker.sh run    | Run the docker container ($IMG:latest)"
    exit 1
fi

# Run as root!
if [ $(whoami) != "root" ]; then
    echo "You need to be root!"
    exit 1
fi

# Check arguments that can be convined. They are in this order because you 
# never want to stop the container on the same command you start it.
if [[ " $* " == *" stop "* ]]; then
    echo "docker stop $IMG"
    docker stop $IMG
fi
if [[ " $* " == *" clean "* || " $* " == *" clear "* ]]; then
    echo "docker rmi $IMG:latest"
    docker rmi $IMG:latest
fi
if [[ " $* " == *" build "* ]]; then
    echo "docker build -t $IMG:latest ."
    docker build -t $IMG:latest .
fi
if [[ " $* " == *" run "* ]]; then
    echo "docker run $RUN_FLAGS $IMG"
    docker run $RUN_FLAGS $IMG
fi

