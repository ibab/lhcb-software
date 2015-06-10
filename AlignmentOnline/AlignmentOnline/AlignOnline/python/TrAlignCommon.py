"""
     Run Brunel in the online environment

     @author M.Frank
"""
__version__ = "$Id: BrunelOnline.py,v 1.25 2010/11/09 12:20:55 frankb Exp $"
__author__  = "Markus Frank <Markus.Frank@cern.ch>"

import importlib
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

def staticVar(varname, value):
    def decorate(func):
        setattr(func, varname, value)
        return func
    return decorate

@staticVar("Online", None)
def importOnline():
  if importOnline.Online:
    return importOnline.Online
  print "RUNINFO in environment: %s" % os.environ.get("RUNINFO", "False")
  if 'RUNINFO' in os.environ:
    runinfo = os.path.dirname(os.environ['RUNINFO'])
    sys.path.insert(1, runinfo)
    Online = importlib.import_module('OnlineEnv')
    sys.path.remove(runinfo)
  else:
    import OnlineEnv as Online
  importOnline.Online = Online
  return Online

def EscherCommon(true_online_version, alignment_module):
  import GaudiConf.DstConf
  import Escher.Configuration
  from Configurables import MagneticFieldSvc
  from Configurables import TAlignment
  from TAlignment.VertexSelections import configuredPVSelection
  from Configurables import RunChangeHandlerSvc
  OnlineEnv = importOnline()
  import ConditionsMap

  escher = Escher.Configuration.Escher()
  escher.DataType = '2015'
  escher.DDDBtag   = ConditionsMap.DDDBTag
  escher.CondDBtag = ConditionsMap.CondDBTag
  escher.OnlineMode = True
  escher.UseDBSnapshot = True
  escher.DBSnapshotDirectory = "/group/online/hlt/conditions/"

  if hasattr(OnlineEnv, "AlignXmlDir"):
    escher.OnlineAligWorkDir = os.path.join(OnlineEnv.AlignXmlDir, 'running')

  sys.path.insert(2, os.path.dirname(OnlineEnv.ConditionsMapping))
  import Online as OnlineConds
  handlerConditions = OnlineConds.ConditionMap
  if true_online_version and os.environ['PARTITION_NAME'] == 'TEST':
    re_year = re.compile('(201\d)')
    for k, v in handlerConditions.items():
      handlerConditions[re_year.sub('2014', k)] = v

  conddb = CondDB()
  conddb.RunChangeHandlerConditions = handlerConditions
  conddb.IgnoreHeartBeat = True

  config_module = importlib.import_module('AlignmentConfigurations.' + alignment_module)
  config_module.configureAlignment()
  return escher

def HostName():
  import socket
  hostname = socket.gethostname()
  return hostname
