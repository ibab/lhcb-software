#!/usr/bin/env python
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
## @file decorators.py LoKiCore/decorators.py
#  The set of basic decorator for objects from LoKiMC library
#  The file is a part of LoKi and Bender projects
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
""" The set of basic decorators for obejcts from LoKiMC library """
_author_ = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
# =============================================================================

from   LoKiMC.functions   import *
import LoKiCore.decorators as _LoKiCore

# =============================================================================
# make the decoration of all objects from this module
# =============================================================================

_decorated  = _LoKiCore.getAndDecorateFunctions (
    __name__                                      , ## module name 
    MCFunc                                        , ## the base 
    LoKi.Dicts.FunCalls(LHCb.MCParticle)          , ## call-traits 
    LoKi.Dicts.FuncOps('const LHCb::MCParticle*') ) ## operators 
_decorated |= _LoKiCore.getAndDecorateFunctions  (
    __name__                                      , ## module name 
    MCVFunc                                       , ## the base 
    LoKi.Dicts.FunCalls(LHCb.MCVertex)            , ## call-traits 
    LoKi.Dicts.FuncOps('const LHCb::MCVertex*')   ) ## operators
_decorated |=  _LoKiCore.getAndDecoratePredicates (
    __name__                                      , ## module name 
    MCCuts                                        , ## the base 
    LoKi.Dicts.CutCalls(LHCb.MCParticle)          , ## call-traits 
    LoKi.Dicts.FuncOps('const LHCb::MCParticle*') ) ## operators 
_decorated |= _LoKiCore.getAndDecoratePredicates (
    __name__                                      , ## module name 
    MCVCuts                                       , ## the base 
    LoKi.Dicts.CutCalls(LHCb.MCVertex)            , ## call-traits 
    LoKi.Dicts.FuncOps('const LHCb::MCParticle*') ) ## the operators 

# =============================================================================
# decorate pids (Comparison with strings, integers and ParticleID objects:
for t in ( MCID , MCABSID ) :
    t = type ( t ) 
    _LoKiCore.decoratePID ( t , LoKi.Dicts.PIDOps ( t ) )

# =============================================================================
if '__main__' == __name__ :
    print 'Number of properly decorated types: %s'%len(_decorated)
    for o in _decorated : print o

# =============================================================================
# The END 
# =============================================================================
