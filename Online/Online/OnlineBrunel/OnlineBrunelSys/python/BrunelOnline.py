"""
     Run Brunel in the online environment

     @author M.Frank
"""
import os
import Configurables as Configs
import Gaudi.Configuration as Gaudi

def dummy(): pass

def patchBrunel(true_online_version):
  """
        Instantiate the options to run Brunel with raw data

        @author M.Frank
  """
  import Brunel.Configuration

  brunel=Brunel.Configuration.Brunel()
  brunel.defineMonitors = dummy
  if true_online_version:
    brunel.__repr__       = dummy
    brunel.noWarnings     = True
    brunel.printFreq      = -1
  brunel.applyConf()
  # Above file requires following special options for Velo
  Configs.DecodeVeloRawBuffer().ForceBankVersion=3
  Configs.DecodeVeloRawBuffer('DecodeVeloClusters').RawEventLocation='Prev2/DAQ/RawEvent'
  Configs.DecodeVeloRawBuffer('DecodeVeloClusters').ForceBankVersion=3
  Configs.UpdateManagerSvc().ConditionsOverride +=  ["Conditions/Online/Velo/MotionSystem := double ResolPosRC =-29. ; double ResolPosLA = 29. ;"]
  Configs.RawBankToSTClusterAlg('CreateTTClusters').rawEventLocation = "/Event/Prev2/DAQ/RawEvent"
  Configs.RawBankToSTLiteClusterAlg('CreateTTLiteClusters').rawEventLocation = "/Event/Prev2/DAQ/RawEvent"
  # Set the property, used to build other file names
  brunel.setProp( "datasetName", 'GaudiSerialize' )

def setupOnline():
  """
        Setup the online environment: Buffer managers, event serialisation, etc.

        @author M.Frank
  """
  import OnlineEnv as Online
  app=Gaudi.ApplicationMgr()
  app.AppName = ''
  app.HistogramPersistency = 'ROOT'
  app.SvcOptMapping.append('LHCb::OnlineEvtSelector/EventSelector')
  app.SvcOptMapping.append('LHCb::FmcMessageSvc/MessageSvc')
  mepMgr = Online.mepManager(Online.PartitionID,Online.PartitionName,['Events','Output'],True)
  app.EvtSel  = Online.mbmSelector(input='Events',type='ONE',decode=False)
  app.Runable = Online.evtRunable(mepMgr)
  app.ExtSvc.append(mepMgr)
  app.OutStream = []
  app.OutStream.append(Online.serialWriter(name='DstWriter',location='/Event/GaudiSerialize'))
  app.OutStream.append(Online.evtMerger(buffer='Output',name='Writer',location='/Event/GaudiSerialize',routing=0x10))
  app.AuditAlgorithms = False
  app.OutputLevel = 4

def patchMessages():
  """
        Messages in the online get redirected.
        Setup here the FMC message service

        @author M.Frank
  """
  app=Gaudi.ApplicationMgr()
  Configs.AuditorSvc().Auditors = []
  app.MessageSvcType = 'LHCb::FmcMessageSvc'
  del Gaudi.allConfigurables['MessageSvc']
  msg=Configs.LHCb__FmcMessageSvc('MessageSvc')
  msg.fifoPath = os.environ['LOGFIFO']
  msg.OutputLevel = 4
  msg.doPrintAlways = False

def start():
  """
        Finish configuration and configure Gaudi application manager

        @author M.Frank
  """
  import OnlineEnv as Online
  Online.end_config(False)

true_online = os.environ.has_key('LOGFIFO') and os.environ.has_key('PARTITION')

if not true_online:
  print '\n            Running terminal version 1.1 of Brunel ONLINE\n\n'  
patchBrunel(true_online)
setupOnline()
if true_online: patchMessages()
start()
