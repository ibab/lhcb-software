"""
     Run Brunel in the online environment

     @author M.Frank
"""
__version__ = "$Id: BrunelOnline.py,v 1.25 2010/11/09 12:20:55 frankb Exp $"
__author__  = "Markus Frank <Markus.Frank@cern.ch>"

import os, sys, re
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
def EscherCommon(true_online_version):
  import GaudiConf.DstConf
  import Escher.Configuration
  import OnlineEnv as Online
  from Configurables import MagneticFieldSvc
  from Configurables import TAlignment
  from TAlignment.VertexSelections import configuredPVSelection
  from Configurables import RunChangeHandlerSvc
  escher = Escher.Configuration.Escher()
  import ConditionsMap
  escher.DDDBtag   = ConditionsMap.DDDBTag
  escher.CondDBtag = ConditionsMap.CondDBTag
  escher.OnlineMode = True
  if hasattr(Online, "AlignXmlDir"):
    escher.OnlineAligWorkDir = os.path.join(Online.AlignXmlDir, 'running')

  handlerConditions = ConditionsMap.RunChangeHandlerConditions
  if true_online_version and os.environ['PARTITION'] == 'TEST':
    re_year = re.compile('(201\d)')
    for k, v in handlerConditions.items():
      handlerConditions[re_year.sub('2014', k)] = v
    
  conddb = CondDB()
  conddb.RunChangeHandlerConditions = handlerConditions
  conddb.IgnoreHeartBeat = True
  conddb.setProp('EnableRunChangeHandler', True)
  conddb.UseDBSnapshot=True
  conddb.DBSnapshotDirectory = "/group/online/hlt/conditions/"
#  conddb.PartitionName = "LHCb"
  conddb.Tags = { "DDDB" : ConditionsMap.DDDBTag,
                  "LHCBCOND" : ConditionsMap.CondDBTag,
                  "ONLINE" : 'fake'}
  MagneticFieldSvc().UseSetCurrent = True
  escher.UseDBSnapshot = True
  configure2012DataAlignment()
  configureVeloHalfAlignment()
  ## TAlignment().OnlineMode = True
  print conddb
  return escher

def HostName():
  import socket
  hostname = socket.gethostname()
  return hostname
