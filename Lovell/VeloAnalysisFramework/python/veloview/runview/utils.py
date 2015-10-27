"""Utilities for GUI run view pages."""
from veloview.config import Config
from veloview.runview.reference_database import ReferenceDatabase
from veloview.utils import paths, rundb

import glob
import os

def run_list():
    """Return a list of run numbers as integers sorted high-to-low."""
    run_file_lines = tuple(open(Config().processed_run_list_path, "r"))
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
    refdb = ReferenceDatabase(Config().reference_db_path)
    polarity = rundb.RunDB().polarity(run)
    return refdb.reference_run_for_plot(run, plot, polarity)

def run_file_path(run):
    """Return TFile object directory path for the given run."""
    return paths.make_dir_tree(run, Config().run_data_dir)

def run_file(run):
    """Return latest run file path from run number. Prefer merged files."""
    base = run_file_path(run)
    merged_files = glob.glob("{0}/*_Clusters_Brunel.root".format(base))
    if merged_files:
        # Return latest merged file if available
        return max(merged_files, key = os.path.getctime)

    files = glob.glob("{0}/*.root".format(base))
    try:
        # Otherwise, return normal file
        return max(files, key = os.path.getctime)
    except ValueError:
        raise IOError("Run file not found for run {0}".format(run))

def reference_run_file(run):
    """Return the reference run file for the given run."""
    refdb = ReferenceDatabase(Config().reference_db_path)
    polarity = rundb.RunDB().polarity(run)
    ref_run = refdb.reference_run(run, polarity)
    return run_file(ref_run)

def is_in_grf(run):
    f = open(Config().grf_runs_file, 'r')
    runs = f.readlines()
    f.close()
    return str(run) in map(lambda x: x.strip(), runs)

def add_to_grf(run):
    if (is_in_grf(run)): return

    f = open(Config().grf_runs_file, 'a')
    f.write(str(run) + "\n")
    f.close()
