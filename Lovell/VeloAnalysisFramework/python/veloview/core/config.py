"""Configuration constants for the veloview module.

The run_data_dir key will take the value of the RUN_DATA_DIR environment
variable, if it is set, else will use a default.
"""
import os

# Absolute path to the directory containing per-run data
try:
    run_data_dir = os.environ['RUN_DATA_DIR']
except KeyError:
    run_data_dir = "/calib/velo/dqm/VeloView/VetraOutput"

# Absolute path to the text file containing the list of processed run numbers
processed_run_list_file = "{0}/RunList.txt".format(run_data_dir)

# Absoute path to the SQLite3 reference run database
reference_db = "{0}/ReferenceDB.sqlite".format(run_data_dir)


def update_run_data_dir(path):
    """Update run data directory configuration key and dependents."""
    from veloview.core import config
    old_dir = config.run_data_dir
    config.run_data_dir = path
    config.processed_run_list_file = config.processed_run_list_file.replace(
        old_dir, path
    )
    config.reference_db = config.reference_db.replace(
        old_dir, path
    )
