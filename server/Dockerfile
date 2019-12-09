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

RUN cmake -G "Unix Makefiles"              \
        -D CMAKE_BUILD_TYPE=Release        \
        -D CMAKE_INSTALL_PREFIX=/usr/local \
        -D ENABLE_TESTS=ON                 \
        /tmp/src &&                        \
    cmake --build . --config Release --target install && \
    /usr/local/bin/unit_tests --log_level=all
    cmake --build server

CMD ["/usr/local/bin/server"]
