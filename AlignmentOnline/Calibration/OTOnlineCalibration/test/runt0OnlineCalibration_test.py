"""
Scripts to test offline, usage:
SetupAlignmentOnline
gaudirun.py runt0OnlineCalibration_test.py
"""

from Configurables import GaudiSequencer
from Configurables import OTt0OnlineClbr


OTGaudiSeq = GaudiSequencer("OTt0OnlineClbrSeq")

OTt0OnlineClbrAlg = OTt0OnlineClbr("OTt0OnlineClbrAlg")

myFiles1 = [ "clbr_hists.root" ]

OTt0OnlineClbrAlg.InputFiles  = myFiles1
#OTt0OnlineClbrAlg.xmlFilePath = "/tmp/fdettori/"
OTt0OnlineClbrAlg.xmlFilePath = "/tmp/lgrillo/"
OTt0OnlineClbrAlg.OutputLevel = 2
OTt0OnlineClbrAlg.Simulation = False 

simulation = OTt0OnlineClbrAlg.Simulation



OTGaudiSeq.Members += [
    OTt0OnlineClbrAlg
    ]

OTGaudiSeq.IgnoreFilterPassed = True



from Configurables import ApplicationMgr
ApplicationMgr().TopAlg            += [ OTGaudiSeq ]

from Configurables import LHCbApp
#LHCbApp().DDDBtag   = "head-20120413"
#LHCbApp().CondDBtag = "cond-20120730"

LHCbApp().Simulation = simulation
if simulation : 
    LHCbApp().DDDBtag   = "dddb-20120831"
    LHCbApp().CondDBtag = "sim-20121025-vc-md100"
else : 
    LHCbApp().DDDBtag   = "default"                                                                                                           
    LHCbApp().CondDBtag = "default"
        
