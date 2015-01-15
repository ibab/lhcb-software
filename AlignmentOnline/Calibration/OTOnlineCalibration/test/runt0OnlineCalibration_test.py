"""
Scripts to test offline, usage:
SetupAlignmentOnline
gaudirun.py runt0OnlineCalibration_test.py
"""

from Configurables import GaudiSequencer
from Configurables import OTt0OnlineClbr

OTGaudiSeq = GaudiSequencer("OTt0OnlineClbrSeq")

OTt0OnlineClbrAlg = OTt0OnlineClbr("OTt0OnlineClbrAlg")

myFiles1 = [ "/afs/cern.ch/work/f/fdettori/Brunel-107577-20120228T184550-EOR.root" ]

OTt0OnlineClbrAlg.InputFiles  = myFiles1
OTt0OnlineClbrAlg.xmlFilePath = "/tmp/fdettori"
OTt0OnlineClbrAlg.OutputLevel = 2


OTGaudiSeq.Members += [
    OTt0OnlineClbrAlg
    ]

OTGaudiSeq.IgnoreFilterPassed = True



from Configurables import ApplicationMgr
ApplicationMgr().TopAlg            += [ OTGaudiSeq ]

from Configurables import LHCbApp
#LHCbApp().DDDBtag   = "head-20120413"
#LHCbApp().CondDBtag = "cond-20120730"
LHCbApp().DDDBtag   = "default"                                                                                                           
LHCbApp().CondDBtag = "default"
