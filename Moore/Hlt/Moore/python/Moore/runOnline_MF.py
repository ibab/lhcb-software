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
    Moore().REQ1 = 'EvType=2;TriggerMask=0x0,0x4,0x0,0x0;VetoMask=0,0,0,0x700;MaskType=ANY;UserType=USER;Frequency=PERC;Perc=100.0'

    ### default database setup
    Moore().Simulation = False
    Moore().DDDBtag    = 'hlt-20100906'
    Moore().CondDBtag  = 'hlt-20100906'
    Moore().UseDBSnapshot = True
    Moore().IgnoreDBHeartBeat = True
    Moore().EnableRunChangeHandler = ( OnlineEnv.HLTType not in ['PA','PassThrough' ] )

    if OnlineEnv.PartitionName == 'FEST' :
       Moore().EnableRunChangeHandler = False
       # Moore().Simulation = True
       # Moore().DDDBtag   = 'MC09-20090602'
       # Moore().CondDBtag = 'sim-20090402-vc-md100'

    ### pick up requested DB tag 
    ### use old capitalization
    if hasattr(OnlineEnv,'condDBTag') and OnlineEnv.condDBTag :
        Moore().CondDBtag = OnlineEnv.condDBTag
        #Moore().DDDBtag   = OnlineEnv.condDBTag
        Moore().UseDBSnapshot = True
    ### canonical capitalization
    if hasattr(OnlineEnv,'CondDBTag') and OnlineEnv.CondDBTag :
        Moore().CondDBtag = OnlineEnv.CondDBTag
        #Moore().DDDBtag   = OnlineEnv.CondDBTag
        Moore().UseDBSnapshot = True

    # Forward all attributes of 'OnlineEnv' to the job options service...
    from GaudiKernel.Proxy.Configurable import ConfigurableGeneric
    c = ConfigurableGeneric("OnlineEnv")
    #[ setattr(c,k,v) for (k,v) in OnlineEnv.__dict__.items() if k not in OnlineConfig.__dict__ ]
    c.AcceptRate = OnlineEnv.AcceptRate
    c.Monitor = True
    OnlineEnv.end_config(False)
