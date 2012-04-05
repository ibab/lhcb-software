"""
     Run Brunel in the online environment

     @author M.Frank
"""
__version__ = "$Id: BrunelOnline2.py,v 1.1 2010/11/09 12:21:19 frankb Exp $"
__author__  = "Markus Frank <Markus.Frank@cern.ch>"

import os, sys
import Configurables as Configs
import Gaudi.Configuration as Gaudi
import GaudiKernel
from GaudiKernel.ProcessJobOptions import PrintOff,InstallRootLoggingHandler,logging

PrintOff(999)
InstallRootLoggingHandler(level=logging.CRITICAL)

GaudiKernel.ProcessJobOptions._parser._parse_units(os.path.expandvars("$STDOPTS/units.opts"))

debug = 0
def dummy(*args,**kwd): pass

#============================================================================================================
def packDST(self,items):
  """
  Write an RDST in MDF format
  """
  import OnlineEnv as Online
  import Configurables

  seq = Gaudi.GaudiSequencer("WriteMDFSeq")
  # Put store explorer in front.
  #exp = Online.storeExplorer(load=1,freq=0.000001)
  #exp.OutputLevel = 1
  #seq.Members += [ exp ]

  # Add environment dump for checkpointing
  env_dump = Configs.LHCb__EnvironDumper("EnvironDumper")
  env_dump.Environment = ['DIM_DNS_NODE','UTGID','LOGFIFO']
  env_dump.OutputLevel = 1
  env_dump.PrintLevel  = 4
  seq.Members += [ env_dump ]

  # Configure DST packer algorithm
  packer = Configurables.WritePackedDst('MdfPacker')
  packer.Containers += items
  # Now configure the FID updater of DSTAddress
  dstUpdate = Online.dstAddressUpdate(name='DstUpdate')
  # Now configure bank merger
  mergeDst = Online.evtMerger(buffer='Output',name='DstMerger',location='/Event/DAQ/DstEvent',routing=0x100)
  mergeDst.FIDLocation = '/Event';
  mergeDst.AddFID = 1;
  mergeDst.Compress = 0; # May use compress=2 as well

  seq.Members += [ packer, dstUpdate, mergeDst ]
  ###print 'Warning: Packing of TES DST data .... commissioned....'
  sys.stdout.flush()


#============================================================================================================
class OnlineApplication:
  """
         Base class to run Gaudi applications in the online environment
         
         @author M.Frank
  """
  def __init__(self, true_online_version):
    self.true_online_version = true_online_version
    self.appMgr = None

  def initialize(self):
    pass
  
  #============================================================================================================
  def setup(self, buffers=['Events','Output']):
    """
          Setup the online environment: Buffer managers, event serialisation, etc.

          @author M.Frank
    """
    import OnlineEnv as Online

    self.appMgr = Gaudi.ApplicationMgr()
    self.appMgr.AppName = ''
    self.appMgr.HistogramPersistency = 'ROOT'

    forker = Configs.LHCb__CheckpointSvc("CheckpointSvc")
    forker.NumberOfInstances   = 5
    forker.UseCores            = False
    forker.ChildSessions       = False
    forker.DumpFiles           = False
    #forker.Checkpoint          = "Checkpoint.data"
    forker.PrintLevel          = 3  # 1=MTCP_DEBUG 2=MTCP_INFO 3=MTCP_WARNING 4=MTCP_ERROR
    forker.OutputLevel         = 2  # 1=VERBOSE 2=DEBUG 3=INFO 4=WARNING 5=ERROR 6=FATAL
    #forker.ExitAfterCheckpoint = False
    self.appMgr.ExtSvc.append(forker)

    self.appMgr.SvcOptMapping.append('LHCb::OnlineEvtSelector/EventSelector')
    ##self.appMgr.SvcOptMapping.append('LHCb::DummyMonitorSvc/MonitorSvc')
    self.appMgr.SvcOptMapping.append('LHCb::FmcMessageSvc/MessageSvc')
    mep = Online.mepManager(Online.PartitionID,Online.PartitionName,buffers,True)
    mep.ConnectWhen = 'start'
    sel = Online.mbmSelector(input=buffers[0],type='ONE',decode=False)
    self.appMgr.ExtSvc.append(mep)
    self.appMgr.ExtSvc.append(sel)

    self.appMgr.EvtSel  = sel
    self.appMgr.Runable = Online.evtRunable(mep)
    self.appMgr.Runable.NumErrorToStop = 1;
    self.appMgr.AuditAlgorithms = False
    ##mon = Configs.LHCb__DummyMonitorSvc("MonitorSvc")
    mon = Configs.MonitorSvc("MonitorSvc")
    mon.OutputLevel = 5
    mon.UniqueServiceNames = 1
    self.appMgr.OutputLevel = 4

  #============================================================================================================
  def patchMessages(self):
    """
          Messages in the online get redirected.
          Setup here the FMC message service

          @author M.Frank
    """
    if self.true_online_version:
      app=Gaudi.ApplicationMgr()
      Configs.AuditorSvc().Auditors = []
      self.appMgr.MessageSvcType = 'LHCb::FmcMessageSvc'
      if Gaudi.allConfigurables.has_key('MessageSvc'):
        del Gaudi.allConfigurables['MessageSvc']
      msg = Configs.LHCb__FmcMessageSvc('MessageSvc')
      msg.fifoPath      = os.environ['LOGFIFO']
      msg.LoggerOnly    = True
      msg.OutputLevel   = 4
      #Configs.ToolsSvc().OutputLevel = 5
      return msg

  #============================================================================================================
  def start(self):
    """
          Finish configuration and configure Gaudi application manager

          @author M.Frank
    """
    import OnlineEnv as Online
    Online.end_config(False)
    #Online.end_config(True)


class OnlineBrunel(OnlineApplication):
#============================================================================================================
  def initialize(self):
    """
          Instantiate the options to run Brunel with raw data

          @author M.Frank
    """
    import Brunel.Configuration
    from Configurables import CondDB, HistogramPersistencySvc, EventLoopMgr

    self.app = Brunel.Configuration.Brunel()
    conddb = CondDB()
    conddb.IgnoreHeartBeat       = True
    self.app.WriteFSR            = False # This crashes Jaap's stuff
    EventLoopMgr().OutputLevel   = 5
    EventLoopMgr().Warnings      = False
    HistogramPersistencySvc().OutputFile = ""

    if self.true_online_version:
      self.app.OutputLevel    = 999
      self.app.PrintFreq      =  -1

  def reprocessingOutput(self):
    import GaudiConf.DstConf
    GaudiConf.DstConf.DstConf._doWriteMDF = packDST
    self.app.PackType   =  'MDF'
    self.app.OutputType = 'SDST'
    self.app.WriteLumi  =  True
    self.app.Histograms = 'None'
    print '[WARN] Running brunel with: Output ',self.app.OutputType,'/',self.app.PackType,\
          ' Lumi:',self.app.WriteLumi,' histogram settings:',self.app.Histograms
    sys.stdout.flush()

  def daqOutput(self):
    from Configurables import DstConf, Serialisation, ProcessPhase
    ###print '[WARN]  Running GaudiSerialize!'
    self.app.WriteLumi  =  False
    self.app.Histograms = 'Online'
    print '[WARN] Running brunel with histogram settings:Online'
    sys.stdout.flush()
    Brunel.Configuration.Brunel.configureOutput = dummy
    ProcessPhase("Output").DetectorList += ['DST']
    self.app.setProp( 'DatasetName', 'GaudiSerialize' )
    DstConf().Writer       = 'DstWriter'
    DstConf().DstType      = 'DST'
    DstConf().PackType     = 'NONE'
    Serialisation().Writer = 'Writer'
    Serialisation()._ConfigurableUser__addPassiveUseOf(DstConf())

#============================================================================================================
def getProcessingType():
  import os
  import OnlineEnv as Online

  if (hasattr(Online,'ActivityType') and getattr(Online,'ActivityType') == 'Reprocessing'):
    return 'Reprocessing'
  if os.environ.has_key('Standalone_test'):
    return 'Reprocessing'
  return 'DataTaking'

processingType = getProcessingType()
#print 'Warning: Setup writing: Processing type :',processingType,hasattr(os.environ,'Standalone_test')

true_online = os.environ.has_key('LOGFIFO') and os.environ.has_key('PARTITION')
debug = not true_online

if not true_online: print '\n            Running terminal version 1.1 of Brunel ONLINE\n\n'  
app = OnlineBrunel(true_online)
app.initialize()
if processingType == 'Reprocessing':
  app.reprocessingOutput()
  app.setup(['Input','Output'])
else:
  app.daqOutput()
  app.setup(['Events','Output'])
if os.environ.has_key('MSGSVC') and os.environ['MSGSVC']=='LHCb::FmcMessageSvc':
  app.true_online_version = True
msg=app.patchMessages()
msg.doPrintAlways = True
msg.OutputLevel   = 3
app.start()
