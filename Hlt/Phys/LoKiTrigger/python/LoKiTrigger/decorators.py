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

from   LoKiTrack.functions   import *

_name = __name__ 
# =============================================================================
## make the decoration of all objects fomr this module
def _decorate ( name = _name  ) :
    """ Make the decoration of all objects fomr this module """
    import LoKiCore.decorators as _LoKiCore
    ## regular functors which accept the track
    _decorated  = _LoKiCore.getAndDecorateFunctions (
        name                                            , ## module name
        TrFunc                                          , ## the base 
        LoKi.Dicts.FunCalls(LHCb.Track)                 , ## call-traits 
        LoKi.Dicts.FuncOps(LHCb.Track)                  ) ## operators 
    _decorated |=  _LoKiCore.getAndDecoratePredicates (
        name                                            , ## module name
        TrCuts                                          , ## the base 
        LoKi.Dicts.CutCalls(LHCb.Track)                 , ## call-traits 
        LoKi.Dicts.FuncOps(LHCb.Track)                  ) ## operators 
    ## regular functors which accept the vertex 
    _decorated |= _LoKiCore.getAndDecorateFunctions  (
        name                                            , ## module name
        RVFunc                                          , ## the base 
        LoKi.Dicts.FunCalls(LHCb.RecVertex)             , ## call-traits 
        LoKi.Dicts.FuncOps(LHCb.RecVertex)              ) ## operators
    _decorated |= _LoKiCore.getAndDecoratePredicates (
        name                                            , ## module name 
        RVCuts                                          , ## the base 
        LoKi.Dicts.CutCalls(LHCb.RecVertex)             , ## call-traits 
        LoKi.Dicts.FuncOps(LHCb.RecVertex)              ) ## the operators
    ## regular functors which accept two tracks
    from LoKiTrack.functions import _HTT
    _decorated |= _LoKiCore.getAndDecorateFunctions  (
        name                                            , ## module name
        TTrFunc                                         , ## the base 
        LoKi.Dicts.FunCalls(_HTT)                       , ## call-traits 
        LoKi.Dicts.FuncOps(_HTT)                        ) ## operators
    _decorated |= _LoKiCore.getAndDecoratePredicates (
        name                                            , ## module name 
        TTrCuts                                         , ## the base 
        LoKi.Dicts.CutCalls(_HTT)                       , ## call-traits 
        LoKi.Dicts.FuncOps(_HTT)                        ) ## the operators
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

