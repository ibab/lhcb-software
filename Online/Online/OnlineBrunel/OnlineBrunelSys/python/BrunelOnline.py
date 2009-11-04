"""
     Run Brunel in the online environment

     @author M.Frank
"""
__version__ = "$Id: BrunelOnline.py,v 1.16 2009-11-04 18:04:12 frankb Exp $"
__author__  = "Markus Frank <Markus.Frank@cern.ch>"

import os
import Configurables as Configs
import Gaudi.Configuration as Gaudi
import GaudiKernel

processingType ='DataTaking'

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

  seq = Gaudi.GaudiSequencer("WriteMDFSeq")
  seq.Members += [ packer, dstUpdate, mergeDst ]
  print 'Warning: Packing of TES DST data .... commissioned....'

#============================================================================================================
def serializeDST(self,items):
  """
  Write an RDST in MDF format
  """
  import OnlineEnv as Online

  # First update data of the standard DST writer
  serial = Online.serialWriter(name='DstWriter',location=loc)
  serial.RequireAlgs += ['Reco']
  serial.OutputLevel = lvl
  # Now configure bank merger
  mergeDst = Online.evtMerger(buffer='Output',name='DstMerger',location='/Event/GaudiSerialize',routing=0x100)
  mergeDst.FIDLocation = '/Event';

  seq = Gaudi.GaudiSequencer("WriteMDFSeq")
  seq.Members = [ serial, mergeDst ]
  print 'Warning: Serialization of TES DST data .... commissioned....'

#============================================================================================================
def patchBrunel(true_online_version):
  """
        Instantiate the options to run Brunel with raw data

        @author M.Frank
  """
  import GaudiConf.DstConf
  import Brunel.Configuration
  from Configurables import DstConf, Serialisation, HistogramPersistencySvc, EventLoopMgr

  brunel = Brunel.Configuration.Brunel()
  EventLoopMgr().OutputLevel = 0
  brunel.DDDBtag   = "head-20090112"
  brunel.CondDBtag = "sim-20090112"
  brunel.WriteFSR  = False # This crashes Jaap's stuff
  brunel.Simulation = True

  if true_online_version:
    brunel.OutputLevel    = 4
    brunel.PrintFreq      = -1
    brunel.Histograms     = 'None'
    
  if processingType == 'Reprocessing':
    GaudiConf.DstConf.DstConf._doWriteMDF = packDST
    brunel.PackType   = 'MDF'
    brunel.OutputType = 'RDST'
  else:
    #GaudiConf.DstConf.DstConf._doWriteMDF = serializeDST
    from Configurables import ProcessPhase
    Brunel.Configuration.Brunel.configureOutput = dummy
    ProcessPhase("Output").DetectorList += [ 'DST' ]
    brunel.setProp( 'DatasetName', 'GaudiSerialize' )
    DstConf().Writer       = 'DstWriter'
    DstConf().DstType      = 'DST'
    DstConf().PackType     = 'NONE'
    Serialisation().Writer = 'Writer'

  HistogramPersistencySvc().OutputFile = ""
  brunel.MainSequence = [ "UpdateAndReset",
                          "ProcessPhase/Init",
                          "ProcessPhase/Reco",
                          "ProcessPhase/Moni",
                          "ProcessPhase/Output" ]
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
  ###print "BUFFERS",buffs
  mep = Online.mepManager(Online.PartitionID,Online.PartitionName,buffs,True)
  sel = Online.mbmSelector(input=buffs[0],type='ONE',decode=False)
  app.EvtSel  = sel
  app.Runable = Online.evtRunable(mep)
  app.Runable.NumErrorToStop = 1;
  app.ExtSvc.append(mep)
  app.ExtSvc.append(sel)
  app.AuditAlgorithms = False
  Configs.MonitorSvc().OutputLevel = 5
  app.OutputLevel = 4

#============================================================================================================
def patchMessages():
  """
        Messages in the online get redirected.
        Setup here the FMC message service

        @author M.Frank
  """
  app=Gaudi.ApplicationMgr()
  Configs.AuditorSvc().Auditors = []
  app.MessageSvcType = 'LHCb::FmcMessageSvc'
  if Gaudi.allConfigurables.has_key('MessageSvc'):
    del Gaudi.allConfigurables['MessageSvc']
  msg=Configs.LHCb__FmcMessageSvc('MessageSvc')
  msg.fifoPath     = os.environ['LOGFIFO']
  msg.LoggerOnly   = True
  msg.OutputLevel  = 4
  msg.doPrintAlways = False

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

if not true_online: print '\n            Running terminal version 1.1 of Brunel ONLINE\n\n'  
patchBrunel(true_online)
setupOnline()
if true_online: patchMessages()
start()
