import sys
sys.path.insert(0, "/afs/cern.ch/user/d/dsaunder/LovellDev_HEAD/VeloAnalysisFramework/scripts")
import retrieve_run_view_plot
import veloview_configuration


def run_list(run_data_dir):
     run_list = veloview_configuration.run_list(run_data_dir)
     return run_list


def runview_config():
     return veloview_configuration.runview_config()


def runview_plot(run, plot, sensor, noreference, run_data_dir):
     return retrieve_run_view_plot.retrieve_run_view_plot(run, plot, sensor, noreference, run_data_dir)