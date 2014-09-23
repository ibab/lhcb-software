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
        DTFconstraints = [ ]
        for configname, constraints in Swimming().getProp('DecayTreeFitterConstraints').iteritems():
            tmpconstraints = { }
            for constraint, value in constraints.iteritems():
                newid = None
                if type(constraint) == int:
                    newid = constraint
                elif isinstance(constraint, basestring):
                    try:
                        newid = gaudi.ppSvc().find(constraint).particleID().pid()
                    except:
                        print "ERROR: failed to convert", constraint, "to LHCb::ParticleID"
                        raise
                else:
                    print "ERROR: currently can't handle conversion of ", constraint, "to LHCb::ParticleID"
                if newid:
                    print "Storing DTF constraint: (%d, %f)" % (newid, value)
                    tmpconstraints[newid] = value
                    tmpconstraints[-newid]= value
            DTFconstraints += [ ( configname, tmpconstraints) ]

        from SwimmingUtils import LifetimeFitter, DTFFitter, BestPVFinder
        self.relatedPVFinder = BestPVFinder(self)

        self.lifetimeFitter = [ ]
        for lifetimefitter in Swimming().getProp('LifetimeFitter'):
            if lifetimefitter == "LifetimeFitter":
                self.lifetimeFitter += [ LifetimeFitter(self) ]
            else:
                # DecayTreeFitter
                # examine the constraints we have been given, and add DTFFitter objects as appropriate
                for configname, constraints in DTFconstraints:
                    self.lifetimeFitter += [ DTFFitter(self, configname, constraints) ]

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
        self.matchCandsUsingPID    = Swimming().getProp('UseCompositePIDsWhenMatching')
        self.storeExtraTPs         = Swimming().getProp('StoreExtraTPs')
