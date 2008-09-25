# Example 2008 data files for Brunel.
# This file must be given as last argument to gaudirun.py, after Brunel<conf>.py

def dummy(): pass

def patchBrunel():
  import Configurables as Configs
  import Brunel.Configuration

  brunel=Brunel.Configuration.Brunel()
  print '\n\n           [ERROR] Running special version 1.1\n\n\n'
  brunel.defineMonitors = dummy
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

#
#  Online Environment:
#
def brunelApp():
  import OnlineEnv as Online
  import Gaudi.Configuration as Gaudi
  import Configurables as Configs
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
  app.MessageSvcType = 'LHCb::FmcMessageSvc'
  del Gaudi.allConfigurables['MessageSvc']
  msg=Configs.LHCb__FmcMessageSvc('MessageSvc')
  msg.fifoPath = '/tmp/Reconstruction_Slice00.fifo'
  msg.OutputLevel = 4
  msg.doPrintAlways = False
  app.OutputLevel = 4
  
patchBrunel()
brunelApp()
