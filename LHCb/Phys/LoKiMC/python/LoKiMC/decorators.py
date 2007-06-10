#!/usr/bin/env python
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

if not 'Bender' in dir() : Bender =  LoKi.Dicts

# =============================================================================
## decorate MC-finder object:
def _valid_(s)         :
    """
    Check the validity of the MC-finder object and
    the underlying pointer to IMCDecayFinder tool

    >>> finder = ...
    >>> if finder.valid() : 
    
    """
    return LoKi.Dicts.MCFinderDicts.valid      (s)
## decorate MC-finder object:
def _not_valid_(s)      :
    """
    Check the validity of the MC-finder object and
    the underlying pointer to IMCDecayFinder tool

    >>> finder = ...
    >>> if not finder : ...
    
    """
    return not s.valid()
# =============================================================================
## update the documentation 
_valid_.__doc__     += LoKi.Dicts.MCFinderDicts.valid.__doc__
# =============================================================================
## find the decays, see IMCDecayFinder::
def _findDecays_(s,decay,*a) :
    """
    Find the decay according t the decay descriptor
    [see IMCDecayFinder interface]

    >>> finder=...
    >>> decays = finder.findDecay( 'B0-> pi+ pi-')
    >>> 

    @param decay the decay descriptor
    @return the Range of found decay trees
    
    """
    return LoKi.Dicts.MCFinderDicts.findDecays ( s , decay , *a )
# =============================================================================
## update the documentation 
_findDecays_.__doc__ += LoKi.Dicts.MCFinderDicts.findDecays.__doc__
# =============================================================================
LoKi.MCFinder.find       = _findDecays_
LoKi.MCFinder.findDecay  = _findDecays_
LoKi.MCFinder.findDecays = _findDecays_
LoKi.MCFinder.valid      = _valid_ 
LoKi.MCFinder.__invert__ = _not_valid_ 


_name = __name__ 
# =============================================================================
## Make the decoration of all objects from this module
def _decorate ( name = _name ) :
    """ Make the decoration of all objects from this module """
    import LoKiCore.decorators as _LoKiCore
    _decorated  = _LoKiCore.getAndDecorateFunctions (
        name                                          , ## module name 
        LoKi.Function('const LHCb::MCParticle*')      , ## the base 
        LoKi.Dicts.FunCalls(LHCb.MCParticle)          , ## call-traits 
        LoKi.Dicts.FuncOps('const LHCb::MCParticle*') ) ## operators 
    _decorated |= _LoKiCore.getAndDecorateFunctions  (
        name                                          , ## module name 
        LoKi.Function('const LHCb::MCVertex*')        , ## the base 
        LoKi.Dicts.FunCalls(LHCb.MCVertex)            , ## call-traits 
        LoKi.Dicts.FuncOps('const LHCb::MCVertex*')   ) ## operators
    _decorated |=  _LoKiCore.getAndDecoratePredicates (
        name                                          , ## module name 
        LoKi.Predicate('const LHCb::MCParticle*')     , ## the base 
        LoKi.Dicts.CutCalls(LHCb.MCParticle)          , ## call-traits 
        LoKi.Dicts.FuncOps('const LHCb::MCParticle*') ) ## operators 
    _decorated |= _LoKiCore.getAndDecoratePredicates (
        name                                          , ## module name 
        LoKi.Predicate('const LHCb::MCVertex*')       , ## the base 
        LoKi.Dicts.CutCalls(LHCb.MCVertex)            , ## call-traits 
        LoKi.Dicts.FuncOps('const LHCb::MCVertex*')   ) ## the operators 
    ## decorate pids (Comparison with strings, integers and ParticleID objects:
    for t in ( MCID , MCABSID ) :
        t = type ( t ) 
        _LoKiCore.decoratePID ( t , LoKi.Dicts.PIDOps ( t ) )
        _decorated.add ( t )
    return _decorated
# =============================================================================


# =============================================================================
## Perform the actual decoration:
_decorated = _decorate() 
# =============================================================================

# =============================================================================
if '__main__' == __name__ :
    print 'Number of properly decorated types: %s'%len(_decorated)
    for o in _decorated : print o

# =============================================================================
# The END 
# =============================================================================
