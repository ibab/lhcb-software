#!/usr/bin/env gaudirun.py
#
# Minimal file for running LumiMon from python prompt
# Syntax is:
#   gaudirun.py ../options/lumiMon.py
# or just
#   ../options/Moore.py
#
from Gaudi.Configuration import *
from LumiMonitor.Configuration import *


#from Gaudi import *

def setupOnline():
  """
        Setup the online environment: Buffer managers, event serialisation, etc.

        @author M.Frank
  """
  import OnlineEnv as Online
  ##app=Gaudi.ApplicationMgr()
  app=ApplicationMgr()
  app.AppName = ''
  app.HistogramPersistency = 'ROOT'
  app.SvcOptMapping.append('LHCb::OnlineEvtSelector/EventSelector')
#  app.SvcOptMapping.append('LHCb::FmcMessageSvc/MessageSvc')
  mepMgr = Online.mepManager(Online.PartitionID,Online.PartitionName,['Events'],True)
  app.EvtSel  = Online.mbmSelector(input='Events',type='USER',decode=False)
  app.Runable = Online.evtRunable(mepMgr)
  app.ExtSvc.append(mepMgr)
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


#//---------------------------------------------------------------------------
#// Histogram output
#//---------------------------------------------------------------------------
ApplicationMgr().HistogramPersistency = 'NONE'

# the following gives the right db tag
importOptions('$DDDBROOT/options/LHCb-2008.py')

LumiMonitor().hltType = 'readBuffer'
LumiMonitor().oldStyle = True
##    files = [ 'mdfData_run_spd.mdf' ]   # this is a local test file of nano events
filetype='RAW'
LumiMonitor().inputType = filetype

extensions = {
    'RAW' : "' SVC='LHCb::MDFSelector'",
    'MDF' : "' SVC='LHCb::MDFSelector'",
    'DST' : "' TYP='POOL_ROOTTREE' OPT='READ'" }

# instantiate the user algorithms
LumiMonitor().userAlgorithms=['LumiAnalyser']

# tell we are online
LumiMonitor().online=True

# optionally, we can enable some auditors...
# LumiMonitor().enableAuditor = [ NameAuditor() ]
# always, always applyConf!
LumiMonitor().applyConf()

## UpdateAndReset.saveHistograms=1
## UpdateAndReset.saverCycle=10
## UpdateAndReset.resetHistosAfterSave=0
## UpdateAndReset.saveSetDir="/hist/Savesets"


# online
setupOnline()
LumiMonitor().reallyApplyConf()

# and tell the world how we are configured
print LumiMonitor()

def run():
  import OnlineEnv as Online
  Online.end_config(False)


