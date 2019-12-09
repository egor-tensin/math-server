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

COPY [".", "/tmp/src/"]

WORKDIR /tmp/build

RUN cmake -G "Unix Makefiles"                       \
        -D CMAKE_BUILD_TYPE=RelWithDebInfo          \
        -D ENABLE_TESTS=ON                          \
        /tmp/src &&                                 \
    cmake --build test &&                           \
    ./test/unit_tests/unit_tests --log_level=all && \
    cmake --build client

ENTRYPOINT ["./client/client"]
CMD ["-c", "2 * 2"]
