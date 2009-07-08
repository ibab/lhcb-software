#!/usr/bin/env gaudirun.py
from GaudiKernel.ProcessJobOptions import PrintOff, InstallRootLoggingHandler
import logging
PrintOff(999)
InstallRootLoggingHandler(level = logging.CRITICAL)


def start() :
    from Moore.Configuration import Moore

    Moore().RunOnline = True
    
    # Forward all attributes of 'OnlineEnv' to the job options service...
    import OnlineEnv 
    from GaudiKernel.Proxy.Configurable import ConfigurableGeneric
    c = ConfigurableGeneric("OnlineEnv")
    #[ setattr(c,k,v) for (k,v) in OnlineEnv.__dict__.items() if k not in OnlineConfig.__dict__ ]
    c.AcceptRate = OnlineEnv.AcceptRate

    OnlineEnv.end_config(False)
