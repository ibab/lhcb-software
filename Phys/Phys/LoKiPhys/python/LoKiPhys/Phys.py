#!/usr/bin/env python
# =============================================================================
## @file
#  collection of utilities for useful 'decoration' of Phys-objects
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
# =============================================================================
"""
Collection of utilities for useful 'decoration' of Phys-objects

      This file is a part of LoKi project - 
'C++ ToolKit  for Smart and Friendly Physics Analysis'

The package has been designed with the kind help from
Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
contributions and advices from G.Raven, J.van Tilburg, 
A.Golutvin, P.Koppenburg have been used in the design.
"""
# =============================================================================
__author__ = 'Vanya BELYAEV ibelyaev@physics.syr.edu'
# =============================================================================

from   LoKiPhys.functions   import LoKi,LHCb,cpp

# =============================================================================
## Get number of child particles :
def nChildren ( p ) :
    """
    Get the number of child particles :

    >>> n = nChildren ( p )
    
    """
    return p.daughters().size()

if not hasattr ( LHCb.Particle ,   'nChildren'    ) :
    LHCb.Particle .   nChildren    = nChildren
if not hasattr ( LHCb.Particle ,   'nDaughters'   ) :
    LHCb.Particle .   nDaughters   = nChildren
if not hasattr ( LHCb.Particle , '__nChildren__'  ) :
    LHCb.Particle . __nChildren__  = nChildren
if not hasattr ( LHCb.Particle , '__nDaughters__' ) :
    LHCb.Particle . __nDaughters__ = nChildren

# =============================================================================
## Get all children from the given LHCb::Particle 
def children ( o ) :
    """
    Get all children from the given LHCb::Particle

    >>> o = ...
    >>> for p in children ( o ) :
    ...    print p
    
    """
    return LoKi.Child.children ( o  )

if not hasattr ( LHCb.Particle ,   'children'    ) :
    LHCb.Particle .   children    =  children
if not hasattr ( LHCb.Particle , '__children__'  ) :
    LHCb.Particle . __children__  =  children
if not hasattr ( LHCb.Particle ,   'daughters'   ) :
    LHCb.Particle .   daughters   =  children
if not hasattr ( LHCb.Particle , '__daughters__' ) :
    LHCb.Particle . __daughters__ =  children

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
    return LoKi.Child.child ( p , *a )

child . __doc__  +=  "\n\n" + LoKi.Child.child    . __doc__ 

if not hasattr ( LHCb.Particle ,   'child'      ) :
    LHCb.Particle  .   child      = child 
if not hasattr ( LHCb.Particle , '__child__'    ) :
    LHCb.Particle  . __child__    = child 
if not hasattr ( LHCb.Particle ,   'daughter'   ) :
    LHCb.Particle  .   daughter   = child 
if not hasattr ( LHCb.Particle , '__daughter__' ) :
    LHCb.Particle  . __daughter__ = child 

# =============================================================================
## Get all descendants from the given LHCb::Particle 
def descendants ( o ) :
    """
    Get all descendants from the given LHCb::Particle

    >>> o = ...
    >>> for p in descendants ( o ) :
    ...    print p
    
    """
    return LoKi.Child.descendants ( o , *a )

descendants . __doc__ += "\n\n" + LoKi.Child.descendants . __doc__  

if not hasattr ( LHCb.Particle ,   'descendants'   ) :
    LHCb.Particle .   descendants    =  descendants
if not hasattr ( LHCb.Particle , '__descendants__' ) :
    LHCb.Particle . __descendants__  =  descendants


# =============================================================================
## Get the particles from the decay tree, which satisfy certain criteria
def particles ( p , c ) :
    """
    Get the particles from the decay tree, which satisfy certrain criteria

    >>> p = ...
    >>> pions = getParticles ( p , 'pi+' == ABSID )
    >>> for pion in pions :
    ...   print pion

    """
    return LoKi.Dicts.Extract.particles ( p , c )

particles . __doc__ += "\n\n" + LoKi.Dicts.Extract.particles . __doc__  

getParticles = particles
extract      = particles

if not hasattr ( LHCb.Particle ,   'particles'      ) :
    LHCb.Particle .   particles       = particles 
if not hasattr ( LHCb.Particle , '__particles__'    ) :
    LHCb.Particle . __particles__     =  particles 
if not hasattr ( LHCb.Particle ,   'getParticles'   ) :
    LHCb.Particle . __getParticles   = particles 
if not hasattr ( LHCb.Particle , '__getParticles__' ) :
    LHCb.Particle . __getParticles__ =  particles 
if not hasattr ( LHCb.Particle ,   'extract'        ) :
    LHCb.Particle .   extract        = particles 
if not hasattr ( LHCb.Particle , '__extract__'      ) :
    LHCb.Particle . __extract__      =  particles 



# =============================================================================
## Count number of 'interesting' elements in the decay tree or container,
def count_if ( s , *a ) :
    """
    Count number of 'interesting' elements in the decay tree or container,
    which satisfy certain criteria

    >>> o = ...
    >>> number_of_pions = count_if ( o , 'pi+' == ABSID )
    
    """
    return LoKi.Dicts.Algs.count_if ( s , *a )

count_if . __doc__ += "\n\n" + LoKi.Dicts.Algs.count_if . __doc__  

# =============================================================================
## Check the presence of 'interesting' elements in the decay tree or container,
def found ( s , *a ) :
    """
    check the presence of 'interesting' elements in the decay tree or container,
    which satisfy certain criteria

    >>> o = ...
    >>> good = found ( o , 'pi+' == ABSID )
    
    """
    return LoKi.Dicts.Algs.found ( s , *a )

found . __doc__ += "\n\n" + LoKi.Dicts.Algs.found . __doc__  

# =============================================================================
## Accumulate through the decay tree or container,
def accumulate  ( s , *a ) :
    """
    Accumulate through the decay tree or container,

    >>> o = ...
    >>> ptOfAllPions= accumulate ( o , PT , 'pi+' == ABSID )
    
    """
    return LoKi.Dicts.Algs.accumulate ( s , *a )

accumulate. __doc__ += "\n\n" + LoKi.Dicts.Algs.accumulate . __doc__  

# =============================================================================
## Find the minimal value through decay tree or  container  
def min_value ( s , *a ) :
    """
    Find the minimal value through decay tree or  container  

    >>> o = ...
    >>> minPT = min_value ( o , PT , 'pi+' == ABSID )
    
    """
    return LoKi.Dicts.Algs.min_value ( s , *a )

min_value. __doc__ += "\n\n" + LoKi.Dicts.Algs.min_value . __doc__  

# =============================================================================
## Find the maximal value through decay tree or  container  
def max_value ( s , *a ) :
    """
    Find the maximal value through decay tree or  container  

    >>> o = ...
    >>> maxPT = max_value ( o , PT , 'pi+' == ABSID )
    
    """
    return LoKi.Dicts.Algs.max_value ( s , *a )

max_value. __doc__ += "\n\n" + LoKi.Dicts.Algs.max_value . __doc__  

# =============================================================================
## Find the minimal element through decay tree or  container  
def min_element ( s , *a ) :
    """
    Find the minimal element through decay tree or  container  

    >>> o = ...
    >>> pion = min_element ( o , PT , 'pi+' == ABSID )
    
    """
    return LoKi.Dicts.Algs.min_element ( s , *a )

min_element. __doc__ += "\n\n" + LoKi.Dicts.Algs.min_element . __doc__  

# =============================================================================
## Find the maximal element through decay tree or  container  
def max_element ( s , *a ) :
    """
    Find the maximal element through decay tree or  container  

    >>> o = ...
    >>> pion = max_element ( o , PT , 'pi+' == ABSID )
    
    """
    return LoKi.Dicts.Algs.max_element ( s , *a )

max_element. __doc__ += "\n\n" + LoKi.Dicts.Algs.max_element . __doc__  

# =============================================================================

if not hasattr ( LHCb.Particle ,   'count_if'      ) :
    LHCb.Particle .   count_if      = count_if
if not hasattr ( LHCb.Particle , '__count_if__'    ) :
    LHCb.Particle . __count_if__    = count_if  
if not hasattr ( LHCb.Particle ,   'found'         ) :
    LHCb.Particle .   found         = found
if not hasattr ( LHCb.Particle , '__found__'       ) :
    LHCb.Particle . __found__       = found 
if not hasattr ( LHCb.Particle ,   'min_value'     ) :
    LHCb.Particle .   min_value     = min_value 
if not hasattr ( LHCb.Particle , '__min_value__'   ) :
    LHCb.Particle . __min_value__   = min_value 
if not hasattr ( LHCb.Particle ,   'max_value'     ) :
    LHCb.Particle .   max_value     = max_value 
if not hasattr ( LHCb.Particle , '__max_value__'   ) :
    LHCb.Particle . __max_value__   = max_value  
if not hasattr ( LHCb.Particle ,   'min_element'   ) :
    LHCb.Particle .   min_element   = min_element 
if not hasattr ( LHCb.Particle , '__min_element__' ) :
    LHCb.Particle . __min_element__ = min_element
if not hasattr ( LHCb.Particle ,   'max_element'   ) :
    LHCb.Particle .   max_element   = max_element 
if not hasattr ( LHCb.Particle , '__max_element__' ) :
    LHCb.Particle . __max_element__ = max_element
if not hasattr ( LHCb.Particle ,   'accumulate'    ) :
    LHCb.Particle .   accumulate    = accumulate 
if not hasattr ( LHCb.Particle , '__accumulate__'  ) :
    LHCb.Particle . __accumulate__  = accumulate 

# =============================================================================
## print the decay  
def printDecay ( s , *a ) :
    """
    Print the decay of LHCb::Particle

    >>> p = ...
    >>> printDecay ( p )
    
    """
    return LoKi.PrintPhys.printDecay ( s , *a )

printDecay. __doc__ += "\n\n" + LoKi.PrintPhys.printDecay . __doc__  

if not hasattr ( LHCb.Particle ,     'printDecay'   ) :
    LHCb.Particle .  printDecay   =   printDecay
    LHCb.Particle .       decay   =   printDecay
if not hasattr ( LHCb.Particle ,   '__printDecay__' ) :
    LHCb.Particle. __printDecay__ =   printDecay
    LHCb.Particle.      __decay__ =   printDecay


import LoKiCore.functions  as _LCF 
LHCb.Particle.__pname__ =  _LCF.__pname__
LHCb.Particle.  pname   =  _LCF.__pname__
LHCb.Particle.   name   =  _LCF.__pname__

_LCF.nChildren     . __doc__ += "\n" + LoKi.Child.nChildren         . __doc__ 
_LCF.child         . __doc__ += "\n" + LoKi.Child.child             . __doc__
_LCF.daughter      . __doc__ += "\n" + LoKi.Child.child             . __doc__ 
_LCF.children      . __doc__ += "\n" + LoKi.Child.children          . __doc__
_LCF.daughters     . __doc__ += "\n" + LoKi.Child.children          . __doc__ 

_LCF.extract       . __doc__ += "\n" + LoKi.Dicts.Extract.particles . __doc__ 
_LCF.particles     . __doc__ += "\n" + LoKi.Dicts.Extract.particles . __doc__ 
_LCF.getParticles  . __doc__ += "\n" + LoKi.Dicts.Extract.particles . __doc__ 

_LCF.count_if      . __doc__ += "\n" + LoKi.Dicts.Algs.count_if     . __doc__ 
_LCF.found         . __doc__ += "\n" + LoKi.Dicts.Algs.found        . __doc__ 
_LCF.accumulate    . __doc__ += "\n" + LoKi.Dicts.Algs.accumulate   . __doc__ 
_LCF.min_value     . __doc__ += "\n" + LoKi.Dicts.Algs.min_value    . __doc__ 
_LCF.max_value     . __doc__ += "\n" + LoKi.Dicts.Algs.max_value    . __doc__ 
_LCF.min_element   . __doc__ += "\n" + LoKi.Dicts.Algs.min_element  . __doc__ 
_LCF.max_element   . __doc__ += "\n" + LoKi.Dicts.Algs.max_element  . __doc__ 

_LCF.printDecay    . __doc__ += "\n" + LoKi.PrintPhys.printDecay    . __doc__ 


# decorate SmartRefVectors:
for _t in ( LHCb.Particle    ,
            LHCb.VertexBase  ) :
    _tt = cpp.SmartRefVector ( _t )
    # redefine the iterator 
    _tt.__iter__ = _LCF._iter_SRV_


# =============================================================================
if '__main__' == __name__ :
    print __doc__ , '\n' , __author__
    for i in dir() : print i 


# =============================================================================
# The END
# =============================================================================
