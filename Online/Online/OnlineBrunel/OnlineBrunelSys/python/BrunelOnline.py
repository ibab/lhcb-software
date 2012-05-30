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
  ##print 'Warning: Packing of TES DST data .... commissioned....'
  sys.stdout.flush()

#============================================================================================================
def configureOutput(self, dstType, withMC, handleLumi):
  pass

#============================================================================================================
def patchBrunel(true_online_version):
  """
        Instantiate the options to run Brunel with raw data

        @author M.Frank
  """
  import GaudiConf.DstConf
  import Brunel.Configuration
  from Configurables import CondDB, DstConf, HistogramPersistencySvc, EventLoopMgr
  import OnlineEnv as Online
  
  brunel = Brunel.Configuration.Brunel()

  try:
    brunel.DDDBtag    = Online.DDDBTag
  except:
    print "DDDBTag not found, use default"

  try:
    brunel.CondDBtag = Online.CondDBTag
  except:
    print "CondDBTag not found, use default"

  conddb = CondDB()
  conddb.IgnoreHeartBeat = True

  brunel.WriteFSR  = False # This crashes Jaap's stuff
  brunel.DataType = "2012"

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
  
  if true_online_version:
    brunel.OutputLevel       = 999
    #brunel.OutputLevel       = MSG_INFO
    brunel.PrintFreq         = -1

  if processingType == 'Reprocessing':
    GaudiConf.DstConf.DstConf._doWriteMDF = packDST
    brunel.PackType   = 'MDF'
    brunel.OutputType = 'RDST'
    brunel.WriteLumi  = True
    brunel.Histograms = 'None'
    print '[WARN] Running brunel with histogram settings:None'
    sys.stdout.flush()
  else:
    #print '[WARN]  Running GaudiSerialize!'
    from Configurables import Serialisation, ProcessPhase
    brunel.WriteLumi = False
    brunel.Histograms = 'Online'
    ##print '[WARN] Running brunel with histogram settings:Online'
    sys.stdout.flush()
    Brunel.Configuration.Brunel.configureOutput = dummy
    """
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

  print brunel
  
  return brunel

#============================================================================================================
def setupOnline():
  """
        Setup the online environment: Buffer managers, event serialisation, etc.

        @author M.Frank
  """
  import OnlineEnv as Online
  
  buffs = ['Events','Output']
  if processingType == 'Reprocessing':
    buffs = ['Input','Output']

  app=Gaudi.ApplicationMgr()
  app.AppName = ''
  app.HistogramPersistency = 'ROOT'
  app.SvcOptMapping.append('LHCb::OnlineEvtSelector/EventSelector')
  app.SvcOptMapping.append('LHCb::FmcMessageSvc/MessageSvc')

  mep = Online.mepManager(Online.PartitionID,Online.PartitionName,buffs,True)
  sel = Online.mbmSelector(input=buffs[0],type='ONE',decode=False)
  if requirement:
    print 'Setting requirements:',requirement
    sel.REQ1 = requirement
  app.EvtSel  = sel
  app.Runable = Online.evtRunable(mep)
  app.Runable.NumErrorToStop = -1;
  app.ExtSvc.append(mep)
  app.ExtSvc.append(sel)
  app.AuditAlgorithms = False
  app.TopAlg.insert(0,"UpdateAndReset")
  Configs.MonitorSvc().OutputLevel = MSG_ERROR
  Configs.MonitorSvc().UniqueServiceNames = 1
  Configs.RootHistCnv__PersSvc("RootHistSvc").OutputLevel = MSG_ERROR
  app.OutputLevel = MSG_DEBUG #WARNING
  #app.OutputLevel = MSG_INFO

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
  #  msg.OutputLevel   = MSG_WARNING
  #  msg.OutputLevel   = Online.OutputLevel
  msg.OutputLevel   = MSG_INFO

#============================================================================================================
def start():
  """
        Finish configuration and configure Gaudi application manager

        @author M.Frank
  """
  import OnlineEnv as Online
  Online.end_config(False)
  #Online.end_config(True)

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

if not true_online:
  print '\n            Running terminal version 1.1 of Brunel ONLINE\n\n'  
  requirement = "EvType=2;TriggerMask=0x0,0x4,0x0,0x0;VetoMask=0,0,0,0x300;MaskType=ANY;UserType=VIP;Frequency=PERC;Perc=100.0"
br = patchBrunel(true_online)
setupOnline()
if true_online: patchMessages()
start()
