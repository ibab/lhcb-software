"""
Scripts to test offline, usage:
SetupAlignmentOnline
gaudirun.py runt0OnlineCalibration_test.py
"""

from Configurables import GaudiSequencer
from Configurables import OTt0OnlineClbr


OTGaudiSeq = GaudiSequencer("OTt0OnlineClbrSeq")

OTt0OnlineClbrAlg = OTt0OnlineClbr("OTt0OnlineClbrAlg")

#myFiles1 = [ "clbr_hists.root" ]
myFiles1 = [ "clbr_hists_109.root" ]

OTt0OnlineClbrAlg.InputFiles  = myFiles1

#OTt0OnlineClbrAlg.InputFileName  = "data_test1_1000ev_sanityNEWCODE_hists.root"
#OTt0OnlineClbrAlg.InputFileName  = "data_test1_1000ev_sanityOLDCODE_hists.root"
OTt0OnlineClbrAlg.InputFileName  = "clbr_hists_109.root"

#OTt0OnlineClbrAlg.InputFileName_2d  = "histog_2d_109_20.root"
OTt0OnlineClbrAlg.InputFileName_2d  = "histog_2d_109.root"

#OTt0OnlineClbrAlg.xmlFilePath = "/tmp/fdettori/"
OTt0OnlineClbrAlg.xmlFilePath = "/tmp/lgrillo/"
OTt0OnlineClbrAlg.OutputLevel = 2
OTt0OnlineClbrAlg.Simulation = False 
OTt0OnlineClbrAlg.Fit_module_contributions = False
OTt0OnlineClbrAlg.GetMean_instead_of_Fit = False
OTt0OnlineClbrAlg.Save_Fits = True

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
        
