#
# Options specific for a given job
# ie. setting of random number seed and name of output files
#
from Gauss.Configuration import *
from Configurables import CondDB, LHCbApp, DDDBConf, CondDBAccessSvc
from Configurables import Gauss

Gauss().Production = 'PGUN'

from Gauss.Configuration import *

#--Generator phase, set random numbers
GaussGen = GenInit("GaussGen")
GaussGen.FirstEventNumber = 1
GaussGen.RunNumber        = 1082

#--Number of events
nEvts=10
LHCbApp().EvtMax = nEvts
Gauss().Production='PGUN'

Gauss().OutputType = 'NONE'
#Gauss().Histograms = 'NONE'
#--Set name of output files for given job (uncomment the lines)
#  Note that if you do not set it Gauss will make a name based on event type,
#  number of events and the date
#idFile = 'Test43r3'
#HistogramPersistencySvc().OutputFile = idFile+'-histos.root'
#
#OutputStream("GaussTape").Output = "DATAFILE='PFN:%s.sim' TYP='POOL_ROOTTREE' OPT='RECREATE'"%idFile
# --- Save ntuple with hadronic cross section information
ApplicationMgr().ExtSvc += [ "NTupleSvc" ]
NTupleSvc().Output = ["FILE1 DATAFILE='Multi_Kplus_inAl_A.root' TYP='ROOT' OPT='NEW'"]

importOptions ("$LBPGUNSROOT/options/TargetMaterialGun.py")




#GenMonitor = GaudiSequencer( "GenMonitor" )
#SimMonitor = GaudiSequencer( "SimMonitor" )
#GenMonitor.Members += [ "GaussMonitor::CheckLifeTimeHepMC/HepMCLifeTime" ]
#SimMonitor.Members += [ "GaussMonitor::CheckLifeTimeMC/MCLifeTime" ]

