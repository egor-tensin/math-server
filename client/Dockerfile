# To build, run from the top-level directory:
# docker build -f client/Dockerfile -t egortensin/math-client .

FROM debian

LABEL maintainer="Egor Tensin <Egor.Tensin@gmail.com>"

# Don't prompt:
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update -yq && \
    apt-get install -yq --no-install-recommends \
        build-essential              \
        cmake                        \
        libboost-filesystem-dev      \
        libboost-program-options-dev \
        libboost-test-dev

ENV src_dir=/tmp/src \
    build_dir=/tmp/build

COPY [".", "$src_dir"]

WORKDIR $build_dir

RUN cmake -G "Unix Makefiles"              \
        -D CMAKE_BUILD_TYPE=Release        \
        -D CMAKE_INSTALL_PREFIX=/usr/local \
        -D ENABLE_TESTS=ON                 \
        "$src_dir" &&                      \
    cmake --build . --config Release --target install && \
    /usr/local/bin/unit_tests --log_level=all

ENTRYPOINT ["/usr/local/bin/client"]
CMD ["-c", "2 * 2"]
