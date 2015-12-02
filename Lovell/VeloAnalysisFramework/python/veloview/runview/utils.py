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

    # first choice: NZS + Clusters + Online Brunel merged files
    merged_files = glob.glob("{0}/*_Clusters_Brunel.root".format(base))
    if merged_files:
        # Return latest merged file if available
        return max(merged_files, key = os.path.getctime)

    # second choice: NZS + Clusters merged files
    merged_files = glob.glob("{0}/*NZS_Clusters.root".format(base))
    if merged_files:
        # Return latest merged file if available
        return max(merged_files, key = os.path.getctime)

    # third choice: any other root file
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
    f = open(Config().grf_runs_file_path, 'r')
    runs = f.readlines()
    f.close()
    return str(run) in map(lambda x: x.strip(), runs)

def add_to_grf(run):
    if (is_in_grf(run)): return

    f = open(Config().grf_runs_file_path, 'a')
    f.write(str(run) + "\n")
    f.close()

class DqVarsParser(object):
    TELL1_NUMBERS_R   = range(42)
    TELL1_NUMBERS_PHI = range(64, 106)

    def __init__(self, varNames = None, expertMode = False):
        self.__varNames = varNames
        self.__expertMode = expertMode

    def get_dq_variables(self):
        return self.__add_variables_branch(Config().analysis_config, '', Config().analysis_config[0]['MasterCombiner'])
    
    def __add_variables_branch(self, config, key, branch, base = ''):
        if self.__varNames:
            result = [(key + '.' + var if key else var, branch['title']) for var in self.__varNames]
        else:
            result = [(key, branch['title'])]
    
        for child in branch['children']:
            if child in config[0].keys():
                result += self.__add_variables_branch(config, child, config[0][child], '{}.{}'.format(base, child) if base else child)
            elif child in config[1].keys():
                result += self.__add_variables_leaf(('{}.{}'.format(base, child)) if base else child, config[1][child])
        return result

    def __add_variables_leaf(self, key, leaf):
        result = []
        if self.__expertMode and "tell1" in leaf.keys():
            for tell1 in self.TELL1_NUMBERS_R + self.TELL1_NUMBERS_PHI:
                if self.__varNames:
                    result += [(key + '.' + key.split('.')[-1] + "_TELL1_{:0>3d}".format(tell1) + '.' + var, leaf['title'] + ' Tell1 {:0>3d}'.format(tell1)) for var in self.__varNames]
                else:
                    result.append((key + '.' + key.split('.')[-1] + "_TELL1_{:0>3d}".format(tell1), leaf['title'] + ' Tell1 {:0>3d}'.format(tell1)))
    
        if self.__varNames:
            return result + [(key + '.' + var, leaf['title']) for var in self.__varNames]
        return result + [(key, leaf['title'])]

def get_trending_variables(expert = False):
    dqVarsParser = DqVarsParser(['score'], expert)
    return dqVarsParser.get_dq_variables()

