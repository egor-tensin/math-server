#!/usr/bin/env bash

# Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
# This file is part of the "math-server" project.
# For details, see https://github.com/egor-tensin/math-server.
# Distributed under the MIT License.

set -o errexit -o nounset -o pipefail
shopt -s inherit_errexit lastpipe

script_name="$( basename -- "${BASH_SOURCE[0]}" )"
readonly script_name
script_dir="$( dirname -- "${BASH_SOURCE[0]}" )"
script_dir="$( cd -- "$script_dir" && pwd )"
readonly script_dir

server_path=
client_path=
readonly stress_test_path="$script_dir/stress_test.py"
readonly server_port=16666
server_pid=

dump() {
    local msg
    for msg; do
        echo "$script_name: $msg"
    done
}

kill_server() {
    dump "Killing the server with PID $server_pid..."
    kill "$server_pid"
    dump "Waiting for the server to terminate..."
    wait "$server_pid"
    dump "Done"
}

run_server() {
    dump "Running the server..."
    "$server_path" --port "$server_port" &
    server_pid="$!"
    dump "Its PID is $server_pid"
    trap kill_server EXIT
}

run_stress_test() {
    dump "Running stress_test.py..."
    "$stress_test_path"         \
        --client "$client_path" \
        --port "$server_port"   \
        --processes 4           \
        --expressions 1000
}

script_usage() {
    echo "usage: $script_name SERVER_PATH CLIENT_PATH"
}

parse_args() {
    if [ "$#" -ne 2 ]; then
        script_usage >&2
        return 1
    fi
    server_path="$1"
    client_path="$2"
}

main() {
    parse_args "$@"
    run_server
    run_stress_test
}

main "$@"
