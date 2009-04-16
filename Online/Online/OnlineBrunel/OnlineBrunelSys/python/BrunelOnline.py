"""
     Run Brunel in the online environment

     @author M.Frank
"""
__version__ = "$Id: BrunelOnline.py,v 1.10 2009-04-16 08:51:24 jblouw Exp $"
__author__  = "Markus Frank <Markus.Frank@cern.ch>"

import os
import Configurables as Configs
import Gaudi.Configuration as Gaudi

debug = 0
def dummy(*args,**kwd): pass

def patchBrunel(true_online_version):
  """
        Instantiate the options to run Brunel with raw data

        @author M.Frank
  """
  import Brunel.Configuration
  from Configurables import HistogramPersistencySvc,EventLoopMgr
  brunel = Brunel.Configuration.Brunel()
  EventLoopMgr().OutputLevel = 0
  # Brunel.Configuration.Brunel.defineMonitors = dummy
  Brunel.Configuration.Brunel.configureOutput = dummy
  #if debug: print dir(brunel)
  #brunel.DDDBtag   = 'default'
  #brunel.CondDBtag = 'default'
  brunel.DDDBtag   = "head-20090112"
  brunel.CondDBtag = "sim-20090112"
  brunel.WriteFSR  = False # This crashes Jaap's stuff
  brunel.Simulation = True
  if true_online_version:
    #brunel.__repr__      = dummy
    brunel.NoWarnings     = True
    brunel.PrintFreq      = -1
    brunel.Histograms     = 'None'
  Brunel.Configuration.ProcessPhase("Output").DetectorList += [ 'DST' ]
  # Set the property, used to build other file names
  brunel.setProp( "DatasetName", 'GaudiSerialize' )
  HistogramPersistencySvc().OutputFile = ""
  brunel.MainSequence = [ "UpdateAndReset",
                          "ProcessPhase/Init",
                          "ProcessPhase/Reco",
                          "ProcessPhase/Moni",
                          "ProcessPhase/Output" ]


def setupOnline():
  """
        Setup the online environment: Buffer managers, event serialisation, etc.

        @author M.Frank
  """
  import OnlineEnv as Online
  from Configurables import DstConf
  from Configurables import Serialisation
  buffs = ['Events','Output']
  app=Gaudi.ApplicationMgr()
  app.AppName = ''
  app.HistogramPersistency = 'ROOT'
  app.SvcOptMapping.append('LHCb::OnlineEvtSelector/EventSelector')
  app.SvcOptMapping.append('LHCb::FmcMessageSvc/MessageSvc')
  mep = Online.mepManager(Online.PartitionID,Online.PartitionName,buffs,True)
  sel = Online.mbmSelector(input='Events',type='ONE',decode=False)
  app.EvtSel  = sel
  app.Runable = Online.evtRunable(mep)
  app.ExtSvc.append(mep)
  app.ExtSvc.append(sel)

  DstConf().Writer       = 'DstWriter'
  DstConf().DstType      = 'DST'
  DstConf().PackType     = 'NONE'
#  DstConf().Simulation   = False
  Serialisation().Writer = 'Writer'
  
  app.AuditAlgorithms = False
  Configs.MonitorSvc().OutputLevel = 5
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
  if Gaudi.allConfigurables.has_key('MessageSvc'):
    del Gaudi.allConfigurables['MessageSvc']
  msg=Configs.LHCb__FmcMessageSvc('MessageSvc')
  msg.fifoPath     = os.environ['LOGFIFO']
  msg.LoggerOnly   = True
  msg.OutputLevel  = 4
  msg.doPrintAlways = False

def start():
  """
        Finish configuration and configure Gaudi application manager

        @author M.Frank
  """
  import OnlineEnv as Online
  Online.end_config(False)
  #Online.end_config(True)

true_online = os.environ.has_key('LOGFIFO') and os.environ.has_key('PARTITION')
debug = not true_online

if not true_online: print '\n            Running terminal version 1.1 of Brunel ONLINE\n\n'  
patchBrunel(true_online)
setupOnline()
if true_online: patchMessages()
start()
