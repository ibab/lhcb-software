#!/usr/bin/env python
# =============================================================================
## @file 
#  The set of basic decorator for objects from LoKiTracklibrary
#  The file is a part of LoKi and Bender projects
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @daet 2007-06-09
# =============================================================================
""" The set of basic decorators for obejcts from LoKiTrack library """
_author_ = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
# =============================================================================

from   LoKiTrigger.functions   import *

_name = __name__ 
# =============================================================================
## make the decoration of all objects fomr this module
def _decorate ( name = _name  ) :
    """
    Make the decoration of all objects from this module
    """
    import LoKiCore.decorators as _LoKiCore
    ## regular functors which accept the track
    _t  =  LHCb.Track
    _pt = 'LHCb::Track*'
    _decorated  = _LoKiCore.getAndDecorateFunctions (
        name                                            , ## module name
        TrFunc                                          , ## the base 
        LoKi.Dicts.FunCalls (_t)                        , ## call-traits 
        LoKi.Dicts.FuncOps  (_t,_pt)                    ) ## operators 
    _decorated |=  _LoKiCore.getAndDecoratePredicates (
        name                                            , ## module name
        TrCuts                                          , ## the base 
        LoKi.Dicts.CutCalls (_t)                        , ## call-traits 
        LoKi.Dicts.CutsOps  (_t,_pt)                    ) ## operators 
    ## regular functors which accept the vertex 
    _v  =  LHCb.RecVertex
    _pv = 'LHCb::RecVertex*'
    _decorated |= _LoKiCore.getAndDecorateFunctions  (
        name                                            , ## module name
        RVFunc                                          , ## the base 
        LoKi.Dicts.FunCalls (_v)                        , ## call-traits 
        LoKi.Dicts.FuncOps  (_v,_pv)                    ) ## operators
    _decorated |= _LoKiCore.getAndDecoratePredicates (
        name                                            , ## module name 
        RVCuts                                          , ## the base 
        LoKi.Dicts.CutCalls (_v)                        , ## call-traits 
        LoKi.Dicts.CutsOps  (_v,_pv)                    ) ## the operators
    ## regular functors which accept two tracks
    from LoKiTrigger.functions import _HTT
    _decorated |= _LoKiCore.getAndDecorateFunctions  (
        name                                            , ## module name
        TTrFunc                                         , ## the base 
        LoKi.Dicts.FunCalls (_HTT)                      , ## call-traits 
        LoKi.Dicts.FuncOps  (_HTT,_HTT)                 ) ## operators
    _decorated |= _LoKiCore.getAndDecoratePredicates (
        name                                            , ## module name 
        TTrCuts                                         , ## the base 
        LoKi.Dicts.CutCalls (_HTT)                      , ## call-traits 
        LoKi.Dicts.CutsOps  (_HTT,_HTT)                 ) ## the operators
    ## regular functors which accept track & vertex 
    from LoKiTrigger.functions import _HTV
    _decorated |= _LoKiCore.getAndDecorateFunctions  (
        name                                            , ## module name
        TrVFunc                                         , ## the base 
        LoKi.Dicts.FunCalls (_HTV)                      , ## call-traits 
        LoKi.Dicts.FuncOps  (_HTV,_HTV)                 ) ## operators
    _decorated |= _LoKiCore.getAndDecoratePredicates (
        name                                            , ## module name 
        TrVCuts                                         , ## the base 
        LoKi.Dicts.CutCalls (_HTV)                      , ## call-traits 
        LoKi.Dicts.CutsOps  (_HTV,_HTV)                 ) ## the operators
    ## regular functors which accept two vertices 
    from LoKiTrigger.functions import _HVV
    _decorated |= _LoKiCore.getAndDecorateFunctions  (
        name                                            , ## module name
        RVVFunc                                         , ## the base 
        LoKi.Dicts.FunCalls (_HVV)                      , ## call-traits 
        LoKi.Dicts.FuncOps  (_HVV,_HVV)                 ) ## operators
    _decorated |= _LoKiCore.getAndDecoratePredicates (
        name                                            , ## module name 
        RVVCuts                                         , ## the base 
        LoKi.Dicts.CutCalls (_HVV)                      , ## call-traits 
        LoKi.Dicts.CutsOps  (_HVV,_HVV)                 ) ## the operators
    ## functional stuff
    _vt = 'std::vector<LHCb::Track*>'     ## std.vector ( _pt      )
    _vv = 'std::vector<LHCb::RecVertex*>' ## std.vector ( _pv      )
    _vd = 'std::vector<double>'           ## std.vector ( 'double' ) 
    _decorated |= _LoKiCore.getAndDecorateMaps       (
        name                                            , ## module name
        LoKi.Functor( _vt , _vd )                       , ## the base
        LoKi.Dicts.MapsOps( _pt )                       ) ## streamers
    _decorated |= _LoKiCore.getAndDecorateMaps       (
        name                                            , ## module name
        LoKi.Functor( _vv , _vd )                       , ## the base
        LoKi.Dicts.MapsOps( _pv )                       ) ## streamers    
    _decorated |= _LoKiCore.getAndDecorateMaps       (
        name                                            , ## module name
        LoKi.Functor( _vt , _vt )                       , ## the base
        LoKi.Dicts.PipeOps (_pt,_t)                     ) ## streamers
    _decorated |= _LoKiCore.getAndDecorateMaps       (
        name                                            , ## module name
        LoKi.Functor( _vv , _vv )                       , ## the base
        LoKi.Dicts.PipeOps (_pv,_v)                     ) ## streamers
    _decorated |= _LoKiCore.getAndDecorateMaps       (
        name                                            , ## module name
        LoKi.Functor         ( _vt , 'double' )         , ## the base
        LoKi.Dicts.FunValOps ( _pt )                    ) ## streamers
    _decorated |= _LoKiCore.getAndDecorateMaps       (
        name                                            , ## module name
        LoKi.Functor         ( _vv , 'double' )         , ## the base
        LoKi.Dicts.FunValOps ( _pv )                    ) ## streamers    
    _decorated |= _LoKiCore.getAndDecorateMaps       (
        name                                            , ## module name
        LoKi.Functor          ( _vt , _pt )             , ## the base
        LoKi.Dicts.ElementOps ( _pt , _t  )             ) ## streamers
    _decorated |= _LoKiCore.getAndDecorateMaps       (
        name                                            , ## module name
        LoKi.Functor          ( _vv , _pv )             , ## the base
        LoKi.Dicts.ElementOps ( _pv , _v  )             ) ## streamers
    _decorated |= _LoKiCore.getAndDecorateMaps       (
        name                                            , ## module name
        LoKi.Functor         ( 'void' , _vt )           , ## the base
        LoKi.Dicts.SourceOps ( _pt , _t )               ) ## stremers
    _decorated |= _LoKiCore.getAndDecorateMaps       (
        name                                            , ## module name
        LoKi.Functor         ( 'void' , _vv )           , ## the base
        LoKi.Dicts.SourceOps ( _pv , _v )               ) ## stremers
    ## smart info:
    _decorated |= _LoKiCore.getAndDecorateInfos      (
        name                                            , ## module name
        TrFunc                                          , ## the base 
        LoKi.Dicts.InfoOps (_t)                         ) ## methods
    _decorated |= _LoKiCore.getAndDecorateInfos      (
        name                                            , ## module name
        RVFunc                                          , ## the base 
        LoKi.Dicts.InfoOps (_v)                         ) ## methods
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

