# To build, run from the top-level directory:
# docker build -f server/Dockerfile -t egortensin/math-server .

FROM debian

LABEL maintainer="Egor Tensin <Egor.Tensin@gmail.com>"

# Don't prompt:
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update -yq && apt-get dist-upgrade -yq

RUN apt-get install -yq build-essential cmake libboost-filesystem-dev libboost-program-options-dev libboost-test-dev

COPY [".", "/tmp/src/"]

WORKDIR /tmp/build

RUN cmake -G "Unix Makefiles"                       \
        -D CMAKE_BUILD_TYPE=Release                 \
        -D ENABLE_TESTS=ON                          \
        -D USE_STATIC_RUNTIME=OFF                   \
        -D CMAKE_CXX_STANDARD_LIBRARIES="-lpthread" \
        /tmp/src &&                                 \
    cmake --build test &&                           \
    ./test/unit_tests/unit_tests --log_level=all && \
    cmake --build server

CMD ["./server/main/server"]
