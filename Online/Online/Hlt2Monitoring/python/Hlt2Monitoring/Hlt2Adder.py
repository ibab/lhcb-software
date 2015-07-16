import os, sys, socket, re

__ports = {'HistoRelay' : {'in' : 31337, 'out' : 31338},
           'InfoRelay'  : {'in' : 31339, 'out' : 31340},
           'Adder'      : {'out' :31341},
           'InfoSvc'    : {'out' : 31342}
          }

def configMsgSvc( app ):
    import os
    from Gaudi.Configuration import allConfigurables

    # setup the message service
    from Configurables import LHCb__FmcMessageSvc as MessageSvc
    if 'MessageSvc' in allConfigurables :
        del allConfigurables['MessageSvc']
    msg=MessageSvc('MessageSvc')
    app.MessageSvcType = msg.getType()
    app.SvcOptMapping.append( msg.getFullName() )
    msg.LoggerOnly = True
    if 'LOGFIFO' not in os.environ :
        os.environ['LOGFIFO'] = '/tmp/logGaudi.fifo'
        log.warning( '# WARNING: LOGFIFO was not set -- now set to ' + os.environ['LOGFIFO'] )
    msg.fifoPath = os.environ['LOGFIFO']
    import OnlineEnv
    msg.OutputLevel = OnlineEnv.OutputLevel
    msg.doPrintAlways = False

def configOnline(appMgr):
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
    configMsgSvc( appMgr )
    OnlineEnv.end_config(False)

def configureTop(appMgr, node_info):
    ## Additional Services
    appMgr.ExtSvc += ["Hlt2AdderSvc", "Hlt2MonInfoSvc", "Hlt2RootPublishSvc", "Hlt2SaverSvc"]

    ## The top info relay ports
    from Configurables import Hlt2MonRelaySvc
    infoRelay = Hlt2MonRelaySvc("InfoRelay")
    infoRelay.FrontConnection = "tcp://*:%d" % __ports['InfoRelay']['in']
    infoRelay.BackConnection  = "ipc:///tmp/hlt2MonInfo_0"

    ## The top histo relay ports
    from Configurables import Hlt2MonRelaySvc
    histoRelay = Hlt2MonRelaySvc("HistoRelay")
    histoRelay.FrontConnection = "tcp://hlt01:%d" % __ports['HistoRelay']['in']
    histoRelay.BackConnection  = "ipc:///tmp/hlt2MonData_0"

    ## The histogram adder service
    from Configurables import Hlt2AdderSvc
    adderSvc = Hlt2AdderSvc()
    adderSvc.FrontConnection = histoRelay.BackConnection
    adderSvc.BackConnection  = "ipc:///tmp/hlt2MonData_1"

    ## The info svc ports
    from Configurables import Hlt2MonInfoSvc
    infoSvc = Hlt2MonRelaySvc()
    infoSvc.FrontConnection = infoRelay.BackConnection
    infoSvc.BackConnection  = "ipc://hlt2MonInfo_1"

    ## The root conversion service
    from Configurables import Hlt2RootPublishSvc
    rootSvc = Hlt2RootPublishSvc()
    rootSvc.FrontConnection = adderSvc.BackConnection
    rootSvc.BackConnection  = "ipc:///tmp/hlt2MonData_2"
    rootSvc.InfoConnection  = infoSvc.BackConnection

    ## The saver svc
    from Configurables import Hlt2MonInfoSvc
    saverSvc = Hlt2SaverSvc()
    saverSvc.DataConnection = rootSvc.BackConnection
    saverSvc.InfoConnection = infoSvc.BackConnection

def configureSubfarm(appMgr, node_info):
    ## The info relay ports
    from Configurables import Hlt2MonRelaySvc
    infoRelay = Hlt2MonRelaySvc("InfoRelay")
    infoRelay.InPort  = __ports['InfoRelay']['in']

    ## The histo relay ports are the same, but on different nodes
    from Configurables import Hlt2MonRelaySvc
    histoRelay = Hlt2MonRelaySvc("HistoRelay")
    histoRelay.InPort  = __ports['HistoRelay']['in']

def configureNode(appMgr, node_info):
    ## The info relay svc needs to be explicitly configured
    from Configurables import Hlt2MonRelaySvc
    infoRelay = Hlt2MonRelaySvc("InfoRelay")
    infoRelay.FrontConnection = "ipc:///tmp/hlt2MonInfo_0"
    infoRelay.BackConnection  = 'tcp://hlt%s:%d' % (node_info['subfarm'], __ports['InfoRelay']['in'])

    ## The histo relay port to connect to on the subfarm relay
    from Configurables import Hlt2MonRelaySvc
    histoRelay = Hlt2MonRelaySvc("HistoRelay")
    histoRelay.InPort = __ports['HistoRelay']['in']

def configure(host_type = None):

    from Configurables import ApplicationMgr
    appMgr = ApplicationMgr()

    hostname = socket.gethostname()
    host_regex = re.compile(r"hlt(01|(?<subfarm>[a-f]{1}[0-9]{2})(?<node>[0-9]{2})?)")
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
    configs[ht](appMgr, node_info)

    configOnline(appMgr)
