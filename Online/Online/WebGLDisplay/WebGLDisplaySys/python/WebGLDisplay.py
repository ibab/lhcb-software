"""
     Run Brunel in the online environment

     @author M.Frank
"""
__version__ = "$Id: BrunelOnline.py,v 1.25 2010/11/09 12:20:55 frankb Exp $"
__author__  = "Markus Frank <Markus.Frank@cern.ch>"

import os, sys
import Configurables as Configs
import Gaudi.Configuration as Gaudi
import GaudiKernel
from   GaudiKernel.ProcessJobOptions import PrintOff,InstallRootLoggingHandler,logging
from   Configurables import CondDB, GaudiSequencer, HistogramPersistencySvc, EventLoopMgr

#PrintOff(999)
InstallRootLoggingHandler(level=logging.CRITICAL)
GaudiKernel.ProcessJobOptions._parser._parse_units(os.path.expandvars("$STDOPTS/units.opts"))

debug = 0
def dummy(*args,**kwd): pass

MSG_VERBOSE = 1
MSG_DEBUG   = 2
MSG_INFO    = 3
MSG_WARNING = 4
MSG_ERROR   = 5
MSG_FATAL   = 6
MSG_ALWAYS  = 7

configureBrunelOutput = None

#============================================================================================================
def patchBrunel(true_online_version):
  """
        Instantiate the options to run Brunel with raw data

        @author M.Frank
  """
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

  conddb = CondDB()
  conddb.IgnoreHeartBeat = True
  #
  # Adjust to pickup the proper online conditions
  #
  import Online as RunChange_All
  conddb.setProp('RunChangeHandlerConditions', RunChange_All.ConditionMap)
  conddb.setProp('EnableRunChangeHandler', True)
  #
  # Brunel output configuration
  #
  brunel.WriteFSR   = False # This crashes Jaap's stuff
  brunel.DataType   = "2013"
  brunel.OutputType = ''

  EventLoopMgr().OutputLevel = MSG_DEBUG #ERROR
  EventLoopMgr().Warnings    = False

  from Configurables import EventClockSvc
  EventClockSvc().InitialTime = 1322701200000000000

  brunel.UseDBSnapshot = True     # try it
  #  brunel.PartitionName = "FEST"
  # Hack by Chris
  print "# Warning using CKThetaQuartzRefractCorrections = [ 0,-0.0001,0 ]"
  from Configurables import RichRecSysConf
  rConf = RichRecSysConf("RichOfflineRec")
  rConf.richTools().photonReco().CKThetaQuartzRefractCorrections = [ 0,-0.001,0 ]
  brunel.OutputLevel       = MSG_WARNING
  brunel.PrintFreq         = -1
  HistogramPersistencySvc().OutputFile = ""
  HistogramPersistencySvc().OutputLevel = MSG_ERROR

  #print brunel
  return brunel

#============================================================================================================
def setupOnline():
  """
        Setup the online environment: Buffer managers, event serialisation, etc.

        @author M.Frank
  """
  import OnlineEnv

  buffs = ['Events']
  app=Gaudi.ApplicationMgr()
  app.AppName = ''
  app.HistogramPersistency = 'ROOT'
  app.SvcOptMapping.append('LHCb::OnlineEvtSelector/EventSelector')
  app.SvcOptMapping.append('LHCb::FmcMessageSvc/MessageSvc')
  mep = OnlineEnv.mepManager(OnlineEnv.PartitionID,OnlineEnv.PartitionName,buffs,True)
  mep.ConnectWhen = "start";
  sel = OnlineEnv.mbmSelector(input=buffs[0],type='USER',decode=False,event_type=2)
  app.EvtSel  = sel
  app.Runable = OnlineEnv.evtRunable(mep)
  app.Runable.NumErrorToStop = -1;
  app.ExtSvc.append(mep)
  app.ExtSvc.append(sel)
  app.AuditAlgorithms = False
  ##app.TopAlg.insert(0,"UpdateAndReset")
  Configs.MonitorSvc().OutputLevel = MSG_ERROR
  Configs.MonitorSvc().UniqueServiceNames = 1
  Configs.RootHistCnv__PersSvc("RootHistSvc").OutputLevel = MSG_ERROR
  app.OutputLevel = MSG_INFO

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
def postConfig():
  import OnlineEnv
  from Configurables import JsonConverter, NoPIDsParticleMaker, LHCb__DelaySleepAlg
  pmaker = NoPIDsParticleMaker ( 'StdAllNoPIDsPions' , Particle = 'pion'  )
  conv  = JsonConverter()
  conv.OutputLevel = MSG_DEBUG
  conv.OutputDirectory = '/group/online/dataflow/cmtuser/EventDisplay/events'
  delay = LHCb__DelaySleepAlg('Delay')
  delay.DelayTime = 1000 # Sleep after each event for 1000 milli-secs
  GaudiSequencer("PhysicsSeq").Members += [ pmaker, conv, delay ]

#============================================================================================================
def start():
  """
        Finish configuration and configure Gaudi application manager

        @author M.Frank
  """
  import OnlineEnv
  Gaudi.appendPostConfigAction(postConfig)
  OnlineEnv.end_config(False)
  #OnlineEnv.end_config(True)

try:
  br = patchBrunel(True)
  setupOnline()
  patchMessages()
  start()

except Exception,X:
  print 'ERROR', X
  raise
