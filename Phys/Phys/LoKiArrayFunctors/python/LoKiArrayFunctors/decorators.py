#!/usr/bin/env python
# =============================================================================
# $Id: decorators.py,v 1.3 2007-12-02 17:10:45 ibelyaev Exp $ 
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
    ## regular functors which accept the track
    _decorated  = _LoKiCore.getAndDecorateFunctions (
        name                                           , ## module name
        AFunc                                          , ## the base 
        LoKi.Dicts.FunCalls(_T)                        , ## call-traits 
        LoKi.Dicts.FuncOps(_T,_T)                      ) ## operators 
    _decorated |=  _LoKiCore.getAndDecoratePredicates (
        name                                            , ## module name
        ACuts                                           , ## the base 
        LoKi.Dicts.CutCalls(_T)                         , ## call-traits 
        LoKi.Dicts.CutsOps(_T,_T)                       ) ## operators 
    ## functional:
    _vd = std.vector( 'double' )
    _va = std.vector( _T ) 
    _decorated |= _LoKiCore.getAndDecorateMaps       (
        name                                             , ## module name
        LoKi.Functor( _va , _vd  )                       , ## the base
        LoKi.Dicts.MapsOps(_T)                           ) ## stremers
    _decorated |= _LoKiCore.getAndDecorateMaps       (
        name                                             , ## module name
        LoKi.Functor( _va , _va )                        , ## the base
        LoKi.Dicts.PipeOps(_T,_T)                        ) ## stremers
    _decorated |= _LoKiCore.getAndDecorateMaps       (
        name                                             , ## module name
        LoKi.Functor( _va , 'double' )                   , ## the base
        LoKi.Dicts.FunValOps(_T)                         ) ## stremers
    _decorated |= _LoKiCore.getAndDecorateMaps       (
        name                                             , ## module name
        LoKi.Functor( _va , _T )                         , ## the base
        LoKi.Dicts.ElementOps(_T,_T)                     ) ## stremers
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

