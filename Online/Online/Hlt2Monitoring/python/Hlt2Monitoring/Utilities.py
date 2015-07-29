import os, sys, socket, re
import importlib

host_regex = r"hlt(0[12]|(?P<subfarm>[a-f]{1}[0-9]{2})(?P<node>[0-9]{2})?)"

connections = {'Hlt2RootPublishSvc' : {'back' : "ipc:///tmp/hlt2MonData_2"},
               'Hlt2MonInfoSvc'     : {'back' : "ipc:///tmp/hlt2MonInfo_1"},
               'Hlt2AdderSvc'       : {'back' : "ipc:///tmp/hlt2MonData_1"}}

## The next few lines make the OnlineNodeEnv do nothig, we don't need it anyway.
class EmptyNodeEnv(object):
    def load_node_info(self):
        pass

sys.modules['OnlineNodeEnv'] = EmptyNodeEnv()

## End hack start regular code
def staticVar(varname, value):
    def decorate(func):
        setattr(func, varname, value)
        return func
    return decorate

@staticVar("Online", None)
def importOnline():
  if importOnline.Online:
    return importOnline.Online
  if 'RUNINFO' in os.environ:
    runinfo = os.environ['RUNINFO']
    d, f = os.path.split(runinfo)
    name, ext = os.path.splitext(f)
    sys.path.insert(1, d)
    Online = importlib.import_module('OnlineEnv')
    sys.path.remove(d)
  else:
    import OnlineEnv as Online
  importOnline.Online = Online
  return Online

def configMsgSvc( app, msgLevel = None ):
    import os
    if 'LOGFIFO' not in os.environ :
        print '# WARNING: LOGFIFO was not set -- not configuring FMCMessageSvc.'
        return

    from Gaudi.Configuration import allConfigurables

    # setup the message service
    from Configurables import LHCb__FmcMessageSvc as MessageSvc
    if 'MessageSvc' in allConfigurables :
        del allConfigurables['MessageSvc']
    msg=MessageSvc('MessageSvc')
    app.MessageSvcType = msg.getType()
    app.SvcOptMapping.append( msg.getFullName() )
    msg.LoggerOnly = True
    msg.fifoPath = os.environ['LOGFIFO']
    OnlineEnv = importOnline()
    msg.OutputLevel = msgLevel if msgLevel != None else OnlineEnv.OutputLevel
    msg.doPrintAlways = False

def configOnline(appMgr, level):
    appMgr.SvcOptMapping         += ["LHCb::OnlineRunable/EmptyEventLoop"]
    appMgr.HistogramPersistency  = "NONE"
    appMgr.EvtSel                = "NONE"
    appMgr.EventLoop             = "LHCb::OnlineRunable/EmptyEventLoop"
    appMgr.Runable               = "LHCb::OnlineRunable/Runable"
    appMgr.OutputLevel = 3

    from Configurables import LHCb__OnlineRunable as Runable
    Runable("Runable").Wait = 3

    from Configurables import AuditorSvc
    AuditorSvc().Auditors = []
    configMsgSvc( appMgr, 2 if level == 'top' else 3 )
    OnlineEnv = importOnline()
    OnlineEnv.end_config(False)
