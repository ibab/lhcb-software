import os, sys
import Configurables as Configs
import Gaudi.Configuration as Gaudi

MSG_VERBOSE = 1
MSG_DEBUG   = 2
MSG_INFO    = 3
MSG_WARNING = 4
MSG_ERROR   = 5
MSG_FATAL   = 6
MSG_ALWAYS  = 7

def setup(nick_name):
  """
        Setup the online environment: Buffer managers, event serialisation, etc.

        @author M.Frank
  """
  import OnlineEnv as Online
  
  buffs = ['Events']
  app=Gaudi.ApplicationMgr()
  app.AppName = ''
  app.HistogramPersistency = 'ROOT'
  app.SvcOptMapping.append('LHCb::OnlineEvtSelector/EventSelector')
  app.SvcOptMapping.append('LHCb::FmcMessageSvc/MessageSvc')

  mon = Online.monSvc()  
  mon.PartitionName   = Online.PartitionName
  mon.ProgramName     = nick_name
  mep = Online.mepManager(Online.PartitionID,Online.PartitionName,buffs,True)
  sel = Online.mbmSelector(input=buffs[0],type='USER',decode=False)
  data = Online.evtDataSvc()
  pers = Online.rawPersistencySvc()
  
  app.EvtSel  = sel
  app.Runable = Online.evtRunable(mep)
  app.Runable.NumErrorToStop = -1;
  app.ExtSvc.append(mep)
  app.ExtSvc.append(sel)
  app.AuditAlgorithms = False
  Configs.MonitorSvc().OutputLevel = MSG_ERROR
  Configs.RootHistCnv__PersSvc("RootHistSvc").OutputLevel = MSG_ERROR
  app.OutputLevel = MSG_WARNING

  # Setup UpdateAndReset options for saving histograms
  uar = Configs.UpdateAndReset()
  uar.saveHistograms       = 1
  uar.saverCycle           = 600 #seconds between savings
  uar.resetHistosAfterSave = 1
  uar.saveSetDir           = "/hist/Savesets/"
  app.TopAlg.insert(0,uar)

  return app

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
  msg.doPrintAlways = False
  msg.OutputLevel   = Online.OutputLevel

#============================================================================================================
def start():
  """
        Finish configuration and configure Gaudi application manager

        @author M.Frank
  """
  import OnlineEnv as Online
  Online.end_config(False)
  #Online.end_config(True)

