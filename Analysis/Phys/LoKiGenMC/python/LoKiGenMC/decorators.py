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
__author__  = "Vanya BELYAEV ibelyaev@physics.syr.edu"
__version__ = "CVS tag $NaMe:$, version $Revision$  "
# =============================================================================

import LoKiCore.decorators as _LoKiCore
from   LoKiGenMC.functions import * 

_name = __name__

# =============================================================================
## make a decoration of the actual functions 
def decorate ( name = _name ) :

    _mcp = 'const LHCb::MCParticle*'
    _mcv = 'const LHCb::MCVertex*'

    ## MC :

    # MCParticle -> double
    
    _decorated  = _LoKiCore.getAndDecorateFunctions ( 
        name                                   , ## module name  
        LoKi.Functor   (_mcp,'double')         , ## the base
        LoKi.Dicts.FunCalls (LHCb.MCParticle)  , ## call-traits
        LoKi.Dicts.FuncOps  (_mcp,_mcp)        ) ## operators&operations
    
    # MCVertex -> double
    
    _decorated |= _LoKiCore.getAndDecorateFunctions ( 
        name                                   , ## module name  
        LoKi.Functor   (_mcv,'double')         , ## the base
        LoKi.Dicts.FunCalls (LHCb.MCVertex)    , ## call-traits
        LoKi.Dicts.FuncOps  (_mcv,_mcv)        ) ## operators&operations
    
    # MCParticle -> bool
    
    _decorated |= _LoKiCore.getAndDecoratePredicates (  
        name                                   , ## module name  
        LoKi.Functor   (_mcp,bool)             , ## the base
        LoKi.Dicts.CutCalls (LHCb.MCParticle)  , ## call-traits
        LoKi.Dicts.CutsOps  (_mcp,_mcp)        ) ## operators&operations

    # MCVertex -> bool
    
    _decorated |= _LoKiCore.getAndDecoratePredicates (  
        name                                   , ## moduel name  
        LoKi.Functor   (_mcv,bool)             , ## the base
        LoKi.Dicts.CutCalls (LHCb.MCVertex)    , ## call-traits
        LoKi.Dicts.CutsOps  (_mcv,_mcv)        ) ## operators&operations
  
    ## HepMC:
    
    _gp  = 'const HepMC::GenParticle*'
    _gv  = 'const HepMC::GenVertex*'

    # HepMC -> double
    
    _decorated |= _LoKiCore.getAndDecorateFunctions ( 
        name                                    , ## moduel name  
        LoKi.Functor   (_gp,'double')           , ## the base
        LoKi.Dicts.FunCalls (HepMC.GenParticle) , ## call-traits
        LoKi.Dicts.FuncOps  (_gp,_gp)           ) ## operators&operations
    _decorated |= _LoKiCore.getAndDecorateFunctions ( 
        name                                    , ## module name  
        LoKi.Functor   (_gv,'double')           , ## the base
        LoKi.Dicts.FunCalls (HepMC.GenVertex)   , ## call-traits
        LoKi.Dicts.FuncOps  (_gv,_gv)           ) ## operators&operations

    # HepMC -> bool

    _decorated |= _LoKiCore.getAndDecoratePredicates (  
        name                                    , ## module name  
        LoKi.Functor   (_gp,bool)               , ## the base
        LoKi.Dicts.CutCalls (HepMC.GenParticle) , ## call-traits
        LoKi.Dicts.CutsOps  (_gp,_gp)           ) ## operators&operations
    _decorated |= _LoKiCore.getAndDecoratePredicates (  
        name                                    , ## module name  
        LoKi.Functor   (_gv,bool)               , ## the base
        LoKi.Dicts.CutCalls (HepMC.GenVertex)   , ## call-traits
        LoKi.Dicts.CutsOps  (_gv,_gv)           ) ## operators&operations

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
