#!/usr/bin/env python
# =============================================================================
# $Id: trees.py,v 1.3 2010-05-30 17:08:28 ibelyaev Exp $ 
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
__author__  = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
__date__    = "????-??-??"
__version__ = "CVS Tag: $Name: not supported by cvs2svn $, version $Revision: 1.3 $ "
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

IMCDecay = Decays.IMCDecay
MCFinder = _LoKiCore.decorateFinder (
    IMCDecay.Finder               ,
    LoKi.Dicts.FinderDicts_(_MCP)
    ) 

# =============================================================================
if '__main__' == __name__ :
    
    print 80*'*'
    print __doc__
    print ' Author  : ' , __author__
    print ' Version : ' , __version__
    print ' Date    : ' , __date__    
    print 80*'*'
    for i in dir() : print i 
    
# =============================================================================
# The END 
# =============================================================================
