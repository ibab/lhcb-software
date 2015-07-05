"""
     Run Data Quality application in the online environment

     @author M.Frank
"""
__version__ = "$Id: DataQuality.py,v 1.25 2010/11/09 12:20:55 frankb Exp $"
__author__  = "Markus Frank <Markus.Frank@cern.ch>"

import os, sys
import Configurables as Configs
import Gaudi.Configuration as Gaudi
import GaudiKernel
from Configurables import GaudiSequencer, RichPIDQCConf
from Configurables import CondDB, EventPersistencySvc, HistogramPersistencySvc
from Configurables import EventLoopMgr
from GaudiKernel.ProcessJobOptions import PrintOff,InstallRootLoggingHandler,logging

#PrintOff(999)
InstallRootLoggingHandler(level=logging.CRITICAL)

GaudiKernel.ProcessJobOptions._parser._parse_units(os.path.expandvars("$STDOPTS/units.opts"))
GaudiKernel.ProcessJobOptions.printing_level=999
requirement = None

def dummy(*args,**kwd): pass

MSG_VERBOSE = 1
MSG_DEBUG   = 2
MSG_INFO    = 3
MSG_WARNING = 4
MSG_ERROR   = 5
MSG_FATAL   = 6
MSG_ALWAYS  = 7

def configureForking(appMgr):
  import OnlineEnv
  from Configurables import LHCb__CheckpointSvc
  numChildren = os.sysconf('SC_NPROCESSORS_ONLN')
  if os.environ.has_key('NBOFSLAVES'):
    numChildren = int(os.environ['NBOFSLAVES'])

  print '++++ configure Brunel for forking with ',numChildren,' children.'
  sys.stdout.flush()
  forker = LHCb__CheckpointSvc("CheckpointSvc")
  forker.NumberOfInstances   = numChildren
  forker.Partition           = OnlineEnv.PartitionName
  forker.TaskType            = os.environ['TASK_TYPE']
  forker.UseCores            = False
  forker.ChildSessions       = False
  forker.FirstChild          = 0
  # Sleep in [ms] for each child in batches of 10:
  forker.ChildSleep          = 500;
  forker.UtgidPattern        = "%UTGID%02d";
  forker.PrintLevel          = 3  # 1=MTCP_DEBUG 2=MTCP_INFO 3=MTCP_WARNING 4=MTCP_ERROR
  forker.OutputLevel         = 4  # 1=VERBOSE 2=DEBUG 3=INFO 4=WARNING 5=ERROR 6=FATAL
  appMgr.ExtSvc.insert(0,forker)
  sys.stdout.flush()

#============================================================================================================
def patchBrunel():
  """
        Instantiate the options to run Brunel with raw data

        @author M.Frank
  """
  import GaudiConf.DstConf
  import Brunel.Configuration
  import OnlineEnv

  brunel = Brunel.Configuration.Brunel()
  brunel.OnlineMode = True
  try:
    brunel.DDDBtag    = OnlineEnv.DDDBTag
  except:
    print "DDDBTag not found, use default"

  try:
    brunel.CondDBtag = OnlineEnv.CondDBTag
  except:
    print "CondDBTag not found, use default"


  ##print '[ERROR]', OnlineEnv.DDDBTag, OnlineEnv.CondDBTag

  conddb = CondDB()
  conddb.IgnoreHeartBeat = True
  #
  # Adjust to pickup the proper online conditions
  #
  import ConditionsMap
  conddb.setProp('RunChangeHandlerConditions', ConditionsMap.RunChangeHandlerConditions)
  conddb.setProp('EnableRunChangeHandler', True)

  brunel.DataType = "2015"
  brunel.UseDBSnapshot = True  # Try it
  brunel.WriteFSR = False     # This crashes Jaap's stuff

  conddb = CondDB()
  conddb.Online = True
  #
  # Adjust to pickup the proper online conditions from ConditionsMap
  #
  conddb.RunChangeHandlerConditions = ConditionsMap.RunChangeHandlerConditions
  conddb.setProp('EnableRunChangeHandler', True)

  # Enabled data-on-demand
  Gaudi.ApplicationMgr().ExtSvc += [ "DataOnDemandSvc" ]

  # The sequencer to run all the PID monitoring in
  seq = GaudiSequencer("PIDMoniSeq")

  # Set up PID monitoring sequence
  pidSeq = GaudiSequencer("RichPIDSelections")
  seq.Members += [ pidSeq ]

  # Set options
  brunel.setOtherProps( RichPIDQCConf(), ['OutputLevel','Context'] )
  RichPIDQCConf().setProp("CalibSequencer",pidSeq)

  class __MonAdd:
    def __init__(self,s):
      self.seq = s
    def addMonitors(self):
      # Append to processing
      GaudiSequencer("PhysicsSeq").Members += [ self.seq ]

  mon = __MonAdd(seq)
  Gaudi.appendPostConfigAction(mon.addMonitors)

  EventLoopMgr().OutputLevel = MSG_DEBUG #ERROR
  EventLoopMgr().Warnings    = False

  brunel.UseDBSnapshot = True     # try it
  Brunel.Configuration.Brunel.configureOutput = dummy
  HistogramPersistencySvc().OutputFile = ""
  HistogramPersistencySvc().OutputLevel = MSG_ERROR
  print brunel
  return brunel

#============================================================================================================
def setupOnline():
  """
        Setup the online environment: Buffer managers, event serialisation, etc.

        @author M.Frank
  """
  import OnlineEnv

  buffs = ['Events']
  sys.stdout.flush()
  app=Gaudi.ApplicationMgr()
  app.AppName = ''
  app.HistogramPersistency = 'ROOT'
  app.SvcOptMapping.append('LHCb::OnlineEvtSelector/EventSelector')
  app.SvcOptMapping.append('LHCb::FmcMessageSvc/MessageSvc')
  mep = OnlineEnv.mepManager(OnlineEnv.PartitionID,OnlineEnv.PartitionName,buffs,True)
  mep.ConnectWhen = "start";
  sel = OnlineEnv.mbmSelector(input=buffs[0],type='ONE',decode=False,event_type=2)
  sel.REQ1 = "EvType=2;TriggerMask=0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF;VetoMask=0,0,0,0;MaskType=ANY;UserType=ONE;Frequency=PERC;Perc=100.0"
  app.EvtSel  = sel
  app.Runable = OnlineEnv.evtRunable(mep)
  app.Runable.NumErrorToStop = -1;
  app.ExtSvc.append(mep)
  app.ExtSvc.append(sel)
  app.AuditAlgorithms = False
  app.TopAlg.insert(0,"UpdateAndReset")
  Configs.MonitorSvc().OutputLevel = MSG_ERROR
  Configs.MonitorSvc().UniqueServiceNames = 1
  Configs.RootHistCnv__PersSvc("RootHistSvc").OutputLevel = MSG_ERROR
  app.OutputLevel = OnlineEnv.OutputLevel
  configureForking(app)
  sys.stdout.flush()

#============================================================================================================
def patchMessages():
  """
        Messages in the online get redirected.
        Setup here the FMC message service

        @author M.Frank
  """
  import OnlineEnv
  app=Gaudi.ApplicationMgr()
  Configs.AuditorSvc().Auditors = []
  app.MessageSvcType = 'LHCb::FmcMessageSvc'
  if Gaudi.allConfigurables.has_key('MessageSvc'):
    del Gaudi.allConfigurables['MessageSvc']
  msg = Configs.LHCb__FmcMessageSvc('MessageSvc')
  msg.fifoPath      = os.environ['LOGFIFO']
  msg.LoggerOnly    = True
  msg.doPrintAlways = False
  msg.OutputLevel   = OnlineEnv.OutputLevel

#============================================================================================================
def start():
  """
        Finish configuration and configure Gaudi application manager

        @author M.Frank
  """
  import OnlineEnv
  OnlineEnv.end_config(False)
  #OnlineEnv.end_config(True)

def run():
  try:
    br = patchBrunel()
    setupOnline()
    patchMessages()
    start()
  except Exception,X:
    print '[ERROR] Exception:',str(X)
    sys.exit(1)
