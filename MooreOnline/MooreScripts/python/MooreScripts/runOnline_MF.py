#!/usr/bin/env python
#import sys
# never search for anything from current directory...
#if sys.path[0]=='' : sys.path.pop(0)
from GaudiKernel.ProcessJobOptions import PrintOff, InstallRootLoggingHandler
import logging
PrintOff(999)
InstallRootLoggingHandler(level = logging.CRITICAL)


def start() :
    from Moore.Configuration import Moore, MooreExpert
    from MooreOnlineConf.Configuration import MooreOnline
    import OnlineEnv 

    MooreOnline().RunOnline = True
    MooreOnline().RunMonitoringFarm = True

    # TODO: change 0x4 into 0x10000 (NoBias)
    MooreOnline().REQ1 = 'EvType=2;TriggerMask=0xffffffff,0xffffffff,0xffffffff,0xffffffff;VetoMask=0,0,0,0;MaskType=ANY;UserType=USER;Frequency=PERC;Perc=100.0'

    ### default database setup
    #not needed Moore().Simulation = False
    Moore().DDDBtag    = 'head-20110302'
    Moore().CondDBtag  = 'head-20110318'
    #not needed MooreOnline().UseDBSnapshot = True
    #not needed Moore().IgnoreDBHeartBeat = True
    MooreOnline().EnableRunChangeHandler = ( OnlineEnv.HLTType not in ['PA','PassThrough' ] )
    #not needed Moore().HistogrammingLevel = 'Line'
    #Moore().Verbose = True
    
    ### pick up requested DB tags
    if hasattr(OnlineEnv,'CondDBTag') and OnlineEnv.CondDBTag : Moore().CondDBtag = OnlineEnv.CondDBTag
    if hasattr(OnlineEnv,'DDDBTag')   and OnlineEnv.DDDBTag   : Moore().DDDBtag   = OnlineEnv.DDDBTag

    from Configurables import MonitorSvc
    MonitorSvc().ExpandCounterServices = 1;
    MonitorSvc().ExpandNameInfix       = "<part>_x_<program>/";
    MonitorSvc().PartitionName         = OnlineEnv.PartitionName;
    MonitorSvc().ProgramName           = "HltExpertMon_00";
    
	
    from Configurables import UpdateAndReset
    UpdateAndReset().saveHistograms = 1	
    #UpdateAndReset().saverCycle     = 3600

    # Forward all attributes of 'OnlineEnv' to the job options service...
    from GaudiKernel.Proxy.Configurable import ConfigurableGeneric
    c = ConfigurableGeneric("OnlineEnv")
    #[ setattr(c,k,v) for (k,v) in OnlineEnv.__dict__.items() if k not in OnlineConfig.__dict__ ]
    c.AcceptRate = OnlineEnv.AcceptRate
    c.Monitor = True
    OnlineEnv.end_config(False)
