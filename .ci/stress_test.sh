#!/usr/bin/env bash

# Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
# This file is part of the "math-server" project.
# For details, see https://github.com/egor-tensin/math-server.
# Distributed under the MIT License.

set -o errexit -o nounset -o pipefail

run_server() {
    "$HOME/install/bin/math-server" --port 16666 &
    trap "kill $?" EXIT
}

run_stress_test() {
    "$TRAVIS_BUILD_DIR/test/stress_test.py" --client "$HOME/install/bin/math-client" --port 16666 --processes 4 --expressions 1000
}

main() {
    run_server
    run_stress_test
}

main
