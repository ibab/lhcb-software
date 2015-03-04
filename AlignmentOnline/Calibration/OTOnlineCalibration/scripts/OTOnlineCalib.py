import os, Gaudi.Configuration
from Configurables import GaudiSequencer, ApplicationMgr, LHCb__FmcMessageSvc
from Configurables import OTt0OnlineClbr
import OnlineConfig as Online


MSG_VERBOSE = 1
MSG_DEBUG   = 2
MSG_INFO    = 3
MSG_WARNING = 4
MSG_ERROR   = 5
MSG_FATAL   = 6
MSG_ALWAYS  = 7

partition = os.environ['PARTITION'] if 'PARTITION' in os.environ else os.environ['PARTITION_NAME']

def setup():
  OTGaudiSeq = GaudiSequencer("OTt0OnlineClbrSeq")
  OTt0OnlineClbrAlg = OTt0OnlineClbr("OTt0OnlineClbrAlg")

  # Rich refrative index calibration job
  OTt0OnlineClbrAlg.InputTasks  = [ "Brunel" ]
  #OTt0OnlineClbrAlg.InputFiles  = [ "/hist/Savesets/2013/LHCb/Brunel/01/20/Brunel-135576-20130120T161302-EOR.root"]
  OTt0OnlineClbrAlg.xmlFilePath = "/group/online/alignment/"
  OTt0OnlineClbrAlg.Partition = partition
  
  OTGaudiSeq.Members += [
      OTt0OnlineClbrAlg,
      ]
  OTGaudiSeq.IgnoreFilterPassed = True

  # Output Level of both
  OTt0OnlineClbrAlg.OutputLevel = 3
  OTt0OnlineClbrAlg.StopAlgSequence = False

  ApplicationMgr().TopAlg            += [ OTGaudiSeq ]
  ApplicationMgr().EvtSel = "NONE"
  ApplicationMgr().ExtSvc += [ "LHCb::PublishSvc", "MonitorSvc" ]
  ApplicationMgr().Runable = "LHCb::OnlineRunable/Runable"

  from Configurables import LHCbApp
  #LHCbApp().DDDBtag   = "head-20120413"
  #LHCbApp().CondDBtag = "cond-20120730"
  LHCbApp().DDDBtag   = "default"
  LHCbApp().CondDBtag = "default"

  path_monolayer = "./db_patches/"
  ddbs = []
  ddbs.append(path_monolayer + "OTMonoGeometry.db/DDDB")
  ddbs.append(path_monolayer + "OTMonoCatalog.db/LHCBCOND")

  ddbs.append(path_monolayer + "OTMonoAlign20141225.db/LHCBCOND")
  ddbs.append(path_monolayer + "OTGeometryT0.db/DDDB")
  ddbs.append(path_monolayer + "OTCondT0.db/LHCBCOND")

  counter = 1
  for db in ddbs:
       from Configurables import ( CondDB, CondDBAccessSvc )
       alignCond = CondDBAccessSvc( 'AlignDDDB' + str(counter) )
       alignCond.ConnectionString = 'sqlite_file:' + db
       CondDB().addLayer( alignCond )
       counter += 1
  
  print '[WARN] .....OTOnlineCalib-fromJibo.py  setup finished....'

#============================================================================================================
def patchMessages():
  """
        Messages in the online get redirected.
        Setup here the FMC message service

        @author M.Frank
  """
  app=ApplicationMgr()
  #Configs.AuditorSvc().Auditors = []
  app.MessageSvcType = 'LHCb::FmcMessageSvc'
  if Gaudi.Configuration.allConfigurables.has_key('MessageSvc'):
    del Gaudi.Configuration.allConfigurables['MessageSvc']
  msg = LHCb__FmcMessageSvc('MessageSvc')
  msg.fifoPath      = os.environ['LOGFIFO']
  msg.LoggerOnly    = True
  msg.doPrintAlways = False
  msg.OutputLevel   = MSG_INFO # Online.OutputLevel

setup()
patchMessages()
Online.end_config(True)
