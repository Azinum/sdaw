#!/bin/bash

set -e

PROG=build/sdaw

APP_NAME="Simple DAW"

DIR="/Applications/${APP_NAME}.app/Contents/MacOS"

mkdir -p "${DIR}"
cp "${PROG}" "${DIR}/${APP_NAME}"
chmod +x "${DIR}/${APP_NAME}";
