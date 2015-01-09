#!/bin/bash

ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
# You may run in trouble on localized Linux installations
# because of ',' parsed as decimal separator instead of '.' char
export LC_ALL=C
# Embree dynamic library is assumed to be located in the 'lib'
# subdirectory.
export LD_PRELOAD="$ROOT/lib/libembree.so.2"

cd "$ROOT"
./luxmark.bin "$@"
