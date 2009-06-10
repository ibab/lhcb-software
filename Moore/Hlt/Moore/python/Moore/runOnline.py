#!/usr/bin/env gaudirun.py

print '---------------------------Moore.runOnline-----------------------'
from Moore.Configuration import Moore


def start(TAE=False) :

    Moore().RunOnline = True
    Moore().TAE       = TAE

    print Moore()
    
    import OnlineEnv
    OnlineEnv.end_config(False)
