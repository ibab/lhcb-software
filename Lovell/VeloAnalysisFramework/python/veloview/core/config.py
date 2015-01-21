"""Configuration constants for the veloview module."""

# Absolute path to the directory containing per-run data
run_data_dir = "/calib/velo/dqm/VeloView/VetraOutput"

# Absolute path to the text file containing the list of processed run numbers
processed_run_list_file = "{0}/RunList.txt".format(run_data_dir)

def update_run_data_dir(path):
    """Update run data directory configuration key and dependents."""
    from veloview.core import config
    old_dir = config.run_data_dir
    config.run_data_dir = path
    config.processed_run_list_file = config.processed_run_list_file.replace(
        old_dir, path
    )
