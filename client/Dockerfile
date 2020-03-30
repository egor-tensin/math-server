# To build, run from the top-level directory:
# docker build -f client/Dockerfile -t egortensin/math-client .

FROM alpine:3.11 AS base

FROM base AS builder

ENV src_dir=/usr/src/math-server
COPY [".", "$src_dir"]

RUN build_deps='boost-dev cmake g++ make python3' && \
    apk add --no-cache $build_deps && \
    cd -- "$src_dir/cmake" && \
    python3 -m project.cmake.build \
        --install /opt/math-server \
        --configuration Release \
        -- \
        "$src_dir" \
        -D ENABLE_TESTS=ON \
        -D Boost_USE_STATIC_LIBS=OFF

FROM base

LABEL maintainer="Egor Tensin <Egor.Tensin@gmail.com>"

COPY --from=builder ["/opt/math-server", "/opt/math-server"]

RUN runtime_deps='boost-filesystem boost-program_options boost-regex boost-unit_test_framework libstdc++' && \
    apk add $runtime_deps && \
    /opt/math-server/bin/math-server-unit-tests --log_level=all

ENTRYPOINT ["/opt/math-server/bin/math-client"]
CMD ["-c", "2 * 2"]
