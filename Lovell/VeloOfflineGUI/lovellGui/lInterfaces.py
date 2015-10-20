import sys
import retrieve_run_view_plot
import veloview_configuration
from veloview.runview import plots
from veloview.config import Config

def run_list(run_data_dir):
    run_list = veloview_configuration.run_list(run_data_dir)
    return run_list


def runview_config():
    return veloview_configuration.runview_config()

'''
def runview_plot(run, name, sensor, run_data_dir, refRun = 'Auto', getRef = False):
    Config().run_data_dir = run_data_dir
    if getRef:
	if refRun == 'Auto': 
	    return plots.get_run_plot_with_reference(name, run)
	else:
	    return plots.get_run_plot_with_reference(name, run, refRun=refRun)
    else: return plots.get_run_plot(name, run)
'''
    