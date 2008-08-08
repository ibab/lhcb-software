## @file 
 #  Test file for HLT background category
 #
 #  @author P. Koppenburg
 #  @date 2007-08-08
 #
##--------------------------------------------------------------
from Gaudi.Configuration import *
from Configurables import HltBackgroundCategory

importOptions ( "$HLTSELECTIONSROOT/options/DVTestHlt2.py" )

ApplicationMgr().TopAlg += [ HltBackgroundCategory() ] 
HltBackgroundCategory().PrintTree = TRUE

importOptions ( "$HLTSELECTIONSROOT/options/MinimumBiasDst.py" )

ApplicationMgr().EvtMax = 10000 
EventSelector().PrintFreq = 100 


