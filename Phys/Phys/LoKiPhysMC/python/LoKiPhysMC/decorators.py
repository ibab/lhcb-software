#!/usr/bin/env python
# =============================================================================
## @file decorators.py LoKiPhysMC/decorators.py
#  The set of basic decorator for objects from LoKiPhysMC library
#  The file is a part of LoKi and Bender projects
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
"""
The set of basic decorators for obejcts from LoKiPhysMC library
"""
# =============================================================================
__author__ = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
# =============================================================================

from   LoKiPhysMC.functions import *

print dir()

# =============================================================================
## decorate MCMatch object:
def _valid_(s)    :
    """
    Check the validity of MCMatch-object

    >>> matcher = ... 
    >>> if matcher.valid() : ...
    
    """
    return LoKi.Dicts.MCMatchDicts.valid(s)
# =============================================================================
## update the documentation 
_valid_.__doc__     += LoKi.Dicts.MCMatchDicts.valid.__doc__
# =============================================================================
## decorate MCMatch object:
def _not_valid_(s) :
    """
    Check the validity of the MCMatch object
    
    >>> matcher = ...
    >>> if not matcher : ...
    
    """
    return not s.valid()
# =============================================================================
## decorate MCMatch object:
def _match_(s,particle,mcobject) :
    """
    Check MC-matching

    >>> matcher=...
    >>> particle = ...
    >>> mcobject = ..
    >>> matcher.match(particle,mcobject)

    """
    return LoKi.Dicts.MCMatchDicts.match(s,particle,mcobject)
# =============================================================================
## update the documentation 
_match_.__doc__     += LoKi.Dicts.MCMatchDicts.match.__doc__
# =============================================================================
LoKi.MCMatch.valid      = _valid_
LoKi.MCMatch.match      = _match_
LoKi.MCMatch.__invert__ = _not_valid_


_name = __name__
# =============================================================================
from LoKiPhys.decorators import _decorate as _decorate1 
from LoKiMC.decorators   import _decorate as _decorate2 
## Make the decoration of functions and predicates 
def _decorate ( name = _name ) :
    """
    Make the decoration of functions and predicates
    """
    import LoKiMC.decorators   as _LMC
    import LoKiPhys.decorators as _LRC
    import LoKiCore.decorators as _L
    _decorated  = _decorate1 ( name )
    _decorated |= _decorate2 ( name )
    
   ## (re) decorate pids (Comparison with strings, integers and ParticleID objects:
    for t in ( _LMC.MCID , _LMC.MCABSID ,
               _LRC.ID   , _LRC.ABSID   ) :
        t = type ( t ) 
        _L.decoratePID ( t , LoKi.Dicts.PIDOps ( t ) )
        _decorated.add ( t )

    ## 
    return _decorated                                   ## RETURN 
# =============================================================================

# =============================================================================
## Perform the actual decoration
_decorated = _decorate( _name ) 
# =============================================================================

# =============================================================================
if '__main__' == __name__ :
    print 'Number of properly decorated types: %s'%len(_decorated)
    for o in _decorated : print o
 
# =============================================================================
# The END 
# =============================================================================

