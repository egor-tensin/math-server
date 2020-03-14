#!/usr/bin/env python3

# Copyright (c) 2019 Egor Tensin <Egor.Tensin@gmail.com>
# This file is part of the "math-server" project.
# For details, see https://github.com/egor-tensin/math-server.
# Distributed under the MIT License.

'''This is a stupid script to feed the server with somewhat random arithmetic
expressions.
'''

import argparse
from contextlib import contextmanager
import logging
import math
from multiprocessing import Pool
import random
import subprocess
from subprocess import PIPE
import sys
from timeit import default_timer


class Client:
    DEFAULT_PATH = 'math-client'
    DEFAULT_HOST = 'localhost'
    DEFAULT_PORT = 18000

    def __init__(self, path=None, host=None, port=None):
        if path is None:
            path = Client.DEFAULT_PATH
        if host is None:
            host = Client.DEFAULT_HOST
        if port is None:
            port = Client.DEFAULT_PORT
        self._path = path
        self._host = host
        self._port = port

    def get_command_line(self):
        return [self._path, '--host', self._host, '--port', str(self._port)]


@contextmanager
def timer(description):
    start = default_timer()
    try:
        yield
    finally:
        duration = default_timer() - start
        logging.info('%s: %s', description, duration)


class Expr:
    def __init__(self, impl):
        self._impl = impl

    def __str__(self):
        return self._impl

    def eval(self):
        try:
            return eval(self._impl)
        except Exception:
            logging.error("Couldn't evaluate expression: %s", self._impl)
            raise


class ExprGen:
    _OPERATORS = '+', '-', '*', '/'
    _MIN_NUMOF_OPERATORS = 10
    _MAX_NUMOF_OPERATORS = 1000
    _MIN_NUMBER = -10e10
    _MAX_NUMBER = 10e10

    @staticmethod
    def _random_operator():
        return ExprGen._OPERATORS[random.randrange(len(ExprGen._OPERATORS))]

    @staticmethod
    def _random_number():
        return random.randint(ExprGen._MIN_NUMBER, ExprGen._MAX_NUMBER)

    @staticmethod
    def gen_expression():
        numof_operators = random.randrange(ExprGen._MIN_NUMOF_OPERATORS,
                                           ExprGen._MAX_NUMOF_OPERATORS + 1)
        expr = ''
        for _ in range(numof_operators):
            expr += f"{ExprGen._random_number()} {ExprGen._random_operator()} "
        expr += str(ExprGen._random_number())
        return Expr(expr)


class Input:
    def __init__(self, expr_lst):
        self._expr_lst = expr_lst

    @staticmethod
    def generate(n):
        if n < 1:
            raise ValueError('input length must be positive')
        return Input([ExprGen.gen_expression() for i in range(n)])

    def stdin(self):
        return '\n'.join(map(str, self._expr_lst)) + '\n'

    def expected_output(self):
        return [expr.eval() for expr in self._expr_lst]


def _float_lists_equal(xs, ys):
    if len(xs) != len(ys):
        return False
    for i in range(len(xs)):
        if not math.isclose(xs[i], ys[i]):
            return False
    return True


class Output:
    def __init__(self, stdout):
        self._stdout = stdout

    def __eq__(self, other):
        return self._stdout == other._stdout

    def __str__(self):
        return self._stdout

    def parse(self):
        values = []
        for line in self._stdout.split('\n')[:-1]:
            try:
                n = float(line)
            except ValueError:
                logging.error('Not a number: %s', line)
                raise
            values.append(n)
        return values

    def equal_to_expected(self, expected):
        this = self.parse()
        if not _float_lists_equal(this, expected):
            logging.error("Actual output doesn't match expected output")
            logging.error('Expected output (length %d):\n%s', len(expected), expected)
            logging.error('Actual output (length %d):\n%s', len(this), this)
            return False
        return True


def _run_client(client, stdin):
    with _logging():
        cmd = client.get_command_line()
        with timer('Client invocation'):
            result = subprocess.run(cmd, stdout=PIPE, stderr=PIPE, input=stdin,
                                    text=True, check=True)
        return Output(result.stdout)


def _run_clients(numof_clients, client, stdin):
    with Pool(numof_clients) as pool:
        results = pool.starmap(_run_client, [(client, stdin) for _ in range(numof_clients)])
        if len(results) != numof_clients:
            raise RuntimeError(f'expected {numof_clients} results, got {len(results)}')
        return results


def _run_stress_test(args):
    client = Client(args.client, args.host, args.port)
    input = Input.generate(args.numof_expressions)
    stdin = input.stdin()
    expected_output = input.expected_output()
    actual_outputs = _run_clients(args.numof_processes, client, stdin)

    # Check that all outputs are equal to each other:
    assert actual_outputs
    actual_output, rest = actual_outputs[0], actual_outputs[1:]
    for other in rest:
        if actual_output == other:
            continue
        logging.error("Client outputs don't match, this should never happen")
        logging.error('For example, this:\n%s', actual_output)
        logging.error('... is not equal to this:\n%s', other)
        return False

    # Check that the first output is equal to the expected output:
    return actual_output.equal_to_expected(expected_output)


def _parse_positive_int(s):
    try:
        n = int(s)
    except ValueError:
        raise argparse.ArgumentTypeError(f'must be a positive integer: {s}')
    if n < 1:
        raise argparse.ArgumentTypeError(f'must be a positive integer: {s}')
    return n


def _parse_args(argv=None):
    if argv is None:
        argv = sys.argv[1:]

    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter)

    parser.add_argument('--host', '-H', metavar='HOST',
                        default=Client.DEFAULT_HOST,
                        help='server host')
    parser.add_argument('--port', '-p', metavar='PORT',
                        type=int, default=Client.DEFAULT_PORT,
                        help='server port')
    parser.add_argument('--processes', '-n', metavar='N',
                        dest='numof_processes',
                        type=_parse_positive_int, default=1,
                        help='number of processes')
    parser.add_argument('--expressions', '-e', metavar='N',
                        dest='numof_expressions',
                        type=_parse_positive_int, default=1,
                        help='number of expressions')
    parser.add_argument('--client', '-c', metavar='PATH',
                        default=Client.DEFAULT_PATH,
                        help='path to the client executable')

    return parser.parse_args(argv)


@contextmanager
def _logging():
    logging.basicConfig(
        format='%(asctime)s | %(levelname)s | %(message)s',
        level=logging.DEBUG)
    try:
        yield
    except Exception as e:
        logging.exception(e)
        raise


def main(argv=None):
    with _logging():
        args = _parse_args(argv)
        if not _run_stress_test(args):
            sys.exit(1)


if __name__ == '__main__':
    main()
