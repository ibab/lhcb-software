from MicroDSTExample import Helpers
from GaudiKernel import SystemOfUnits
from GaudiPython import gbl, Helper, InterfaceCast
from ROOT import Double
#==============================================================================
class MCAssociator :
    def __init__(self, MCAssociatorTool, verbose = False ) :
        self.mcAssoc = MCAssociatorTool
        self.verbose = verbose
    def __call__(self, p) :
        mcp = self.mcAssoc.relatedMCP(p)
        if self.verbose and (mcp == None) :
            print "Found no associated MCP"
        return mcp
#==============================================================================
class MassRes:
    def __init__(self, assocFun) :
        self.assocFun = assocFun
    def __call__(self, particle) :
        mcp = self.assocFun(particle)
        if (mcp != None) :
            return particle.momentum().mass()-mcp.momentum().mass()
#==============================================================================
class TauRes:
    def __init__(self, tauFunc, mcTauFunc, assocFun) :
        self.assocFun = assocFun
        self.tauFunc = tauFunc
        self.mcTauFunc = mcTauFunc
    def __call__(self, particle) :
        mcp = self.assocFun(particle)
        if (mcp != None) :
            tau = self.tauFunc(particle)
            mcTau = self.mcTauFunc(mcp)
            if tau != None and mcTau != None :
                return tau-mcTau
#==============================================================================
class Mass:
    def __call__(self, particle) :
        return particle.momentum().mass()
#==============================================================================
class PIDMass:
    def __init__(self, partProp) :
        self.partProp = partProp
    def __call__(self, particle) :
        pid = particle.particleID()
        return self.partProp( pid ).mass()
#==============================================================================
class ParticleName:
    def __init__(self, partProp) :
        self.partProp = partProp
    def __call__(self, particle) :
        pid = particle.particleID()
        pProp=self.partProp( pid )
        return pProp.particle()
#==============================================================================
class PropTime:
    def __init__(self, bestVertex, fitter) :
        self.bv = bestVertex
        self.fitter = fitter
    def __call__(self, particle) :
        vertex = self.bv(particle)
        if (vertex != None) :
            tau = Double(-999999/SystemOfUnits.picosecond)
            error = Double(0.)
            chi2=Double(0.)
            self.fitter.fit(vertex, particle, tau, error, chi2)
            return tau/SystemOfUnits.picosecond
#==============================================================================
class BestTo:
    def __init__(self, table) :
        self.table = table
    def __call__(self, frm) :
        if (self.table !=None) :
            range = self.table.relations(frm)
            if ( not range.empty()) :
                return range.back().to()
#==============================================================================
class HistoPlotter :
    def __init__(self, histos, functor, keyGetter = None) :
        self.histos = histos
        self.functor = functor
        self.key = keyGetter
    def __call__(self, obj) :
        if obj != None :
            value = self.functor(obj)
            if value != None :
                if type(self.histos) == list or type(self.histos) == dict :
                    self.histos[self.key(obj)].fill(value)
                else :
                    self.histos.fill(value)

#==============================================================================
class NextEvent:
    def __init__(self, am ) :
        self.appMgr = am
    def __call__(self) :
        self.appMgr.run(1)
        return bool(self.appMgr.evtSvc()['/Event'])
#==============================================================================
class PartPropSvc:
    def __init__(self, am) :
        self.appMgr = am
        _svc = Helper.service(self.appMgr._svcloc, 'LHCb::ParticlePropertySvc')
        self.Svc = InterfaceCast(gbl.LHCb.IParticlePropertySvc)(_svc)
    def __call__(self, pid) :
        pidx = gbl.LHCb.ParticleID(pid)
        return self.Svc.find( pidx )
#==============================================================================
class TESContainerLoop:
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
    def __init__(self, functor):
        self.functor = functor
    def __call__(self, container) :
        for obj in container:
            obj = Helpers.deSmartRef(obj)
            self.functor(obj)
#==============================================================================
class ContainerRecursiveLoop :
    def __init__(self, functor, recursion):
        self.functor = functor
        self.recursor = recursion
    def __call__(self, container) :
        for obj in container:
            obj = Helpers.deSmartRef(obj)
            self.functor(obj)
            daughters = self.recursor(obj)
            if daughters != None and not daughters.empty() :
                self.__call__(daughters)
#==============================================================================
