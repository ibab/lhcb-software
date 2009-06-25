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
    for k,v in OnlineEnv.__dict__.iteritems() : setattr(c,k,v)

    OnlineEnv.end_config(False)
