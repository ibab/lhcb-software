"""Utilities for GUI run view pages."""
from veloview.core import config
from veloview.utils import paths


def run_list():
    """Return a list of run numbers as integers sorted high-to-low."""
    run_file_lines = tuple(open(config.processed_run_list_file, "r"))
    return sorted([int(l.strip()) for l in run_file_lines], reverse=True)


def valid_run(run):
    """Return True if run is a run number present in the run number file.

    Run numbers not present do not have corresponding run files and so are
    considered invalid.
    """
    return run in run_list()


def sensor_list():
    """Return a list of sensor numbers as integers."""
    return range(0, 47) + range(64, 107)


def valid_sensor(sensor):
    """Return True if sensor is a valid sensor number."""
    return sensor in sensor_list()


def reference_run(plot, run):
    """Return the reference run number for the plot and nominal run number."""
    # TODO need to implement the reference database
    # The method can then query that DB and return the run number
    return run


def run_file_path(run):
    """Return TFile object directory path for the given run."""
    return paths.make_dir_tree(run, config.run_data_dir)
