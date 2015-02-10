from __future__ import print_function

import argparse
import inspect
import os
import subprocess
import sys


def create_parser():
    parser = argparse.ArgumentParser(description='Run VeloAnalysisFramework test')
    parser.add_argument('test', metavar='test', type=str,
                        help='Test to run')
    return parser


def run_test(filename):
    basedir = os.path.dirname(inspect.stack()[0][1])
    cmd = 'python ' + os.path.join(basedir, filename)
    try:
        output = subprocess.check_output(cmd, stderr=subprocess.STDOUT,
                                         shell=True)
    except subprocess.CalledProcessError as e:
        output = e.output
    if 'OK' not in output:
        print('Tests failed with output:', file=sys.stderr)
        print(output, file=sys.stderr)


if __name__ == '__main__':
    parser = create_parser()
    args = parser.parse_args()
    test_name = args.test

    if not test_name.startswith('test_'):
        test_name = 'test_' + test_name
    if not test_name.endswith('.py'):
        test_name = test_name + '.py'

    run_test(test_name)
