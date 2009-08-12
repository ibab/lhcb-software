#!/usr/bin/env python
# =============================================================================
## @file decorators.py LoKiPhysMC/decorators.py
#  The set of basic decorator for objects from LoKiPhysMC library
#  The file is a part of LoKi and Bender projects
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
"""
The set of basic decorators for obejcts from LoKiPhysMC library
"""
# =============================================================================
__author__ = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
# =============================================================================

from LoKiCore.decorators import LoKi 
from LoKiCore.decorators import LHCb

MCTRUTH  = LoKi.PhysMCParticles.MCTruth 
GMCTRUTH = LoKi.PhysMCParticles.GenMCTruth
RCTRUTH  = LoKi.PhysMCParticles.RCTruth


MCSELMATCH   = LoKi.PhysMCParticles.MCSelMatch
MCTREEMATCH  = LoKi.PhysMCParticles.MCTreeMatch
MCNODEMATCH  = LoKi.PhysMCParticles.MCNodeMatch

MCMATCH      = MCTREEMATCH



# =============================================================================
def _mcMatch  ( func , obj , *args ) :
    """
    Helper function to create functor for Monte Carlo truth match
    
    """
    from LoKiCore.functions import strings
    
    if not args : return func ( obj )
    
    if 1 == len(args) :
        lst = args[0]
        if type ( lst ) in ( list , tuple ) :
            return func ( obj , strings ( lst ) )
        return func ( obj , lst )
    
    return func ( obj , srings ( args ) ) 

# =============================================================================
def mcMatch ( obj , *args ) :
    """
    Helper function to define MC-truth matchig functor


    >>> cut1 = mcMatch ( 'B0->pi+ pi-' )

    
    >>> cut2 = mcMatch ( 'B0-> ^pi+ ^pi-' )

    
    >>> cut3 = mcMatch ( 'pi+' == MCABSID )

    
    >>> cut4 = mcMatch ( Nodes.StableCharged  )
    
    
    ATTENTION: It is only for
    CombineParticles/FilterDesktop/DecaytreeTuple framework
    
    """
    from PartProp.Nodes    import iNode
    from PartProp.Nodes    import iMCTree
    from LoKiMC.decorators import MCCuts
    #
    if   issubclass ( obj.__class__ , str     )  :
        return _mcMatch ( MCTREEMATCH , obj , *args )
    elif issubclass ( obj.__class__ , iNode   ) :
        return _mcMatch ( MCNODEMATCH , obj , *args )
    elif issubclass ( obj.__class__ , MCCuts  ) :
        return _mcMatch ( MCSELMATCH  , obj , *args )
    elif issubclass ( obj.__class__ , iMCTree )  :
        return _mcMatch ( MCTREEMATCH , obj , *args )
    
    raise AttributeError, 'Unknown object type %s'%type(obj)


# =============================================================================
if '__main__' == __name__ :
    for o in dir() : print o        

# =============================================================================
# The END 
# =============================================================================

