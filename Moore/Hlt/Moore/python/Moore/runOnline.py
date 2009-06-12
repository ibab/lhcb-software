#!/usr/bin/env gaudirun.py

print '---------------------------Moore.runOnline-----------------------'
from Moore.Configuration import Moore
import Gaudi,GaudiKernel.ProcessJobOptions
from Gaudi.Configuration import importOptions
GaudiKernel.ProcessJobOptions.printing_level=999


def start() :

    Moore().RunOnline = True
    print Moore()
    
    import OnlineEnv as Online
    from GaudiKernel.Proxy.Configurable import ConfigurableGeneric
    ConfigurableGeneric("OnlineEnv").AcceptRate = Online.AcceptRate

    Online.end_config(False)
