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

#============================================================================================================
def packDST(self,items):
  """
  Write an RDST in MDF format
  """
  import OnlineEnv
  import Configurables

  seq = Gaudi.GaudiSequencer("WriteMDFSeq")
  # Put store explorer in front.
  #exp = OnlineEnv.storeExplorer(load=1,freq=0.000001)
  #exp.OutputLevel = 1
  #seq.Members += [ exp ]

  # Configure DST packer algorithm
  packer = Configurables.WritePackedDst('MdfPacker')
  packer.Containers += items
  # Now configure the FID updater of DSTAddress
  dstUpdate = OnlineEnv.dstAddressUpdate(name='DstUpdate')
  # Now configure bank merger
  mergeDst = OnlineEnv.evtMerger(buffer='Output',name='DstMerger',location='/Event/DAQ/DstEvent',routing=0x100)
  mergeDst.FIDLocation = '/Event';
  mergeDst.AddFID = 1;
  mergeDst.Compress = 0; # May use compress=2 as well

  seq.Members += [ packer, dstUpdate, mergeDst ]
  ##print 'Warning: Packing of TES DST data .... commissioned....'
  sys.stdout.flush()

def dstconf_write_root( self, items, optItems ):
  print '[ERROR] Configure DstConf output ....'
  sys.stdout.flush()
  writer = OutputStream("DstWriter")
  writer.Preload = False
  writer.ItemList += items
  writer.OptItemList += optItems
  writer.Output = "DATAFILE='root.buffers' SVC='Gaudi::SerializeCnvSvc' OPT='RECREATE'"

#============================================================================================================
def configureOutput(self, dstType, withMC, handleLumi):
  global configureBrunelOutput
  print '[ERROR] Configure Brunel output ....'
  sys.stdout.flush()
  if configureBrunelOutput:
    import OnlineEnv
    print '[ERROR] Configure Brunel output sequence ....'
    sys.stdout.flush()
    Configs.DstConf._doWriteROOT=dstconf_write_root
    configureBrunelOutput(self, dstType, withMC, handleLumi)
    ser = Gaudi__SerializeCnvSvc('Gaudi::SerializeCnvSvc')
    ser.Banks = '/Event/GaudiSerialize'
    ser.OutputLevel = 1
    EventPersistencySvc().CnvServices.append(ser)
    writer = OnlineEnv.evtMerger(buffer='Output',name='BankWriter',location='/Event/GaudiSerialize',routing=0x100)
    writer.OutputLevel = 1
    seq = GaudiSequencer('Output')
    seq.Members += [OutputStream('DstWriter'),writer]
    Gaudi.ApplicationMgr().OutStream = [seq]

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
  forker.UtgidPattern        = "%P_%NN_%T_%02d";
  forker.PrintLevel          = 3  # 1=MTCP_DEBUG 2=MTCP_INFO 3=MTCP_WARNING 4=MTCP_ERROR
  forker.OutputLevel         = 4  # 1=VERBOSE 2=DEBUG 3=INFO 4=WARNING 5=ERROR 6=FATAL
  appMgr.ExtSvc.insert(0,forker)

#============================================================================================================
def patchBrunel(true_online_version):
  """
        Instantiate the options to run Brunel with raw data

        @author M.Frank
  """
  from   Configurables import TrackSys, CondDB
  import GaudiConf.DstConf
  import Brunel.Configuration
  import ConditionsMap
  import OnlineEnv

  brunel = Brunel.Configuration.Brunel()
  brunel.OnlineMode = True
  try:
    brunel.DDDBtag  = OnlineEnv.DDDBTag
  except:
    print "DDDBTag not found, use default"
  try:
    brunel.CondDBtag = OnlineEnv.CondDBTag
  except:
    print "CondDBTag not found, use default"

  brunel.DataType = "2015"
  brunel.UseDBSnapshot = True  # Try it

  conddb = CondDB()
  conddb.Online = True
  #
  # Adjust to pickup the proper online conditions from ConditionsMap
  #
  conddb.RunChangeHandlerConditions = ConditionsMap.RunChangeHandlerConditions
  conddb.setProp('EnableRunChangeHandler', True)
  #
  # Stops the reconstruction if tracking hits beyond some reasonable limit.
  # Not necessary anymore, since memory leak in Brunel is fixed....
  #### TrackSys().GlobalCuts = { 'Velo': 5000, 'IT': 3000, 'OT': 8000 }
  #
  # Brunel output configuration
  #
  brunel.WriteFSR  = False     # This crashes Jaap's stuff
  EventLoopMgr().OutputLevel = MSG_DEBUG #ERROR
  EventLoopMgr().Warnings    = False

  # from Configurables import EventClockSvc
  # EventClockSvc().InitialTime = 1322701200000000000

  if true_online_version:
    # brunel.OutputLevel       = MSG_INFO
    brunel.OutputLevel       = MSG_WARNING
    brunel.PrintFreq         = -1

  if processingType == 'Reprocessing':
    GaudiConf.DstConf.DstConf._doWriteMDF = packDST
    brunel.PackType   = 'MDF'
    brunel.OutputType = 'RDST'
    brunel.WriteLumi  = True
    brunel.Histograms = 'Online'
    sys.stdout.flush()
  else:
    global configureBrunelOutput
    brunel.WriteLumi = False
    brunel.Histograms = 'Online'
    Brunel.Configuration.Brunel.configureOutput = dummy
    """
    configureBrunelOutput = Brunel.Configuration.Brunel.configureOutput
    Brunel.Configuration.Brunel.configureOutput = configureOutput
    brunel.setProp( 'DatasetName', 'GaudiSerialize' )
    brunel.OutputType = 'DST'
    brunel.PackType = 'TES'
    """

    """ This does no longer work:
    ##from Configurables import DstConf, Serialisation, ProcessPhase
    print '[WARN]  Running GaudiSerialize!'
    ProcessPhase("Output").DetectorList += [ 'DST' ]
    brunel.setProp( 'DatasetName', 'GaudiSerialize' )
    DstConf().Writer       = 'DstWriter'
    DstConf().DstType      = 'DST'
    DstConf().PackType     = 'NONE'
    Serialisation().Writer = 'Writer'
    ##Serialisation().Explorer = True
    Serialisation()._ConfigurableUser__addPassiveUseOf(DstConf())
    """
  HistogramPersistencySvc().OutputFile = ""
  HistogramPersistencySvc().OutputLevel = MSG_ERROR

  ####print brunel
  return brunel

#============================================================================================================
def setupOnline():
  """
        Setup the online environment: Buffer managers, event serialisation, etc.

        @author M.Frank
  """
  import OnlineEnv

  buffs = ['Events','Output']
  app=Gaudi.ApplicationMgr()
  app.AppName = ''
  app.HistogramPersistency = 'ROOT'
  app.SvcOptMapping.append('LHCb::OnlineEvtSelector/EventSelector')
  app.SvcOptMapping.append('LHCb::FmcMessageSvc/MessageSvc')
  mep = OnlineEnv.mepManager(OnlineEnv.PartitionID,OnlineEnv.PartitionName,buffs,True)
  mep.ConnectWhen = "start";
  sel = OnlineEnv.mbmSelector(input=buffs[0],type='ONE',decode=False,event_type=2)
  if requirement:
    print '++++ Warning: Setting requirements:',requirement
    sel.REQ1 = requirement
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
  app.OutputLevel = MSG_INFO
  if OnlineEnv.RecoStartupMode>0:      ### os.environ.has_key('NBOFSLAVES'):
    configureForking(app)

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
  msg.OutputLevel   = MSG_INFO # OnlineEnv.OutputLevel

#============================================================================================================
def start():
  """
        Finish configuration and configure Gaudi application manager

        @author M.Frank
  """
  import OnlineEnv
  OnlineEnv.end_config(False)
  #OnlineEnv.end_config(True)

#============================================================================================================
def getProcessingType():
  import os
  import OnlineEnv

  if (hasattr(OnlineEnv,'ActivityType') and getattr(OnlineEnv,'ActivityType') == 'Reprocessing'):
    return 'Reprocessing'
  if os.environ.has_key('Standalone_test'):
    return 'Reprocessing'
  return 'DataTaking'

processingType = getProcessingType()
#print 'Warning: Setup writing: Processing type :',processingType,hasattr(os.environ,'Standalone_test')

true_online = os.environ.has_key('LOGFIFO') and (os.environ.has_key('PARTITION') or os.environ.has_key('PARTITION_NAME'))
debug = not true_online

if not true_online:
  print '\n            Running terminal version 1.1 of Brunel Online\n\n'
  requirement = "EvType=2;TriggerMask=0x0,0x4,0x0,0x0;VetoMask=0,0,0,0x300;MaskType=ANY;UserType=VIP;Frequency=PERC;Perc=100.0"

  ##requirement = "EvType=1;TriggerMask=0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF;VetoMask=0,0,0,0x0;MaskType=ANY;UserType=VIP;Frequency=PERC;Perc=100.0"
br = patchBrunel(true_online)
setupOnline()
if true_online: patchMessages()
start()
