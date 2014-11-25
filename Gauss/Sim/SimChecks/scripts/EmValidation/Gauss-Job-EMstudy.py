#########################################################
#	Job file for EM physics studies and validation		#
#	Useful configurables set in Emstudy_config.py		#
#			Peter Griffith 21.11.14						#
#########################################################
import sys
sys.path.append('./')
from Gauss.Configuration import *
from Configurables import CondDB, LHCbApp, DDDBConf, CondDBAccessSvc
from Configurables import Gauss
from EMstudy_config import config

#importOptions("EMstudy_config.py")
opts = config()

#saveSim = config()
#testType = config()
#nEvts = config()
saveSim = opts['saveSim']
testType = opts['testType']
nEvts = opts['nEvts']

Gauss().Production = 'PGUN'


#--Generator phase, set random numbers
GaussGen = GenInit("GaussGen")
GaussGen.FirstEventNumber = 1
GaussGen.RunNumber        = 4585

LHCbApp().EvtMax = nEvts
Gauss().Production='PGUN'

Gauss().OutputType = 'NONE'

if (saveSim):
	tape = OutputStream("GaussTape")
	tape.Output = "DATAFILE='PFN:myOutputFile.sim' TYP='POOL_ROOTTREE' OPT='RECREATE'"
	ApplicationMgr( OutStream = [tape] )

#to avoid decimal points in output file, energy converted to MeV
e_in_mev = float(opts['pgunE'])
#*1000

histosName = 'emstudy_'+str(opts['emPL'])+'_'+str(opts['pgunID'])+'_'+str(e_in_mev)+'_'+str(opts['veloType'])+'_'+str(opts['testType'])+'-histos.root'

HistogramPersistencySvc().OutputFile = histosName
ApplicationMgr().ExtSvc += [ "NTupleSvc" ]
NTupleSvc().Output = ["FILE1 DATAFILE='testout.root' TYP='ROOT' OPT='NEW'"]

importOptions ("./EMstudy_PGun.py")

if (testType == 'dedx' or testType == 'both'):
	from Configurables import EMGaussMoni
	SimMonitor = GaudiSequencer( "SimMonitor" )
	SimMonitor.Members += [ EMGaussMoni( "VeloGaussMoni") ]

	
