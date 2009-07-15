from ROOT import TCanvas, TH1D, Double
from GaudiPython import gbl, AppMgr, Helper, InterfaceCast
from math import sqrt, fabs
from GaudiKernel import SystemOfUnits, PhysicalConstants
from LHCbMath import XYZVector, XYZPoint
#==============================================================================
class Plots :
    def __init__(self, name="Plots") :
        self.name = name
        self.plots = {}

    def bookPlot(self, plotID, s0, s1, nBins, xMin, xMax) :
        if (not self.plots.has_key(plotID) ) :
            self.plots[plotID] = TH1D(s0, s1, nBins, xMin, xMax)
        return self.plots[plotID]

    def exists(plotID) :
        return self.plots.has_key(plotID)

    def plot(self, plotID, value) :
        if (self.plots.has_key(plotID)) :
            self.plots[plotID].Fill(value)
        else :
            print name, ": Plot with ID ", plotID, " does not exist. Ignoring."

    def bookAndPlot(self, plotID, s0, s1, nBins, xMin, xMax, value) :
        if (not self.plots.has_key(plotID)) :
            self.bookPlot(plotID, s0, s1, nBins, xMin, xMax)
        self.plot(plotID, value)
        
#==============================================================================
class HistoAttributes:
    def __init__(self, bins, low, high, centre=0.) :
        self.nBins = bins
        self._low = low + centre
        self._high = high + centre
    def low(self, p) :
        return self._low
    def high(self, p) :
        return self._high
#==============================================================================
class AdaptiveHistoAttributes:
    def __init__(self, bins, low, high, centreFunc) :
        self.nBins = bins
        self._low = low
        self._high = high
        self.centreFunc = centreFunc
    def low(self, p) :
        return self._low + self.centreFunc(p)
    def high(self, p) :
        return self._high + self.centreFunc(p)
#==============================================================================
def printXYZPosition(point) :
    return point.X(), point.Y(), point.Z()
#==============================================================================
def printXYZMomentum(mom) :
    return mom.Px(), mom.Py(), mom.Pz()
#==============================================================================
def deSmartRef(T) :
    if hasattr(T, 'target'): T = T.target()
    return T
#==============================================================================
def properTime(particle, vertex, fitter) :
    tau =Double(-999999/SystemOfUnits.picosecond)
    if vertex :
        error = Double(0.)
        chi2=Double(0.)
        fitter.fit(vertex, particle, tau, error, chi2)
    return tau/SystemOfUnits.picosecond
#==============================================================================
def particleID(particle) :
    return particle.particleID()
#==============================================================================
def pid(particle) :
    return particle.particleID().pid()
#==============================================================================
def properTimeMC(particle) :
    tau = -99999.
    if  particle or pid(particle) == -99000000  : return tau
    originVertex = particle.originVertex()
    if originVertex : return tau
    decayVtx = decayVertex(particle)
    if decayVtx :
        decayPos = decayVtx.position()
        originPos = originVertex.position()
        flightVector = XYZVector( decayPos.x()-originPos.x(),
                                  decayPos.y()-originPos.y(),
                                  decayPos.z()-originPos.z() )
        p = particle.momentum()
        m = p.mass()
        tau = ( (m/(SystemOfUnits.picosecond)) * (p.Vect().Dot(flightVector)) ) / p.Vect().mag2()
        tau /= PhysicalConstants.c_light
    return tau
#==============================================================================
def decayVertex(particle) :
    vertices = particle.endVertices()
    iCount=0
    for v in vertices :
        return deSmartRef(v)
#==============================================================================
def particleTreeLoop(particles, functor):
    for p in particles:
        p = deSmartRef(p)
        functor(p)
        daughters = p.daughters()
        particleTreeLoop(daughters, functor)
#==============================================================================
