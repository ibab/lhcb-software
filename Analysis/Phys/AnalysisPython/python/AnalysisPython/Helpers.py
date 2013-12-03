#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
This module contains a set of random helper functions
for GaudiPython analysis.
"""
__author__ = "Juan PALACIOS juan.palacios@nikhef.nl"
from ROOT import Double
from GaudiPython import gbl, AppMgr, Helper, InterfaceCast
from GaudiKernel import SystemOfUnits, PhysicalConstants
import LHCbMath.Types
Gaudi = gbl.Gaudi
__all__= ('HistoAttributes',
          'AdaptiveHistoAttributes',
          'printXYZPosition',
          'printXYZMomentum',
          'deSmartRef',
          'properTime',
          'particleID',
          'pid',
          'properTimeMC',
          'decayVertex',
          'printProperties')
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
    if  (not particle) or pid(particle) == -99000000  : return tau
    originVertex = particle.originVertex()
    if not originVertex : return tau
    decayVtx = decayVertex(particle)
    if decayVtx :
        decayPos = decayVtx.position()
        originPos = originVertex.position()
        flightVector = Gaudi.XYZVector( decayPos.x()-originPos.x(),
                                        decayPos.y()-originPos.y(),
                                        decayPos.z()-originPos.z() )
        p = particle.momentum()
        m = p.mass()
        tau = ( (m/(SystemOfUnits.picosecond)) * (p.Vect().Dot(flightVector)) ) / p.Vect().mag2()
        tau /= PhysicalConstants.c_light
    return tau
#==============================================================================
def decayVertex(particle) :
    """
    Return the first decay vertex of a particle.
    """
    vertices = particle.endVertices()
    iCount=0
    for v in vertices :
        return deSmartRef(v)
#==============================================================================
def printProperties(configurable) :
    '''
    Print the properties of a configurable. Useage:
    >>> conf = ...
    >>> printProperties(conf)
    '''
    props = configurable.properties()
    for p, v in props.iteritems() :
        if v!='<no value>' : print p, v
#==============================================================================
if '__main__' == __name__ :
    import sys 
    print __doc__
    for o in __all__ :
        print o
        print sys.modules[__name__].__dict__[o].__doc__

