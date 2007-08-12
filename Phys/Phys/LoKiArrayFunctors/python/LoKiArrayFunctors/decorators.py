#!/usr/bin/env python
# =============================================================================
## @file 
#  The set of basic decorator for objects from LoKiHlt library
#  The file is a part of LoKi and Bender projects
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @daet 2007-06-09
# =============================================================================
""" The set of basic decorators for objects from LoKiHlt library """
_author_ = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
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
    ## regular functors which accept the track
    _decorated  = _LoKiCore.getAndDecorateFunctions (
        name                                           , ## module name
        AFunc                                          , ## the base 
        LoKi.Dicts.FunCalls(_T)                        , ## call-traits 
        LoKi.Dicts.FuncOps(_T)                         ) ## operators 
    _decorated |=  _LoKiCore.getAndDecoratePredicates (
        name                                            , ## module name
        ACuts                                           , ## the base 
        LoKi.Dicts.CutCalls(_T)                         , ## call-traits 
        LoKi.Dicts.FuncOps(_T)                          ) ## operators 
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

