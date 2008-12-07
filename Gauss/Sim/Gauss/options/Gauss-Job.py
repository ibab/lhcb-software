#
# Options specific for a given job
# ie. setting of random number seed and name of output files
#

from Gauss.Configuration import *
import time

#--Generator phase, set random numbers
GaussGen = GenInit("GaussGen")
GaussGen.FirstEventNumber = 1
GaussGen.RunNumber        = 83

## Generator = GaudiSequencer("Generator")
## Generator.Members  = [GaussGen]

## Generator.Members += ["Generation"]
## Generator.Members += ["GaudiSequencer/GenMonitor"]

#--Number of events
nEvts = 5
ApplicationMgr( EvtMax     = nEvts )

#--Event type
eventType = '11144101'
#eventType = '51000005'
#eventType = '60001008'
#eventType = '60040000'
importOptions('$DECFILESROOT/options/'+eventType+'.opts')

#--Set name of output files for given job and read in options
idFile = str(time.localtime().tm_year)
if time.localtime().tm_mon < 10:
    idFile += '0'
idFile += str(time.localtime().tm_mon)
if time.localtime().tm_mday < 10:
    idFile += '0'
idFile += str(time.localtime().tm_mday)
idFile = eventType+'-'+str(nEvts)+'ev-'+idFile

HistogramPersistencySvc().OutputFile = idFile+'-histos.root'

tape = OutputStream("GaussTape")
tape.Output = "DATAFILE='PFN:%s.sim' TYP='POOL_ROOTTREE' OPT='RECREATE'"%idFile
ApplicationMgr( OutStream = [tape] )

print idFile
