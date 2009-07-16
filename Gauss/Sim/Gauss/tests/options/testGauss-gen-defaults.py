#
# Options specific for a given job
# ie. setting of random number seed and name of output files
#

from Gauss.Configuration import *
import time

#--Generator phase, set random numbers
GaussGen = GenInit("GaussGen")
GaussGen.FirstEventNumber = 1
GaussGen.RunNumber        = 1082

#--Number of events
nEvts = 1000
LHCbApp().EvtMax = nEvts

#--Set the 'first part' of the output files names
fName = 'Gauss'

#--Event type can either be imported here (uncomment the following lines)
#  or given as additional argument of gaudirun.py
#  The 'first part' of the output files names can be overwritten
eventType = ''
#eventType = '30000000'
#eventType = '11144103'
if (eventType != '') :
    fName = fName+'-'+eventType 

#--Set name of output files for given job, based on number of events and
#  date, above the first part is the name of the application or in case
idFile = str(time.localtime().tm_year)
if time.localtime().tm_mon < 10:
    idFile += '0'
idFile += str(time.localtime().tm_mon)
if time.localtime().tm_mday < 10:
    idFile += '0'
idFile += str(time.localtime().tm_mday)
idFile = fName+'-'+str(nEvts)+'ev-'+idFile
print idFile

HistogramPersistencySvc().OutputFile = idFile+'-histos.root'

tape = OutputStream("GaussTape")
tape.Output = "DATAFILE='PFN:%s.sim' TYP='POOL_ROOTTREE' OPT='RECREATE'"%idFile


GenMonitor = GaudiSequencer( "GenMonitor" )
SimMonitor = GaudiSequencer( "SimMonitor" )
GenMonitor.Members += [ "GaussMonitor::CheckLifeTimeHepMC/Hepmclifetime" ]
SimMonitor.Members += [ "GaussMonitor::CheckLifeTimeMC/MCLifeTime" ]
