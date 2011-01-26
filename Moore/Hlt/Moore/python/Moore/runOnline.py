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

    ### default database setup -- require an explit tag when running in the LHCb partition...
    Moore().Simulation = False
    Moore().DDDBtag    = 'hlt-20100906' if OnlineEnv.PartitionName != 'LHCb' else 'unknown'
    Moore().CondDBtag  = 'hlt-20100906' if OnlineEnv.PartitionName != 'LHCb' else 'unknown'

    Moore().UseDBSnapshot = True
    Moore().IgnoreDBHeartBeat = True
    Moore().EnableRunChangeHandler = ( OnlineEnv.HLTType not in ['PA','PassThrough' ] )

    if OnlineEnv.PartitionName == 'FEST' :
       Moore().EnableRunChangeHandler = False
       # Moore().Simulation = True
       # Moore().DDDBtag   = 'MC09-20090602'
       # Moore().CondDBtag = 'sim-20090402-vc-md100'

    ### pick up requested DB tag 
    if hasattr(OnlineEnv,'CondDBTag') and OnlineEnv.CondDBTag :
       tags = OnlineEnv.CondDBTag.split('|')
       Moore().CondDBtag = tags[0]
       Moore().DDDBtag   = tags[1]
       Moore().UseDBSnapshot = True

    # Forward all attributes of 'OnlineEnv' to the job options service...
    from GaudiKernel.Proxy.Configurable import ConfigurableGeneric
    c = ConfigurableGeneric("OnlineEnv")
    #[ setattr(c,k,v) for (k,v) in OnlineEnv.__dict__.items() if k not in OnlineConfig.__dict__ ]
    c.AcceptRate = OnlineEnv.AcceptRate

    OnlineEnv.end_config(False)
