#!/usr/bin/env python
"""Return configuration parameters from the veloview module.

Supported configuration parameter names are:
    run_view_config -- Run view GUI configuration dictionary
    run_list -- Available run numbers sorted high-to-low
"""
# This is due to what appears to be a ROOT bug, where doing `import ROOT`
# prints a control sequence to stdout on xterm terminal types
import os
os.environ["TERM"] = "dumb"

import sys
import argparse
import json

from veloview.config import Config


def create_parser():
    parser = argparse.ArgumentParser(
        description=__doc__.split("\n")[0],
        epilog="\n".join(__doc__.split("\n")[1:]),
        formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("param",
                        type=str,
                        choices=[
                            "run_view_config",
                            "run_list"
                        ],
                        help="Configuration parameter to return")
    parser.add_argument("--run-data-dir", default=Config().run_data_dir,
                        help="Directory to search for run list and data")
    return parser


def veloview_configuration(param, run_data_dir):
    from veloview.runview import utils

    # Change the run data directory to the user-specified one
    Config().run_data_dir = run_data_dir

    if param == "run_view_config":
        sys.stdout.write(json.dumps(Config().run_view_pages))
    elif param == "run_list":
        sys.stdout.write("\n".join([str(r) for r in utils.run_list()]))
        
        
def runview_config():
     return Config().run_view_pages   
       
        
def run_list(run_data_dir):
     from veloview.runview import utils
     Config().run_data_dir = run_data_dir
     return utils.run_list()


def main():
    parser = create_parser()
    args = parser.parse_args()
    veloview_configuration(args.param, args.run_data_dir)


if __name__ == "__main__":
    main()
