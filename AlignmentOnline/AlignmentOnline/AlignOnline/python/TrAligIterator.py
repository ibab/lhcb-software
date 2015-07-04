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

PrintOff(999)
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

MAX_NITER   = 20

configureBrunelOutput = None
from TAlignment.AlignmentScenarios import *
from TAlignment.TrackSelections import *
from TAlignment.AlignmentScenarios import *
from TrAlignCommon import *

def patchEscher(true_online_version, alignment_module):
  import GaudiConf.DstConf
  import Escher.Configuration
  from Configurables import MagneticFieldSvc
  from TAlignment.VertexSelections import configuredPVSelection
  from Configurables import GaudiSequencer

  escher = EscherCommon(true_online_version, alignment_module)
  TAlignment().NumIterations = MAX_NITER

  print escher
  return escher

def setupOnline():
  """
        Setup the online environment: Buffer managers, event serialisation, etc.

        @author M.Frank
  """
  from Configurables import LHCb__FILEEvtSelector as es
  from Configurables import LHCb__AlignDrv as Adrv
  from Configurables import EventClockSvc
  Online = importOnline()

  app=Gaudi.ApplicationMgr()
  app.AppName = ''
  app.HistogramPersistency = 'ROOT'
  app.SvcOptMapping.append('LHCb::FILEEvtSelector/EventSelector')
  app.SvcOptMapping.append('LHCb::FmcMessageSvc/MessageSvc')
#app.EvtMax = 10000
  Online.rawPersistencySvc()
  evtloop = Configs.EventLoopMgr('LHCb::OnlineRunable/EmptyEventLoop')
  evtloop.Warnings = False
  evtloop.EvtSel = "NONE"
  app.EventLoop = evtloop
  app.HistogramPersistency  = "NONE"
#   runable = Configs.Runable
#  runable.MEPManager        = ""
  app.AuditAlgorithms = False

  Configs.MonitorSvc().OutputLevel = MSG_ERROR
  Configs.MonitorSvc().UniqueServiceNames = 1
  Configs.RootHistCnv__PersSvc("RootHistSvc").OutputLevel = MSG_ERROR
  app.OutputLevel = MSG_INFO

  def __propAtt(att, fr, to, d = None):
    if hasattr(fr, att):
      setattr(to, att, getattr(fr, att))
    elif d:
      setattr(to, att, d)

  from Configurables import AlignOnlineIterator as Aiter
  ad = Adrv("AlignDrv")
  ad.PartitionName     = Online.PartitionName
  ad.FitterClass       = "AlignOnlineIterator"
  ad.FitterName        = "AlIterator"
  __propAtt('RefFileName', Online, ad)

    ## The Alignment driver is the runable
  app.Runable = ad.getType() + "/" + ad.getName()

  ad.addTool( Aiter, ad.FitterName )
  ai = ad.AlIterator
  ai.PartitionName  = Online.PartitionName
  ai.ASDFilePattern = "_Escher.out"
  ai.OutputLevel    = 3
  ai.MaxIteration   = MAX_NITER
  ai.ServiceInfix   = ""

  runType = os.environ.get('RUN_TYPE', 'Unknown')
  runType = runType.split('|')[-1].strip() if '|' in runType else runType
  if runType == 'Tracker':
    sds = ['TT', 'IT', 'OT']
  elif runType in ('Velo', 'Muon'):
    sds = [runType]
  else:
    print 'WARNING: RUN_TYPE is not one of Velo, Tracker or Muon. Will assume all subdetectors'
    sds = ['Velo', 'TT', 'IT', 'OT', 'Muon']
  ai.SubDetectors = sds

  for attr, default in [('ASDDir', "/group/online/alignment/EscherOut/"),
                        ('OnlineXmlDir', "/group/online/alignment"),
                        ('AlignXmlDir', "/group/online/AligWork")]:
    __propAtt(attr, Online, ai, default)

  initialTime = long(time.time() * 1e9)
  clkSvc = EventClockSvc()
  clkSvc.InitialTime = initialTime

  from Configurables import FakeEventTime
  clkSvc.EventTimeDecoder = "FakeEventTime"
  clkSvc.addTool(FakeEventTime, "FakeEventTime")
  clkSvc.FakeEventTime.StartTime = initialTime

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

def doIt(alignment_module = "VeloHalfAlignment"):
  true_online = ('LOGFIFO' in os.environ and 'RUNINFO' in os.environ)
  debug = not true_online

  if not true_online:
    print '\n            Running terminal version 1.1 of ESCHER ONLINE\n\n'
    requirement = "EvType=2;TriggerMask=0x0,0x4,0x0,0x0;VetoMask=0,0,0,0x300;MaskType=ANY;UserType=VIP;Frequency=PERC;Perc=100.0"

  br = patchEscher(true_online, alignment_module)
  setupOnline()
  if true_online: patchMessages()
  start()
