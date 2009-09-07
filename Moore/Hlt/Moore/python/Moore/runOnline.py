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
    	Moore().CondDBtag = 'MC09-20090602'
    	Moore().DDDBtag   = 'sim-20090402-vc-md100'
    else : # TODO: make these very specific tags, not default!
    	Moore().CondDBtag = 'default'
    	Moore().DDDBtag   = 'default'
    
    # Forward all attributes of 'OnlineEnv' to the job options service...
    from GaudiKernel.Proxy.Configurable import ConfigurableGeneric
    c = ConfigurableGeneric("OnlineEnv")
    #[ setattr(c,k,v) for (k,v) in OnlineEnv.__dict__.items() if k not in OnlineConfig.__dict__ ]
    c.AcceptRate = OnlineEnv.AcceptRate

    OnlineEnv.end_config(False)
