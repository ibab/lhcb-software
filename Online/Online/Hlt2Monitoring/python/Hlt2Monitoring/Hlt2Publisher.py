import os, sys, socket, re
import importlib

from Utilities import *

def configurePublisher(app, alg_name, initial_file):
    OnlineEnv = importOnline()

    ## The publisher svc
    from Configurables import Hlt2HistPublishSvc
    publishSvc = Hlt2HistPublishSvc(alg_name)
    publishSvc.DataConnection = connections['Hlt2RootPublishSvc']['back']
    publishSvc.OutputLevel = 3
    publishSvc.PartitionName = OnlineEnv.PartitionName
    publishSvc.InitialFile = initial_file
    publishSvc.HistogramDirectory = alg_name

    ## Add to ExtSvc
    app.ExtSvc += ["Hlt2HistPublishSvc/" + alg_name]


def configure(host_type = None, initial_file = ""):
    OnlineEnv = importOnline()

    ## ApplicationMgr
    from Configurables import ApplicationMgr
    appMgr = ApplicationMgr()


    # from Gaudi.Configuration import importOptions
    # importOptions("$HLT2MONITORINGROOT/options/Hlt2Publisher.opts")

    from Utilities import host_regex
    hostname = socket.gethostname()
    host_re = re.compile(host_regex)
    r = host_re.match(hostname)
    ht = ''
    if host_type and host_type in ('top', 'node', 'subfarm'):
        ht = host_type
    elif r:
        if r.group(1) in ('01', '02'):
            ht = 'top'
        elif r.group('subfarm') and not r.group('node'):
            ht = 'subfarm'
        elif r.group('subfarm') and r.group('node'):
            ht = 'node'

    if not ht:
        print '[ERROR] cannot determine type of host for running.'
        return
    elif ht != 'top':
        print '[ERROR] attempting to start publish service on another node than the top.'
        return

    from Configurables import MonitorSvc
    MonitorSvc().disableDimPropServer = 1
    MonitorSvc().disableDimCmdServer  = 1

    MonitorSvc().ExpandCounterServices = 0;
    MonitorSvc().ExpandNameInfix       = "<part>_x_<program>/"
    MonitorSvc().PartitionName         = OnlineEnv.PartitionName
    MonitorSvc().ProgramName           = 'Moore2Saver_00'

    # setup the histograms and the monitoring service
    from Configurables import RootHistCnv__PersSvc
    RootHistCnv__PersSvc().OutputEnabled = False

    ## Needed services
    appMgr.ExtSvc += ["IncidentSvc", "HistogramSvc", "HistogramDataSvc", "MonitorSvc"]

    for alg in ("Hlt2RoutingBitsWriter", "Hlt2MassMonitor"):
        configurePublisher(appMgr, alg, initial_file)

    configOnline(appMgr, ht)
