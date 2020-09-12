math-server
===========

[![Travis (.com) branch](https://img.shields.io/travis/com/egor-tensin/math-server/master?label=Travis%20%28GCC,%20Docker%29)](https://travis-ci.com/egor-tensin/math-server)
[![AppVeyor branch](https://img.shields.io/appveyor/ci/egor-tensin/math-server/master?label=AppVeyor%20%28Visual%20Studio%29)](https://ci.appveyor.com/project/egor-tensin/math-server/branch/master)

Development
-----------

Build using CMake.
Depends on Boost.{Filesystem,Program_options,Regex,Test}.
Boost version 1.67 or higher is required (it would be Boost 1.66 in a perfect
world, but the tests [fail to compile] with that).

[fail to compile]: https://lists.boost.org/boost-bugs/2018/01/49711.php

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
