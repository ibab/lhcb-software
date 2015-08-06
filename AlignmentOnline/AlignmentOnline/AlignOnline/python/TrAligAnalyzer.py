"""
     Run Brunel in the online environment

     @author M.Frank
"""
__version__ = "$Id: BrunelOnline.py,v 1.25 2010/11/09 12:20:55 frankb Exp $"
__author__  = "Markus Frank <Markus.Frank@cern.ch>"

import os, sys, time
import Configurables as Configs
import Gaudi.Configuration as Gaudi
import GaudiKernel
from GaudiKernel.ProcessJobOptions import PrintOff,InstallRootLoggingHandler,logging
from Configurables import CondDB, GaudiSequencer, EventPersistencySvc, \
    HistogramPersistencySvc, EventLoopMgr, OutputStream, Gaudi__SerializeCnvSvc, \
    DstConf

#PrintOff(999)
InstallRootLoggingHandler(level=logging.CRITICAL)

processingType ='DataTaking'

GaudiKernel.ProcessJobOptions._parser._parse_units(os.path.expandvars("$STDOPTS/units.opts"))
requirement = None

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
from TAlignment.AlignmentScenarios import *
from TAlignment.TrackSelections import *
from TAlignment.AlignmentScenarios import *
from TrAlignCommon import *

class fakeEventTime(object):
  def __init__(self, initialTime):
    self.__initialTime = initialTime

  def __call__(self):
    from Configurables import EventClockSvc, FakeEventTime
    clkSvc = EventClockSvc()
    clkSvc.EventTimeDecoder = "FakeEventTime"
    clkSvc.addTool(FakeEventTime, "FakeEventTime")
    clkSvc.FakeEventTime.StartTime = self.__initialTime + 1

    from Configurables import GaudiSequencer, createODIN
    initSeq = GaudiSequencer("InitEscherSeq")
    initSeq.Members.insert(0, createODIN())

def patchEscher(true_online_version, alignment_module, n = -1):
  import GaudiConf.DstConf
  import Escher.Configuration
  from Configurables import MagneticFieldSvc
  from Configurables import TAlignment
  from TAlignment.VertexSelections import configuredPVSelection
  Online = importOnline()

  from Configurables import EventClockSvc
  initialTime = long(time.time() * 1e9)
  clkSvc = EventClockSvc()
  clkSvc.InitialTime = initialTime

  from Gaudi.Configuration import appendPostConfigAction
  appendPostConfigAction(fakeEventTime(initialTime))

  TAlignment().RunList=Online.DeferredRuns if hasattr(Online, "DeferredRuns") else []
  sys.stdout.flush()
  sys.stderr.flush()
  escher = EscherCommon(true_online_version, alignment_module)
  hostname = HostName()
  escher.InputType  = "MDF"
  escher.PrintFreq = 10000
#   escher.EvtMax=300
  asddir = Online.ASDDir + "/" if hasattr(Online, "ASDDir") else "/group/online/alignment/EscherOut/"
  if n == -1:
    suffix = "_Escher.out"
  else:
    suffix = ("_%02d_Escher.out" % n)
  TAlignment().OutputDataFile = asddir + hostname + suffix
  TAlignment().UpdateInFinalize = False
  return escher

def setupOnline(directory, prefix, filename):
  """
        Setup the online environment: Buffer managers, event serialisation, etc.

        @author M.Frank
  """
  class __MonAdd:
    def __init__(self):
      pass
    def addMonitors(self):
      GaudiSequencer("EscherSequencer").Members += ["HistogramResetter"]

  from Configurables import LHCb__FILEEvtSelector as es
  Online = importOnline()

  app=Gaudi.ApplicationMgr()
  app.AppName = ''
  HistogramPersistencySvc().OutputFile = ""
  app.HistogramPersistency = ''
  app.SvcOptMapping.append('LHCb::FILEEvtSelector/EventSelector')
  app.SvcOptMapping.append('LHCb::FmcMessageSvc/MessageSvc')
  app.ExtSvc.insert(0,"MonitorSvc")
  sel = es("EventSelector")
  sel.Input = filename
  sel.Directory = directory
  sel.OutputLevel = MSG_INFO
  sel.DeleteFiles = False;
  sel.FilePrefix = prefix
  sel.Decode = False
  sel.Pause = True
  sel.PauseSleep = 5000
  sel.AllowedRuns = Online.DeferredRuns if hasattr(Online, "DeferredRuns") else []
  app.EvtSel  = sel
  app.EvtMax = -1
  Online.rawPersistencySvc()
  evtloop = Configs.EventLoopMgr('EventLoopMgr')
  evtloop.Warnings = False
  evtloop.EvtSel = sel
  app.EventLoop = evtloop
  runable = Configs.LHCb__EventRunable("Runable")
  runable.MEPManager        = ""
  app.Runable = runable
  app.AuditAlgorithms = False
  Configs.MonitorSvc().OutputLevel = MSG_ERROR
  Configs.MonitorSvc().UniqueServiceNames = 1
  Configs.MonitorSvc().CounterUpdateInterval = -1
  Configs.MonitorSvc().DimUpdateInterval = -1
  Configs.RootHistCnv__PersSvc("RootHistSvc").OutputLevel = MSG_ERROR
  mon = __MonAdd()
  Gaudi.appendPostConfigAction(mon.addMonitors)
  app.OutputLevel = MSG_INFO

  # Disable run check handler otherwise complain that database not up to date
  cdb = CondDB()
  cdb.EnableRunStampCheck = False
  # print sel

#============================================================================================================
def patchMessages():
  """
        Messages in the online get redirected.
        Setup here the FMC message service

        @author M.Frank
  """
  Online = importOnline()

  app=Gaudi.ApplicationMgr()
  Configs.AuditorSvc().Auditors = []
  app.MessageSvcType = 'LHCb::FmcMessageSvc'
  if Gaudi.allConfigurables.has_key('MessageSvc'):
    del Gaudi.allConfigurables['MessageSvc']
  msg = Configs.LHCb__FmcMessageSvc('MessageSvc')
  msg.fifoPath      = os.environ['LOGFIFO']
  msg.LoggerOnly    = True
  msg.doPrintAlways = False
  msg.OutputLevel   = MSG_INFO # Online.OutputLevel

#============================================================================================================
def start():
  """
        Finish configuration and configure Gaudi application manager

        @author M.Frank
  """
  Online = importOnline()
#   Online.end_config(True)
  Online.end_config(False)

#============================================================================================================
def getProcessingType():
  import os
  Online = importOnline()

  if (hasattr(Online,'ActivityType') and getattr(Online,'ActivityType') == 'Reprocessing'):
    return 'Reprocessing'
  if os.environ.has_key('Standalone_test'):
    return 'Reprocessing'
  return 'DataTaking'

def doIt(filename = "/localdisk/Run_112181_0000000182.raw",
         alignment_module = "VeloHalfAlignment", n = -1):
  true_online = ('LOGFIFO' in os.environ and 'RUNINFO' in os.environ)
  debug = not true_online

  if not true_online:
    print '\n            Running terminal version 1.1 of ESCHER ONLINE\n\n'
    requirement = "EvType=2;TriggerMask=0x0,0x4,0x0,0x0;VetoMask=0,0,0,0x300;MaskType=ANY;UserType=VIP;Frequency=PERC;Perc=100.0"

  br = patchEscher(true_online, alignment_module, n)
  if os.path.isdir(filename):
    directory = filename
    prefix = 'Run_'
  else:
    directory = os.path.dirname(filename)
    prefix = os.path.basename(filename)[:4]

  setupOnline(directory, prefix, filename)
  if true_online: patchMessages()
  start()
