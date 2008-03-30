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
    _vp  = 'std::vector<const LHCb::Particle*>'   ## std.vector( p )
    _vv  = 'std::vector<const LHCb::VertexBase*>' ## std.vector( v )
    _vd  = 'std::vector<double>'                  ## std.vector( 'double' )
    #
    _decorated  = _LoKiCore.getAndDecorateFunctions (
        name                                          , ## module name 
        LoKi.Functor        (p,'double')              , ## the base 
        LoKi.Dicts.FunCalls (LHCb.Particle)           , ## call-traits 
        LoKi.Dicts.FuncOps  (p,p)                     ) ## operators 
    _decorated |= _LoKiCore.getAndDecorateFunctions  (
        name                                          , ## module name 
        LoKi.Functor        (v,'double')              , ## the base 
        LoKi.Dicts.FunCalls (LHCb.VertexBase)         , ## call-traits 
        LoKi.Dicts.FuncOps  (v,v)                     ) ## operators
    _decorated |=  _LoKiCore.getAndDecoratePredicates (
        name                                          , ## module name 
        LoKi.Functor        (p,bool)                  , ## the base 
        LoKi.Dicts.CutCalls (LHCb.Particle)           , ## call-traits 
        LoKi.Dicts.CutsOps  (p,p)                     ) ## operators 
    _decorated |= _LoKiCore.getAndDecoratePredicates (
        name                                          , ## module name 
        LoKi.Functor        (v,bool)                  , ## the base 
        LoKi.Dicts.CutCalls (LHCb.VertexBase)         , ## call-traits 
        LoKi.Dicts.CutsOps  (v,v)                     ) ## the operators
    ## functional part:
    _decorated |= _LoKiCore.getAndDecorateMaps       (
        name                                          , ## module name
        LoKi.Functor( _vp , _vd )                     , ## the base
        LoKi.Dicts.MapsOps(p)                         ) ## streamers
    _decorated |= _LoKiCore.getAndDecorateMaps       (
        name                                          , ## module name
        LoKi.Functor( _vv , _vd )                     , ## the base
        LoKi.Dicts.MapsOps(v)                         ) ## streamers    
    _decorated |= _LoKiCore.getAndDecorateMaps       (
        name                                          , ## module name
        LoKi.Functor( _vp , _vp )                     , ## the base
        LoKi.Dicts.PipeOps(p,p)                       ) ## streamers
    _decorated |= _LoKiCore.getAndDecorateMaps       (
        name                                          , ## module name
        LoKi.Functor( _vv , _vv )                     , ## the base
        LoKi.Dicts.PipeOps(v,v)                       ) ## streamers
    _decorated |= _LoKiCore.getAndDecorateMaps       (
        name                                          , ## module name
        LoKi.Functor( _vp , 'double' )                , ## the base
        LoKi.Dicts.FunValOps(p)                       ) ## streamers
    _decorated |= _LoKiCore.getAndDecorateMaps       (
        name                                          , ## module name
        LoKi.Functor( _vv , 'double' )                , ## the base
        LoKi.Dicts.FunValOps(v)                       ) ## streamers    
    _decorated |= _LoKiCore.getAndDecorateMaps       (
        name                                          , ## module name
        LoKi.Functor( _vp , p )                       , ## the base
        LoKi.Dicts.ElementOps(p,p)                    ) ## stremers
    _decorated |= _LoKiCore.getAndDecorateMaps       (
        name                                          , ## module name
        LoKi.Functor( _vv , v )                       , ## the base
        LoKi.Dicts.ElementOps(v,v)                    ) ## stremers
    # 'sources'
    _decorated |= _LoKiCore.getAndDecorateMaps       (
        name                                          , ## module name
        LoKi.Functor( 'void' , _vp )                  , ## the base
        LoKi.Dicts.SourceOps(p,p)                     ) ## streamers
    _decorated |= _LoKiCore.getAndDecorateMaps       (
        name                                          , ## module name
        LoKi.Functor( 'void' , _vv )                  , ## the base
        LoKi.Dicts.SourceOps(v,v)                     ) ## streamers
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




# =============================================================================
if '__main__' == __name__ :
    print 'Number of properly decorated types: %s'%len(_decorated)
    for o in _decorated : print o

# =============================================================================
# The END 
# =============================================================================

