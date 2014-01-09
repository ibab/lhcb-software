#!/usr/bin/env python
#import sys
# never search for anything from current directory...
#if sys.path[0]=='' : sys.path.pop(0)
from GaudiKernel.ProcessJobOptions import PrintOff, InstallRootLoggingHandler
import logging
PrintOff(999)
InstallRootLoggingHandler(level = logging.CRITICAL)


def start() :
    from Moore.Configuration import Moore
    import OnlineEnv 

    Moore().RunOnline = True
    Moore().RunMonitoringFarm = True

    # TODO: change 0x4 into 0x10000 (NoBias)
    Moore().REQ1 = 'EvType=2;TriggerMask=0xffffffff,0xffffffff,0xffffffff,0xffffffff;VetoMask=0,0,0,0;MaskType=ANY;UserType=USER;Frequency=PERC;Perc=100.0'

    ### default database setup
    Moore().Simulation = False
    Moore().DDDBtag    = 'head-20110302'
    Moore().CondDBtag  = 'head-20110318'
    Moore().UseDBSnapshot = True
    Moore().IgnoreDBHeartBeat = True
    Moore().EnableRunChangeHandler = ( OnlineEnv.HLTType not in ['PA','PassThrough' ] )
    Moore().HistogrammingLevel = 'Line'
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
