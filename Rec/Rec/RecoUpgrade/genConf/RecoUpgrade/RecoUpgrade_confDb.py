##  -*- python -*-  
# db file automatically generated by genconf on: Thu Mar 25 16:18:13 2010
## insulates outside world against anything bad that could happen
## also prevents global scope pollution
def _fillCfgDb():
    from GaudiKernel.Proxy.ConfigurableDb import CfgDb

    # get a handle on the repository of Configurables
    cfgDb = CfgDb()

    # populate the repository with informations on Configurables 

    cfgDb.add( configurable = 'VeloPixChecker',
               package = 'RecoUpgrade',
               module  = 'RecoUpgrade.RecoUpgradeConf',
               lib     = 'RecoUpgrade' )
    cfgDb.add( configurable = 'TrackAssociatorUpgrade',
               package = 'RecoUpgrade',
               module  = 'RecoUpgrade.RecoUpgradeConf',
               lib     = 'RecoUpgrade' )
    cfgDb.add( configurable = 'Tf__PatVeloPixFitLHCbIDs',
               package = 'RecoUpgrade',
               module  = 'RecoUpgrade.RecoUpgradeConf',
               lib     = 'RecoUpgrade' )
    cfgDb.add( configurable = 'PatLHCbIDUp2MCParticle',
               package = 'RecoUpgrade',
               module  = 'RecoUpgrade.RecoUpgradeConf',
               lib     = 'RecoUpgrade' )
    cfgDb.add( configurable = 'PatLHCbIDUp2MCHit',
               package = 'RecoUpgrade',
               module  = 'RecoUpgrade.RecoUpgradeConf',
               lib     = 'RecoUpgrade' )
    cfgDb.add( configurable = 'CheatedVeloPixPat',
               package = 'RecoUpgrade',
               module  = 'RecoUpgrade.RecoUpgradeConf',
               lib     = 'RecoUpgrade' )
    return #_fillCfgDb
# fill cfgDb at module import...
try:
    _fillCfgDb()
    #house cleaning...
    del _fillCfgDb
except Exception,err:
    print "Py:ConfigurableDb   ERROR Problem with [%s] content!" % __name__
    print "Py:ConfigurableDb   ERROR",err
    print "Py:ConfigurableDb   ERROR   ==> culprit is package [RecoUpgrade] !"
