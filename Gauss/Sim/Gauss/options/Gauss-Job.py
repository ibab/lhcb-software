#
# Options specific for a given job
# ie. setting of random number seed and name of output files
#

from Gauss.Configuration import *

#--Generator phase, set random numbers
GaussGen = GenInit("GaussGen")
GaussGen.FirstEventNumber = 1
GaussGen.RunNumber        = 1082

#--Number of events
nEvts = 5
ApplicationMgr( EvtMax     = nEvts )

#--Set name of output files for given job and read in options
idFile = 'GaussExample'
HistogramPersistencySvc().OutputFile = idFile+'-histos.root'

tape = OutputStream("GaussTape")
tape.Output = "DATAFILE='PFN:%s.sim' TYP='POOL_ROOTTREE' OPT='RECREATE'"%idFile
ApplicationMgr( OutStream = [tape] )
