#!/usr/bin/env bash
SCRIPT=$(realpath "$0")
SOURCES=$(dirname "${SCRIPT}")

. ${SOURCES}/.build_settings.sh
set -e

docker cp ${SOURCES}/boost_fix/write.hpp ${IMAGE}:/usr/include/boost/property_tree/json_parser/detail/write.hpp

docker exec ${IMAGE} ${BUILD_DIR_DOCKER}/build.sh -b ${RELEASE_BUILD_DIR} -t Release -g 1 && \
	docker exec ${IMAGE} ${BUILD_DIR_DOCKER}/build.sh -b ${DEBUG_BUILD_DIR} -t Debug -g 1
