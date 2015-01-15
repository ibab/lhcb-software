"""
Scripts to test offline, usage:
SetupAlignmentOnline
gaudirun.py runt0OnlineCalibration_test.py
"""

#from Configurables import GaudiSequencer
#OTt0OnlineClbrSeq = GaudiSequencer("OTt0OnlineClbrSeq")

from Configurables import OTt0OnlineClbr

#OTt0OnlineClbrAlg = OTt0OnlineClbrSeq("OTt0OnlineClbrAlg")

myFiles1 = [ "/afs/cern.ch/work/l/lgrillo/Brunel-107577-20120228T184550-EOR.root" ]

OTt0OnlineClbr.InputFiles  = myFiles1
OTt0OnlineClbr.xmlFilePath = "/tmp/lgrillo" 


#OTt0OnlineClbrSeq.Members += [
#    OTt0OnlineClbrAlg
#    ]

#OTt0OnlineClbrAlgSeq.IgnoreFilterPassed = True

#OTt0OnlineClbrAlgAlg.OutputLevel = 3

from Configurables import ApplicationMgr
ApplicationMgr().TopAlg            += [ OTt0OnlineClbr ]

from Configurables import LHCbApp
#LHCbApp().DDDBtag   = "head-20120413"
#LHCbApp().CondDBtag = "cond-20120730"
LHCbApp().DDDBtag   = "default"                                                                                                           
LHCbApp().CondDBtag = "default"
