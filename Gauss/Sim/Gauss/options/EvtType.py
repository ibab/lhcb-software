#
# Example file to run Gauss setting the event type and number of events
# with a given geometry.
# 
# Syntax is:
#   gaudirun.py $GAUSSOPTS/lhcb-200802.py $GAUSSOPTS/EvtType.py
#
from Gaudi.Configuration import *
import time

#--Number of events, event type, and idfile based on date
nEvts = 5
#eventType = '11144103'
eventType = '30000000'
idFile = str(time.localtime().tm_year)
if time.localtime().tm_mon < 10:
    idFile += '0'
idFile += str(time.localtime().tm_mon)
if time.localtime().tm_mday < 10:
    idFile += '0'
idFile += str(time.localtime().tm_mday)
idFile = eventType+'-'+str(nEvts)+'ev-'+idFile

#--Number of events
ApplicationMgr( EvtMax     = nEvts )

#--Options for given event type
importOptions('$DECFILESROOT/options/'+eventType+'.opts')

#--Set name of output files for given job and read in options
HistogramPersistencySvc().OutputFile = idFile+'-histos.root'

tape = OutputStream("GaussTape")
tape.Output = "DATAFILE='PFN:%s.sim' TYP='POOL_ROOTTREE' OPT='RECREATE'"%idFile
ApplicationMgr( OutStream = [tape] )

