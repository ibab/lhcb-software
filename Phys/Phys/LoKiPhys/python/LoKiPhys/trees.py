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
_global  = _LoKiCore._global
cpp      = _global
std      = cpp.std
Decays   = cpp.Decays
LoKi     = cpp.LoKi

_RCP = 'const LHCb::Particle*'

## decay trees
iTree     = Decays.iTree_       ( _RCP )
Tree      = Decays.Tree_        ( _RCP )
_Tree_    = Decays.Trees._Tree_ ( _RCP )
SubTrees  = std.vector( _Tree_ )
Exclusive = Decays.Trees.Exclusive
Inclusive = Decays.Trees.Inclusive
Optional  = Decays.Trees.Optional


_decorated = _LoKiCore.decorateTrees (
    ( iTree , Tree  , Exclusive , Inclusive , Optional ) ,
    LoKi.Dicts.TreeOps( _RCP ) )

# =============================================================================
if '__main__' == __name__ :
    for o in dir() : print o        
    
# =============================================================================
# The END 
# =============================================================================
