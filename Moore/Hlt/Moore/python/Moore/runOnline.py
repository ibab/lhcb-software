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
    # TODO: record these tags somewhere...
    if OnlineEnv.PartitionName == 'FEST' :
       Moore().Simulation = True
       Moore().DDDBtag   = 'MC09-20090602'
       Moore().CondDBtag = 'sim-20090402-vc-md100'
       Moore().UseSnapshotOnline = True
    else : # TODO: make these very specific tags, not default!
       Moore().Simulation = False
       Moore().CondDBtag = 'head-20090713'
       Moore().DDDBtag   = 'head-20090713'
       Moore().UseSnapshotOnline = True
    
    # Forward all attributes of 'OnlineEnv' to the job options service...
    from GaudiKernel.Proxy.Configurable import ConfigurableGeneric
    c = ConfigurableGeneric("OnlineEnv")
    #[ setattr(c,k,v) for (k,v) in OnlineEnv.__dict__.items() if k not in OnlineConfig.__dict__ ]
    c.AcceptRate = OnlineEnv.AcceptRate

    OnlineEnv.end_config(False)
