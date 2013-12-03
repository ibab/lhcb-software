# -*- coding: utf-8 -*-
#==============================================================================
"""
This module contains various functors useful for interactive DSt and MicroDST
analysis in GaudiPython. it exports the following classes:
   - MCAssociator
   - MassRes
   - TauRes
   - Mass
   - PIDMass
   - ParticleName
   - PropTime
   - BestTo
   - HistoPlotter
   - NextEvent
   - PartPropSvc
   - TESContainerLoop
   - ContainerLoop
   - ContainerRecursiveLoop
"""
__author__ = "Juan PALACIOS juan.palacios@nikhef.nl"
#==============================================================================
__all__ = ( "MCAssociator",
            "MassRes",
            "TauRes",
            "Mass",
            "PIDMass",
            "ParticleName",
            "PropTime",
            "BestTo",
            "HistoPlotter",
            "NextEvent",
            "PartPropSvc",
            "TESContainerLoop",
            "ContainerLoop",
            "ContainerRecursiveLoop" )
import Helpers
from GaudiKernel import SystemOfUnits
from GaudiPython import gbl, Helper, InterfaceCast
from ROOT import Double
#==============================================================================
class MCAssociator :
    '''
    Return the best related MCParticle for a given Particle.
    Constructed from an associator implementing relatedMCP(Particle).
    '''
    def __init__(self, MCAssociatorTool, verbose = False ) :
        self.mcAssoc = MCAssociatorTool
        self.verbose = verbose
    def __call__(self, p) :
        mcp = self.mcAssoc.relatedMCP(p)
        if self.verbose and mcp :
            print "Found no associated MCP"
        return mcp
#==============================================================================
class Mass:
    '''
    Return the mass of an object in MeV. The object must have an attribute
    momentum() which must in turn have an attribute mass(). In practice,
    this means LHCb.Particle and LHCb.MCParticle
    '''
    def __call__(self, particle) :
        return particle.momentum().mass()
#==============================================================================
class MassRes:
    '''
    Return the mass resolution for a particle in MeV.
    Constructed from a functor that returns the related MCParticle for a
    Particle.
    Usage:
    >>> assocFunctor = ...
    >>> massResFunctor = MassRes(assocFunctor)
    >>> p = LHCb.Particle( ... )
    >>> massRes = massResFunctor(p)
    '''
    def __init__(self, assocFun) :
        self.assocFun = assocFun
    def __call__(self, particle) :
        mcp = self.assocFun(particle)
        if mcp :
            mass = Mass()
            return mass(particle) - mass(mcp)
#==============================================================================
class TauRes:
    '''
    Return the proper time resolution for a particle in picoseconds.
    Constructed from three functors:
    1) Functor that returns the proper time for a Particle
    2) Functor that returns the proper time for an MCParticle
    3) Functor that returns the related MCParticle for a
    Particle.
    Usage:
    >>> tauFunctor = ...
    >>> mcTauFunctor = ...
    >>> assocFunctor = ...
    >>> tauResFunctor = TauRes(tauFunctor, mcTauFunctor, assocFunctor)
    >>> p = LHCb.Particle( ... )
    >>> tauRes = tauResFunctor(p)
    '''
    def __init__(self, tauFunc, mcTauFunc, assocFun) :
        self.assocFun = assocFun
        self.tauFunc = tauFunc
        self.mcTauFunc = mcTauFunc
    def __call__(self, particle) :
        mcp = self.assocFun(particle)
        if mcp :
            tau = self.tauFunc(particle)
            mcTau = self.mcTauFunc(mcp)
            if tau and mcTau :
                return tau-mcTau

#==============================================================================
class PIDMass:
    '''
    Return the PDG mass of a particle corresponding to a particle ID PID.
    Constructed from a functor that returns the ParticleProperty for a PID
    Usage:
    >>> pProp = ...
    >>> pdgMassFunctor = PIDMass(pProp)
    >>> pid = 531
    >>> pdgMass = pdgMassFunctor(pid)
    '''
    def __init__(self, partProp) :
        self.partProp = partProp
    def __call__(self, particle) :
        pid = particle.particleID()
        return self.partProp( pid ).mass()
#==============================================================================
class ParticleName:
    '''
    Return the name of a particle. Constructed from a
    functor that returns the ParticleProperty for a PID
    Usage:
    >>> pProp = ...
    >>> nameFunctor = ParticleName(pProp)
    >>> p = LHCb.Particle(....)
    >>> print "Name of particle is ", nameFunctor(p)
    '''
    def __init__(self, partProp) :
        self.partProp = partProp
    def __call__(self, particle) :
        pid = particle.particleID()
        pProp=self.partProp( pid )
        return pProp.particle()
#==============================================================================
class PropTime:
    '''
    Estimate the proper time of a particle in picoseconds.
    Constructed from a functors and an ILifeTimeFitter
    Functor returns the "best" vertex given a particle
    Usage:
    >>> propTimeFitter = toolSvc.create('PropertimeFitter',
                                        interface='ILifetimeFitter')
    >>> bestVertexFunctor = ...
    >>> propTimeFunctor = PropTime(bestVertexFunctor, propTimeFitter)
    >>> p = LHCb.Particle(...)
    >>> properTime = propTimeFunctor(p)
    '''
    def __init__(self, bestVertex, fitter ) :
        self.bv = bestVertex
        self.fitter = fitter
    def __call__(self, particle) :
        vertex = self.bv(particle)
        if vertex :
            tau = Double(-999999/SystemOfUnits.picosecond)
            error = Double(0.)
            chi2=Double(0.)
            self.fitter.fit(vertex, particle, tau, error, chi2)
            return tau/SystemOfUnits.picosecond
#==============================================================================
class BestTo:
    '''
    Return the "best" To given a From. Constructed from a weighted relations
    table associating Froms to Tos.
    Usage (example, Particle -> MCParticle association):
    >>> particle2MCParticleRelTable = ...
    >>> assocMCP = BestTo(particle2MCParticleRelTable)
    >>> p = LHCb.Particle(...)
    >>> mcp = assocMCP(p)
    '''
    def __init__(self, table) :
        self.table = table
    def __call__(self, frm) :
        if self.table :
            range = self.table.relations(frm)
            if ( not range.empty()) :
                return range.back().to()
#==============================================================================
class HistoPlotter :
    '''
    Fill a histogram with information obtained from an object via a functor.
    Possibility to select which of a dictionary of histograms to fill by use
    of an optional key-getting functor.
    Constructed from a histogram, or a list or dictionary of histograms.
    If constructed from a list or dictionary, the keyGetter argument must be
    used.
    Usage:
    >>> # simple plotter with one histogram
    >>> massHisto = HistoUtils.book(.....)
    >>> massFunctor = Functors.Mass()
    >>> massPlotter = Functors.HistoPlotter(massHisto, massFunctor)
    >>> particles = evtSvc["Phys/SomeLocation/Particles"]
    >>> for p in particles :
    >>>    massPlotter(p)
    >>> # example with histogram dictionary, key is particle ID.
    >>> pids = [531,-531, 333, 443 ]
    >>> massPlots = {}
    >>> for pid in pids :
    >>>    massPlots[pid] = HistoUtils.book(.....)
    >>> massFunctor = Functors.Mass()
    >>> keyFunctor = Helpers.pid
    >>> massPlotter = Functors.HistoPlotter(massHistos, massFunctor, keyFunctor)
    >>> for p in particles :
    >>>   massPlotter(p)
    '''
    def __init__(self, histos, functor, keyGetter = None) :
        '''
        histos: single histogram, or list or dictionary of histograms
        functor: functor that returns the value to be put into the histogram starting from an object. value = functor(object).
        keyGetter: get a key from an object to find the correct histogram from
        the list of dictionary. key = keyGetter(object), histo = histos[key]
        '''
        self.histos = histos
        self.functor = functor
        self.key = keyGetter
    def __call__(self, obj) :
        if obj :
            value = self.functor(obj)
            if value :
                if type(self.histos) == list or type(self.histos) == dict :
                    self.histos[self.key(obj)].fill(value)
                else :
                    self.histos.fill(value)

#==============================================================================
class NextEvent:
    '''
    Get the next event in an event loop. Constructed from an initialised
    AppMgr. Option to specify maximum number of events via EvtMax.
    EvtMax default is -1, which selects all events.
    Usage:
    >>> appMgr = AppMgr(.....)
    >>> nextEvt = NextEvent(appMgr, EvtMax = 500)
    >>> nEvents = 0
    >>> while nextEvt() :
    >>>   print "Event", nEvents
    >>>   nEvents += 1
    '''
    def __init__(self, am, EvtMax = -1 ) :
        self.appMgr = am
        self.max=EvtMax
        self.count=0
    def __call__(self) :
        self.appMgr.run(1)
        self.count += 1
        data = bool(self.appMgr.evtSvc()['/Event'])
        if self.max < 0 :
            return data
        else :
            return (self.max >= self.count) and data 
#==============================================================================
class PartPropSvc:
    '''
    Wrapper around LHCb.ParticlePropertySvc. Does necessary initialisation and
    provides __call__ method returning the ParticleParticle for a given pid.
    Constructed from an initialised AppMgr.
    Usage:
    >>> appMgr = AppMgr(...)
    >>> ppSvc = PartiPropSvc(appMgr)
    >>> pid = 443
    >>> jpsiProp = ppSvc(443)
    '''
    def __init__(self, am) :
        self.appMgr = am
        _svc = Helper.service(self.appMgr._svcloc, 'LHCb::ParticlePropertySvc')
        self.Svc = InterfaceCast(gbl.LHCb.IParticlePropertySvc)(_svc)
    def __call__(self, pid) :
        pidx = gbl.LHCb.ParticleID(pid)
        return self.Svc.find( pidx )
#==============================================================================
class TESContainerLoop:
    '''
    loop over the contents of a container in a TES location and apply
    a functor to each element.
    Constructed from the event service and a functor.
    Usage:
    >>> evtSvc = appMge.evtSvc()
    >>> functor = ...
    >>> loop = TESContainerLoop(evtSvc, functor)
    >>> loop("/Event/Rev/Vertex/Primary")
    '''
    def __init__(self, evt, fun ) :
        self.evt = evt
        self.fun = fun
    def __call__(self, location) :
        container = self.evt[location]
        for item in container :
            self.fun(item)
        return container.size()
#==============================================================================
class ContainerLoop :
    '''
    loop over the contents of a container in a TES location and apply
    a functor to each element.
    Constructed from a functor.
    Usage:
    >>> evtSvc = appMge.evtSvc()
    >>> vertices = evtSvc["/Event/Rev/Vertex/Primary"]
    >>> functor = ...
    >>> loop = ContainerLoop(functor)
    >>> loop(vertices) # applies functor(v) to each v in vertices
    '''
    def __init__(self, functor):
        self.functor = functor
    def __call__(self, container) :
        for obj in container:
            obj = Helpers.deSmartRef(obj)
            self.functor(obj)
#==============================================================================
class ContainerRecursiveLoop :
    '''
    loop over the contents of a container in a TES location and apply
    a functor to each element. Get a container from each element and apply
    the same functor. Do this recusrively until no more containers are found.
    Constructed from a standard functor and a recursion funtcor, which obtains
    a container of object from an objetc.
    Usage:
    >>> # Apply functor to container of particles and recursively to all
    >>> # descendants
    >>> evtSvc = appMge.evtSvc()
    >>> vertices = evtSvc["/Event/Phys/SomeAlgo/Particles"]
    >>> functor = ...
    >>> recursion = LHCb.Particle.daughters
    >>> loop = ContainerLoop(functor, recusrionFunctor)
    >>> loop(vertices) # applies functor(v) to each v in vertices
    '''
    def __init__(self, functor, recursion):
        self.functor = functor
        self.recursor = recursion
    def __call__(self, container) :
        for obj in container:
            obj = Helpers.deSmartRef(obj)
            self.functor(obj)
            daughters = self.recursor(obj)
            if daughters and not daughters.empty() :
                self.__call__(daughters)
# =============================================================================
if '__main__' == __name__ :
    import sys 
    print __doc__
    for o in __all__ :
        print o
        print sys.modules[__name__].__dict__[o].__doc__
        
    
# =============================================================================
# The END 
# =============================================================================
