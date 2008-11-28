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
    _v  =  LHCb.RecVertex
    _pv = 'LHCb::RecVertex*'
    
    # "function" : Track -> double 
    
    _functions = _LoKiCore.getInherited (
        name                                   , ## moduel name  
        TrFunc                                 ) ## the base
    _decorated  = _LoKiCore.decorateCalls       (
        _functions                             , ## list of functor types
        LoKi.Dicts.FunCalls (_t)               ) ## call-traits
    _decorated |= _LoKiCore.decorateFunctionOps (
        _functions                             , ## list of functor types
        LoKi.Dicts.FuncOps  (_t,_pt)           ) ## operators&operations
    

    # "function" : RecVertex -> double 
    
    _functions = _LoKiCore.getInherited (
        name                                   , ## moduel name  
        RVFunc                                 ) ## the base
    _decorated |= _LoKiCore.decorateCalls       (
        _functions                             , ## list of functor types
        LoKi.Dicts.FunCalls (_v)               ) ## call-traits
    _decorated |= _LoKiCore.decorateFunctionOps (
        _functions                             , ## list of functor types
        LoKi.Dicts.FuncOps  (_v,_pv)           ) ## operators&operations
    
    # "predicate/cut" :  Track -> bool
    
    _functions = _LoKiCore.getInherited (
        name                                   , ## moduel name  
        TrCuts                                 ) ## the base
    _decorated |= _LoKiCore.decorateCalls       (
        _functions                             , ## list of functor types
        LoKi.Dicts.CutCalls (_t)               ) ## call-traits
    _decorated |= _LoKiCore.decoratePredicateOps (
        _functions                             , ## list of functor types
        LoKi.Dicts.CutsOps  (_t,_pt)           ) ## operators&operations
  
    # "predicate/cut" :  RecVertex -> bool
    
    _functions = _LoKiCore.getInherited (
        name                                   , ## moduel name  
        RVCuts                                 ) ## the base
    _decorated |= _LoKiCore.decorateCalls       (
        _functions                             , ## list of functor types
        LoKi.Dicts.CutCalls (_v)               ) ## call-traits
    _decorated |= _LoKiCore.decoratePredicateOps (
        _functions                             , ## list of functor types
        LoKi.Dicts.CutsOps  (_v,_pv)           ) ## operators&operations

    
    from LoKiTrigger.functions import _HTT

    ##  track pair -> double
    
    _functions = _LoKiCore.getInherited (
        name                                   , ## moduel name  
        TTrFunc                                ) ## the base
    _decorated |= _LoKiCore.decorateCalls       (
        _functions                             , ## list of functor types
        LoKi.Dicts.FunCalls (_HTT)             ) ## call-traits
    _decorated |= _LoKiCore.decorateFunctionOps (
        _functions                             , ## list of functor types
        LoKi.Dicts.FuncOps  (_HTT,_HTT)        ) ## operators&operations

    ##  track pair -> bool 
    
    _functions = _LoKiCore.getInherited (
        name                                   , ## moduel name  
        TTrCuts                                ) ## the base
    _decorated |= _LoKiCore.decorateCalls       (
        _functions                             , ## list of functor types
        LoKi.Dicts.CutCalls (_HTT)             ) ## call-traits
    _decorated |= _LoKiCore.decoratePredicateOps (
        _functions                             , ## list of functor types
        LoKi.Dicts.CutsOps  (_HTT,_HTT)        ) ## operators&operations


    from LoKiTrigger.functions import _HTV
    
    ## track-vertex pair -> double
    
    _functions = _LoKiCore.getInherited (
        name                                   , ## moduel name  
        TrVFunc                                ) ## the base
    _decorated |= _LoKiCore.decorateCalls       (
        _functions                             , ## list of functor types
        LoKi.Dicts.FunCalls (_HTV)             ) ## call-traits
    _decorated |= _LoKiCore.decorateFunctionOps (
        _functions                             , ## list of functor types
        LoKi.Dicts.FuncOps  (_HTV,_HTV)        ) ## operators&operations
    
    ##  track pair -> bool 
    
    _functions = _LoKiCore.getInherited (
        name                                   , ## moduel name  
        TrVCuts                                ) ## the base
    _decorated |= _LoKiCore.decorateCalls       (
        _functions                             , ## list of functor types
        LoKi.Dicts.CutCalls (_HTV)             ) ## call-traits
    _decorated |= _LoKiCore.decoratePredicateOps (
        _functions                             , ## list of functor types
        LoKi.Dicts.CutsOps  (_HTV,_HTV)        ) ## operators&operations
    
    from LoKiTrigger.functions import _HVV
    
    ## vertex pair -> double
    
    _functions = _LoKiCore.getInherited (
        name                                   , ## moduel name  
        RVVFunc                                ) ## the base
    _decorated |= _LoKiCore.decorateCalls       (
        _functions                             , ## list of functor types
        LoKi.Dicts.FunCalls (_HVV)             ) ## call-traits
    _decorated |= _LoKiCore.decorateFunctionOps (
        _functions                             , ## list of functor types
        LoKi.Dicts.FuncOps  (_HVV,_HVV)        ) ## operators&operations
    
    ## vertex pair -> bool
    
    _functions = _LoKiCore.getInherited (
        name                                   , ## moduel name  
        RVVCuts                                ) ## the base
    _decorated |= _LoKiCore.decorateCalls       (
        _functions                             , ## list of functor types
        LoKi.Dicts.CutCalls (_HVV)             ) ## call-traits
    _decorated |= _LoKiCore.decoratePredicateOps (
        _functions                             , ## list of functor types
        LoKi.Dicts.CutsOps  (_HVV,_HVV)        ) ## operators&operations
    
    ## functional stuff
    _vt = 'std::vector<LHCb::Track*>'     ## std.vector ( _pt      )
    _vv = 'std::vector<LHCb::RecVertex*>' ## std.vector ( _pv      )
    _vd = 'std::vector<double>'           ## std.vector ( 'double' )

    
    # "map" :  vector<T> -> vector<double>
    

    _functions = _LoKiCore.getInherited (
        name                                   , ## moduel name  
        LoKi.Functor (_vt,_vd)                 ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.MapsOps(_pt)                ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.MapsOps(_pt)                ) ## call-traits
    
    _functions = _LoKiCore.getInherited (
        name                                   , ## moduel name  
        LoKi.Functor (_vv,_vd)                 ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.MapsOps(_pv)                ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.MapsOps(_pv)                ) ## call-traits

    # "pipe" : vector<T> -> vector<T>

    _functions = _LoKiCore.getInherited (
        name                                   , ## module name  
        LoKi.Functor   (_vt,_vt)               ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.PipeOps(_pt,_t)             ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.PipeOps(_pt,_t)             ) ## call-traits

    _functions = _LoKiCore.getInherited (
        name                                   , ## module name  
        LoKi.Functor   (_vv,_vv)               ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.PipeOps(_pv,_v)             ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.PipeOps(_pv,_v)             ) ## call-traits
  
    # "funval" : vector<T> -> double

    _functions = _LoKiCore.getInherited (
        name                                   , ## module name  
        LoKi.Functor   (_vt,'double')          ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.FunValOps(_pt)              ) ## call-traits
    _decorated |= _LoKiCore.decorateFunctionOps (
        _functions                             , ## list of functor types
        LoKi.Dicts.FunValOps(_pt)              ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.FunValOps(_pt)              ) ## call-traits

    _functions = _LoKiCore.getInherited (
        name                                   , ## module name  
        LoKi.Functor   (_vv,'double')          ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.FunValOps(_pv)              ) ## call-traits
    _decorated |= _LoKiCore.decorateFunctionOps (
        _functions                             , ## list of functor types
        LoKi.Dicts.FunValOps(_pv)              ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.FunValOps(_pv)              ) ## call-traits

    # "cutval" : vector<T> -> bool
    
    _functions = _LoKiCore.getInherited (
        name                                   , ## module name  
        LoKi.Functor   (_vt,bool)              ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.CutValOps(_pt)              ) ## call-traits
    _decorated |= _LoKiCore.decoratePredicateOps (
        _functions                             , ## list of functor types
        LoKi.Dicts.CutValOps(_pt)              ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.CutValOps(_pt)              ) ## call-traits

    _functions = _LoKiCore.getInherited (
        name                                   , ## module name  
        LoKi.Functor   (_vv,bool)              ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.CutValOps(_pv)              ) ## call-traits
    _decorated |= _LoKiCore.decorateFunctionOps (
        _functions                             , ## list of functor types
        LoKi.Dicts.CutValOps(_pv)              ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.CutValOps(_pv)              ) ## call-traits

    # "element": vector<T> -> T

    _functions = _LoKiCore.getInherited (
        name                                   , ## module name  
        LoKi.Functor   (_vt,_pt)               ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.ElementOps(_pt,_t)          ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.ElementOps(_pt,_t)          ) ## call-traits

    _functions = _LoKiCore.getInherited (
        name                                   , ## module name  
        LoKi.Functor   (_vv,_pv)               ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.ElementOps(_pv,_v)          ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.ElementOps(_pv,_v)          ) ## call-traits
  
    # "source" : void -> vector<T>

    _functions = _LoKiCore.getInherited (
        name                                   , ## module name  
        LoKi.Functor   ('void',_vt)            ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.SourceOps(_pt,_t)           ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.SourceOps(_pt,_t)           ) ## call-traits

    _functions = _LoKiCore.getInherited (
        name                                   , ## module name  
        LoKi.Functor   ('void',_vv)            ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.SourceOps(_pv,_v)           ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.SourceOps(_pv,_v)           ) ## call-traits

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

