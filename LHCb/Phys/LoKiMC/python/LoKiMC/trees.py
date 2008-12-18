#!/usr/bin/env python
# =============================================================================
## @file functions.py LoKiMC/trees.py
#  The set of basic decorator for Tree-objects from LoKiMC library
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
The set of basic decorators for Tree-objects from LoKiMC library

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

_MCP = 'const LHCb::MCParticle*'

_decorated = _LoKiCore.decorateTrees (
    ( Decays.iTree_      ( _MCP ) ,
      Decays.Tree_       ( _MCP ) ,
      Decays.Trees.Any_  ( _MCP ) ,
      Decays.Trees.MCExclusive    ,
      Decays.Trees.MCInclusive    ,
      Decays.Trees.MCOptional     ,
      Decays.Trees.Photos         ,
      Decays.Trees.PhotosOptional ) , LoKi.Dicts.TreeOps( _MCP ) )


## decay trees
iMCTree             = Decays.iTree_       ( _MCP )
MCTree              = Decays.Tree_        ( _MCP )
MCAny               = Decays.Trees.Any_   ( _MCP ) () ## instance!!!
MCExclusive         = Decays.Trees.MCExclusive
MCInclusive         = Decays.Trees.MCInclusive
MCOptional          = Decays.Trees.MCOptional
MCPhotos            = Decays.Trees.Photos
MCPhotosOptional    = Decays.Trees.PhotosOptional
Photos              = Decays.Trees.Photos
PhotosOptional      = Decays.Trees.PhotosOptional


# =============================================================================
if '__main__' == __name__ :
    for o in dir() : print o        
    
# =============================================================================
# The END 
# =============================================================================
