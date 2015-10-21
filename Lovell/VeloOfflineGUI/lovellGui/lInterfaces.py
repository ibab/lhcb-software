import sys
from veloview.runview import plots, utils
from veloview.config import Config, run_view

def run_list(run_data_dir):
    Config().run_data_dir = run_data_dir
    return utils.run_list()


def runview_config():
    return run_view.run_view_pages


def runview_plot(run, name, sensor, run_data_dir, refRun = 'Auto', 
                 getRef = False, normalise = False):
    Config().run_data_dir = run_data_dir
    err = False
    # Need to append the sensor number to the name.
#     if not utils.valid_run(run):
#         err = True
#         msg = "Invalid run number provided: {0}".format(run)
#         print msg
    if not utils.valid_sensor(sensor):
        err = True
        msg = "Invalid sensor number provided: {0}".format(sensor)
        print msg
        
    name = name.format(sensor)
    
    if getRef:
        if refRun == 'Auto':
            print 'Getting auto ref'
            return plots.get_run_plot_with_reference(name, run, normalise = normalise)
        else:
            print 'Getting specified ref'
            return plots.get_run_plot_with_reference(name, run, refRun=refRun, normalise = normalise)
    else:
        return plots.get_run_plot(name, run, normalise = normalise)