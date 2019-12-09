# To build, run from the top-level directory:
# docker build -f server/Dockerfile -t egortensin/math-server .

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
        -D CMAKE_BUILD_TYPE=Release                 \
        -D CMAKE_CXX_STANDARD_LIBRARIES="-lpthread" \
        -D ENABLE_TESTS=ON                          \
        -D CC_CXX_STANDARD=17                       \
        /tmp/src &&                                 \
    cmake --build test &&                           \
    ./test/unit_tests/unit_tests --log_level=all && \
    cmake --build server

CMD ["./server/main/server"]
