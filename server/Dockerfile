# To build, run from the top-level directory:
# docker build -f server/Dockerfile -t egortensin/math-server .

FROM debian:trixie-slim AS base

ARG src_dir=/usr/src/math-server

FROM base AS builder

ARG src_dir
COPY [".", "$src_dir"]

RUN build_deps='libboost-all-dev cmake g++ make python3' && \
    apt update && \
    DEBIAN_FRONTEND=noninteractive \
        apt install -y --no-install-recommends $build_deps && \
    cd -- "$src_dir" && \
    make install CONFIGURATION=Release CMAKE_FLAGS='--cmake-arg=-DMATH_SERVER_TESTS=ON --cmake-arg=-DBoost_USE_STATIC_LIBS=OFF'

FROM base

LABEL maintainer="Egor Tensin <egor@tensin.name>"

ARG src_dir
COPY --from=builder ["$src_dir/build/install", "/opt/math-server"]

RUN runtime_deps='libboost-filesystem1.83.0 libboost-program-options1.83.0 libboost-regex1.83.0 libboost-test1.83.0' && \
    apt update && \
    DEBIAN_FRONTEND=noninteractive \
        apt install -y --no-install-recommends $runtime_deps && \
    /opt/math-server/bin/math-server-unit-tests --log_level=all

CMD ["/opt/math-server/bin/math-server"]
