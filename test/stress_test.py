import argparse
import ast
from contextlib import contextmanager
import math
from multiprocessing import Pool
import random
import subprocess
from subprocess import PIPE
import sys
from timeit import default_timer

# This is a crappy script to feed the server with somewhat random arithmetic
# expressions.
# It's throwaway, hence the poor quality.

DEFAULT_CLIENT_PATH = 'client'
DEFAULT_HOST = 'localhost'
DEFAULT_PORT = 18000

class Client:
    def __init__(self, path=DEFAULT_CLIENT_PATH, host=DEFAULT_HOST, port=DEFAULT_PORT):
        self._path = path
        self._host = host
        self._port = port

    def get_command_line(self):
        return [self._path, '--host', self._host, '--port', str(self._port)]

@contextmanager
def timer(description):
    start = default_timer()
    yield
    duration = default_timer() - start
    print(f"{description}: {duration}")

def run_client(i, client, stdin):
    with timer(f"Invocation #{i}"):
        cmd = client.get_command_line()
        result = subprocess.run(cmd, stdout=PIPE, stderr=PIPE, input=stdin,
                                universal_newlines=True)
        result.check_returncode()
        return result

OPERATORS = '+', '-', '*', '/'
MIN_OPERATORS = 10
MAX_OPERATORS = 1000
MIN_NUMBER = -10e10
MAX_NUMBER = 10e10

def random_operator():
    return OPERATORS[random.randrange(len(OPERATORS))]

def random_number():
    return random.randint(MIN_NUMBER, MAX_NUMBER)

def gen_expression():
    numof_operators = random.randrange(MIN_OPERATORS, MAX_OPERATORS + 1)
    expression = ''
    for i in range(numof_operators):
        expression += f"{random_number()} {random_operator()} "
    expression += str(random_number())
    return expression

def gen_expressions(n):
    for i in range(n):
        yield gen_expression()

def run_stress_test(args):
    client = Client(args.client, args.host, args.port)
    expressions = list(gen_expressions(args.expressions))
    stdin = '\n'.join(expressions)
    expected_output = [eval(expr) for expr in expressions]
    with Pool(args.processes) as pool:
        results = pool.starmap(run_client, [(i, client, stdin) for i in range(args.processes)])
        assert results
        assert all((results[0].stdout == other.stdout for other in results[1:]))
        actual_output = list(map(float, results[0].stdout.split('\n')[:-1]))
        assert len(expected_output) == len(actual_output)
        for i in range(len(expected_output)):
            if math.isclose(expected_output[i], actual_output[i]):
                continue
            print(f"Expression: {expressions[i]}")
            print(f"Expected output: {expected_output[i]}")
            print(f"Actual output:   {actual_output[i]}")

def parse_args(argv=None):
    if argv is None:
        argv = sys.argv[1:]
    parser = argparse.ArgumentParser()
    parser.add_argument('--host', '-H', metavar='HOST',
                        default=DEFAULT_HOST,
                        help='set host')
    parser.add_argument('--port', '-p', metavar='PORT',
                        type=int, default=DEFAULT_PORT,
                        help='set port')
    parser.add_argument('--processes', '-n', metavar='N',
                        type=int, default=1,
                        help='set number of processes')
    parser.add_argument('--expressions', '-e', metavar='N',
                        type=int, default=1,
                        help='set number of expressions')
    parser.add_argument('--client', '-c', metavar='PATH',
                        default=DEFAULT_CLIENT_PATH,
                        help='set path to client.exe')
    args = parser.parse_args(argv)
    # Bleh
    assert args.processes > 0
    assert args.expressions > 0
    return args

def main(argv=None):
    args = parse_args(argv)
    run_stress_test(args)

if __name__ == '__main__':
    main()
