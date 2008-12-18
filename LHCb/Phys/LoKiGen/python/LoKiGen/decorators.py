#!/usr/bin/env python
# =============================================================================
## @file LoKiGen/decorators.py
#  The set of basic decorator for objects from LoKiGen library
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
# =============================================================================
"""
The set of basic decorators for obejcts from LoKiGen library

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

from   LoKiGen.functions   import *
from   LoKiGen.HepMC       import *
from   LoKiGen.trees       import *

_name = __name__ 
# =============================================================================
## make the decoration of all objects fomr this module
def _decorate ( name = _name  ) :
    """ Make the decoration of all objects fomr this module """
    import LoKiCore.decorators as _LoKiCore
    _gp  = 'const HepMC::GenParticle*'
    _gv  = 'const HepMC::GenVertex*'
    
    ## _vgp = std.vector ( _gp      )
    ## _vgv = std.vector ( _gv      )
    ## _vd  = std.vector ( 'double' )

    _vgp = 'std::vector<const HepMC::GenParticle*>'
    _vgv = 'std::vector<const HepMC::GenVertex*>'
    _vd  = 'std::vector<double>'

    # T -> double

    _functions = _LoKiCore.getInherited (
        name                                    , ## moduel name  
        LoKi.Functor   (_gp,'double')           ) ## the base
    _decorated  = _LoKiCore.decorateCalls       (
        _functions                              , ## list of functor types
        LoKi.Dicts.FunCalls (HepMC.GenParticle) ) ## call-traits
    _decorated |= _LoKiCore.decorateFunctionOps (
        _functions                              , ## list of functor types
        LoKi.Dicts.FuncOps  (_gp,_gp)           ) ## operators&operations

    _functions = _LoKiCore.getInherited (
        name                                    , ## moduel name  
        LoKi.Functor   (_gv,'double')           ) ## the base
    _decorated |= _LoKiCore.decorateCalls       (
        _functions                              , ## list of functor types
        LoKi.Dicts.FunCalls (HepMC.GenVertex)   ) ## call-traits
    _decorated |= _LoKiCore.decorateFunctionOps (
        _functions                              , ## list of functor types
        LoKi.Dicts.FuncOps  (_gv,_gv)           ) ## operators&operations

    # T -> bool
    
    _functions = _LoKiCore.getInherited (
        name                                    , ## moduel name  
        LoKi.Functor   (_gp,bool)               ) ## the base
    _decorated |= _LoKiCore.decorateCalls        (
        _functions                              , ## list of functor types
        LoKi.Dicts.CutCalls (HepMC.GenParticle) ) ## call-traits
    _decorated |= _LoKiCore.decoratePredicateOps (
        _functions                              , ## list of functor types
        LoKi.Dicts.CutsOps  (_gp,_gp)           ) ## operators&operations

    _functions = _LoKiCore.getInherited (
        name                                    , ## moduel name  
        LoKi.Functor   (_gv,bool)               ) ## the base
    _decorated |= _LoKiCore.decorateCalls        (
        _functions                              , ## list of functor types
        LoKi.Dicts.CutCalls (HepMC.GenVertex)   ) ## call-traits
    _decorated |= _LoKiCore.decoratePredicateOps (
        _functions                              , ## list of functor types
        LoKi.Dicts.CutsOps  (_gv,_gv)           ) ## operators&operations

    # functional part

    # map: vector<T> -> vector<double>

    _functions = _LoKiCore.getInherited (
        name                                   , ## moduel name  
        LoKi.Functor   (_vgp,_vd)              ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.MapsOps(_gp)                ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.MapsOps(_gp)                ) ## call-traits
    
    _functions = _LoKiCore.getInherited (
        name                                   , ## moduel name  
        LoKi.Functor   (_vgv,_vd)              ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.MapsOps(_gv)                ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.MapsOps(_gv)                ) ## call-traits

    # pipe: vector<T> -> vector<T>

    _functions = _LoKiCore.getInherited (
        name                                   , ## module name  
        LoKi.Functor   (_vgp,_vgp)             ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.PipeOps(_gp,_gp)            ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.PipeOps(_gp,_gp)            ) ## call-traits

    _functions = _LoKiCore.getInherited (
        name                                   , ## module name  
        LoKi.Functor   (_vgv,_vgv)             ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.PipeOps(_gv,_gv)            ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.PipeOps(_gv,_gv)            ) ## call-traits
    
    # vector<T> -> double 

    _functions = _LoKiCore.getInherited (
        name                                   , ## module name  
        LoKi.Functor   (_vgp,'double')         ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.FunValOps(_gp)              ) ## call-traits
    _decorated |= _LoKiCore.decorateFunctionOps (
        _functions                             , ## list of functor types
        LoKi.Dicts.FunValOps(_gp)              ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.FunValOps(_gp)              ) ## call-traits

    _functions = _LoKiCore.getInherited (
        name                                   , ## module name  
        LoKi.Functor   (_vgv,'double')         ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.FunValOps(_gv)              ) ## call-traits
    _decorated |= _LoKiCore.decorateFunctionOps (
        _functions                             , ## list of functor types
        LoKi.Dicts.FunValOps(_gv)              ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.FunValOps(_gv)              ) ## call-traits

    # vector<T> -> bool

    _functions = _LoKiCore.getInherited (
        name                                   , ## module name  
        LoKi.Functor   (_vgp,bool)             ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.CutValOps(_gp)              ) ## call-traits
    _decorated |= _LoKiCore.decoratePredicateOps (
        _functions                             , ## list of functor types
        LoKi.Dicts.CutValOps(_gp)              ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.CutValOps(_gp)              ) ## call-traits

    _functions = _LoKiCore.getInherited (
        name                                   , ## module name  
        LoKi.Functor   (_vgv,bool)             ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.CutValOps(_gv)              ) ## call-traits
    _decorated |= _LoKiCore.decoratePredicateOps (
        _functions                             , ## list of functor types
        LoKi.Dicts.CutValOps(_gv)              ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.CutValOps(_gv)              ) ## call-traits

    # vector<T> -> T

    _functions = _LoKiCore.getInherited (
        name                                   , ## module name  
        LoKi.Functor   (_vgp,_gp)              ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.ElementOps(_gp,_gp  )       ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.ElementOps(_gp,_gp)         ) ## call-traits

    _functions = _LoKiCore.getInherited (
        name                                   , ## module name  
        LoKi.Functor   (_vgv,_gv)              ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.ElementOps(_gv,_gv  )       ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.ElementOps(_gv,_gv)         ) ## call-traits

    #sources : void -> vector<T>

    _functions = _LoKiCore.getInherited (
        name                                   , ## module name  
        LoKi.Functor   ('void',_vgp)           ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.SourceOps(_gp,_gp)          ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.SourceOps(_gp,_gp)          ) ## call-traits

    _functions = _LoKiCore.getInherited (
        name                                   , ## module name  
        LoKi.Functor   ('void',_vgv)           ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.SourceOps(_gv,_gv)          ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.SourceOps(_gv,_gv)          ) ## call-traits

    
    # decorate pids (Comparison with strings, integers and ParticleID objects:
    for t in ( GID , GABSID ) :
        t = type ( t ) 
        _LoKiCore.decoratePID ( t , LoKi.Dicts.PIDOps ( t ) )
        _decorated.add( t ) 
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
