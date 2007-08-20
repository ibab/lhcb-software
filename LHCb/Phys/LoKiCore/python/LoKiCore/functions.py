#!/usr/bin/env python
# =============================================================================
## @file functions.py LoKiCore/function.py
#  The set of basic functions for from LoKiCore library
#  The file is a part of LoKi and Bender projects
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
""" The set of basic functions for from LoKiCore library """
_author_ = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
# =============================================================================
from LoKiCore.decorators import LoKi

# =============================================================================
def monitor ( o  , m ) :
    """
    Create the monitored pedicate/function:

    Make monitored function ( see LoKi::Monitoring::Stat )
    >>>  counter = ...
    >>>  fun = monitor ( fun , counter )
    
    Make monitored function ( see LoKi::Monitoring::Plot )
    >>>  histogram = ...
    >>>  fun = monitor ( fun , histogram )
    
    Make monitored predicate ( see LoKi::Monitoring::Counter)
    >>>  counter = ...
    >>>  cut = monitor ( cut , counter )
    
    """
    return o.__monitor__ ( m )

# =============================================================================
def switch ( c  , v1 , v2  ) :
    """
    Create the function which acts according to the rule:

            result  = c ? v1 : v2 

    Make monitored function ( see LoKi::Monitoring::Stat )
    >>>  fun1 = switch ( 0 < Q , 1 , -1 ) 
    >>>  fun2 = sqitch ( 0 < Q ,  PT , 1/PT )
    >>>  fun3 = sqitch ( 0 < Q ,  P  , -100 )
    
    """
    return c.__switch__ ( v1 , v2  )


# =============================================================================
def equal_to  ( f  , v ) :
    """
    Create the predicate which efficiently checks the equality of the
    function to some predefined value. Logically it is just 'operator==',
    b ut it should be more efficient 

    >>> cut = equal_to ( TrTYPE , LHCb.Track.Long )
    
    """
    if hasattr ( f , '__equal_to__' ) : return f.__equal_to__ ( v ) 
    if hasattr ( v , '__equal_to__' ) : return v.__equal_to__ ( f )
    # use the generic version:
    return f == v 

# =============================================================================
## Get the number of children for the object.
def nChildren ( o )  :
    """
    Get the number of children for the object.

    The method works for:
     - LHCb::Particle
     - LHCb::MCParticle
     - HepMC::GenParticle
     - LoKi::Loop

    >>> p = ...
    >>> n = nChildren ( p )
    
    """
    return p.__nChildren__ ()

# =============================================================================
## Get the child for 'particle'
def child ( p , *a )  :
    """
    Get the child for 'particle'
    
    The method works for:
     - LHCb::Particle
     - LHCb::MCParticle
     - HepMC::GenParticle
     - LoKi::Loop

    >>> p = ...
    >>> d1  = child ( p , 1 ) 
    >>> d12 = child ( p , 1 , 2 )

    ATTTENTION: incides starts from 1.
    Index 0 corresponds to the particle itself.
    
    """
    return p.__child__ (*a)

# =============================================================================
## Get all 'out'-particles 
def particles_out ( v ) :
    """
    Get all 'out'-particles for the given obejct

    >>> v = ...
    >>> for p in particles_in ( v ) :
    ...   print p

    """
    return v.__particles_out__ ()

# =============================================================================
## Get all parents 
def parents ( o ) :
    """
    Get the parents :

    >>> o = ...
    >>> ps = parents ( o ) 
    """
    return o.__parents__ ()

# =============================================================================
## Get all descendants
def descendants ( o ) :
    """
    Get the descendants :

    >>> o = ...
    >>> ps = descendants ( o ) 
    """
    return o.__descendats__ ()

# =============================================================================
## Get all ancestors
def ancestors ( o ) :
    """
    Get the ancestors :

    >>> o = ...
    >>> ps = ancestors ( o ) 
    """
    return o.__ancestors__ ()

# =============================================================================
## Get children:
def children ( o ) :
    """
    Get the children :

    >>> o = ...
    >>> for p in children ( o ) :
    ...   print p
    
    """
    return o.__children__ ()

# =============================================================================
# Get the daughter :
def daughter ( o , *a )  :
    """
    Get the child for 'particle'
    
    The method works for:
     - LHCb::Particle
     - LHCb::MCParticle
     - HepMC::GenParticle
     - LoKi::Loop

    >>> p = ...
    >>> d1  = daughter ( p , 1 ) 
    >>> d12 = daughter ( p , 1 , 2 )
    
    """
    return child ( o , *a )

# =============================================================================
# Get the daughters :
def daughters ( o )  :
    """
    Get the daughters :

    >>> o = ...
    >>> for p in daughters ( o ) :
    ...   print p
    
    """
    return children ( o ) 

# =============================================================================
## Extract the various subelements from decay trees and containers
def extract ( s , *a ) :
    """
    Extract the various subelements from decay trees and containers,
    which satisfy the seelction criteria 
    - see LoKi::Extract
    
    """
    return s.__extract__ ( *a )

# =============================================================================
## get the various particles  from decay trees and cntainers
def particles ( s , *a ) :
    """
    Extract the various subelements from decay trees and containers,
    which satisfy the selection criteria 
    - see LoKi::Extract
    
    """
    return s.__particles__ ( *a )

# =============================================================================
## get the various particles  from decay trees and cntainers
def getParticles ( s , *a ) :
    """
    Extract the various subelements from decay trees and containers,
    which satisfy the selection criteria 
    - see LoKi::Extract
    
    """
    return s.__getParticles__ ( *a )


# =============================================================================
## count number of elements in decay tree or container 
def count_if  ( s , *a ) :
    """
    Count number of elements in decay tree or container 
    """
    return s.__count_if__ ( *a )


# =============================================================================
## Check the presence of scertsin elements in decay tree or container 
def found  ( s , *a ) :
    """
    Check the presence of scertsin elements in decay tree or container 
    """
    return s.__found__ ( *a )

# =============================================================================
## Accumulate the value of th efunction through decay tree or container
def accumulate ( s , *a ) :
    """x
    Accumulate the value of th efunction through decay tree or container
    """
    return s.__accumulate__ ( *a )

# =============================================================================
## find a min-value of certain function ober decay tree or container 
def min_value ( s , *a ) :
    """
    Find a min-value fo certain function ober decay tree or container 
    """
    return s.__min_value__ ( *a )


# =============================================================================
## find a max-value of certain function over the decay tree or container 
def max_value ( s , *a ) :
    """
    Find a max-value fo certain function ober decay tree or container 
    """
    return s.__max_value__ ( *a )


# =============================================================================
## find the element from decay tree or container whcih minimize the function
def min_element ( s , *a ) :
    """
    Find the element from decay tree or container whcih minimize the function
    """
    return s.__min_element__ ( *a )

# =============================================================================
## find the element from decay tree or container whcih maximizes the function
def max_element ( s , *a ) :
    """
    Find the element from decay tree or container whcih minimize the function
    """
    return s.__min_element__ ( *a )

# =============================================================================
# Print the decay tree for the (XXX)Particle:
def printDecay ( s , *a ) :
    """
    Print the decay tree for the (XXX)Particle:

    >>> p = ...
    >>> printDecay ( p )

    """
    return s.__printDecay__ ( *a )


# =============================================================================
## Iterator (with derefernecing) througth SmartRefVector:
def _iter_SRV_ ( self ) :
    """
    Iterator (with derefernecing) througth SmartRefVector:

    >>> srf = ...
    >>> for o in srv :
    ...   print o

    """
    vct   = self
    size  = svct.size() 
    index = 0
    while index < size :
        o = vct[index]
        if hassattr ( o , '__deref__' ) : o = o.__deref__()
        yield o
        index += 1

        
# =============================================================================
# The END
# =============================================================================
