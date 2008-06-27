### @file 
 #  Test file for HLT statistics
 #
 #  @author P. Koppenburg
 #  @date 2008-06-24
 #
from Gaudi.Configuration import *
from Configurables import Hlt2Statistics

importOptions("$HLTSELECTIONSROOT/options/DVTestHlt.opts")


ApplicationMgr().TopAlg += [ "Hlt2Statistics" ]
# Hlt2Statistics().OutputLevel = 1 ;

# importOptions("$HLTSELECTIONSROOT/options/Bs2JpsiPhiDigi.opts")
# importOptions("$HLTSELECTIONSROOT/options/Bs2PhiPhiDigi.opts")
# importOptions("$HLTSELECTIONSROOT/options/B2DstarX.opts")
importOptions("$HLTSELECTIONSROOT/options/MinimumBiasPFN.opts")
# importOptions("$HLTSELECTIONSROOT/options/MinimumBiasRaw.opts")
# importOptions("$HLTSELECTIONSROOT/options/Bd2MuMuKstarPFN.opts")

ApplicationMgr().EvtMax = 1000

EventSelector().PrintFreq = 100 


