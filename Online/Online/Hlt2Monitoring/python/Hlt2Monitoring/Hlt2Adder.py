import os, sys, socket, re
import importlib

from Utilities import *

__ports = {'HistoRelay' : {'in' : 31337, 'out' : 31338},
           'InfoRelay'  : {'in' : 31339, 'out' : 31340},
           'Adder'      : {'out' :31341},
           'InfoSvc'    : {'out' : 31342}
          }

def configureTop(appMgr, node_info):
    ## Additional Services
    appMgr.ExtSvc += ["Hlt2AdderSvc", "Hlt2MonInfoSvc", "Hlt2RootPublishSvc", "Hlt2SaverSvc"]

    ports = node_info['ports']

    ## The top info relay ports
    from Configurables import Hlt2MonRelaySvc
    infoRelay = Hlt2MonRelaySvc("InfoRelay")
    infoRelay.FrontConnection = "tcp://*:%d" % ports['InfoRelay']['in']
    infoRelay.BackConnection  = "ipc:///tmp/hlt2MonInfo_0"
    infoRelay.Capture = True
    infoRelay.OutputLevel = 3

    ## The top histo relay ports
    from Configurables import Hlt2MonRelaySvc
    histoRelay = Hlt2MonRelaySvc("HistoRelay")
    histoRelay.FrontConnection = "tcp://*:%d" % ports['HistoRelay']['in']
    histoRelay.BackConnection  = "ipc:///tmp/hlt2MonData_0"
    histoRelay.Capture = True
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
    if 'HistogramDirectory' in node_info:
        saverSvc.BaseDirectory = node_info['HistogramDirectory']
    saverSvc.OutputLevel = 3

    return (infoRelay, histoRelay, adderSvc, infoSvc, rootSvc, saverSvc)

def configureSubfarm(appMgr, node_info):
    ## The info relay ports
    from Configurables import Hlt2MonRelaySvc
    infoRelay = Hlt2MonRelaySvc("InfoRelay")
    infoRelay.InPort  = ports['InfoRelay']['in']

    ## The histo relay ports are the same, but on different nodes
    from Configurables import Hlt2MonRelaySvc
    histoRelay = Hlt2MonRelaySvc("HistoRelay")
    histoRelay.InPort  = ports['HistoRelay']['in']

    return (infoRelay, histoRelay)

def configureNode(appMgr, node_info):
    ## The info relay svc needs to be explicitly configured
    from Configurables import Hlt2MonRelaySvc
    infoRelay = Hlt2MonRelaySvc("InfoRelay")
    infoRelay.FrontConnection = "ipc:///tmp/hlt2MonInfo_0"
    infoRelay.BackConnection  = 'tcp://hlt%s:%d' % (node_info['subfarm'], ports['InfoRelay']['in'])

    ## The histo relay port to connect to on the subfarm relay
    from Configurables import Hlt2MonRelaySvc
    histoRelay = Hlt2MonRelaySvc("HistoRelay")
    histoRelay.InPort = ports['HistoRelay']['in']

    return (infoRelay, histoRelay)

def configure(host_type = None, directory = None, ports = None):

    from Configurables import ApplicationMgr
    appMgr = ApplicationMgr()

    ## configure services
    configs = {'node' : configureNode,
               'subfarm' : configureSubfarm,
               'top' : configureTop}
    node_info = {}

    if not ports:
        node_info['ports'] = __ports
    else:
        node_info['ports'] = ports

    hostname = socket.gethostname()
    host_regex = re.compile(r"hlt(0[12]|(?P<subfarm>[a-f]{1}[0-9]{2})(?P<node>[0-9]{2})?)")
    r = host_regex.match(hostname)
    ht = ''
    if host_type and host_type in configs:
        ht = host_type
    elif r:
        if r.group(1) in ('01', '02'):
            ht = 'top'
        elif r.group('subfarm') and not r.group('node'):
            ht = 'subfarm'
        elif r.group('subfarm') and r.group('node'):
            ht = 'node'

        info = {'top' : lambda r: r.group(1) == '01',
                'subfarm' : lambda r: r.group('subfarm'),
                'node' : lambda r: r.group('node')}
        node_info.update({k : v(r) for k, v in info.iteritems()})

    if not ht:
        print '[ERROR] cannot determine type of host for running.'
        return

    if directory and host_type == 'top':
        node_info['HistogramDirectory'] = directory

    ## IncidentSvc and relay services are needed everywhere
    appMgr.ExtSvc += ["IncidentSvc", "Hlt2MonRelaySvc/HistoRelay", "Hlt2MonRelaySvc/InfoRelay"]

    svcConfs = configs[ht](appMgr, node_info)

    ## Set the partition we are running in
    OnlineEnv = importOnline()
    for svcConf in svcConfs:
        svcConf.PartitionName = OnlineEnv.PartitionName

    configOnline(appMgr, ht)
