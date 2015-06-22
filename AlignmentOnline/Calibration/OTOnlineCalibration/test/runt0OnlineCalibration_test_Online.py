"""
Scripts to test offline, usage:
SetupAlignmentOnline
gaudirun.py runt0OnlineCalibration_test.py
"""

from Configurables import GaudiSequencer
from Configurables import OTt0OnlineClbr


OTGaudiSeq = GaudiSequencer("OTt0OnlineClbrSeq")

OTt0OnlineClbrAlg = OTt0OnlineClbr("OTt0OnlineClbrAlg")

#OTt0OnlineClbrAlg.InputTasks  = [ "Brunel" ]
#OTt0OnlineClbrAlg.xmlFilePath = "/group/online/alignment/" #<- this will be the right one
OTt0OnlineClbrAlg.XMLFilePath = "/tmp/lgrillo/"

OTt0OnlineClbrAlg.RunOnline = False
OTt0OnlineClbrAlg.StopAlgSequence = False

#initialTime = long(time.time() * 1e9)
initialTime = long(0.0 * 1e9)

OTt0OnlineClbrAlg.InitialTime = initialTime

OTt0OnlineClbrAlg.Threshold = 0.04
OTt0OnlineClbrAlg.MaxDifference = 2

myFiles1 = [ "Brunel-109.root" ]
#OTt0OnlineClbrAlg.InputFiles  = myFiles1
#OTt0OnlineClbrAlg.InputFilePath = "/afs/cern.ch/work/l/lgrillo/OT_CALIBRATION_files/"
#OTt0OnlineClbrAlg.InputFileName  = "clbr_hists_109.root"
OTt0OnlineClbrAlg.InputFileName  = "Brunel-109.root"
#OTt0OnlineClbrAlg.InputFilePath  = "/home/lgrillo/cmtuser/AlignmentOnlineDev_v10r4/Calibration/OTOnlineCalibration/test/"
#OTt0OnlineClbrAlg.InputFilePath  = "/afs/cern.ch/user/l/lgrillo/cmtuser/AlignmentOnlineDev_v10r4/Calibration/OTOnlineCalibration/test/"
OTt0OnlineClbrAlg.InputFilePath  = "/afs/cern.ch/user/l/lgrillo/cmtuser/AlignmentOnlineDev_v10r4/Calibration/OTOnlineCalibration/test/LHCb/Brunel/06/22/"
#OTt0OnlineClbrAlg.InputFileName_2d  = "histog_2d_109.root"


#OTt0OnlineClbrAlg.CondStructure = ""
#line below should be the default in the db. its default in the analysistask
#OTt0OnlineClbrAlg.CondStructure = "Conditions/OT/Calibration/" 

#OTt0OnlineClbrAlg.globalt0_threshold = 0.1


OTt0OnlineClbrAlg.OutputLevel = 2
#OTt0OnlineClbrAlg.Simulation = False 
#OTt0OnlineClbrAlg.Fit_module_contributions = False
OTt0OnlineClbrAlg.UseMean = False
OTt0OnlineClbrAlg.SaveFits = False

#simulation = OTt0OnlineClbrAlg.Simulation


OTGaudiSeq.Members += [
    OTt0OnlineClbrAlg
    ]

OTGaudiSeq.IgnoreFilterPassed = True



from Configurables import ApplicationMgr
ApplicationMgr().TopAlg            += [ OTGaudiSeq ]

ApplicationMgr().EvtSel = "NONE"
# ApplicationMgr().ExtSvc += [ "MonitorSvc" ]
# ApplicationMgr().Runable = "LHCb::OnlineRunable/Runable"

from Configurables import LHCbApp
#LHCbApp().DDDBtag   = "head-20120413"
#LHCbApp().CondDBtag = "cond-20120730"

#LHCbApp().Simulation = simulation

#if simulation : 
#    LHCbApp().DDDBtag   = "dddb-20120831"
#    LHCbApp().CondDBtag = "sim-20121025-vc-md100"
#else : 
LHCbApp().DDDBtag   = "default"                                                                                                           
LHCbApp().CondDBtag = "default"

import time
from Configurables import EventClockSvc
EventClockSvc().InitialTime = int((time.time() - 3600) * 1e9)

#databases from Wouter (the monolayer alignment will be in the
#default database)
# path_monolayer = "/afs/cern.ch/user/w/wouter/public/AlignDB/"
# ddbs = []
# ddbs.append(path_monolayer + "OTMonoGeometry.db/DDDB")
# ddbs.append(path_monolayer + "OTMonoCatalog.db/LHCBCOND")
# ddbs.append(path_monolayer + "OTMonoAlign20141225.db/LHCBCOND")

# ddbs.append(path_monolayer + "OTGeometryT0.db/DDDB")
# ddbs.append(path_monolayer + "OTCondT0.db/LHCBCOND")

# counter = 1
# for db in ddbs:
#      from Configurables import ( CondDB, CondDBAccessSvc )
#      alignCond = CondDBAccessSvc( 'AlignDDDB' + str(counter) )
#      alignCond.ConnectionString = 'sqlite_file:' + db
#      CondDB().addLayer( alignCond )
#      counter += 1

from GaudiPython.Bindings import AppMgr
gaudi = AppMgr()
gaudi.initialize()
