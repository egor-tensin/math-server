# To build, run from the top-level directory:
# docker build -f server/Dockerfile -t egortensin/math-server .

FROM debian

LABEL maintainer="Egor Tensin <Egor.Tensin@gmail.com>"

# Don't prompt:
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update -yq \
    && apt-get install -yq --no-install-recommends \
        build-essential              \
        cmake                        \
        libboost-filesystem-dev      \
        libboost-program-options-dev \
        libboost-test-dev            \
        python3

ENV src_dir=/usr/src/app

COPY [".", "$src_dir"]

RUN "$src_dir/cmake/build/build.py" \
        --src "$src_dir"            \
        --install /usr/local        \
        --clean                     \
        --configuration Release     \
        -- -DENABLE_TESTS=ON        \
    && /usr/local/bin/math-server-unit-tests --log_level=all

CMD ["/usr/local/bin/math-server"]
