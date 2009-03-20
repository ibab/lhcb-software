from MicroDSTExample import Helpers
from GaudiKernel import SystemOfUnits
from ROOT import Double
#==============================================================================
class AssocMCPFromTable :
    def __init__(self, table) :
        self.table = table
    def __call__(self, particle) :
        return Helpers.assocMCP(particle, self.table)
#==============================================================================
class MCAssociator :
    def __init__(self, MCAssociatorTool, verbose = False ) :
        self.mcAssoc = MCAssociatorTool
        self.verbose = verbose
    def __call__(self, p) :
        mcp = self.mcAssoc.bestRelatedMCP(p)
        if self.verbose and (mcp == None) :
            print "Found no assoc trees"
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
class Mass:
    def __call__(self, particle) :
        return particle.momentum().mass()
#==============================================================================
class PIDMass:
    def __init__(self, partProp) :
        self.partProp = partProp
    def __call__(self, particle) :
        pid = Helpers.particleID(particle)
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
class DefaultName:
    def __init__(self, name):
        self.name = name
    def __call__(self, p) :
        return self.name
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
class BestVertex:
    def __init__(self, table) :
        self.table = table
    def __call__(self, particle) :
        if (self.table !=None) :
            PVRange = self.table.relations(particle)
            if ( not PVRange.empty()) :
                return PVRange.back().to()
#==============================================================================
class GenericPlotter:
    def __init__(self,
                 plots,
                 valueFunc,
                 keyFunc,
                 nameFunc,
                 plotAttributes,
                 tag):
        self.plots = plots
        self.valueFunc  = valueFunc
        self.keyFunc = keyFunc
        self.nameFunc = nameFunc
        self.plotAtt = plotAttributes 
        self.tag   = tag
    def __call__(self, obj) :
        value = self.valueFunc(obj)
        key = self.keyFunc(obj)
        xMin = self.plotAtt.low(obj)
        xMax = self.plotAtt.high(obj)
        if (value != None) :
            s0 = self.nameFunc(obj) + " " + self.tag
            self.plots.bookAndPlot(key,
                                   s0,
                                   s0,
                                   self.plotAtt.nBins,
                                   xMin,
                                   xMax,
                                   value)
#==============================================================================
