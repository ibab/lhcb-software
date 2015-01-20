#!/usr/bin/env python
"""Return configuration parameters from the veloview module.

Supported configuration parameter names are:
    run_view_config -- Run view GUI configuration dictionary
    run_list -- Available run numbers sorted high-to-low
"""
import sys
import argparse
import json


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
    return parser


def veloview_configuration(param):
    from veloview.core import run_view_config
    from veloview.runview import utils

    if param == "run_view_config":
        sys.stdout.write(json.dumps(run_view_config.run_view_pages))
    elif param == "run_list":
        sys.stdout.write("\n".join([str(r) for r in utils.run_list()]))


def main():
    parser = create_parser()
    args = parser.parse_args()
    veloview_configuration(args.param)


if __name__ == "__main__":
    main()
