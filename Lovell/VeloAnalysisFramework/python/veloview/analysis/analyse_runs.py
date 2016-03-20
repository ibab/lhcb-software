#!/usr/bin/env python
"""Analyse runs by comparing it to reference values.

A run number is valid if it is present in the run list.
"""

import os
import sys
import argparse

from veloview.config import Config
from veloview.core import AnalysisConfigWrapper
from veloview.core.io import DQDB
from veloview.runview.utils import run_file_path

def create_parser():
    parser = argparse.ArgumentParser(
        description=__doc__.split("\n")[0],
        epilog="\n".join(__doc__.split("\n")[1:]))
    parser.add_argument("runs", type=int, nargs='+', help="Run numbers")
    parser.add_argument("--run-data-dir", default=Config().run_data_dir,
                        help="Directory to search for run list and data")
    parser.add_argument("--output-dir", default=None,
                        help="Directory to write output to")
    return parser

def main():
    parser = create_parser()
    args = parser.parse_args()
    runs = frozenset(args.runs)
    run_data_dir = args.run_data_dir
    output_dir = args.output_dir
    if not output_dir:
        output_dir = run_file_path(args.runs[0])

    # Change the run data directory to the user-specified one
    Config().run_data_dir = run_data_dir

    configWrapper = AnalysisConfigWrapper(Config().analysis_config)

    # Create the DB if it doesn't exist already
    file = Config().dq_db_file_path
    if os.path.isfile(file):
        db = DQDB(Config().dq_db_file_path)
        existing_runs = frozenset(db.get_runs())
        db.close()
    else:
        db = DQDB(file, 'CREATE')
        db.close()
        existing_runs = frozenset()

    runs_to_process = sorted(list(runs - existing_runs))

    for run in runs_to_process:
        combinerTrunk = configWrapper.getTrunkForRun(run)
        combinerTrunk.evaluate()
        combinerTrunk.write_to_db()

if __name__ == "__main__":
    main()
    sys.exit(0)
