#!/usr/bin/env bash

set -o errexit -o nounset -o pipefail -o xtrace

readonly boost_fs="boost_${boost_version//\./_}"
readonly boost_dir="$HOME/$boost_fs"
readonly boost_librarydir="$boost_dir/stage/$arch/$build_type/lib"

readonly build_dir="$HOME/build"

main() {
    mkdir -p -- "$build_dir"
    cd -- "$build_dir"
    cmake                                           \
        -D "CMAKE_BUILD_TYPE=$build_type"           \
        -D "CMAKE_CXX_STANDARD_LIBRARIES=-lpthread" \
        -D "BOOST_ROOT=$boost_dir"                  \
        -D "BOOST_LIBRARYDIR=$boost_librarydir"     \
        -D CC_CXX_STANDARD=17                       \
        -D ENABLE_TESTS=ON                          \
        "$TRAVIS_BUILD_DIR"
    cmake --build . -- -j
    ./test/unit_tests/unit_tests --log_level=all
}

main
