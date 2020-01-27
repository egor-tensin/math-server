# To build, run from the top-level directory:
# docker build -f server/Dockerfile -t egortensin/math-server .

FROM alpine:3.11 AS base

FROM base AS builder

ENV src_dir=/usr/src/math-server
COPY [".", "$src_dir"]

RUN build_deps='boost-dev cmake g++ make python3' && \
    apk add --no-cache $build_deps && \
    "$src_dir/cmake/cmake/build/build.py" \
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

CMD ["/opt/math-server/bin/math-server"]
