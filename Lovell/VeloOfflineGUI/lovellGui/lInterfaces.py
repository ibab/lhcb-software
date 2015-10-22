import sys
from veloview.runview import plots, utils
from veloview.config import Config, run_view


def run_list(run_data_dir):
    Config().run_data_dir = run_data_dir
    return utils.run_list()


def runview_config():
    return run_view.run_view_pages


def runview_plot(run, name, sensor, run_data_dir, refRun = 'Auto', 
                 getRef = False, normalise = False, notifyBox = None):
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
            return plots.get_run_plot_with_reference(name, run, 
                                                     normalise = normalise, 
                                                     notifyBox = notifyBox)
        else:
            print 'Getting specified ref'
            return plots.get_run_plot_with_reference(name, run, refRun=refRun, 
                                                     normalise = normalise, 
                                                     notifyBox = notifyBox)
    else:
        return plots.get_run_plot(name, run, normalise = normalise,
                                  notifyBox = notifyBox)

    
def IV_plot(name, moduleID, dataDate, IV_directory, sensor_mapping, refDate = None, 
            getRef = False, notifyBox = None):
    nominal = getIVcurve(name, moduleID, dataDate, IV_directory, sensor_mapping)
    if getRef: 
        reference = getIVcurve(name, moduleID, dataDate, IV_directory, sensor_mapping)
        return nominal, reference 
    else: return nominal    
    return nominal


def getIVcurve(name, moduleID, filename, IV_directory, sensor_mapping):
    plot = {'axis_titles': ['V', 'I']}
    bins = []
    vals = []
    if moduleID in sensor_mapping: 
        mapped_name = sensor_mapping[moduleID]
        
        wholeFilename = IV_directory + "/" + filename
        # Loop over the file.
        f = open(wholeFilename, 'r')
        i=0
        turned = False
        prevV = None
        for line in f:
            if i==0: 
                i+=1
                continue
            
            lineBits = line.split()
            V = float(lineBits[2])
            if mapped_name not in lineBits[0]: continue
            if prevV != None:
                if V<prevV: turned = True
            prevV = V
            if name == 'Ramp Up':
                if turned: continue
            else:
                if turned==False: continue
            
            bins.append([V, V])
            vals.append(float(lineBits[3]))
            i += 1
        
    plot['data'] = {'data': {'binning': bins, 'values': vals}}
    return plot
    