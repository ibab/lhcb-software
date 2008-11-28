#!/usr/bin/env python
# =============================================================================
# $Id: decorators.py,v 1.5 2008-11-28 16:58:44 ibelyaev Exp $
# =============================================================================
## @file decorators.py LoKiNumbers/decorators.py
#  The set of basic decorator for objects from LoKiNumberslibrary
#  The file is a part of LoKi and Bender projects
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
"""
The set of basic decorators for objects from LoKiNumbers library
"""
_author_ = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
# =============================================================================

from   LoKiNumbers.functions   import *
from   LoKiNumbers.sources     import *



_name = __name__ 
# =============================================================================
## make the decoration of all objects fomr this module
def _decorate ( name = _name  ) :
    """
    Make the decoration of all objects fomr this module
    """
    import LoKiCore.decorators as _LoKiCore
    _d = 'double'
    _v = 'std::vector<double>' 

    # double -> double 
    _decorated   = _LoKiCore.getAndDecorateFunctions  (
        name                                             , ## module name 
        LoKi.Functor        (_d,_d)                      , ## the base 
        LoKi.Dicts.FunCalls (_d)                         , ## call-traits 
        LoKi.Dicts.FuncOps  (_d,_d)                      ) ## operators
    # vetcor<double> -> double 
    _decorated  |= _LoKiCore.getAndDecorateFunctions  (
        name                                             , ## module name 
        LoKi.Functor        (_v,_d)                      , ## the base 
        LoKi.Dicts.FunCalls (_v)                         , ## call-traits 
        LoKi.Dicts.FuncOps  (_v,_v)                      ) ## operators
    # double -> bool 
    _decorated  |= _LoKiCore.getAndDecoratePredicates (
        name                                             , ## module name 
        LoKi.Functor        (_d,bool)                    , ## the base 
        LoKi.Dicts.CutCalls (_d)                         , ## call-traits 
        LoKi.Dicts.CutsOps  (_d,_d)                      ) ## operators
    # vector<double> -> bool 
    _decorated  |= _LoKiCore.getAndDecoratePredicates (
        name                                             , ## module name 
        LoKi.Functor        (_v,bool)                    , ## the base 
        LoKi.Dicts.CutCalls (_v)                         , ## call-traits 
        LoKi.Dicts.CutsOps  (_v,_v)                      ) ## operators
    # vector<double> -> vector<double>
    _decorated  |= _LoKiCore.getAndDecorateMaps       (
        name                                             , ## module name
        LoKi.Functor        (_v,_v)                      , ## the base
        LoKi.Dicts.PipeOps  (_d,_d)                      ) ## stremers
    # vector<double> -> double 
    _decorated  |= _LoKiCore.getAndDecorateMaps       (
        name                                             , ## module name
        LoKi.Functor        (_v,_d)                      , ## the base
        LoKi.Dicts.FunValOps(_d)                         ) ## stremers    

    return _decorated


def decorateVoids ( name = _name ) :
    """
    Make the decoration of 'void'-functors this module
    """
    import LoKiCore.decorators as _LoKiCore
    _d = 'double'
    _b = 'bool'
    _v = 'void'

    # void -> double 
    _decorated   = _LoKiCore.getAndDecorateFunctions  (
        name                                             , ## module name 
        LoKi.Functor        (_v,_d)                      , ## the base 
        LoKi.Dicts.FunCalls (_v)                         , ## call-traits 
        LoKi.Dicts.FuncOps  (_v,_v)                      ) ## operators

    # void -> bool 
    _decorated  |= _LoKiCore.getAndDecoratePredicates (
        name                                             , ## module name 
        LoKi.Functor        (_v,_b)                      , ## the base 
        LoKi.Dicts.CutCalls (_v)                         , ## call-traits 
        LoKi.Dicts.CutsOps  (_v,_v)                      ) ## operators
    
    return _decorated 

# =============================================================================
## perform the decoration 
_decorated  = _decorate     ()
_decorated |= decorateVoids ()                       ## ATTENTION 
# =============================================================================

# =============================================================================
if '__main__' == __name__ :
    print 'Number of properly decorated types: %s'%len(_decorated)
    for o in _decorated : print o
        
# =============================================================================
# The END
# =============================================================================
