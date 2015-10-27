#!/usr/bin/env python
"""Analyse a run by comparing it to reference values.

A run number is valid if it is present in the run list.
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
    parser.add_argument("runs", type=int, nargs='+', help="Run number")
    parser.add_argument("--run-data-dir", default=Config().run_data_dir,
                        help="Directory to search for run list and data")
    parser.add_argument("--output-dir", default=None,
                        help="Directory to write output to")
    return parser

def main():
    parser = create_parser()
    args = parser.parse_args()
    runs = args.runs
    run_data_dir = args.run_data_dir
    output_dir = args.output_dir
    if not output_dir:
        output_dir = run_file_path(runs[0])


    # Change the run data directory to the user-specified one
    Config().run_data_dir = run_data_dir

    configWrapper = AnalysisConfigWrapper(Config().analysis_config)

    for run in runs:
        if not is_in_grf(run):
            combinerTrunk = configWrapper.getTrunkForRun(run)
            combinerTrunk.evaluate()
            combinerTrunk.write_to_grf()
            add_to_grf(run)

if __name__ == "__main__":
    main()
    sys.exit(0)
