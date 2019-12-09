#!/usr/bin/env bash

set -o errexit -o nounset -o pipefail -o xtrace

readonly boost_fs="boost_${boost_version//\./_}"
readonly boost_url="https://dl.bintray.com/boostorg/release/$boost_version/source/$boost_fs.tar.gz"
readonly boost_dir="$HOME/$boost_fs"

address_model=32
[ "$arch" = x64 ] && address_model=64
readonly address_model

clean() {
    cd -- "$HOME/"
}

download() {
    trap clean RETURN
    wget -- "$boost_url"
    tar xzvf "$boost_fs.tar.gz" > /dev/null
}

bootstrap() {
    trap clean RETURN
    cd -- "$boost_dir"
    ./bootstrap.sh
}

build() {
    trap clean RETURN
    cd -- "$boost_dir"
    ./b2                                     \
        "address-model=$address_model"       \
        link=static                          \
        variant="$build_type"                \
        "--stagedir=stage/$arch/$build_type" \
        --with-filesystem                    \
        --with-program_options               \
        --with-test
}

main() {
    clean
    download
    bootstrap
    build
}

main
