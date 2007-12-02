#!/usr/bin/env python
# =============================================================================
## @file decorators.py LoKiCore/decorators.py
#  The set of basic decorator for objects from LoKiGenMC library
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
#
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
"""
The set of basic decorators for obejcts from LoKiGenMC library

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

## import sys,PyCintex
## if sys.platform == 'win32' :
##     PyCintex.loadDict (      "HepMCRflx" )  
##     PyCintex.loadDict (    "LoKiGenDict" )
##     PyCintex.loadDict (     "LoKiMCDict" )
## else                       :
##     PyCintex.loadDict ( "libHepMCRflx"   )  
##     PyCintex.loadDict ( "libLoKiGenDict" )
##     PyCintex.loadDict ( "libLoKiMCDict"  )

import LoKiCore.decorators as _LoKiCore
from   LoKiGenMC.functions import * 

_name = __name__

# =============================================================================
## make a decoration of the actual functions 
def decorate ( name = _name ) :

    _mcp = 'const LHCb::MCParticle*'
    _mcv = 'const LHCb::MCVertex*'
    std  = _LoKiCore.std 
    _vp  = std.vector( _mcp )
    _vv  = std.vector( _mcv )
    _vd  = std.vector( 'double' )
    ## MC :
    _decorated  = _LoKiCore.getAndDecorateFunctions (
        name                                          , ## module name 
        LoKi.Functor        (_mcp,'double')           , ## the base 
        LoKi.Dicts.FunCalls (LHCb.MCParticle)         , ## call-traits 
        LoKi.Dicts.FuncOps  (_mcp,_mcp)               ) ## operators 
    _decorated |= _LoKiCore.getAndDecorateFunctions  (
        name                                          , ## module name 
        LoKi.Functor        (_mcv,'double')           , ## the base 
        LoKi.Dicts.FunCalls (LHCb.MCVertex)           , ## call-traits 
        LoKi.Dicts.FuncOps  (_mcv,_mcv)               ) ## operators
    _decorated |=  _LoKiCore.getAndDecoratePredicates (
        name                                          , ## module name 
        LoKi.Functor        (_mcp,bool)               , ## the base 
        LoKi.Dicts.CutCalls (LHCb.MCParticle)         , ## call-traits 
        LoKi.Dicts.CutsOps  (_mcp,_mcp)               ) ## operators 
    _decorated |= _LoKiCore.getAndDecoratePredicates (
        name                                          , ## module name 
        LoKi.Functor        (_mcv,bool)               , ## the base 
        LoKi.Dicts.CutCalls (LHCb.MCVertex)           , ## call-traits 
        LoKi.Dicts.CutsOps  (_mcv,_mcv)               ) ## the operators
    ## HepMC:
    _gp  = 'const HepMC::GenParticle*'
    _gv  = 'const HepMC::GenVertex*'
    _vgp = std.vector ( _gp      )
    _vgv = std.vector ( _gv      )
    _vd  = std.vector ( 'double' )
    _decorated  |= _LoKiCore.getAndDecorateFunctions (
        name                                             , ## module name 
        LoKi.Functor('const HepMC::GenParticle*,double') , ## the base 
        LoKi.Dicts.FunCalls(HepMC.GenParticle)           , ## call-traits 
        LoKi.Dicts.FuncOps(_gp,_gp)                      ) ## operators
    _decorated |= _LoKiCore.getAndDecorateFunctions  (
        name                                             , ## module name 
        LoKi.Functor('const HepMC::GenVertex*,double')   , ## the base 
        LoKi.Dicts.FunCalls(HepMC.GenVertex)             , ## call-traits 
        LoKi.Dicts.FuncOps(_gv,_gv)                      ) ## operators
    _decorated |=  _LoKiCore.getAndDecoratePredicates (
        name                                             , ## module name 
        LoKi.Functor('const HepMC::GenParticle*,bool')   , ## the base 
        LoKi.Dicts.CutCalls(HepMC.GenParticle)           , ## call-traits 
        LoKi.Dicts.CutsOps(_gp,_gp)                      ) ## operators 
    _decorated |= _LoKiCore.getAndDecoratePredicates (
        name                                             , ## module name 
        LoKi.Functor('const HepMC::GenVertex*,bool')     , ## the base 
        LoKi.Dicts.CutCalls(HepMC.GenVertex)             , ## call-traits 
        LoKi.Dicts.CutsOps(_gv,_gv)                      ) ## the operators
    ## 
    return _decorated                                      ## RETURN 
# =============================================================================

# =============================================================================
## perform the actual decoration 
_decorated = decorate() 
# =============================================================================

# =============================================================================
if '__main__' == __name__ :
    print 'Number of properly decorated types: %s'%len(_decorated)
    for o in _decorated : print o
    
# =============================================================================
# The END
# =============================================================================
