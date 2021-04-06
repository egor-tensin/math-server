# To build, run from the top-level directory:
# docker build -f server/Dockerfile -t egortensin/math-server .

FROM alpine:3.11 AS base

FROM base AS builder

ENV src_dir=/usr/src/math-server
COPY [".", "$src_dir"]

RUN build_deps='bash boost-dev cmake g++ make python3' && \
    apk add --no-cache $build_deps && \
    cd -- "$src_dir" && \
    make install CONFIGURATION=Release CMAKE_FLAGS='-D MATH_SERVER_TESTS=ON -D Boost_USE_STATIC_LIBS=OFF'

FROM base

LABEL maintainer="Egor Tensin <Egor.Tensin@gmail.com>"

COPY --from=builder ["/usr/src/math-server/.build/install", "/opt/math-server"]

RUN runtime_deps='boost-filesystem boost-program_options boost-regex boost-unit_test_framework libstdc++' && \
    apk add $runtime_deps && \
    /opt/math-server/bin/math-server-unit-tests --log_level=all

CMD ["/opt/math-server/bin/math-server"]
