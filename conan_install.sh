#!/usr/bin/env bash

SCRIPT=$(realpath "$0")
SOURCES=$(dirname "${SCRIPT}")

function usage {
    echo "usage: [build type]"
    echo "  where build type can be:"
    echo "  --all|-a - install all the configuration types required"
    echo "  --debug|-d - install only debug build dependecies"
    echo "  --release|-r - install only release dependencies"
    echo "  --rwd|-w - install release with debug info"
    echo "  -i|--install: install any missing system wide dependecies (this requird for use to have sudo access). Note that this MUST be first parameter."
    ehco "or with -h|--help to print this message"
    exit 1
}

function install {
    cd $SOURCES
    if [ "$INSTALL_DEPS" != "" ]; then
	    extra_args="-c tools.system.package_manager:mode=install -c tools.system.package_manager:sudo=true --build=missing"
    fi
    echo "running installation for $@ with extra args: [$extra_args]"
    for bt in $@; do
        conan install . -s build_type=$bt $extra_args || {
            echo "failed to install conan dependecies for $bt"
            return 1
        }
    done
    echo "successfully install all conan dependecies for $@"
    return 0
}

while [[ $# -gt 0 ]]; do
    case $1 in
    -a|--all|--ALL)
        install Release Debug RelWithDebInfo
        exit $?
        ;;
    -d|--debug|--DEBUG)
        install Debug
        exit $?
        ;;
    -r|--release|--RELEASE)
        install Release
        exit $?
        ;;
    -w|--rwd)
        install RelWithDebInfo
        exit $?
        ;;
    -h|--help)
        usage
        ;;
    -i|--install)
	INSTALL_DEPS="yes"
	shift
	;;
    *)
        usage
        ;;
    esac
done
install Release Debug RelWithDebInfo