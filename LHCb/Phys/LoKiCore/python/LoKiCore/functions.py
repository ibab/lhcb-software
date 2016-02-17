#!/usr/bin/env python
# =============================================================================
# $Id$
# =============================================================================
# $URL$ 
# =============================================================================
## @file functions.py LoKiCore/function.py
#  The set of basic functions for from LoKiCore library
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
The set of basic functions for from LoKiCore library

      This file is a part of LoKi project - 
``C++ ToolKit  for Smart and Friendly Physics Analysis''

The package has been designed with the kind help from
Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
contributions and advices from G.Raven, J.van Tilburg, 
A.Golutvin, P.Koppenburg have been used in the design.

"""
# =============================================================================
__author__  = "Vanya BELYAEV ibelyaev@physics.syr.edu"
__date__    = "????-??-??"
__version__ = "Version $Revision$ "
# =============================================================================

from LoKiCore.basic import cpp, std, LoKi, LHCb, Gaudi

# =============================================================================
def monitor ( o , *m ) :
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
def scale ( s , sf ) :
    """
    Create the 'scaled' predicate which acts according to the rule:

            result  = s ( argument ) ? sf() : False 
            
    Keep the maximal rate os access less than 50 Hz:
    >>>  cut1 = scale ( L0_DECISION , FRATE  ( 50 * Gaudi.Units.Hz ) ) 
    
    ## postscale at 10% o fpositive decisions (random) 
    >>>  cut2 = scale ( L0_DECISION , FSCALE ( 0.1 ) )
    
    ## accept only every 3rd positive decion :
    >>>  cut3 = scale ( L0_DECISION , FSKIP  ( 3   ) )
     
    """
    return s.__scale__ ( sf )

# =============================================================================
## Create the predicate which efficiently checks the equality of the
#   function to some predefined value. Logically it is just 'operator==',
#    but it should be more efficient 
#
#  @code
#
#    >>> cut = equal_to ( TrTYPE , LHCb.Track.Long )

#  @endcode
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
def equal_to  ( f  , v , *args ) :
    """
    Create the predicate which efficiently checks the equality of the
    function to some predefined value. Logically it is just 'operator==',
    but it should be more efficient 

    >>> cut = equal_to ( TrTYPE , LHCb.Track.Long )
    >>> cut = equal_to ( TrTYPE , LHCb.Track.Long , LHCb.Track.Upstream )

    >>> lst = [ .... ] 
    >>> cut = equal_to ( TrTYPE , lst )
    
    
    """
    
    if args :
        v = doubles ( v , *args )
        return equal_to ( f , v )
    
    if   hasattr ( f , '__equal_to__' ) :
        if list == type ( v )  : v = doubles ( v )
        return f.__equal_to__ ( v ) 
    elif hasattr ( v , '__equal_to__' ) :
        if list == type ( f )  : f = doubles ( f ) 
        return v.__equal_to__ ( f )

    # use the generic version:
    return f == v

# =============================================================================
##    Create 'in-range' predicate, that checks the valeu of
#     certain function is 'in-range' :
#
#              retult = low <= fun ( argument ) <= high 
#  @code
#
#    >>> fun = PT
#    >>> cut = in_range ( 0.5 * GeV , PT , 3 * GeV ) ## 0.5 * GeV <= PT <= 3 * GeV
#
#  @endcode
#
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date   2009-11-21
def in_range ( low , fun , high ) :
    """
    Create 'in-range' predicate, that checks the valeu of
    certain function is 'in-range' :

              retult = low <= fun ( argument ) <= high 

    >>> fun = PT
    >>> cut = in_range ( 0.5 * GeV , PT , 3 * GeV ) ## 0.5 * GeV <= PT <= 3 * GeV  
    
    """
    return fun.__in_range__ ( low , high )

## ditto...
inRange  = in_range
in_Range = in_range

# ==============================================================================
## Create 'in-list' preducate, that checks the value of certain function
#  is within the list
#
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date   2009-11-21
def in_list ( fun , lst , *args ) :
    """
    Create 'in-list' preducate, that checks the value of certain function
    is within the list

    >>> fun = ... # get the function
    >>> cut = in_list ( fun ,   1 , 2, 3, 4 , 5   ) ## construct the predicate 
    >>> cut = in_list ( fun , [ 1 , 2, 3, 4 , 5 ] ) ## construct the predicate 
    >>> cut = in_list ( fun , ( 1 , 2, 3, 4 , 5 ) ) ## construct the predicate 
    
    """
    if issubclass ( type(lst) , ( tuple , list ) ) or args :  
        vct = vct_from_list ( lst , *args ) 
        return fun.__in_list__ ( vct )
    return fun.__in_list__ ( lst , *args )

## fitto 
inList  = in_list
in_List = in_list 
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

    Also one can use ``child-selector'' here
    
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
## Check the presence of certain elements in decay tree or container 
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
## get a sum over stream 
def sum ( s , *a ) :
    """
    Get a sum over functionin the stream
    
    """
    return s.__sum__ ( *a )

# =============================================================================
## get a sum over the stream 
def product ( s , *a ) :
    """
    Get a sum over functionin the stream
    
    """
    return s.__product__ ( *a )

# =============================================================================
## find the element from decay tree or container whcih minimize the function
def min_element ( s , *a ) :
    """
    1. Find the element from decay tree or container which minimize the function
    
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
## define __repr__ function for SmartRef
#  essentially it delegates the prints to the object itself
#  @code
#  obj = ...
#  print repr(obj)
#  @endcode
def _repr_SR_ ( self ) :
    """Define __repr__ function for SmartRef
    essentially it delegates the prints to the object itself
    >>> obj = ...
    >>> print repr(obj)
    """
    _t = self.target()
    if _t : return _t.__repr__()
    return 'SmartRef(%s)(nullptr)'%s.__class__.__name__[9:-1].replace('::','.')

# =============================================================================
## define __str__ function for SmartRef
#  essentially it delegates the prints to the object itself
#  @code
#  obj = ...
#  print str(obj)
#  @endcode
def _str_SR_ ( self ) :
    """Define __repr__ function for SmartRef
    essentially it delegates the prints to the object itself
    >>> obj = ...
    >>> print str(obj)
    """
    _t = self.target()
    if _t : return _t.__str__()
    return 'SmartRef(%s)(nullptr)'%s.__class__.__name__[9:-1].replace('::','.')

# =============================================================================
## Simple function, which prints the name of the particle
def   pname ( p ) :
    """ Simple function, which prints the name of the particle
    >>> p = ...
    >>> pname ( p )     
    """    
    return p.__pname__ ()

# =============================================================================
## simple function, which prints the name of the particle 
def __pname__ ( self ) :
    """Simple function, which prints the name of the particle
    
    >>> p = ...
    >>> p.name()      
    """
    return LoKi.Particles.nameFromPID( self.particleID() ) 

# =============================================================================
## Construct 'select/filter' vector-function from the scalar predicate:
def select ( s ) :
    """Construct 'select/filter' vector-function from the scalar predicate:

    >>> cut = ...
    >>> selector = select ( cut )
        
    The concept belongs to Gerhard 'The Great' Raven            
    """
    return s.__select__() 

# =============================================================================
## Construct 'process' vector-function from the scalar functor 
def process ( s ) :
    """Construct 'process' vector-function from the scalar functor
    
    >>> functor = ...
    >>> processor = process ( functor )
    
    The concept belongs to Gerhard 'The Great' Raven            
    """
    return s.__process__() 

def tee ( s ) :
    """Construct 'tee' vector-function form the vector functor
    >>> functor  = ...
    >>> functorT = tee ( functor )        
    The concept belongs to Gerhard 'The Great' Raven            
    """
    return s.__tee__ () 
    
# =============================================================================
## Construct 'yield/map' vector-function from the scalar functor 
def yields ( s ) :
    """Construct 'yield/map' vector-function from the scalar functor
    
    >>> functor = ...
    >>> mapper = yields ( functor )
    
    The concept belongs to Gerhard 'The Great' Raven            
    """
    return s.__yields__() 


# =============================================================================
## construct "smart Extra Info" functor
def info ( key , fun , update = False ) :
    """Construct ''Smart-extra-info''/``cache'' functor,
    which returns the valeu of extraInfo(index)/cache(key), if the information
    present, otherwise it evaluates the functor and (optionally)
    updates the extraInfo/cache field
    
    >>> functor = ...
    >>> smart = info ( 15 , functor )
    
    >>> particle = ...
    >>> value = smart ( particle )

    The concept belongs to Jose Angel Hernado Morata and Hugo Ruiz Peres
    
    """
    return fun.__info__ ( index , update ) 

# =============================================================================
## construct logging-functor
def logging ( fun , *args ) :
    """Construct logging functor,
    that writes into cache the resutl sof own evaluation 
    
    >>> functor = ...
    >>> logged  = logging ( functor )
    
    The concept belongs to Gerhard ``The Great'' Raven     
    """
    return fun.__logging__ ( *args ) 


# =============================================================================
def has ( cut ) :
    """Create the vector function that checks certain scalar predicate

    >>> cut = ...
    >>> checker = has ( cut )
    >>> ok = checker ( data )
    
    The concept belongs to Gerhard 'The Great' Raven    
    """
    return cut.__has__ ()

# =============================================================================
def count ( cut ) :
    """Create the vector function that counts certain scalar predicate

    >>> cut = ...
    >>> counter = count ( cut )
    
    The concept belongs to Gerhard 'The Great' Raven    
    """
    return cut.__count__ ()

# =============================================================================
## Construct 'fetch/reduce' vector-function from the scalar functor
#
#  Usefult in steams:
# 
#  @code
#
#    " ... >> fetch ( PT , 1 , -1 )  >> ... "  #
#
#
#    " ... >> fetch ( PT > 1 * GeV , 0 )  "  # terminate stream 
#
#  @endcode     
#
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date 2011-03-28
def fetch ( s , *args ) :
    """Construct 'fetch/reduce' vector-function from the scalar functor 

    >>> functor = ...
    >>> f = fetch ( functor , 1 , -1 )         
    """
    return s.__fetch__( *args ) 


# =============================================================================
##  create the 'union' for two streamers
#
#  @code
#
#    >>> fun1 = ...  # the first streamer
#    >>> fun2 = ...  # the first streamer
#    
#    >>> result = union ( fun1 , fun2 )
#
#  @endcode     
#
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2010-06-06
def union ( fun1 , fun2 ) :
    """ Create ``union'' for two streamers

    >>> fun1 = ...  # the first streamer
    >>> fun2 = ...  # the first streamer
    
    >>> result = union ( fun1 , fun2 )     
    """
    if hasattr ( fun1 , '__union__' ) : return fun1.__union__ ( fun2 )
    return fun2.__union__ ( fun1 )

# =============================================================================
##  create the 'intersection' for two streamers
#
#  @code
#
#    >>> fun1 = ...  # the first streamer
#    >>> fun2 = ...  # the first streamer
#    
#    >>> result = intersection( fun1 , fun2 )
#
#  @endcode     
#
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2010-06-06
def intersection ( fun1 , fun2 ) :
    """Create ``intersection'' for two streamers

    >>> fun1 = ...  # the first streamer
    >>> fun2 = ...  # the first streamer
    
    >>> result = intersection ( fun1 , fun2 )     
    """
    if hasattr ( fun1 , '__intersection__' ) :
        return fun1.__intersection__ ( fun2 )
    return fun2.__intersection__ ( fun1 )


# =============================================================================
##  create the 'difference' for two streamers
#
#  @code
#
#    >>> fun1 = ...  # the first streamer
#    >>> fun2 = ...  # the first streamer
#    
#    >>> result = difference ( fun1 , fun2 )
#
#  @endcode     
#
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2010-06-06
def difference ( fun1 , fun2 ) :
    """Create ``difference'' for two streamers

    >>> fun1 = ...  # the first streamer
    >>> fun2 = ...  # the first streamer
    
    >>> result = difference ( fun1 , fun2 )     
    """
    return fun1.__difference__ ( fun2 )

# =============================================================================
##  create the 'symmetric-difference' for two streamers
#
#  @code
#
#    >>> fun1 = ...  # the first streamer
#    >>> fun2 = ...  # the first streamer
#    
#    >>> result = sym_difference ( fun1 , fun2 )
#
#  @endcode     
#
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2010-06-06
def sym_difference ( fun1 , fun2 ) :
    """Create ``symmetric difference'' for two streamers

    >>> fun1 = ...  # the first streamer
    >>> fun2 = ...  # the first streamer
    
    >>> result = sym_difference ( fun1 , fun2 )     
    """
    if hasattr ( fun1 , '__sym_difference__' ) :
        return fun1.__sym_difference__ ( fun2 )
    return fun2.__sym_difference__ ( fun1 )


# =============================================================================
##  create the 'includes' for two streamers
#
#  @code
#
#    >>> fun1 = ...  # the first streamer
#    >>> fun2 = ...  # the first streamer
#    
#    >>> result = includes ( fun1 , fun2 )
#
#  @endcode     
#
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2010-06-06
def includes ( fun1 , fun2 ) :
    """Create ``includes'' for two streamers

    >>> fun1 = ...  # the first streamer
    >>> fun2 = ...  # the first streamer
    
    >>> result = includes ( fun1 , fun2 )     
    """
    return fun1.__includes__ ( fun2 )


# =============================================================================
## create the timer predicate or timer symbol :
#
#  @code
#
#    >>> fun   = ...
#    >>> fun_t = timer ( 'ququ' ) % fun
#
#    >>> fun   = ... 
#    >>> fun_t = timer ( fun , 'wuwu' ) 
#
#  @endcode     
#
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2010-06-06 
def timer ( obj , *args ) :
    """ Define the timers
    >>> fun   = ...
    >>> fun_t = timer ( fun ) 
    """
    ##
    if hasattr ( obj , '__timer__' )         : return obj.__timer__ ( *args )
    ## 
    if isinstance ( obj , str ) :
        if   not args          : return LoKi.Timer ( obj )
        elif 1 == len ( args ) :
            arg0 = args[0]
            if hasattr ( arg0 , '__timer__' ) : return timer ( arg0 , obj ) 
    ##
    raise TypeErorr, "Invalid arguments "

# =============================================================================
## create the simple dump-1 meta-functor
#
#  @code
#
#    >>> fun   = ...
#    >>> fun_t = dump1 ( fun )
#
#  @endcode     
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2012-01-28 
def dump1 ( obj , *args ) :
    """Define the dump-1 functor

    >>> fun   = ...
    >>> fun_t = dump1 ( fun )    
    """
    return obj.__dump1__ ( *args ) 

# =============================================================================
## create 'mean-over-stream' vector-functor:
#
#  @code
#
#    >>> fun = PT
#    >>> meanPT = mean ( PT )
#
#  @endcode 
#
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date 2011-03-04
def mean ( obj , *args ) :
    """Create ``mean-over-stream'' vector-functor
    
    >> mean ( PT ) >>     
    """
    return obj.__mean__ ( *args )
# =============================================================================
## create 'mean-error-over-stream' vector-functor:
#
#  @code
#
#    >>> fun = PT
#    >>> errPT = meanErr ( PT )
#
#  @endcode 
#
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date 2011-03-04
def meanErr ( obj , *args ) :
    """Create ``mean-error-over-stream'' vector-functor

    >> meanErr ( PT ) >>     
    """
    return obj.__meanErr__ ( *args )
# =============================================================================
## create 'rms-over-stream' vector-functor:
#
#  @code
#
#    >>> fun = PT
#    >>> rmsPT = rms ( PT )
#
#  @endcode 
#
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date 2011-03-04
def rms ( obj , *args ) :
    """Create ``mean-error-over-stream'' vector-functor

    >> rms ( PT ) >>     
    """
    return obj.__rms__ ( *args )
# =============================================================================
## create 'efficiency-over-stream' vector-functor:
#
#  @code
#
#    >>> fun = PT > 1 * GeV 
#    >>> effPT = eff ( fun )
#
#  @endcode 
#
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date 2011-03-04
def eff ( obj , *args ) :
    """Create ``mean-error-over-stream'' vector-functor

    >> eff ( PT  > 1 * GeV  ) >>     
    """
    return obj.__eff__ ( *args )
# =============================================================================
## create 'efficiency-error-over-stream' vector-functor:
#
#  @code
#
#    >>> fun = PT > 1 * GeV 
#    >>> effPT = eff ( fun )
#
#  @endcode 
#
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date 2011-03-04
def effErr ( obj , *args ) :
    """Create ``mean-error-over-stream'' vector-functor

    >> effErr ( PT  > 1 * GeV  ) >>     
    """
    return obj.__effErr__ ( *args )

# =============================================================================
##  create the 'conditional source/cause' 
#
#  @code
#
#    >>> cond   = ...  # the condition 
#    >>> source = ...  # the source
#    
#    >>> result = cause ( cond , source ) 
#
#  @endcode     
#
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date 2010-11-18
def cause ( fun1 , fun2 ) :
    """Create ``conditional source/cause'' 

    >>> cond   = ...  # the condition 
    >>> source = ...  # the source
    
    >>> result = cause ( cond , source )     
    """    
    return fun1._cause_ ( fun2 ) if hasattr ( fun1 , '_cause_' ) else fun2._cause_ ( fun1 )


# =============================================================================
## sort the stream  (and get the first N-elements)
#
#  @code
#
#    >>> stream >> sort ( PT )
#
#    >>> stream >> sort ( PT , 5 ) ## get no more than 5 elements 
#
#  @endcode     
#
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date 2011-10-30
def sort( fun , *args ) :
    """Sort the stream  (and get the first N-elements)
    
    >>> stream >> sort ( PT )
    
    >>> stream >> sort ( PT , 5 ) ## get no more than 5 elements     
    """    
    return fun.__sort__ ( *args  ) 

# =============================================================================
def mark ( tree ) :
    """Create the 'marked' tree:

    >>> tree = ...
    >>> marked = mark ( tree )    
    """
    return tree.__mark__()

# =============================================================================
# make various vectors 
# =============================================================================
def _append_   ( v , o ) :
    """
    Generic ``append''-function
    """

    ## list 
    if issubclass ( type(v) , list ) :
        if issubclass ( type(o) , list ) :
            v += o
        else : v.append ( o )
        return v

    ## std.vector
    if hasattr ( v , 'push_back' ) :
        if hasattr ( o , 'push_back' ) :
            for i in o : v.push_back ( i )
        else : v.push_back ( o )
        return v

    ## increment 
    if hasattr ( v , '__iadd__' ) :
        v += o
        return v

    ## everything else
    v = v + o 
    return v 
        
## build vector of certain type
def _make_vct_ ( results         ,
                 arg1            ,
                 func = _append_ ,
                 *arg            ) :
    """
    Make vector/container of certain type from the arguments 
    """
    ## 
    if issubclass ( type ( arg1 ) , ( list , tuple, set ) ) :
        for a in arg1 : results = func ( results , a    ) 
    else :              results = func ( results , arg1 ) 
    ##
    if arg : return _make_vct_ ( results , arg[0] , func , *arg[1:] )
    ##
    return results

# ==============================================================================
## check the arguments for presense of some elements
def _check_arg_ ( check , 
                  arg1  ,
                  *arg  ) :
    """
    Check the arguments
    """
    if issubclass ( type ( arg1 ) , ( list , tuple, set ) ) :
        for a in arg1 :
            if check ( a    ) : return True  
    elif       check ( arg1 ) : return True  
    ##
    if arg : return _check_arg_ ( check , arg[0] , *arg[1:] )
    ##
    return False


# =============================================================================
## construct std::vector<std::string> from the arguments 
def strings ( arg1 , *arg ) :
    """Construct the std::vector<std::string> from the arguments
    
    >>> v1 = strings ( 'sadfsdf' )
    >>> v2 = strings ( 'sadfsdf' , 'sadf' , 'afadf' )
    >>> v3 = strings ( [ 'sadfsdf' , 'sadf' , 'afadf' ] )    
    """
    VT  = std.vector('std::string')
    vct = VT()
    return _make_vct_ ( vct  ,
                        arg1 ,
                        lambda v,s :  _append_ ( v , str ( s ) ) ,
                        *arg ) 

# =============================================================================
## construct std::vector<double> from the arguments 
def doubles ( arg1 , *arg ) :
    """Construct the std::vector<double> from the arguments
    
    >>> v1 = doubles ( 1.01 )
    >>> v2 = doubles ( 1.01 , 1.02 , 1.03  )
    >>> v3 = doubles ( [ 1.01 , 1.02 , 1.03 ] )    
    """
    VT  = std.vector('double')
    vct = VT()
    return _make_vct_ ( vct  ,
                        arg1 ,
                        lambda v,s :  _append_ ( v , s ) ,
                        *arg ) 

# =============================================================================
## construct std::vector<int> from the arguments 
def ints ( arg1 , *arg ) :
    """Construct the std::vector<int> from the arguments
    
    >>> v1 = ints ( 1 )
    >>> v2 = ints ( 1 , 1 , 10  )
    >>> v3 = ints ( [ 1 , 2 , 3 ] )    
    """
    VT  = std.vector('int')
    vct = VT()
    return _make_vct_ ( vct  ,
                        arg1 ,
                        lambda v,s :  _append_ ( v , s ) ,
                        *arg ) 

# =============================================================================
## construct std::vector<unsigned int> from the arguments 
def uints ( arg1 , *arg ) :
    """
    Construct the std::vector<unsigned int> from the arguments
    
    >>> v1 = uints ( 1 )
    >>> v2 = uints ( 1 , 1 , 10  )
    >>> v3 = uints ( [ 1 , 2 , 3 ] )
    
    """
    VT  = std.vector('unsigned int')
    vct = VT()
    return _make_vct_ ( vct  ,
                        arg1 ,
                        lambda v,s :  _append_ ( v , s ) ,
                        *arg ) 

# =============================================================================
## check the presence of at least one string argument:
def _has_string ( arg1 , *arg ) :
    """
    Check the presence of at least one string argument 
    """
    return _check_arg_  ( lambda s : issubclass ( type(s) , str ) ,
                          arg1 ,
                          *arg ) 

## ============================================================================
## check the presence of at least one float argument:
def _has_float ( arg1 , *arg ) :
    ##
    """
    Check the presence of at least one float argument 
    """
    return _check_arg_  ( lambda s : issubclass ( type(s) , float ) ,
                          arg1 ,
                          *arg ) 

## ============================================================================
def _has_nega ( arg1 , *arg ) :
    ##
    """
    Check the presence of at least one negative argument 
    """
    return _check_arg_  ( lambda s : s<0 ,
                          arg1 ,
                          *arg ) 
    
# =============================================================================
## convert the "list" into C++ vector of primitives 
def vct_from_list  ( lst , *args ) :
    """
    Convert the ``list'' into C++ vector of primitives
    """    
    ## at least one string?
    if _has_string  ( lst , *args ) : return strings ( lst , *args )
    
    ## at least one double?
    if _has_float   ( lst , *args ) : return doubles ( lst , *args )
    ##
    _hn = _has_nega ( lst , *args )
    ##
    ok = True 
    try : 
        if _hn : return  ints   ( lst , *args )
        else   : return uints   ( lst , *args )
    except TypeError :
        ok = False
        print ' Error here: ', lst, args  
        pass
    ##
    return doubles ( lst , *args )


# =============================================================================
## @see LoKi::FirstN
def first_n ( N ) :
    """
    helper object to select the first N-elements from the stream 
    """
    return LoKi.FirstN( N )

# =============================================================================
## @see LoKi::Dump
Dump    = LoKi.Dump
dump    = LoKi.Dump ()
## @see LoKi::Reverse 
reverse = LoKi.Reverse()


## add the sequence to the vector 
def _iadd_ ( v , o ) :
    """
    add the sequence to the vector
    """
    if hasattr ( o , '__len__' ) and not issubclass ( type(o) , str ) : 
        for i in o : v.push_back ( i )
    else :           v.push_back ( o )
    return v

## Summation of the vector and the sequence
def _add_  ( v , o ) :
    """
    Summation of the vector and the sequence
    """
    #
    v2 = type(v)( v )
    if hasattr ( o , '__len__' ) and not issubclass ( type(o) , str ) : 
        for i in o : v2.push_back ( i )
    else :           v2.push_back ( o )
    #
    return v2    
    
## minor decorations 
for t in ( 'int'    ,
           'double' ,
           'long'   ,
           'string' ) :
    
    k = std.vector( t )
    k.__str__  = lambda s : str( [ i for i in s ] )
    k.__repr__ = k.__str__
    k.__iadd__ = _iadd_
    k.__add__  = _add_
    
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
