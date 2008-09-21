#!/usr/bin/env python
# =============================================================================
## @file LoKiHlt/decorators.py
#  The set of basic decorator for objects from LoKiHlt library
#
#  This file is a part of LoKi project - 
#   'C++ ToolKit  for Smart and Friendly Physics Analysis'
#
#  The package has been designed with the kind help from
#  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
#  contributions and advices from G.Raven, J.van Tilburg, 
#  A.Golutvin, P.Koppenburg have been used in the design.
#
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
# =============================================================================
"""
The set of basic decorators for obejcts from LoKiHlt library

This file is a part of LoKi project - 
'C++ ToolKit  for Smart and Friendly Physics Analysis'

The package has been designed with the kind help from
Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
contributions and advices from G.Raven, J.van Tilburg, 
A.Golutvin, P.Koppenburg have been used in the design.
"""
# =============================================================================
__author__ = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl" 
__version__ = " CVS Tag $Name: not supported by cvs2svn $, version $Revision: 1.1.1.1 $  "
# =============================================================================

from   LoKiHlt.functions   import *
_name = __name__ 
# =============================================================================
## make the decoration of all objects fomr this module
def _decorate ( name = _name  ) :
    """
    Make the decoration of all objects fomr this module
    """
    import LoKiCore.decorators as _LoKiCore
    _o1  = 'const LHCb::ODIN*'
    _l0  = 'const LHCb::L0DUReport*'
    _hlt = 'const LHCb::HltDecReports*'
    o1   = LHCb.ODIN
    l0   = LHCb.L0DUReport
    hlt  = LHCb.HltDecReports

    ## L0 
    _decorated  = _LoKiCore.getAndDecorateFunctions (
        name                                             , ## module name 
        LoKi.Functor('const LHCb::L0DUReport*,double')   , ## the base 
        LoKi.Dicts.FunCalls(l0)                          , ## call-traits 
        LoKi.Dicts.FuncOps(_l0,_l0)                      ) ## operators
    _decorated |=  _LoKiCore.getAndDecoratePredicates (
        name                                             , ## module name 
        LoKi.Functor('const LHCb::L0DUReport*,bool')     , ## the base 
        LoKi.Dicts.CutCalls(l0)                          , ## call-traits 
        LoKi.Dicts.CutsOps(_l0,_l0)                      ) ## operators

    ## O1 
    _decorated |= _LoKiCore.getAndDecorateFunctions (
        name                                             , ## module name 
        LoKi.Functor('const LHCb::ODIN*,double')         , ## the base 
        LoKi.Dicts.FunCalls(o1)                          , ## call-traits 
        LoKi.Dicts.FuncOps(_o1,_o1)                      ) ## operators
    _decorated |=  _LoKiCore.getAndDecoratePredicates (
        name                                             , ## module name 
        LoKi.Functor('const LHCb::ODIN*,bool')           , ## the base 
        LoKi.Dicts.CutCalls(o1)                          , ## call-traits 
        LoKi.Dicts.CutsOps(_o1,_o1)                      ) ## operators
    
    ## HLT
    _decorated |= _LoKiCore.getAndDecorateFunctions (
        name                                             , ## module name 
        LoKi.Functor('const LHCb::HltDecReports*,double'), ## the base 
        LoKi.Dicts.FunCalls(hlt)                         , ## call-traits 
        LoKi.Dicts.FuncOps(_hlt,_hlt)                    ) ## operators
    _decorated |=  _LoKiCore.getAndDecoratePredicates (
        name                                             , ## module name 
        LoKi.Functor('const LHCb::HltDecReports*,bool')  , ## the base 
        LoKi.Dicts.CutCalls(hlt)                         , ## call-traits 
        LoKi.Dicts.CutsOps(_hlt,_hlt)                    ) ## operators
    
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
