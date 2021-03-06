#!/usr/bin/env python
# =============================================================================
## @file functions.py LoKiPhys/trees.py
#  The set of basic decorator for Tree-objects from LoKiPhys library
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
The set of basic decorators for Tree-objects from LoKiPhys library

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

import LoKiCore.decorators    as      _LoKiCore

# Namespaces:
from LoKiCore.basic import cpp, std, LoKi 
Decays   = cpp.Decays

_RCP = 'const LHCb::Particle*'

_decorated = _LoKiCore.decorateTrees (
    ( Decays.iTree_        ( _RCP ) ,
      Decays.Tree_         ( _RCP ) ,
      Decays.Trees.Any_    ( _RCP ) ,
      Decays.Trees.None_   ( _RCP ) ,
      Decays.Trees.Stable_ ( _RCP ) ,
      Decays.Trees.Exclusive ,
      Decays.Trees.Inclusive ,
      Decays.Trees.Optional  ) ,
    LoKi.Dicts.TreeOps( _RCP ) )

## decay trees
iTree     = Decays.iTree_       ( _RCP )
Tree      = Decays.Tree_        ( _RCP )
PAny      = Decays.Trees.Any_   ( _RCP ) () # instance!!
PNone     = Decays.Trees.None_  ( _RCP ) () # instance!!  
PStable   = Decays.Trees.None_  ( _RCP )  
Exclusive = Decays.Trees.Exclusive
Inclusive = Decays.Trees.Inclusive
Optional  = Decays.Trees.Optional

IDecay = Decays.IDecay
Finder = _LoKiCore.decorateFinder (
    IDecay.Finder               ,
    LoKi.Dicts.FinderDicts_(_RCP)
    ) 



# =============================================================================
if '__main__' == __name__ :
    for o in dir() : print o        
    
# =============================================================================
# The END 
# =============================================================================
