# Example 2008 data files for Brunel.
# This file must be given as last argument to gaudirun.py, after Brunel<conf>.py

# Syntax is:
#   gaudirun.py Brunel<conf>.py 2008-Files.py
#
from Gaudi.Configuration import *
from GaudiConf.Configuration import *
from Brunel.Configuration import *
from Configurables import (RecInit,ProcessPhase)

def dummy(): pass

brunel=Brunel()
brunel.defineMonitors=dummy
brunel.__repr__=dummy
brunel.noWarnings = True
brunel.printFreq = -1
brunel.applyConf()

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml",
                           "xmlcatalog_file:$BRUNELROOT/job/NewCatalog.xml" ]

# Above file requires following special options for Velo
from Configurables import (RawBankToSTClusterAlg, RawBankToSTLiteClusterAlg,
                           DecodeVeloRawBuffer, UpdateManagerSvc )

DecodeVeloRawBuffer().ForceBankVersion=3
DecodeVeloRawBuffer('DecodeVeloClusters').RawEventLocation='Prev2/DAQ/RawEvent'
DecodeVeloRawBuffer('DecodeVeloClusters').ForceBankVersion=3
UpdateManagerSvc().ConditionsOverride +=  ["Conditions/Online/Velo/MotionSystem := double ResolPosRC =-29. ; double ResolPosLA = 29. ;"]
RawBankToSTClusterAlg('CreateTTClusters').rawEventLocation = "/Event/Prev2/DAQ/RawEvent"
RawBankToSTLiteClusterAlg('CreateTTLiteClusters').rawEventLocation = "/Event/Prev2/DAQ/RawEvent"

# Set the property, used to build other file names
brunel.setProp( "datasetName", 'GaudiSerialize' )
#
#  Online Environment:
#
def brunelApp():
  import OnlineEnv as Online
  app=ApplicationMgr()
  app.AppName = ''
  app.HistogramPersistency = 'ROOT'
  app.SvcOptMapping.append('LHCb::OnlineEvtSelector/EventSelector')
  mepMgr = Online.mepManager(Online.PartitionID,Online.PartitionName,['Events','Output'],True)
  app.EvtSel  = Online.mbmSelector(input='Events',type='ONE',decode=False)
  app.Runable = Online.evtRunable(mepMgr)
  app.ExtSvc.append(mepMgr)
  #ProcessPhase("Output").Members = \
  # [Online.serialWriter(name='DstWriter',location='/Event/GaudiSerialize'),
  #  Online.evtMerger(buffer='Output',name='Writer',location='/Event/GaudiSerialize',routing=0x10)]
  app.OutStream = []
  app.OutStream.append(Online.serialWriter(name='DstWriter',location='/Event/GaudiSerialize'))
  app.OutStream.append(Online.evtMerger(buffer='Output',name='Writer',location='/Event/GaudiSerialize',routing=0x10))
  #print app.OutStream
  #print ProcessPhase("Output")
  MessageSvc().OutputLevel = 4
  app.OutputLevel = 4
  Online.end_config(print_config=False)
  return app

brunelApp()
