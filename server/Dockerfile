# To build, run from the top-level directory:
# docker build -f server/Dockerfile -t egortensin/math-server .

FROM alpine:3.11

LABEL maintainer="Egor Tensin <Egor.Tensin@gmail.com>"

ENV src_dir=/usr/src/math-server
COPY [".", "$src_dir"]

RUN build_deps='boost-dev cmake g++ make python3' && \
    apk add --no-cache $build_deps && \
    "$src_dir/cmake/ci/build.py" \
        --src "$src_dir" \
        --install /usr/local \
        --clean \
        --configuration Release \
        -- -D ENABLE_TESTS=ON \
        -- -D Boost_USE_STATIC_LIBS=OFF && \
    rm -rf -- "$src_dir" && \
    apk del --purge --rdepends $build_deps && \
    runtime_deps='boost-filesystem boost-program_options boost-unit_test_framework libstdc++' && \
    apk add $runtime_deps && \
    /usr/local/bin/math-server-unit-tests --log_level=all

CMD ["/usr/local/bin/math-server"]
