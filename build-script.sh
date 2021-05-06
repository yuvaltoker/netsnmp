#!/bin/bash


# the next example of handling flags and arguments from command line has been brought from here:
# https://pretzelhands.com/posts/command-line-flags 


HELP='Usage: ./build-script.h [OPTIONS]\n\n'
HELP+='A script for building image from dockerfiles\n\n'
HELP+='Options:\n\n'
HELP+='    -c, --context    Specify the build context of the images\n'
HELP+='    -f, --file       Specify dockerfile the image will be built from\n'
HELP+='    -n, --name       Image name\n'
HELP+='    -t, --tag        In case of production, tag the commit the image is built on\n'
HELP+='\n'

# Default values of arguments
IMAGE_NAME=''
TAG=''
DOCKERFILE_NAME=''
BUILD_CONTEXT=''

# Loop through arguments and process them
for arg in "$@"
do
    case $arg in
        -n|--name)
        IMAGE_NAME="$2"
        shift # Remove argument name from processing
        shift # Remove argument value from processing
        ;;
        -t|--tag)
        TAG="$2"
        shift # Remove argument name from processing
        shift # Remove argument value from processing
        ;;
        -f|--file)
        DOCKERFILE_NAME="$2"
        shift # Remove argument name from processing
        shift # Remove argument value from processing
        ;;
        -c|--context)
        BUILD_CONTEXT="$2"
        shift # Remove argument name from processing
        shift # Remove argument value from processing
        ;;
        -h|--help)
        echo -e "$HELP"
        exit 1
    esac
done

if [ "$IMAGE_NAME" = '' ]; then 
    echo "Warning - No image name was given"
    echo "# image name: Will be randomized"
else
    echo "# image name:     $IMAGE_NAME"
fi

if [ "$BUILD_CONTEXT" = '' ]; then 
    echo "No build context was given"
    echo -e "$HELP"
    exit 1
else
    echo "# build context:  $BUILD_CONTEXT"
fi

if [ "$DOCKERFILE_NAME" = '' ]; then 
    echo "No dockerfile was given"
    echo -e "$HELP"
    exit 1
else
    echo "# dockerfile:     $DOCKERFILE_NAME"
fi

if [ ! "$TAG" = '' ]; then 
    echo "# tag:            $TAG"
    echo "# commit hash: $(git rev-parse HEAD)"
fi

echo "Do you want to continue? y/n"
read continue_script

if [ "$continue_script" = "n" ]; then 
    exit 1
fi

echo $(git rev-parse HEAD) >> sourceHash.txt
docker build --tag $IMAGE_NAME --file dockerfiles/$DOCKERFILE_NAME $BUILD_CONTEXT

if [ ! "$TAG" = '' ]; then 
    docker push $IMAGE_NAME

    git tag -a $TAG $(git rev-parse HEAD) -m "$TAG"
    git push origin $TAG
fi
