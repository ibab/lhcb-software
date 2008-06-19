# import everything ! from ROOT
from ROOT import *

#import gaudi stuff
import GaudiPython
from GaudiPython import *

#gaudimodule.gbl.makeClass('GAlign')
GaudiPython.loaddict('TAlignmentDict')
gaudi= GaudiPython.AppMgr(outputlevel = 3, joboptions='$ESCHEROPTS/v200601.opts')
incSvc = gaudi.service('IncidentSvc', 'IIncidentSvc' )
glofit = gbl.Incident('GlobalFit', 'GlobalMPedeFit' )
converged = gbl.Incident('Converged', 'Converged' )
#take alialg from list of topalgs
gaudi.algorithm('Align').Enable=0

#initialize the AppMgr
gaudi.initialize()

align = GaudiPython.Helper.algorithm( gaudi._algmgr,  'Align')

# change some properties in choosen options files
gaudi.config( files = [ '../options/v200601.opts',
                        '../options/Alignment.opts'],
              options = [ 'ApplicationMgr.EvtMax = 1000', #should be the same as 'events_maxIter' !
                          'Align.evtsPerRun = 1000',  #should be the same as 'EvtMax' !
                          'Align.MaxIterations = 3'] )

# get some properties...
# max no events for one detector alignment
jobopt = gaudi.optSvc();
max_evt = jobopt.getProperty( 'ApplicationMgr' , 'EvtMax' )
max_iter = jobopt.getProperty( 'Align','MaxIterations')
force_iter = jobopt.getProperty( 'Align', 'forceIterations' )
print "Maximum number of iterations ", max_iter, " Force iterations ? : ", force_iter


# get event selector
sel = gaudi.evtsel()
#sel.open(['PFN:/lhcb/production/DC06/phys-lumi2/00001540/DST/0000/00001540_00000001_5.dst',
#          'PFN:/lhcb/production/DC06/phys-lumi2/00001540/DST/0000/00001540_00000002_5.dst'])

#initialize Align=GAlign Algo

gaudi.algorithm('Align').initialize() 
#align.initialize()


for runs in range(max_iter):

    print "___ run ", runs, " -----"

    for i in range(max_evt):
        gaudi.run(1)
        gaudi.algorithm('Align').execute()

    print "Doing global fit!"
    incSvc.fireIncident(glofit)
    print "Checking convergence..."
    conv = align.Converged()
    print "Converged ? ", conv
    if conv == 1 and force_iter == 0 :
        break
    sel.rewind()
        

gaudi.algorithm('Align').finalize()
