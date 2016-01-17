#!/usr/bin/env python
# =============================================================================
# $Id$ 
# =============================================================================
## @file LoKiMC/MC.py
#  collection of utilities for useful 'decoration' of MC-objects
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
#  @date 2007-08-11
#
#                    $Revision$
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""
Collection of utilities for useful 'decoration' of MC-objects

      This file is a part of LoKi project - 
'C++ ToolKit  for Smart and Friendly Physics Analysis'

The package has been designed with the kind help from
Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
contributions and advices from G.Raven, J.van Tilburg, 
A.Golutvin, P.Koppenburg have been used in the design.

"""
# =============================================================================
__author__  = 'Vanya BELYAEV ibelyaev@physics.syr.edu'
__date__    = "2007-08-11"
__version__ = "$Revision$ "
# =============================================================================

from   LoKiMC.functions    import LoKi,LHCb,cpp


# ============================================================
## get the decay vertex for the particle
def decayVertex ( mcp ) :
    """
    Get the decay vertex for the (MC) particle
    
    >>> mcp = ...
    >>> decVertex = decayVertex ( mcp )

    >>> mcp = ...
    >>> decVertex = mcp.decayVertex()
    
    """
    return LoKi.MCVertices.decayVertex ( mcp )

decayVertex . __doc__  +=  "\n\n" + LoKi.MCVertices.decayVertex    . __doc__ 

LHCb.MCParticle  .   decayVertex      = decayVertex 
LHCb.MCParticle  . __decayVertex__    = decayVertex 


# =============================================================================
## Get number of child particles :
def nChildren ( p ) :
    """
    Get the number of child particles :

    >>> n = nChildren ( p )
    
    """
    return LoKi.MCChild.nChildren ( p )

nChildren . __doc__  +=  "\n\n" + LoKi.MCChild.nChildren    . __doc__ 

LHCb.MCParticle  . nChildren      = nChildren
LHCb.MCParticle  . nDaughters     = nChildren
LHCb.MCParticle  . __nChildren__  = nChildren
LHCb.MCParticle  . __nDaughters__ = nChildren

LHCb.MCParticle.hex_id  = lambda s : LoKi.MCParticles.hex_id ( s ) 

# =============================================================================
## get the child 
def child ( p , *a ) :
    """
    Get the child for 'particle'
    
    >>> p = ...
    >>> d1  = child ( p , 1 ) 
    >>> d12 = child ( p , 1 , 2 )

    ATTTENTION: indices start from 1.
    Index 0 corresponds to the particle itself.
    """
    return LoKi.MCChild.child ( p , *a )

child . __doc__  +=  "\n\n" + LoKi.MCChild.child    . __doc__ 

LHCb.MCParticle  .   child      = child 
LHCb.MCParticle  . __child__    = child 
LHCb.MCParticle  .   daughter   = child 
LHCb.MCParticle  . __daughter__ = child 

LHCb.MCParticle  . __call__     = child
LHCb.MCParticle  . __getitem__  = child

# =============================================================================
## Get all children from the given LHCb::MCParticle 
def children ( o , *a ) :
    """
    Get all children from the given LHCb::MCParticle

    >>> o = ...
    >>> for p in children ( o ) :
    ...    print p
    
    """
    return LoKi.MCChild.children ( o , *a )

children . __doc__ += "\n\n" + LoKi.MCChild.children . __doc__  

LHCb.MCParticle .   children    =  children
LHCb.MCParticle . __children__  =  children
LHCb.MCParticle .   daughters   =  children
LHCb.MCParticle . __daughters__ =  children


# =============================================================================
## Get all descendants from the given LHCb::MCParticle 
def descendants ( o , *a ) :
    """
    Get all descendants from the given LHCb::MCParticle

    >>> o = ...
    >>> for p in descendants ( o ) :
    ...    print p
    
    """
    return LoKi.MCChild.descendants ( o , *a )

descendants . __doc__ += "\n\n" + LoKi.MCChild.descendants . __doc__  

LHCb.MCParticle .   descendants    =  descendants
LHCb.MCParticle . __descendants__  =  descendants

# =============================================================================
## Get all ancestors from the given LHCb::MCParticle 
def ancestors ( o ) :
    """
    Get all ancestors from the given LHCb::MCParticle

    >>> o = ...
    >>> for p in ancestors ( o ) :
    ...    print p
    
    """
    return LoKi.MCChild.ancestors ( o )

ancestors . __doc__ += "\n\n" + LoKi.MCChild.ancestors . __doc__  

LHCb.MCParticle .   ancestors    =  ancestors
LHCb.MCParticle . __ancestors__  =  ancestors

# =============================================================================
## Get all (0 or 1) parents  from the given LHCb::MCParticle 
def parents ( o ) :
    """
    Get all parents from the given LHCb::MCParticle

    >>> o = ...
    >>> for p in parents ( o ) :
    ...    print p
    
    """
    return LoKi.MCChild.parents ( o )

parents . __doc__ += "\n\n" + LoKi.MCChild.parents . __doc__  

LHCb.MCParticle .   parents    = parents
LHCb.MCParticle . __parents__  = parents

# =============================================================================
## Get the mother particle:
def mother ( p ) :
    """
    Get the mother particle:

    >>> p = ...
    >>> m = mother ( p )
    
    """
    return LoKi.MCChild.mother ( p )

mother . __doc__ += "\n\n" + LoKi.MCChild.mother . __doc__  

if not hasattr ( LHCb.MCParticle ,   'mother'   ) : LHCb.MCParticle .   mother     = mother
if not hasattr ( LHCb.MCParticle , '__mother__' ) : LHCb.MCParticle . __mother__   = mother


# =============================================================================
## Get certain MC-particles form the decay tree:
def mcParticles ( p , *a ) :
    """
    Get certain MC-particles form the decay tree:

    >>> p = ...
    >>> daughterPions = mcParticles ( p , 'pi+' == MCABSID , True )

    """
    return LoKi.MCExtract.mcParticles ( p , *a )

mcParticles . __doc__ += "\n\n" + LoKi.MCExtract.mcParticles . __doc__  

getParticles = mcParticles 
particles    = mcParticles
extract      = mcParticles

# =============================================================================
## Count number of 'interesting' elements in the decay tree or container,
def count_if ( s , *a ) :
    """
    Count number of 'interesting' elements in the decay tree or container,
    which satisfy certain criteria

    >>> o = ...
    >>> number_of_pions = count_if ( o , 'pi+' == MCABSID )
    
    """
    return LoKi.Dicts.MCAlgs.count_if ( s , *a )

count_if . __doc__ += "\n\n" + LoKi.Dicts.MCAlgs.count_if . __doc__  

# =============================================================================
## Check the presence of 'interesting' elements in the decay tree or container,
def found ( s , *a ) :
    """
    check the presence of 'interesting' elements in the decay tree or container,
    which satisfy certain criteria

    >>> o = ...
    >>> good = found ( o , 'pi+' == MCABSID )
    
    """
    return LoKi.Dicts.MCAlgs.found ( s , *a )

found . __doc__ += "\n\n" + LoKi.Dicts.MCAlgs.found . __doc__  


# =============================================================================
## Accumulate through the decay tree or container,
def accumulate  ( s , *a ) :
    """
    Accumulate through the decay tree or container,

    >>> o = ...
    >>> ptOfAllPions= accumulate ( o , MCPT , 'pi+' == MCABSID )
    
    """
    return LoKi.Dicts.MCAlgs.accumulate ( s , *a )

accumulate. __doc__ += "\n\n" + LoKi.Dicts.MCAlgs.accumulate . __doc__  


# =============================================================================
## Find the minimal value through decay tree or  container  
def min_value ( s , *a ) :
    """
    Find the minimal value through decay tree or  container  

    >>> o = ...
    >>> minPT = min_value ( o , MCPT , 'pi+' == MCABSID )
    
    """
    return LoKi.Dicts.MCAlgs.min_value ( s , *a )

min_value. __doc__ += "\n\n" + LoKi.Dicts.MCAlgs.min_value . __doc__  

# =============================================================================
## Find the maximal value through decay tree or  container  
def max_value ( s , *a ) :
    """
    Find the maximal value through decay tree or  container  

    >>> o = ...
    >>> maxPT = max_value ( o , MCPT , 'pi+' == MCABSID )
    
    """
    return LoKi.Dicts.MCAlgs.max_value ( s , *a )

max_value. __doc__ += "\n\n" + LoKi.Dicts.MCAlgs.max_value . __doc__  

# =============================================================================
## Find the minimal element through decay tree or  container  
def min_element ( s , *a ) :
    """
    Find the minimal element through decay tree or  container  

    >>> o = ...
    >>> pion = min_element ( o , MCPT , 'pi+' == MCABSID )
    
    """
    return LoKi.Dicts.MCAlgs.min_element ( s , *a )

min_element. __doc__ += "\n\n" + LoKi.Dicts.MCAlgs.min_element . __doc__  


# =============================================================================
## Find the maximal element through decay tree or  container  
def max_element ( s , *a ) :
    """
    Find the maximal element through decay tree or  container  

    >>> o = ...
    >>> pion = max_element ( o , MCPT , 'pi+' == MCABSID )
    
    """
    return LoKi.Dicts.MCAlgs.max_element ( s , *a )

max_element. __doc__ += "\n\n" + LoKi.Dicts.MCAlgs.max_element . __doc__  

for t in ( LHCb.MCParticle , LHCb.MCVertex   ) :
    
    #@ MCAlgs:
    if not hasattr ( t ,   'count_if'       ) : t. count_if        = count_if  
    if not hasattr ( t , '__count_if__'     ) : t. __count_if__    = count_if  
    if not hasattr ( t ,   'found'          ) : t.   found         = found 
    if not hasattr ( t , '__found__'        ) : t. __found__       = found  
    if not hasattr ( t ,   'accumulate'     ) : t.   accumulate    = accumulate
    if not hasattr ( t , '__accumulate__'   ) : t. __accumulate__  = accumulate  
    if not hasattr ( t ,   'min_value'      ) : t.   min_value     = min_value
    if not hasattr ( t , '__min_value__'    ) : t. __min_value__   = min_value
    if not hasattr ( t ,   'max_value'      ) : t.   max_value     = max_value
    if not hasattr ( t , '__max_value__'    ) : t. __max_value__   = max_value
    if not hasattr ( t ,   'min_element'    ) : t.   min_element   = min_element
    if not hasattr ( t , '__min_element__'  ) : t. __min_element__ = min_element
    if not hasattr ( t ,   'max_element'    ) : t.   max_element   = max_element
    if not hasattr ( t , '__max_element__'  ) : t. __max_element__ = max_element
    
    if not hasattr ( t ,   'mcParticles'    ) : t.   mcParticles    = mcParticles 
    if not hasattr ( t , '__mcParticles__'  ) : t. __mcParticles__  = mcParticles 
    if not hasattr ( t ,   'getParticles'   ) : t.   getParticles   = mcParticles 
    if not hasattr ( t , '__getParticles__' ) : t. __getParticles__ = mcParticles 
    if not hasattr ( t ,   'particles'      ) : t.   particles      = mcParticles 
    if not hasattr ( t , '__particles__'    ) : t. __particles__    = mcParticles 
    if not hasattr ( t ,   'extract'        ) : t.   extract        = mcParticles 
    if not hasattr ( t , '__extract__'      ) : t. __extract__      = mcParticles 


# =============================================================================
## print the decay  
def printDecay ( s , *a ) :
    """
    Print the decay of LHCb::MCParticle

    >>> p = ...
    >>> printDecay ( p )
    
    """
    return LoKi.PrintMC.printDecay ( s , *a )

printDecay. __doc__ += "\n\n" + LoKi.PrintMC.printDecay . __doc__  

if not hasattr ( LHCb.MCParticle ,     'printDecay'   ) :
    LHCb.MCParticle .  printDecay   =   printDecay
    LHCb.MCParticle .       decay   =   printDecay
if not hasattr ( LHCb.MCParticle ,   '__printDecay__' ) :
    LHCb.MCParticle. __printDecay__ =   printDecay
    LHCb.MCParticle.      __decay__ =   printDecay


import LoKiCore.functions  as _LCF 
LHCb.MCParticle.__pname__ =  _LCF.__pname__
LHCb.MCParticle.  pname   =  _LCF.__pname__
LHCb.MCParticle.   name   =  _LCF.__pname__

_LCF.nChildren     . __doc__ += "\n" + LoKi.MCChild.nChildren         . __doc__ 
_LCF.child         . __doc__ += "\n" + LoKi.MCChild.child             . __doc__
_LCF.daughter      . __doc__ += "\n" + LoKi.MCChild.child             . __doc__ 
_LCF.parents       . __doc__ += "\n" + LoKi.MCChild.parents           . __doc__
_LCF.children      . __doc__ += "\n" + LoKi.MCChild.children          . __doc__
_LCF.daughters     . __doc__ += "\n" + LoKi.MCChild.children          . __doc__ 
_LCF.descendants   . __doc__ += "\n" + LoKi.MCChild.descendants       . __doc__ 
_LCF.ancestors     . __doc__ += "\n" + LoKi.MCChild.ancestors         . __doc__ 

_LCF.extract       . __doc__ += "\n" + LoKi.MCExtract.mcParticles . __doc__ 
_LCF.particles     . __doc__ += "\n" + LoKi.MCExtract.mcParticles . __doc__ 
_LCF.getParticles  . __doc__ += "\n" + LoKi.MCExtract.mcParticles . __doc__ 

_LCF.count_if      . __doc__ += "\n" + LoKi.Dicts.MCAlgs.count_if     . __doc__ 
_LCF.found         . __doc__ += "\n" + LoKi.Dicts.MCAlgs.found        . __doc__ 
_LCF.accumulate    . __doc__ += "\n" + LoKi.Dicts.MCAlgs.accumulate   . __doc__ 
_LCF.min_value     . __doc__ += "\n" + LoKi.Dicts.MCAlgs.min_value    . __doc__ 
_LCF.max_value     . __doc__ += "\n" + LoKi.Dicts.MCAlgs.max_value    . __doc__ 
_LCF.min_element   . __doc__ += "\n" + LoKi.Dicts.MCAlgs.min_element  . __doc__ 
_LCF.max_element   . __doc__ += "\n" + LoKi.Dicts.MCAlgs.max_element  . __doc__ 


_LCF.printDecay    . __doc__ += "\n" + LoKi.PrintMC.printDecay        . __doc__ 


# decorate SmartRefVectors:
for _t in ( LHCb.MCParticle ,
            LHCb.MCVertex   ) :
    
    _tt = cpp.SmartRefVector ( _t )
    # redefine the iterator 
    _tt.__iter__ = _LCF._iter_SRV_

    
LHCb.MCParticle.ConstVector = cpp.std.vector ('const LHCb::MCParticle*')
LHCb.MCVertex.ConstVector   = cpp.std.vector ('const LHCb::MCVertex*'  )
    
LHCb.MCParticle.Range = cpp.Gaudi.NamedRange_ ( LHCb.MCParticle.ConstVector ) 
LHCb.MCVertex.Range   = cpp.Gaudi.NamedRange_ ( LHCb.MCVertex.ConstVector   )

for r in ( LHCb.MCParticle.Range ,
           LHCb.MCVertex.Range   ,
           cpp.Gaudi.Range_ ( LHCb.MCParticle.ConstVector ) ,
           cpp.Gaudi.Range_ ( LHCb.MCVertex.ConstVector   ) ) :

    import LoKiCore.decorators as _LCD
    
    r.__iter__     = _LCD . _iter_1_
    r.__getslice__ = _LCD . _slice_
    r.__getitem__  =    r . __call__ 
    r.__setitem__  =          None
    

LHCb.MCParticle.Container = cpp.KeyedContainer(LHCb.MCParticle,'Containers::KeyedObjectManager<Containers::hashmap>')

# =============================================================================
## define various printers 
def _print_ ( self                                     ,
              accept   = None                          ,
              mark     = None                          ,
              maxDepth = 10                            ,
              vertex   = False                         , 
              mode     = LoKi.DecayChainBase.LV_WITHPT ,
              fg       = cpp.MSG.YELLOW                ,
              bg       = cpp.MSG.RED                   ,
              vertexe  = True                          ,
              vertexd  = True                          ) :
    
    """
    Define the print functions for some MC-objects
    for details see LoKi::MCDecayChain 
    """
    _printer = LoKi.MCDecayChain ( maxDepth ,
                                   vertex   , 
                                   mode     ,
                                   fg       ,
                                   bg       ,
                                   vertexe  ,
                                   vertexd  ) 
    if    accept and not mark : 
        return _printer.print_ ( self , accept )
    elif  accept and     mark : 
        return _printer.print_ ( self , accept , mark )
    #
    return _printer.print_ ( self )

_print_ . __doc__ += "\n" + LoKi.MCDecayChain.print_ . __doc__ 


for t in ( LHCb.MCParticle             ,
           LHCb.MCParticle.Range       , 
           LHCb.MCParticle.ConstVector ,
           LHCb.MCParticle.Container   ) :
    t._print_   = _print_
    t.__str__   = _print_
    t.__repr__  = _print_
    
                      

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
