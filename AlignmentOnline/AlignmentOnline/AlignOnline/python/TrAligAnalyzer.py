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

def patchEscher(true_online_version):
  import GaudiConf.DstConf
  import Escher.Configuration
  import OnlineEnv as Online
  from Configurables import MagneticFieldSvc
  from Configurables import TAlignment
  from TAlignment.VertexSelections import configuredPVSelection
  escher = EscherCommon(true_online_version)
  hostname = HostName()
  escher.DataType = "2012"
  escher.InputType  = "MDF"
  escher.PrintFreq = 10000
#   escher.EvtMax=300
  TAlignment().OutputDataFile = "/group/online/alignment/EscherOut/"+hostname+"_Escher.out"
# specify the input to the alignment
  #from TAlignment.ParticleSelections import defaultHLTD0Selection
# specify what we actually align for
  configure2012DataAlignment()
  TAlignment().TrackSelections = [
#                                  NoPIDTracksFromHlt(),
                                 GoodLongTracks(),
                                 VeloOverlapTracks(),
                                 VeloBackwardTracks(),
                                 ITBoxOverlapTracks() ]

# add the default PV selection
  TAlignment().PVSelection = configuredPVSelection()
  TAlignment().UpdateInFinalize = False
# specify what we actually align for
  configureVeloHalfAlignment()
#   print TAlignment()
  print escher
  return escher

def setupOnline():
  """
        Setup the online environment: Buffer managers, event serialisation, etc.

        @author M.Frank
  """
  import OnlineEnv as Online
  from Configurables import LHCb__FILEEvtSelector as es
  app=Gaudi.ApplicationMgr()
  app.AppName = ''
  app.HistogramPersistency = 'ROOT'
  app.SvcOptMapping.append('LHCb::FILEEvtSelector/EventSelector')
  app.SvcOptMapping.append('LHCb::FmcMessageSvc/MessageSvc')
  sel = es("EventSelector")
  sel.Input = "/localdisk/Run_112181_0000000182.raw"
  sel.DeleteFiles = False;
  sel.Decode=False
  sel.Pause = True
  app.EvtSel  = sel
  app.EvtMax = 100
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
  Configs.RootHistCnv__PersSvc("RootHistSvc").OutputLevel = MSG_ERROR
  app.OutputLevel = MSG_INFO
  print sel

#============================================================================================================
def patchMessages():
  """
        Messages in the online get redirected.
        Setup here the FMC message service

        @author M.Frank
  """
  import OnlineEnv as Online
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
  import OnlineEnv as Online
  import sys
#   Online.end_config(True)
  Online.end_config(False)

#============================================================================================================
def getProcessingType():
  import os
  import OnlineEnv as Online

  if (hasattr(Online,'ActivityType') and getattr(Online,'ActivityType') == 'Reprocessing'):
    return 'Reprocessing'
  if os.environ.has_key('Standalone_test'):
    return 'Reprocessing'
  return 'DataTaking'

def doIt():
  true_online = os.environ.has_key('LOGFIFO') and os.environ.has_key('PARTITION')
  debug = not true_online

  if not true_online:
    print '\n            Running terminal version 1.1 of ESCHER ONLINE\n\n'
    requirement = "EvType=2;TriggerMask=0x0,0x4,0x0,0x0;VetoMask=0,0,0,0x300;MaskType=ANY;UserType=VIP;Frequency=PERC;Perc=100.0"

  br = patchEscher(true_online)
  setupOnline()
  if true_online: patchMessages()
  start()
doIt()
