#!/usr/bin/env python
# =============================================================================
## @file decorators.py LoKiPhys/decorators.py
#  The set of basic decorator for objects from LoKiPhys library
#  The file is a part of LoKi and Bender projects
#   
#        This file is a part of LoKi project - 
#    "C++ ToolKit  for Smart and Friendly Physics Analysis"
#
#  The package has been designed with the kind help from
#  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
#  contributions and advices from G.Raven, J.van Tilburg, 
#  A.Golutvin, P.Koppenburg have been used in the design.
#
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
"""
The set of basic decorators for obejcts from LoKiPhys library

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

from GaudiKernel.SystemOfUnits import MeV,GeV,micrometer,millimeter,centimeter,meter,picosecond,femtosecond,nanosecond,perCent
um = micrometer
mm = millimeter
cm = centimeter 
ns = picosecond
ps = picosecond
fs = femtosecond

from   LoKiPhys.functions  import *
from   LoKiCore.functions  import  equal_to 
import LoKiPhys.Phys

_name = __name__ 
# =============================================================================
## make the decoration of all objects from this module
def _decorate ( name = _name ) :
    """
    Make the decoration of all objects from this module
    """
    import LoKiCore.decorators as     _LoKiCore
    p = 'const LHCb::Particle*'
    v = 'const LHCb::VertexBase*'
    d = 'double'
    _vp  = 'std::vector<const LHCb::Particle*>'   ## std.vector( p )
    _vv  = 'std::vector<const LHCb::VertexBase*>' ## std.vector( v )
    _vd  = 'std::vector<double>'                  ## std.vector( 'double' )
    #
    
    # "function" : Particle -> double 
    
    _functions = _LoKiCore.getInherited (
        name                                   , ## moduel name  
        LoKi.Functor   (p,'double')            ) ## the base
    _decorated  = _LoKiCore.decorateCalls       (
        _functions                             , ## list of functor types
        LoKi.Dicts.FunCalls (LHCb.Particle)    ) ## call-traits
    _decorated |= _LoKiCore.decorateFunctionOps (
        _functions                             , ## list of functor types
        LoKi.Dicts.FuncOps  (p,p)              ) ## operators&operations
    
    # "function" : Vertex -> double

    _functions = _LoKiCore.getInherited (
        name                                   , ## moduel name  
        LoKi.Functor   (v,'double')            ) ## the base
    _decorated  = _LoKiCore.decorateCalls       (
        _functions                             , ## list of functor types
        LoKi.Dicts.FunCalls (LHCb.VertexBase)  ) ## call-traits
    _decorated |= _LoKiCore.decorateFunctionOps (
        _functions                             , ## list of functor types
        LoKi.Dicts.FuncOps  (v,v)              ) ## operators&operations

    # "predicate/cut" :  Particle -> bool

    _functions = _LoKiCore.getInherited (
        name                                   , ## moduel name  
        LoKi.Functor   (p,bool)                ) ## the base
    _decorated |= _LoKiCore.decorateCalls       (
        _functions                             , ## list of functor types
        LoKi.Dicts.CutCalls (LHCb.Particle)    ) ## call-traits
    _decorated |= _LoKiCore.decoratePredicateOps (
        _functions                             , ## list of functor types
        LoKi.Dicts.CutsOps  (p,p)              ) ## operators&operations
    
    # "predicate/cut" :  Vertex -> bool
    
    _functions = _LoKiCore.getInherited (
        name                                   , ## moduel name  
        LoKi.Functor   (v,bool)                ) ## the base
    _decorated |= _LoKiCore.decorateCalls       (
        _functions                             , ## list of functor types
        LoKi.Dicts.CutCalls (LHCb.VertexBase)  ) ## call-traits
    _decorated |= _LoKiCore.decoratePredicateOps (
        _functions                             , ## list of functor types
        LoKi.Dicts.CutsOps  (v,v)              ) ## operators&operations

    ## functional part:

    
    # "map" : vector<T> -> vector<double>

    _functions = _LoKiCore.getInherited (
        name                                   , ## moduel name  
        LoKi.Functor (_vp,_vd)                 ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.MapsOps(p)                  ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.MapsOps(p)                  ) ## call-traits
    
    _functions = _LoKiCore.getInherited (
        name                                   , ## moduel name  
        LoKi.Functor (_vv,_vd)                 ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.MapsOps(v)                  ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.MapsOps(v)                  ) ## call-traits
    
    # "pipe" : vector<T> -> vector<T>
    
    _functions = _LoKiCore.getInherited (
        name                                   , ## module name  
        LoKi.Functor   (_vp,_vp)               ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.PipeOps(p,p)                ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.PipeOps(p,p)                ) ## call-traits

    _functions = _LoKiCore.getInherited (
        name                                   , ## module name  
        LoKi.Functor   (_vv,_vv)               ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.PipeOps(v,v)                ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.PipeOps(v,v)                ) ## call-traits
    
    # "funval" : vector<T> -> double 
    
    _functions = _LoKiCore.getInherited (
        name                                   , ## module name  
        LoKi.Functor   (_vp,'double')          ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.FunValOps(p)                ) ## call-traits
    _decorated |= _LoKiCore.decorateFunctionOps (
        _functions                             , ## list of functor types
        LoKi.Dicts.FunValOps(p)                ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.FunValOps(p)                ) ## call-traits

    _functions = _LoKiCore.getInherited (
        name                                   , ## module name  
        LoKi.Functor   (_vv,'double')          ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.FunValOps(v)                ) ## call-traits
    _decorated |= _LoKiCore.decorateFunctionOps (
        _functions                             , ## list of functor types
        LoKi.Dicts.FunValOps(v)                ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.FunValOps(v)                ) ## call-traits

    # "cutval" : vector<T> -> bool

    _functions = _LoKiCore.getInherited (
        name                                   , ## module name  
        LoKi.Functor   (_vp,bool)              ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.CutValOps(p)                ) ## call-traits
    _decorated |= _LoKiCore.decoratePredicateOps (
        _functions                             , ## list of functor types
        LoKi.Dicts.CutValOps(p)                ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.CutValOps(p)                ) ## call-traits

    _functions = _LoKiCore.getInherited (
        name                                   , ## module name  
        LoKi.Functor   (_vv,bool)              ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.CutValOps(v)                ) ## call-traits
    _decorated |= _LoKiCore.decorateFunctionOps (
        _functions                             , ## list of functor types
        LoKi.Dicts.CutValOps(v)                ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.CutValOps(v)                ) ## call-traits


    # "element": vector<T> -> T 
    
    _functions = _LoKiCore.getInherited (
        name                                   , ## module name  
        LoKi.Functor   (_vp,p)                 ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.ElementOps(p,p)             ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.ElementOps(p,p)             ) ## call-traits

    _functions = _LoKiCore.getInherited (
        name                                   , ## module name  
        LoKi.Functor   (_vv,v)                 ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.ElementOps(v,v)             ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.ElementOps(v,v)             ) ## call-traits
    
    
    # 'source' : void -> vector<T>

    _functions = _LoKiCore.getInherited (
        name                                   , ## module name  
        LoKi.Functor   ('void',_vp)            ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.SourceOps(p,p)              ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.SourceOps(p,p)              ) ## call-traits

    _functions = _LoKiCore.getInherited (
        name                                   , ## module name  
        LoKi.Functor   ('void',_vv)            ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.SourceOps(v,v)              ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.SourceOps(v,v)              ) ## call-traits

    # 'infos'
    
    _decorated |= _LoKiCore.getAndDecorateInfos      (
        name                                            , ## module name
        LoKi.Functor       (p,'double')                 , ## the base 
        LoKi.Dicts.InfoOps (p)                          ) ## methods
    _decorated |= _LoKiCore.getAndDecorateInfos      (
        name                                            , ## module name
        LoKi.Functor       (v,'double')                 , ## the base 
        LoKi.Dicts.InfoOps (v)                          ) ## methods
    # decorate pids (Comparison with strings, integers and ParticleID objects:
    for t in ( ID , ABSID ) :
        t = type ( t ) 
        _LoKiCore.decoratePID ( t , LoKi.Dicts.PIDOps ( t ) )
        _decorated.add ( t )
    ##
    return  _decorated 
# =============================================================================

# =============================================================================
## perform the ctual decoration
_decorated = _decorate()                                     ## ATTENTION
# =============================================================================

# play a bit with random cuts
import LoKiNumbers.decorators as     _LoKiNumbers
RNDM   =  ONE>>_LoKiNumbers.XRNDM
GAUSS  =  ONE>>_LoKiNumbers.XGAUSS
RAND   =  ONE>>_LoKiNumbers.XRAND
VRNDM  = VONE>>_LoKiNumbers.XRNDM
VGAUSS = VONE>>_LoKiNumbers.XGAUSS
VRAND  = VONE>>_LoKiNumbers.XRAND


## @see LoKi::Cuts::ISLONG
ISLONG   =  equal_to ( TRTYPE , LHCb.Track.Long       )
## @see LoKi::Cuts::ISUP   
ISUP     =  equal_to ( TRTYPE , LHCb.Track.Upstream   ) 
## @see LoKi::Cuts::ISDOWN
ISDOWN   =  equal_to ( TRTYPE , LHCb.Track.Downstream ) 
## @see LoKi::Cuts::ISTTRACK
ISTTRACK =  equal_to ( TRTYPE , LHCb.Track.Ttrack     ) 


# =============================================================================
if '__main__' == __name__ :
    print 'Number of properly decorated types: %s'%len(_decorated)
    for o in _decorated : print o

# =============================================================================
# The END 
# =============================================================================

