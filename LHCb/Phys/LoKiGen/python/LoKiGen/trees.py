#!/usr/bin/env python
# =============================================================================
## @file functions.py LoKiGen/trees.py
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
The set of basic decorators for Tree-objects from LoKiGen library

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

_GP = 'const HepMC::GenParticle*'

_decorated = _LoKiCore.decorateTrees (
    ( Decays.iTree_        ( _GP ) ,
      Decays.Tree_         ( _GP ) ,
      Decays.Trees.Any_    ( _GP ) ,
      Decays.Trees.None_   ( _GP ) ,
      Decays.Trees.Stable_ ( _GP ) ,
      Decays.Trees.GenExclusive  ,
      Decays.Trees.GenInclusive  ,
      Decays.Trees.GenOptional   ,
      Decays.Trees.GenPhotos     ,
      Decays.Trees.GenPhotosOptional ) , LoKi.Dicts.TreeOps( _GP ) )

#print dir( LoKi.Dicts.TreeOps(_GP) )

## decay trees
iGTree             = Decays.iTree_        ( _GP )
GTree              = Decays.Tree_         ( _GP )
GAny               = Decays.Trees.Any_    ( _GP ) () ## instance!!!
GNone              = Decays.Trees.None_   ( _GP ) () ## instance!!!
GStable            = Decays.Trees.Stable_ ( _GP )
GenExclusive       = Decays.Trees.GenExclusive
GenInclusive       = Decays.Trees.GenInclusive
GenOptional        = Decays.Trees.GenOptional
GenPhotos          = Decays.Trees.GenPhotos
GenPhotosOptional  = Decays.Trees.GenPhotosOptional


IGenDecay = Decays.IGenDecay
GenFinder = _LoKiCore.decorateFinder (
    IGenDecay.Finder               ,
    LoKi.Dicts.FinderDicts_(_GP)
    )

# =============================================================================
#if '__main__' == __name__ :
#    for o in dir() : print o

# =============================================================================
# The END
# =============================================================================
