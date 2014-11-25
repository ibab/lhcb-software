#############################################
#	 EM physics study and validation		#
#   with EMGaussMoni and BremVeloCheck		#
#											#
#	Detector and tool config configuration	#
#	     Peter Griffith 21.11.14			#
#############################################

# imports and functions
####################################################################################

import sys
sys.path.append('./')
from Configurables import Gauss
from Gauss.Configuration import *

from Gaudi.Configuration import *
from Configurables import LHCbApp
from Configurables import CondDB
from Configurables import CondDBAccessSvc
from EMstudy_config import config

importOptions("./EMstudy_config.py")
#emPL = config()
#veloType = config()
#testType = config()
#dRays = config()
opts = config()
veloType = opts['veloType']
testType = opts['testType']
dRays = opts['dRays']
emPL = opts['emPL']



#Get physics list from temp. file as this depends on whether PL scan is activated or not
print 'EM physics list set in config: '+str(emPL)




def switchOffRICHCuts():
	from Configurables import SimulationSvc
	SimulationSvc().SimulationDbLocation = "$GAUSSROOT/xml/SimulationRICHesOff.xml"

def addTool():
    from Configurables import GiGa, BremVeloCheck
    giga = GiGa()
    giga.TrackSeq.Members.append( "BremVeloCheck" )
####################################################################################

if (veloType == 'velo'):
	LHCbApp().DDDBtag   = "dddb-20130312-1"
	LHCbApp().CondDBtag = "sim-20130222-1-vc-md100"

	Gauss.DetectorGeo = {"Detectors": ["PuVeto","Velo","Rich1","Rich2"] }
	Gauss.DetectorSim = {"Detectors": ["PuVeto","Velo","Rich1","Rich2"] }
	Gauss.DetectorMoni ={"Detectors": ["PuVeto","Velo","Rich1","Rich2"] }
	importOptions ("$GAUSSOPTS/RICHesOff.py"  )


	appendPostConfigAction( switchOffRICHCuts )


Gauss.PhysicsList = {"Em":emPL, "Hadron":'QGSP_BERT', "GeneralPhys":True, "LHCbPhys":False}

from Configurables import SimulationSvc
Gauss.DeltaRays = dRays


if (testType == 'brem' or testType == 'both'):
	appendPostConfigAction(addTool)
