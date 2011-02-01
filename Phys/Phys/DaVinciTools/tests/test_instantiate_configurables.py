#!/usr/bin/env python

if __name__ == '__main__' :

    import sys
    from Gaudi.Configuration import *
    from GaudiConfUtils import getConfigurablesFromModule
    import DaVinciTools.DaVinciToolsConf
    
    errors = ['======== Error Summary =========================================']
    successes = ['======== Success Summary =======================================']

    components = getConfigurablesFromModule(DaVinciTools.DaVinciToolsConf)

    for component in components :
        try :
            c = component()
            successes.append('SUCCESS: Instantiated Configurable ' + c.name() )
        except :
            errors.append('ERROR: Failed to instantiate Configurable ' + str(component) )

    errors.append('================================================================')
    successes.append('================================================================')
    if len(errors) > 2  :
        for error in errors :
            sys.stderr.write(error + '\n')

    if len(successes) > 2  :
        for success in successes :
            sys.stdout.write(success+'\n')



