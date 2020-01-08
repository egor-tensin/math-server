# To build, run from the top-level directory:
# docker build -f client/Dockerfile -t egortensin/math-client .

FROM alpine:3.11

LABEL maintainer="Egor Tensin <Egor.Tensin@gmail.com>"

ENV src_dir=/usr/src/math-server
COPY [".", "$src_dir"]

RUN build_deps='boost-dev cmake g++ make python3' && \
    apk add --no-cache $build_deps && \
    "$src_dir/cmake/cmake/build/build.py" \
        --install /usr/local \
        --configuration Release \
        -- \
        "$src_dir" \
        -D ENABLE_TESTS=ON \
        -D Boost_USE_STATIC_LIBS=OFF && \
    rm -rf -- "$src_dir" && \
    apk del --purge --rdepends $build_deps && \
    runtime_deps='boost-filesystem boost-program_options boost-regex boost-unit_test_framework libstdc++' && \
    apk add $runtime_deps && \
    /usr/local/bin/math-server-unit-tests --log_level=all

ENTRYPOINT ["/usr/local/bin/math-client"]
CMD ["-c", "2 * 2"]
