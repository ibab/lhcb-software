#!/usr/bin/env python
# =============================================================================
# $Id: decorators.py,v 1.6 2009-05-04 17:35:51 ibelyaev Exp $ 
# =============================================================================
## @file  LoKiArrayFunctors/decorators.py
#  The set of basic decorator for objects from LoKiHlt library
#  The file is a part of LoKi and Bender projects
#
#  This file is a part of LoKi project - 
#   'C++ ToolKit  for Smart and Friendly Physics Analysis'
#
#  The package has been designed with the kind help from
#  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
#  contributions and advices from G.Raven, J.van Tilburg, 
#  A.Golutvin, P.Koppenburg have been used in the design.
#
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @daet 2007-06-09
# =============================================================================
"""
The set of basic decorators for objects from LoKiHlt library

This file is a part of LoKi project - 
'C++ ToolKit  for Smart and Friendly Physics Analysis'

The package has been designed with the kind help from
Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
contributions and advices from G.Raven, J.van Tilburg, 
A.Golutvin, P.Koppenburg have been used in the design.
"""
# =============================================================================
__author__ = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
# =============================================================================

from   LoKiArrayFunctors.functions   import *
import LoKiArrayFunctors.Array

_name = __name__ 
# =============================================================================
## make the decoration of all objects fomr this module
def _decorate ( name = _name  ) :
    """ Make the decoration of all objects fomr this module """
    import LoKiCore.decorators as _LoKiCore
    from LoKiArrayFunctors.functions import _T
    
    ## regular functors which accept the combination
    
    # "function" : Combination -> double 
    
    _functions = _LoKiCore.getInherited (
        name                                     , ## moduel name  
        AFunc                                    ) ## the base
    _decorated  = _LoKiCore.decorateCalls        (
        _functions                               , ## list of functor types
        LoKi.Dicts.FunCalls (_T)    ) ## call-traits
    _decorated |= _LoKiCore.decorateFunctionOps (
        _functions                               , ## list of functor types
        LoKi.Dicts.FuncOps  (_T,_T)              ) ## operators&operations

    
    # "predicate/cut" : Combination -> bool
    
    _functions = _LoKiCore.getInherited (
        name                                     , ## moduel name  
        ACuts                                    ) ## the base
    _decorated |= _LoKiCore.decorateCalls        (
        _functions                               , ## list of functor types
        LoKi.Dicts.CutCalls (_T)    ) ## call-traits
    _decorated |= _LoKiCore.decoratePredicateOps (
        _functions                               , ## list of functor types
        LoKi.Dicts.CutsOps  (_T,_T)              ) ## operators&operations

##     ## functional:
    
##     _va = 'std::vector<LoKi::Range_<std::vector<const LHCb::Particle*> > >' ## std.vector ( _T    )
##     _vd = 'std::vector<double>'                                             ## std.vector ('double')

##     # "map" : vector<T> -> vector<double>
    
##     _functions = _LoKiCore.getInherited (
##         name                                   , ## moduel name  
##         LoKi.Functor (_va,_vd)                 ) ## the base
##     _decorated |= _LoKiCore.decorateCalls  (
##         _functions                             , ## list of functor types
##         LoKi.Dicts.MapsOps(_T)                 ) ## call-traits
##     _decorated |= _LoKiCore.decorateMaps   (
##         _functions                             , ## list of functor types
##         LoKi.Dicts.MapsOps(_T)                 ) ## call-traits
  
##     # "pipe" : vector<T> -> vector<T>
    
##     _functions = _LoKiCore.getInherited (
##         name                                   , ## module name  
##         LoKi.Functor   (_va,_va)               ) ## the base
##     _decorated |= _LoKiCore.decorateCalls  (
##         _functions                             , ## list of functor types
##         LoKi.Dicts.PipeOps(_T,_T)              ) ## call-traits
##     _decorated |= _LoKiCore.decorateMaps   (
##         _functions                             , ## list of functor types
##         LoKi.Dicts.PipeOps(_T,_T)              ) ## call-traits

##     # "funval" : vector<T> -> double 
    
##     _functions = _LoKiCore.getInherited (
##         name                                   , ## module name  
##         LoKi.Functor   (_va,'double')          ) ## the base
##     _decorated |= _LoKiCore.decorateCalls  (
##         _functions                             , ## list of functor types
##         LoKi.Dicts.FunValOps(_T)               ) ## call-traits
##     _decorated |= _LoKiCore.decorateFunctionOps (
##         _functions                             , ## list of functor types
##         LoKi.Dicts.FunValOps(_T)               ) ## call-traits
##     _decorated |= _LoKiCore.decorateMaps   (
##         _functions                             , ## list of functor types
##         LoKi.Dicts.FunValOps(_T)               ) ## call-traits

##     # "cutval" : vector<T> -> bool

##     _functions = _LoKiCore.getInherited (
##         name                                   , ## module name  
##         LoKi.Functor   (_va,bool)              ) ## the base
##     _decorated |= _LoKiCore.decorateCalls  (
##         _functions                             , ## list of functor types
##         LoKi.Dicts.CutValOps(_T)               ) ## call-traits
##     _decorated |= _LoKiCore.decoratePredicateOps (
##         _functions                             , ## list of functor types
##         LoKi.Dicts.CutValOps(_T)               ) ## call-traits
##     _decorated |= _LoKiCore.decorateMaps   (
##         _functions                             , ## list of functor types
##         LoKi.Dicts.CutValOps(_T)               ) ## call-traits
    
##     # "element": vector<T> -> T 
    
##     _functions = _LoKiCore.getInherited (
##         name                                   , ## module name  
##         LoKi.Functor   (_va,_T)                ) ## the base
##     _decorated |= _LoKiCore.decorateCalls  (
##         _functions                             , ## list of functor types
##         LoKi.Dicts.ElementOps(_T,_T)           ) ## call-traits
##     _decorated |= _LoKiCore.decorateMaps   (
##         _functions                             , ## list of functor types
##         LoKi.Dicts.ElementOps(_T,_T)           ) ## call-traits

    ## 
    return _decorated                            ## RETURN

# =============================================================================
## perform the decoration 
_decorated = _decorate ()                         ## ATTENTION 
# =============================================================================

# =============================================================================
if '__main__' == __name__ :
    print 'Number of properly decorated types: %s'%len(_decorated)
    for o in _decorated : print o
    

# =============================================================================
# The END 
# =============================================================================

