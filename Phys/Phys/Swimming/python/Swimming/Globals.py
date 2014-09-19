#Defines the "global" objects to be
#used across the various modules and 
#functions
from Configurables import Swimming
#from GaudiKernel.PhysicalConstants import c_light
from ROOT import Double

__all__ = ['GlobalParams']

class GlobalParams(object):
    def __init__(self, gaudi, triggers):
        from GaudiPython import InterfaceCast
        tmpSvc = gaudi.service("IncidentSvc")
        self.incidentSvc = InterfaceCast("IIncidentSvc")(tmpSvc.getInterface())

        self.gaudi = gaudi
        self.TES   = gaudi.evtSvc()

        import GaudiPython
        from GaudiPython import InterfaceCast
        from GaudiPython.Bindings import gbl
        svc = gaudi.service('Swimming::Service')
        GaudiPython.loaddict('Swimming')
        #GaudiPython.loaddict('SwimmingEventDict')
        self.GaudiPython = GaudiPython
        self.service = InterfaceCast(gbl.Swimming.Interfaces.IService)(svc.getInterface())
     
        self.distanceTool  = gaudi.toolsvc().create(Swimming().getProp('DistCalc'),
                                                    interface = 'IDistanceCalculator')
        from SwimmingUtils import LifetimeFitter, DTFFitter, BestPVFinder
        self.relatedPVFinder = BestPVFinder(self)
        if Swimming().getProp('LifetimeFitter') == "LifetimeFitter":
            self.lifetimeFitter = LifetimeFitter(self)
        else:
            self.lifetimeFitter = DTFFitter(self)

        self.tistostool    = gaudi.toolsvc().create('TriggerTisTos', interface='ITriggerTisTos')
        self.tistosbools   = [self.tistostool.kAnything,
                              self.tistostool.kTrueRequired,
                              self.tistostool.kFalseRequired]
        self.triggers      = triggers
       
        self.hlt1recoline  = Swimming().getProp('Hlt1RecoLine')
        self.hlt2recoline  = Swimming().getProp('Hlt2RecoLine')
 
        self.granularityRefinement = Swimming().getProp('GranularityRefinement')
        self.DEBUGMODE             = Swimming().getProp('Debug')
        self.startingEvent         = Swimming().getProp('SkipEvents')
        self.swimStripping         = Swimming().getProp('SwimStripping')
        self.swimOffline           = Swimming().getProp('SwimOffSel')
        self.offSelName            = Swimming().getProp('OffSelModuleName')
        self.offCands              = Swimming().getProp('OffCands')
        self.offlinePVs            = Swimming().getProp('OfflinePV')
        self.refitPVs              = Swimming().getProp('RefitPVs')
