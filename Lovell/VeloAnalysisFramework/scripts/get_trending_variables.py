#!/usr/bin/env python
"""Get a list of trending variables.
"""

import sys
import argparse

from veloview.config import Config
from veloview.core import AnalysisConfigWrapper
from veloview.runview.utils import run_file_path, is_in_grf, add_to_grf

def create_parser():
    parser = argparse.ArgumentParser(
        description=__doc__.split("\n")[0],
        epilog="\n".join(__doc__.split("\n")[1:]))
    parser.add_argument("-e", dest='e', action='store_true', default=False,
                        help="Expert mode (list all individual sensor quantities)")
    return parser

def main():
    parser = create_parser()
    global args
    args = parser.parse_args()

    result = addBranch(Config().analysis_config, '', Config().analysis_config[0]['MasterCombiner'])
    print result

def addBranch(config, key, branch, base = ''):
    result = [(key + '.score' if key else 'score', branch['title'])]
    for child in branch['children']:
        if child in config[0].keys():
            result += addBranch(config, child, config[0][child], '{}.{}'.format(base, child) if base else child)
        elif child in config[1].keys():
            result += addLeaf(('{}.{}'.format(base, child)) if base else child, config[1][child])
    return result

def addLeaf(key, leaf):
    result = []
    global args
    if args.e and "tell1" in leaf.keys():
        for tell1 in AnalysisConfigWrapper.TELL1_NUMBERS_R + AnalysisConfigWrapper.TELL1_NUMBERS_PHI:
            result.append((key + '.' + key.split('.')[-1] + "_TELL1_{:0>3d}".format(tell1) + '.score', leaf['title'] + ' Tell1 {:0>3d}'.format(tell1)))
    return result + [(key + '.score', leaf['title'])]

if __name__ == "__main__":
    main()
    sys.exit(0)
