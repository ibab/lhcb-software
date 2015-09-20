#!/usr/bin/env python
from GaudiKernel.ProcessJobOptions import PrintOff, InstallRootLoggingHandler
import logging
PrintOff(999)
InstallRootLoggingHandler(level = logging.CRITICAL)

from Gaudi.Configuration import *
import GaudiKernel.ProcessJobOptions

def configMsgSvc(app):
    import os
    from Gaudi.Configuration import allConfigurables

    # Setup the message service
    from Configurables import LHCb__FmcMessageSvc as MessageSvc
    if 'MessageSvc' in allConfigurables:
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
    # msg.OutputLevel = OnlineEnv.OutputLevel
    msg.OutputLevel = 3
    msg.doPrintAlways = False


def configOnline(app, alg, name):
    from Configurables import ApplicationMgr, UpdateAndReset
    appMgr = ApplicationMgr()

    ## Configure saving of histograms
    ur = UpdateAndReset()
    ur.saveHistograms = 1
    appMgr.TopAlg.insert(0, ur)

    from Configurables import LoKiSvc
    LoKiSvc().Welcome = False

    import OnlineEnv
    app.DDDBtag = OnlineEnv.DDDBTag
    app.CondDBtag = OnlineEnv.CondDBTag

    from Configurables import CondDB
    conddb = CondDB()
    conddb.IgnoreHeartBeat = True
    conddb.UseDBSnapshot = True
    conddb.EnableRunChangeHandler = True
    conddb.EnableRunStampCheck = False
    conddb.Tags['ONLINE'] = 'fake'

    import ConditionsMap
    conddb.RunChangeHandlerConditions = ConditionsMap.RunChangeHandlerConditions

    from Gaudi.Configuration import EventLoopMgr
    EventLoopMgr().Warnings = False

    from Configurables import MonitorSvc
    MonitorSvc().disableDimPropServer      = 1
    MonitorSvc().disableDimCmdServer       = 1

    MonitorSvc().ExpandCounterServices = 0;
    MonitorSvc().ExpandNameInfix       = "<part>_x_<program>/";
    MonitorSvc().PartitionName         = OnlineEnv.PartitionName;
    MonitorSvc().ProgramName           = name + "_00";

    # setup the histograms and the monitoring service
    appMgr.ExtSvc.append( 'MonitorSvc' )
    from Configurables import RootHistCnv__PersSvc
    RootHistCnv__PersSvc().OutputEnabled = False

    from Gaudi.Configuration import allConfigurables
    # set up the event selector
    if 'EventSelector' in allConfigurables :
        del allConfigurables['EventSelector']

    input = 'Events'
    mepMgr = OnlineEnv.mepManager(OnlineEnv.PartitionID,OnlineEnv.PartitionName,[input],True)
    mepMgr.ConnectWhen = "start";

    appMgr.Runable = OnlineEnv.evtRunable(mepMgr)
    appMgr.SvcOptMapping.append('LHCb::OnlineEvtSelector/EventSelector')
    appMgr.SvcOptMapping.append('LHCb::FmcMessageSvc/MessageSvc')
    appMgr.ExtSvc.append(mepMgr)

    eventSelector = OnlineEnv.mbmSelector(input = input, type = 'ONE', decode = False, event_type = 2)
    appMgr.ExtSvc.append(eventSelector)
    appMgr.ExtSvc.append(mepMgr)
    appMgr.OutputLevel = 3

    OnlineEnv.evtDataSvc()
    eventSelector.REQ1 = 'EvType=2;TriggerMask=0x0,0x4,0x0,0x0;VetoMask=0,0,0,0x300;MaskType=ANY;UserType=USER;Frequency=PERC;Perc=100.0'
    appMgr.EvtSel = eventSelector

    from Configurables import AuditorSvc
    AuditorSvc().Auditors = []
    configMsgSvc( appMgr )
    OnlineEnv.end_config(False)

def configMonitor():
    import os
    from Gaudi.Configuration import EventPersistencySvc, HistogramPersistencySvc
    from Configurables import (LHCbApp,
                               LHCb__RawDataCnvSvc,
                               GaudiSequencer,
                               UpdateAndReset,
                               createODIN,
                               ApplicationMgr )

    app = LHCbApp()
    app.DataType = '2015'
    app.EvtMax = -1

    EventPersistencySvc().CnvServices.append( LHCb__RawDataCnvSvc('RawDataCnvSvc') )
    HistogramPersistencySvc().OutputFile = ''
    HistogramPersistencySvc().Warnings = False

    UpdateAndReset().saveHistograms = 1
    #UpdateAndReset().saverCycle     = 3600

    from Configurables import EventClockSvc
    EventClockSvc().EventTimeDecoder = 'OdinTimeDecoder'

    appMgr = ApplicationMgr()

    # Decoder
    from Configurables import HCRawBankDecoder
    decoder = HCRawBankDecoder()
    decoder.Monitoring = True

    # Monitor
    from Configurables import HCDigitMonitor
    monitor = HCDigitMonitor()
    monitor.CrateB = 0
    monitor.CrateF = 1
    monitor.ChannelsB0 = [47, 46, 45, 44]
    monitor.ChannelsB1 = [23, 22, 21, 20]
    monitor.ChannelsB2 = [11, 10,  9,  8]
    monitor.ChannelsF1 = [23, 22, 21, 46]
    monitor.ChannelsF2 = [11, 10,  9,  8]

    # Top level sequence
    topSeq = GaudiSequencer("TopSequence")
    topSeq.Members = [createODIN(), decoder, monitor]

    appMgr.TopAlg = [topSeq]
    return app, monitor

def start(online) :
    app, alg = configMonitor()
    if online == True: configOnline(app, alg, "HCDigitMonitor")
