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
def monitor ( o  , *m ) :
    """
    Create the monitored pedicate/function:

    Make monitored function ( see LoKi::Monitoring::Printer )
    >>>  counter = ...
    >>>  fun = monitor ( fun , '\n' )
    
    Make monitored function ( see LoKi::Monitoring::Stat )
    >>>  counter = ...
    >>>  fun = monitor ( fun , counter )
    
    Make monitored function ( see LoKi::Monitoring::Plot )
    >>>  histogram = ...
    >>>  fun = monitor ( fun , histogram )
    
    Make monitored predicate ( see LoKi::Monitoring::Counter)
    >>>  counter = ...
    >>>  cut = monitor ( cut , counter )
    
    The concept belongs to Jose Angel Hernado Morata, Hugo Ruiz Peres,
                Gerhard 'The Great' Raven and Patrick Koppenburg
                
    """
    return o.__monitor__ ( *m )
# =============================================================================
def debug  ( c , *arg ) :
    """
    Create 'debug'- predicate/function:

    >>> obj = ...
    >>> deb = debug ( obj )

    """
    return monitor ( c , *arg ) 
# =============================================================================
def switch ( c  , v1 , v2  ) :
    """
    Create the function which acts according to the rule:

            result  = c ? v1 : v2 

    >>>  fun1 = switch ( 0 < Q , 1 , -1 ) 
    >>>  fun2 = switch ( 0 < Q ,  PT , 1/PT )
    >>>  fun3 = switch ( 0 < Q ,  P  , -100 )
    >>>  cut4 = switch ( 0 < Q ,  P>0.5*GeV  , P>1.5*GeV)
    
    """
    return c.__switch__ ( v1 , v2  )


# =============================================================================
def equal_to  ( f  , v ) :
    """
    Create the predicate which efficiently checks the equality of the
    function to some predefined value. Logically it is just 'operator==',
    but it should be more efficient 

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
def children ( o , *a ) :
    """
    Get the children :

    >>> o = ...
    >>> for p in children ( o ) :
    ...   print p
    
    """
    return o.__children__ ( *a )

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
## Accumulate the value of the function through decay tree or container
def accumulate ( s , *a ) :
    """
    Accumulate the value of the function through decay tree or container
    """
    return s.__accumulate__ ( *a )

# =============================================================================
## find a min-value of certain function ober decay tree or container 
def min_value ( s , *a ) :
    """
    1. Find a min-value fo certain function ober decay tree or container

    2. Create 'min_value' vector function from the scalar function
    
    """
    return s.__min_value__ ( *a )

# =============================================================================
## Create 'min_abs_value' vector function from the scalar function
def min_abs_value ( s , *a ) :
    """
    Create 'min_abs_value' vector function from the scalar function:
    
    >>> fun = ...
    >>> vfun = min_abs_value ( fun ) 
    
    """
    return s.__min_abs_value__ ( *a )


# =============================================================================
## find a max-value of certain function over the decay tree or container 
def max_value ( s , *a ) :
    """
    1. Find a max-value fo certain function ober decay tree or container
    
    2. Create 'min_value' vector function from the scalar function
    
    """
    return s.__max_value__ ( *a )

# =============================================================================
## Create 'max_abs_value' vector function from the scalar function
def max_abs_value ( s , *a ) :
    """
    Create 'max_abs_value' vector function from the scalar function:
    
    >>> fun = ...
    >>> vfun = max_abs_value ( fun ) 
    
    """
    return s.__max_abs_value__ ( *a )

# =============================================================================
## find the element from decay tree or container whcih minimize the function
def min_element ( s , *a ) :
    """
    1. Find the element from decay tree or container whcih minimize the function
    
    2. Create 'min_element' vector function from the scalar function
    
    """
    return s.__min_element__ ( *a )

# =============================================================================
## Create 'min_abs_element' vector function from the scalar function
def min_abs_element ( s , *a ) :
    """
    Create 'min_abs_element' vector function from the scalar function

    >>> fun = ...
    >>> vfun = min_abs_element ( fun ) 
    
    """
    return s.__min_abs_element__ ( *a )

# =============================================================================
## find the element from decay tree or container whcih maximizes the function
def max_element ( s , *a ) :
    """
    1. Find the element from decay tree or container whcih minimize the function
    
    2. Create 'min_element' vector function from the scalar function    
    
    """
    return s.__max_element__ ( *a )

# =============================================================================
## Create 'max_abs_element' vector function from the scalar function
def max_abs_element ( s , *a ) :
    """
    Create 'max_abs_element' vector function from the scalar function

    >>> fun = ...
    >>> vfun = max_abs_element ( fun ) 
    
    """
    return s.__max_abs_element__ ( *a )

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
## Iterator (with dereferencing) througth SmartRefVector:
def _iter_SRV_ ( self ) :
    """
    Iterator (with derefernecing) througth (SmartRef)Vector:

    >>> srv = ...
    >>> for o in srv :
    ...   print o

    """
    vct   = self
    size  = vct.size() 
    index = 0
    while index < size :
        o = vct[index]
        if hasattr ( o , '__deref__' ) : o = o.__deref__()
        yield o
        index += 1

# =============================================================================
## Simple function, which prints the name of the particle
def   pname ( p ) :
    """
    Simple function, which prints the name of the particle

    >>> p = ...
    >>> pname ( p ) 
    
    """    
    return p.__pname__ ()

# =============================================================================
## simple function, which prints the name of the particle 
def __pname__ ( self ) :
    """
    Simple function, which prints the name of the particle
    
    >>> p = ...
    >>> p.name()  
    
    """
    return LoKi.Particles.nameFromPID( self.particleID() ) 

# =============================================================================
## Construct 'select/filter' vector-function from the scalar predicate:
def select ( s ) :
    """
    Construct 'select/filter' vector-function from the scalar predicate:

    >>> cut = ...
    >>> selector = select ( cut )
        
    The concept belongs to Gerhard 'The Great' Raven
            
    """
    return s.__select__() 

# =============================================================================
## Construct 'process' vector-function from the scalar functor 
def process ( s ) :
    """
    Construct 'process' vector-function from the scalar functor 

    >>> functor = ...
    >>> processor = process ( functor )
        
    The concept belongs to Gerhard 'The Great' Raven
            
    """
    return s.__process__() 

def tee ( s ) :
    """
    Construct 'tee' vector-function form the vector functor

    >>> functor  = ...
    >>> functorT = tee ( functor )
        
    The concept belongs to Gerhard 'The Great' Raven
            
    """
    return s.__tee__ () 
    
# =============================================================================
## Construct 'yield/map' vector-function from the scalar functor 
def yields ( s ) :
    """
    Construct 'yield/map' vector-function from the scalar functor 

    >>> functor = ...
    >>> mapper = yields ( functor )
    
    The concept belongs to Gerhard 'The Great' Raven
            
    """
    return s.__yields__() 


# =============================================================================
## construct "smart Extra Info" functor
def info ( index , fun , update = False ) :
    """
    Construct ''Smart-extra-info'' functor,
    which returns the valeu of extraInfo(index), if the information
    present, otherwise it evaluates the functor and (optionally)
    updates the extraInfo field

    >>> functor = ...
    >>> smart = infor ( 15 , functor )
    
    >>> particle = ...
    >>> value = smart ( particle )

    The concept belongs to Jose Angel Hernado Morata and Hugo Ruiz Peres
    
    """
    return fun.__info__ ( index , update ) 


# =============================================================================
def has ( cut ) :
    """
    Create the vector function that checks certain scalar predicate

    >>> cut = ...
    >>> checker = has ( cut )
    >>> ok = checker ( data )
    
    The concept belongs to Gerhard 'The Great' Raven
    
    """
    return cut.__has__ ()

# =============================================================================
def count ( cut ) :
    """
    Create the vector function that counts certain scalar predicate

    >>> cut = ...
    >>> counter = count ( cut )
    
    The concept belongs to Gerhard 'The Great' Raven
    
    """
    return cut.__count__ ()

# =============================================================================
## construct std::vector<std::string> from the arguments 
def strings ( arg1 , *args ) :
    """
    Construct the std::vector<std::string> from the arguments
    
    >>> v1 = strings ( 'sadfsdf' )
    >>> v2 = strings ( 'sadfsdf' , 'sadf' , 'afadf' )
    >>> v3 = strings ( [ 'sadfsdf' , 'sadf' , 'afadf' ] )
    
    """
    from LoKiCore.decorators import std
    _vt = std.vector('std::string')
    vct = _vt () 
    if 0 != len( args ) :
        vct.push_back ( arg1 )
        for a in args : vct.push_back ( a )
        return vct
    if list == type( arg1 )  :
        for a in arg1 : vct.push_back ( a )
        return vct
    vct.push_back ( arg1 )
    return vct 

# =============================================================================
## construct std::vector<double> from the arguments 
def doubles ( arg1 , *args ) :
    """
    Construct the std::vector<double> from the arguments
    
    >>> v1 = doubles ( 1.01 )
    >>> v2 = doubles ( 1.01 , 1.02 , 1.03  )
    >>> v3 = doubles ( [ 1.01 , 1.02 , 1.03 ] )
    
    """
    from LoKiCore.decorators import std
    _vt = std.vector('double')
    vct = _vt () 
    if 0 != len( args ) :
        vct.push_back ( arg1 )
        for a in args : vct.push_back ( a )
        return vct
    if list == type( arg1 )  :
        for a in arg1 : vct.push_back ( a )
        return vct
    vct.push_back ( arg1 )
    return vct 

# =============================================================================
## construct std::vector<int> from the arguments 
def ints ( arg1 , *args ) :
    """
    Construct the std::vector<int> from the arguments
    
    >>> v1 = ints ( 1 )
    >>> v2 = ints ( 1 , 1 , 10  )
    >>> v3 = ints ( [ 1 , 2 , 3 ] )
    
    """
    from LoKiCore.decorators import std
    _vt = std.vector('int')
    vct = _vt () 
    if 0 != len( args ) :
        vct.push_back ( arg1 )
        for a in args : vct.push_back ( a )
        return vct
    if list == type( arg1 )  :
        for a in arg1 : vct.push_back ( a )
        return vct
    vct.push_back ( arg1 )
    return vct 

# =============================================================================
# The END
# =============================================================================
