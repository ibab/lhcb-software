import os, sys, socket, re
import importlib

__ports = {'HistoRelay' : {'in' : 31337, 'out' : 31338},
           'InfoRelay'  : {'in' : 31339, 'out' : 31340},
           'Adder'      : {'out' :31341},
           'InfoSvc'    : {'out' : 31342}
          }

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

def configureTop(appMgr, node_info):
    ## Additional Services
    appMgr.ExtSvc += ["Hlt2AdderSvc", "Hlt2MonInfoSvc", "Hlt2RootPublishSvc", "Hlt2SaverSvc"]

    ## The top info relay ports
    from Configurables import Hlt2MonRelaySvc
    infoRelay = Hlt2MonRelaySvc("InfoRelay")
    infoRelay.FrontConnection = "tcp://*:%d" % __ports['InfoRelay']['in']
    infoRelay.BackConnection  = "ipc:///tmp/hlt2MonInfo_0"
    infoRelay.OutputLevel = 3

    ## The top histo relay ports
    from Configurables import Hlt2MonRelaySvc
    histoRelay = Hlt2MonRelaySvc("HistoRelay")
    histoRelay.FrontConnection = "tcp://*:%d" % __ports['HistoRelay']['in']
    histoRelay.BackConnection  = "ipc:///tmp/hlt2MonData_0"
    histoRelay.OutputLevel = 3

    ## The histogram adder service
    from Configurables import Hlt2AdderSvc
    adderSvc = Hlt2AdderSvc()
    adderSvc.FrontConnection = histoRelay.BackConnection
    adderSvc.BackConnection  = "ipc:///tmp/hlt2MonData_1"
    adderSvc.OutputLevel = 3

    ## The info svc ports
    from Configurables import Hlt2MonInfoSvc
    infoSvc = Hlt2MonInfoSvc()
    infoSvc.FrontConnection = infoRelay.BackConnection
    infoSvc.BackConnection  = "ipc:///tmp/hlt2MonInfo_1"
    infoSvc.OutputLevel = 3

    ## The root conversion service
    from Configurables import Hlt2RootPublishSvc
    rootSvc = Hlt2RootPublishSvc()
    rootSvc.FrontConnection = adderSvc.BackConnection
    rootSvc.BackConnection  = "ipc:///tmp/hlt2MonData_2"
    rootSvc.InfoConnection  = infoSvc.BackConnection
    rootSvc.OutputLevel = 3

    ## The saver svc
    from Configurables import Hlt2SaverSvc
    saverSvc = Hlt2SaverSvc()
    saverSvc.DataConnection = rootSvc.BackConnection
    saverSvc.InfoConnection = infoSvc.BackConnection
    saverSvc.OutputLevel = 2

    return (infoRelay, histoRelay, adderSvc, infoSvc, rootSvc, saverSvc)

def configureSubfarm(appMgr, node_info):
    ## The info relay ports
    from Configurables import Hlt2MonRelaySvc
    infoRelay = Hlt2MonRelaySvc("InfoRelay")
    infoRelay.InPort  = __ports['InfoRelay']['in']
    infoRelay.ControlConnection = "ipc:///tmp/hlt2InfoControl"

    ## The histo relay ports are the same, but on different nodes
    from Configurables import Hlt2MonRelaySvc
    histoRelay = Hlt2MonRelaySvc("HistoRelay")
    histoRelay.InPort  = __ports['HistoRelay']['in']
    histoRelay.ControlConnection = "ipc:///tmp/hlt2HistoControl"

    return (infoRelay, histoRelay)

def configureNode(appMgr, node_info):
    ## The info relay svc needs to be explicitly configured
    from Configurables import Hlt2MonRelaySvc
    infoRelay = Hlt2MonRelaySvc("InfoRelay")
    infoRelay.ControlConnection = "ipc:///tmp/hlt2InfoControl"
    infoRelay.FrontConnection = "ipc:///tmp/hlt2MonInfo_0"
    infoRelay.BackConnection  = 'tcp://hlt%s:%d' % (node_info['subfarm'], __ports['InfoRelay']['in'])

    ## The histo relay port to connect to on the subfarm relay
    from Configurables import Hlt2MonRelaySvc
    histoRelay = Hlt2MonRelaySvc("HistoRelay")
    histoRelay.ControlConnection = "ipc:///tmp/hlt2HistoControl"
    histoRelay.InPort = __ports['HistoRelay']['in']

    return (infoRelay, histoRelay)

def configure(host_type = None):

    from Configurables import ApplicationMgr
    appMgr = ApplicationMgr()

    hostname = socket.gethostname()
    host_regex = re.compile(r"hlt(0[12]|(?P<subfarm>[a-f]{1}[0-9]{2})(?P<node>[0-9]{2})?)")
    r = host_regex.match(hostname)
    ht = ''
    if host_type and host_type in services:
        ht = host_type
    elif r:
        if r.group(1) == '01':
            ht = 'top'
        elif r.group('subfarm') and not r.group('node'):
            ht = 'subfarm'
        elif r.group('subfarm') and r.group('node'):
            ht = 'node'

        info = {'top' : lambda r: r.group(1) == '01',
                'subfarm' : lambda r: r.group('subfarm'),
                'node' : lambda r: r.group('node')}
        node_info = {k : v(r) for k, v in info.iteritems()}

    if not ht:
        print '[ERROR] cannot determine type of host for running.'
        return

    ## IncidentSvc and relay services are needed everywhere
    appMgr.ExtSvc += ["IncidentSvc", "Hlt2MonRelaySvc/HistoRelay", "Hlt2MonRelaySvc/InfoRelay"]

    ## Now configure services specific to levels
    configs = {'node' : configureNode,
               'subfarm' : configureSubfarm,
               'top' : configureTop}
    svcConfs = configs[ht](appMgr, node_info)

    ## Set the partition we are running in
    OnlineEnv = importOnline()
    for svcConf in svcConfs:
        svcConf.PartitionName = OnlineEnv.PartitionName

    configOnline(appMgr, ht)
