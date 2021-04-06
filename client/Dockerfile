# To build, run from the top-level directory:
# docker build -f client/Dockerfile -t egortensin/math-client .

FROM alpine:3.11 AS base

ARG src_dir=/usr/src/math-server

FROM base AS builder

ARG src_dir
COPY [".", "$src_dir"]

RUN build_deps='bash boost-dev cmake g++ make python3' && \
    apk add --no-cache $build_deps && \
    cd -- "$src_dir" && \
    make install CONFIGURATION=Release CMAKE_FLAGS='-D MATH_SERVER_TESTS=ON -D Boost_USE_STATIC_LIBS=OFF'

FROM base

LABEL maintainer="Egor Tensin <Egor.Tensin@gmail.com>"

ARG src_dir
COPY --from=builder ["$src_dir/.build/install", "/opt/math-server"]

RUN runtime_deps='boost-filesystem boost-program_options boost-regex boost-unit_test_framework libstdc++' && \
    apk add $runtime_deps && \
    /opt/math-server/bin/math-server-unit-tests --log_level=all

ENTRYPOINT ["/opt/math-server/bin/math-client"]
CMD ["-c", "2 * 2"]
