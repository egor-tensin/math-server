# To build, run from the top-level directory:
# docker build -f client/Dockerfile -t egortensin/math-client .

FROM debian

LABEL maintainer="Egor Tensin <Egor.Tensin@gmail.com>"

# Don't prompt:
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update -yq && \
    apt-get install -yq --no-install-recommmends \
        build-essential              \
        cmake                        \
        libboost-filesystem-dev      \
        libboost-program-options-dev \
        libboost-test-dev

COPY [".", "/tmp/src/"]

WORKDIR /tmp/build

RUN cmake -G "Unix Makefiles"                       \
        -D CMAKE_BUILD_TYPE=RelWithDebInfo          \
        -D CMAKE_CXX_STANDARD_LIBRARIES="-lpthread" \
        -D ENABLE_TESTS=ON                          \
        -D CC_CXX_STANDARD=17                       \
        /tmp/src &&                                 \
    cmake --build test &&                           \
    ./test/unit_tests/unit_tests --log_level=all && \
    cmake --build client

ENTRYPOINT ["./client/client"]
CMD ["-c", "2 * 2"]
