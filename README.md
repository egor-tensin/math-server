math-server
===========

[![Docker Cloud Build Status](https://img.shields.io/docker/cloud/build/egortensin/math-server?label=Docker%20%28math-server%29)](https://hub.docker.com/repository/docker/egortensin/math-server/builds)
[![Docker Cloud Build Status](https://img.shields.io/docker/cloud/build/egortensin/math-client?label=Docker%20%28math-client%29)](https://hub.docker.com/repository/docker/egortensin/math-client/builds)
[![Travis (.com) branch](https://img.shields.io/travis/com/egor-tensin/math-server/master?label=Travis)](https://travis-ci.com/egor-tensin/math-server)
[![AppVeyor branch](https://img.shields.io/appveyor/ci/egor-tensin/math-server/master?label=AppVeyor)](https://ci.appveyor.com/project/egor-tensin/math-server/branch/master)

Development
-----------

Build using CMake.
Depends on Boost.{Filesystem,Program_options,Test}.
Boost version 1.66 or higher is required.

Usage
-----

### `math-server`

If you just run `math-server`, it'll start a server on port 18000.
You can `telnet` to it, type in arithmetic expressions, and it'll give you the
results:

    1
    1
    2 * 2
    4
    1 / (3 - -3)
    0.16666666666666666
    1 / (-3 - -3)
    server error: parser error: division by zero
    -4 ^ 2
    -16
    (-4) ^ 2
    16

Consult `math-server --help` for more info.

### `math-client`

A `telnet`-like client for the server.
It'll try to connect to the server on localhost:18000.

Supports reading input from:

* the command line,

      > math-client -c "2 * 2"
      4

* a file(s),

      > math-client test.txt
      result1
      result2

* stdin.

      > math-client
      1
      1
      2 * 2
      4
      1 / (3 - -3)
      0.16666666666666666
      1 / (-3 - -3)
      server error: parser error: division by zero
      -4 ^ 2
      -16
      (-4) ^ 2
      16

Consult `math-client --help` for more info.

### Docker

Start the server using

    docker-compose pull && docker-compose up -d server

Connect to localhost:18000 to query it or run the client:

    > docker-compose run --rm client -c "69 * 420"
    28980

    > docker-compose run --rm client
    12 * 12
    144
    asdf
    server error: lexer error: invalid input at: asdf

License
-------

Distributed under the MIT License.
See [LICENSE.txt] for details.

[LICENSE.txt]: LICENSE.txt
