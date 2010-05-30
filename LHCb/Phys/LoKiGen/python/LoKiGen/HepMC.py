#!/usr/bin/env python
# =============================================================================
# $Id: HepMC.py,v 1.9 2010-05-30 17:09:55 ibelyaev Exp $ 
# =============================================================================
## @file
#  collection of utilities for useful 'decoration' of HepMC-objects
#
#  This file is a part of LoKi project - 
#   'C++ ToolKit  for Smart and Friendly Physics Analysis'
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
Collection of utilities for useful 'decoration' of HepMC-objects

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
__version__ = "CVS Tag: $Name: not supported by cvs2svn $, version $Revision: 1.9 $ "
# =============================================================================
import LoKiCore.decorators as _LoKiCore
from   LoKiGen.functions   import HepMC,LoKi, Gaudi, std, cpp 
from   LoKiCore.decorators import LHCb

# =============================================================================
## Get number of child particles :
def nChildren ( p ) :
    """
    Get number of child particles :

    >>> n = nChildren ( p )
    """
    return LoKi.GenChild.nChildren ( p )

nChildren . __doc__  +=  "\n\n" + LoKi.GenChild.nChildren    . __doc__ 

if not hasattr ( HepMC.GenParticle , 'nChildren'      ) :
    HepMC.GenParticle  . nChildren      = nChildren
if not hasattr ( HepMC.GenParticle , 'nDaughters'     ) :
    HepMC.GenParticle  . nDaughters     = nChildren
if not hasattr ( HepMC.GenParticle , '__nChildren__'  ) :
    HepMC.GenParticle  . __nChildren__  = nChildren
if not hasattr ( HepMC.GenParticle , '__nDaughters__' ) :
    HepMC.GenParticle  . __nDaughters__ = nChildren

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
    return LoKi.GenChild.child ( p , *a )

child . __doc__  +=  "\n\n" + LoKi.GenChild.child    . __doc__ 

if not hasattr ( HepMC.GenParticle , 'child'         ) :
    HepMC.GenParticle  . child         = child 
if not hasattr ( HepMC.GenParticle , 'daughter'      ) :
    HepMC.GenParticle  . daughter      = child 
if not hasattr ( HepMC.GenParticle , '__child__'     ) :
    HepMC.GenParticle  . __child__     = child 
if not hasattr ( HepMC.GenParticle , '__daughter__'  ) :
    HepMC.GenParticle  . __daughter__  = child 


# =============================================================================
## Get all 'in'-particles for the given HepMC::Vertex
def particles_in ( v ) :
    """
    Get all 'in'-particles for the given HepMC::Vertex

    >>> v = ...
    >>> for p in particles_in ( v ) :
    ...   print p

    """
    return LoKi.GenChild.particles_in ( v )

particles_in . __doc__ += "\n\n" + LoKi.GenChild.particles_in . __doc__  

if not hasattr ( HepMC.GenVertex , 'particles_in'      ) :
    HepMC.GenVertex  . particles_in      =  particles_in
if not hasattr ( HepMC.GenVertex , '__particles_in__'  ) :
    HepMC.GenVertex  . __particles_in__  =  particles_in
    

# =============================================================================
## Get all 'out'-particles for the given HepMC::Vertex
def particles_out ( v ) :
    """
    Get all 'out'-particles for the given HepMC::Vertex

    >>> v = ...
    >>> for p in particles_out ( v ) :
    ...   print p

    """
    return LoKi.GenChild.particles_out ( v )

particles_out . __doc__ += "\n\n" + LoKi.GenChild.particles_out . __doc__  

if not hasattr ( HepMC.GenVertex , 'particles_out'      ) :
    HepMC.GenVertex  . particles_out      =  particles_out
if not hasattr ( HepMC.GenVertex , '__particles_out__'  ) :
    HepMC.GenVertex  . __particles_out__  =  particles_out
    

# =============================================================================
## Get all particles for the given HepMC::GenEvent
def particles_all ( e ) :
    """
    Get all particles for the given HepMC::GenEvent 

    >>> e = ...
    >>> for p in particles_all ( e ) :
    ...   print p

    """
    return LoKi.GenChild.particles_all ( e )

particles_all . __doc__ += "\n\n" + LoKi.GenChild.particles_all . __doc__  

if not hasattr ( HepMC.GenEvent , 'particles_all'      ) :
    HepMC.GenEvent  . particles_all      =  particles_all
if not hasattr ( HepMC.GenVertex , '__particles_all__'  ) :
    HepMC.GenEvent  . __particles_all__  =  particles_all


# =============================================================================
## Get all vertices for the given HepMC::GenEvent
def vertices_all ( e ) :
    """
    Get all vertices for the given HepMC::GenEvent 

    >>> e = ...
    >>> for v in vertices_all ( e ) :
    ...   print v

    """
    return LoKi.GenChild.vertices_all ( e )

vertices_all . __doc__ += "\n\n" + LoKi.GenChild.vertices_all . __doc__  

if not hasattr ( HepMC.GenEvent , 'vertices_all'      ) :
    HepMC.GenEvent  . vertices_all      =  vertices_all
if not hasattr ( HepMC.GenEvent , '__vertices_all__'  ) :
    HepMC.GenEvent  . __vertices_all__  =  vertices_all


# =============================================================================
## Get all parents from the given HepMC::GenVertex
def parents ( v ) :
    """
    Get all parents from the given HepMC::GenVertex

    >>> v = ...
    >>> for p in parents ( v ) :
    ...    print p
    
    """
    return LoKi.GenChild.parents ( v )

parents . __doc__ += "\n\n" + LoKi.GenChild.parents . __doc__  

if not hasattr ( HepMC.GenVertex , 'parents'      ) :
    HepMC.GenVertex . parents        =  parents
if not hasattr ( HepMC.GenVertex , '__parents__'  ) :
    HepMC.GenVertex . __parents__    =  parents


# =============================================================================
## Get all children from the given HepMC::GenParticle or HepMC::GenVertex 
def children ( o ) :
    """
    Get all children from the given HepMC::GenVertex or HepMC::GenParticle

    >>> o = ...
    >>> for p in children ( o ) :
    ...    print p
    
    """
    return LoKi.GenChild.children ( o )

children . __doc__ += "\n\n" + LoKi.GenChild.children . __doc__  

if not hasattr ( HepMC.GenVertex   ,   'children'      ) :
    HepMC.GenVertex .   children     =  children
if not hasattr ( HepMC.GenVertex   , '__children__'    ) :
    HepMC.GenVertex . __children__   =  children
if not hasattr ( HepMC.GenVertex   ,   'daughters'     ) :
    HepMC.GenVertex .   daughters    =  children
if not hasattr ( HepMC.GenVertex   , '__daughters__'   ) :
    HepMC.GenVertex . __daughters__  =  children

if not hasattr ( HepMC.GenParticle ,   'children'      ) :
    HepMC.GenParticle .  children    =  children
if not hasattr ( HepMC.GenParticle , '__children__'    ) :
    HepMC.GenParticle. __children__  =  children
if not hasattr ( HepMC.GenParticle ,   'daughters'     ) :
    HepMC.GenParticle .  daughters   =  children
if not hasattr ( HepMC.GenParticle , '__daughters__'   ) :
    HepMC.GenParticle. __daughters__ =  children


# =============================================================================
## Get all 'family'-particles from the given HepMC::GenVertex
def family ( v ) :
    """
    Get all 'family'-particles from the given HepMC::GenVertex

    >>> v = ...
    >>> for p in family ( v ) :
    ...    print p
    
    """
    return LoKi.GenChild.family ( v )

family . __doc__ += "\n\n" + LoKi.GenChild.family . __doc__  

if not hasattr ( HepMC.GenVertex ,    'family'      ) :
    HepMC.GenVertex .   family      =  family
if not hasattr ( HepMC.GenVertex ,  '__family__'    ) :
    HepMC.GenVertex . __family__    =  family


# =============================================================================
## Get all ancestors from the given HepMC::GenParticel or GenVertex 
def ancestors ( o ) :
    """
    Get all ancestors from the given HepMC::GenVertex or HepMC::GenParticle

    >>> o = ...
    >>> for p in ancestors ( o ) :
    ...    print p
    
    """
    return LoKi.GenChild.ancestors ( o )

ancestors . __doc__ += "\n\n" + LoKi.GenChild.ancestors . __doc__  

if not hasattr ( HepMC.GenVertex   ,   'ancestors'      ) :
    HepMC.GenVertex .   ancestors    =  ancestors
if not hasattr ( HepMC.GenVertex   , '__ancestors__'    ) :
    HepMC.GenVertex . __ancestors__  =  ancestors

if not hasattr ( HepMC.GenParticle ,   'ancestors'      ) :
    HepMC.GenParticle .  ancestors   =  ancestors
if not hasattr ( HepMC.GenParticle , '__ancestors__'    ) :
    HepMC.GenParticle. __ancestors__ =  ancestors


# =============================================================================
## Get all descendants from the given HepMC::GenParticel or GenVertex 
def descendants ( o ) :
    """
    Get all descendants from the given HepMC::GenVertex or HepMC::GenParticle

    >>> o = ...
    >>> for p in descendants ( o ) :
    ...    print p
    
    """
    return LoKi.GenChild.descendants ( o )

descendants . __doc__ += "\n\n" + LoKi.GenChild.descendants . __doc__  

if not hasattr ( HepMC.GenVertex   ,     'descendants'      ) :
    HepMC.GenVertex .   descendants    =  descendants
if not hasattr ( HepMC.GenVertex   ,   '__descendants__'    ) :
    HepMC.GenVertex . __descendants__  =  descendants

if not hasattr ( HepMC.GenParticle ,     'descendants'      ) :
    HepMC.GenParticle .  descendants   =  descendants
if not hasattr ( HepMC.GenParticle ,   '__descendants__'    ) :
    HepMC.GenParticle. __descendants__ =  descendants


# =============================================================================
## Get all 'relatives'-particles from the given HepMC::GenVertex
def relatives ( v ) :
    """
    Get all 'relatives'-particles from the given HepMC::GenVertex

    >>> v = ...
    >>> for p in relatives ( v ) :
    ...    print p
    
    """
    return LoKi.GenChild.relatives ( v )

relatives . __doc__ += "\n\n" + LoKi.GenChild.relatives . __doc__  

if not hasattr ( HepMC.GenVertex ,    'relatives'      ) :
    HepMC.GenVertex .   relatives   =  relatives
if not hasattr ( HepMC.GenVertex ,  '__relatives__'    ) :
    HepMC.GenVertex . __relatives__ =  relatives


# =============================================================================
## Extract the certain elements from decay trees and/or containers
def genParticles  ( o , *a ) :
    """
    Extract the certain elements from decay trees and/or containers

    >>> event = ... ;
    >>> for pion in genParticles ( event , 'pi+' == GABSID ) :
    ...    print pion
    
    """
    return LoKi.GenExtract.genParticles ( o , *a )

genParticles . __doc__ += "\n\n" + LoKi.GenExtract.genParticles . __doc__  
getParticles = genParticles
particles    = genParticles
extract      = genParticles

# =============================================================================
## Count number of 'interesting' elements in the decay tree or container,
def count_if ( s , *a ) :
    """
    Count number of 'interesting' elements in the decay tree or container,
    which satisfy certain criteria

    >>> o = ...
    >>> number_of_pions = count_if ( o , 'pi+' == GABSID )
    
    """
    return LoKi.Dicts.GenAlgs.count_if ( s , *a )

count_if . __doc__ += "\n\n" + LoKi.Dicts.GenAlgs.count_if . __doc__  

# =============================================================================
## Check the presence of 'interesting' elements in the decay tree or container,
def found ( s , *a ) :
    """
    check the presence of 'interesting' elements in the decay tree or container,
    which satisfy certain criteria

    >>> o = ...
    >>> good = found ( o , 'pi+' == GABSID )
    
    """
    return LoKi.Dicts.GenAlgs.found ( s , *a )

found . __doc__ += "\n\n" + LoKi.Dicts.GenAlgs.found . __doc__  


# =============================================================================
## Accumulate through the decay tree or container,
def accumulate  ( s , *a ) :
    """
    Accumulate through the decay tree or container,

    >>> o = ...
    >>> ptOfAllPions= accumulate ( o , GPT , 'pi+' == GABSID )
    
    """
    return LoKi.Dicts.GenAlgs.accumulate ( s , *a )

accumulate. __doc__ += "\n\n" + LoKi.Dicts.GenAlgs.accumulate . __doc__  


# =============================================================================
## Find the minimal value through decay tree or  container  
def min_value ( s , *a ) :
    """
    Find the minimal value through decay tree or  container  

    >>> o = ...
    >>> minPT = min_value ( o , GPT , 'pi+' == GABSID )
    
    """
    return LoKi.Dicts.GenAlgs.min_value ( s , *a )

min_value. __doc__ += "\n\n" + LoKi.Dicts.GenAlgs.min_value . __doc__  

# =============================================================================
## Find the maximal value through decay tree or  container  
def max_value ( s , *a ) :
    """
    Find the maximal value through decay tree or  container  

    >>> o = ...
    >>> maxPT = max_value ( o , GPT , 'pi+' == GABSID )
    
    """
    return LoKi.Dicts.GenAlgs.max_value ( s , *a )

max_value. __doc__ += "\n\n" + LoKi.Dicts.GenAlgs.max_value . __doc__  

# =============================================================================
## Find the minimal element through decay tree or  container  
def min_element ( s , *a ) :
    """
    Find the minimal element through decay tree or  container  

    >>> o = ...
    >>> pion = min_element ( o , GPT , 'pi+' == GABSID )
    
    """
    return LoKi.Dicts.GenAlgs.min_element ( s , *a )

min_element. __doc__ += "\n\n" + LoKi.Dicts.GenAlgs.min_element . __doc__  

# =============================================================================
## Find the maximal element through decay tree or  container  
def max_element ( s , *a ) :
    """
    Find the maximal element through decay tree or  container  

    >>> o = ...
    >>> pion = max_element ( o , GPT , 'pi+' == GABSID )
    
    """
    return LoKi.Dicts.GenAlgs.max_element ( s , *a )

max_element. __doc__ += "\n\n" + LoKi.Dicts.GenAlgs.max_element . __doc__  


for t in ( HepMC.GenParticle ,
           HepMC.GenVertex   ,
           HepMC.GenEvent    ,
           LHCb.HepMCEvent   ) :
    
    ## GenExtract
    if not hasattr ( t ,   'genParticles'   ) : t.   genParticles   = genParticles 
    if not hasattr ( t , '__genParticles__' ) : t. __genParticles__ = genParticles 
    if not hasattr ( t ,   'getParticles'   ) : t.   getParticles   = genParticles 
    if not hasattr ( t , '__genParticles__' ) : t. __getParticles__ = genParticles 
    if not hasattr ( t ,   'particles'      ) : t.   particles      = genParticles 
    if not hasattr ( t , '__particles__'    ) : t. __particles__    = genParticles 
    if not hasattr ( t ,   'extract'        ) : t.   extract        = genParticles 
    if not hasattr ( t , '__extract__'      ) : t. __extract__      = genParticles

    #@ GenAlgs:
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



# =============================================================================
## print the decay  
def printDecay ( s , *a ) :
    """
    Print the decay of HepMC::GenParticle

    >>> p = ...
    >>> printDecay ( p )
    
    """
    return LoKi.PrintHepMC.printDecay ( s , *a )

printDecay. __doc__ += "\n\n" + LoKi.PrintHepMC.printDecay . __doc__  

if not hasattr ( HepMC.GenParticle ,     'printDecay'   ) :
    HepMC.GenParticle .  printDecay   =   printDecay
    HepMC.GenParticle .       decay   =   printDecay
if not hasattr ( HepMC.GenParticle ,   '__printDecay__' ) :
    HepMC.GenParticle. __printDecay__ =   printDecay
    HepMC.GenParticle.      __decay__ =   printDecay


def __particleID__ ( self ) :
    """
    Get the ParticleID obejct for the particle

    >>> p =
    >>> pid = p.particleID()
    
    """
    return LHCb.ParticleID( self.pdg_id() )

HepMC.GenParticle.__particleID__  = __particleID__
HepMC.GenParticle.  particleID    = __particleID__

import LoKiCore.functions  as _LCF 
HepMC.GenParticle.  pname   = _LCF.__pname__
HepMC.GenParticle.   name   = _LCF.__pname__
HepMC.GenParticle.__pname__ = _LCF.__pname__


_LCF.nChildren     . __doc__ += "\n" + LoKi.GenChild.nChildren         . __doc__ 
_LCF.child         . __doc__ += "\n" + LoKi.GenChild.child             . __doc__
_LCF.daughter      . __doc__ += "\n" + LoKi.GenChild.child             . __doc__ 
_LCF.particles_out . __doc__ += "\n" + LoKi.GenChild.particles_out     . __doc__
_LCF.parents       . __doc__ += "\n" + LoKi.GenChild.parents           . __doc__
_LCF.children      . __doc__ += "\n" + LoKi.GenChild.children          . __doc__
_LCF.daughters     . __doc__ += "\n" + LoKi.GenChild.children          . __doc__ 
_LCF.descendants   . __doc__ += "\n" + LoKi.GenChild.descendants       . __doc__ 
_LCF.ancestors     . __doc__ += "\n" + LoKi.GenChild.ancestors         . __doc__ 

_LCF.extract       . __doc__ += "\n" + LoKi.GenExtract.genParticles . __doc__ 
_LCF.particles     . __doc__ += "\n" + LoKi.GenExtract.genParticles . __doc__ 
_LCF.getParticles  . __doc__ += "\n" + LoKi.GenExtract.genParticles . __doc__ 

_LCF.count_if      . __doc__ += "\n" + LoKi.Dicts.GenAlgs.count_if     . __doc__ 
_LCF.found         . __doc__ += "\n" + LoKi.Dicts.GenAlgs.found        . __doc__ 
_LCF.accumulate    . __doc__ += "\n" + LoKi.Dicts.GenAlgs.accumulate   . __doc__ 
_LCF.min_value     . __doc__ += "\n" + LoKi.Dicts.GenAlgs.min_value    . __doc__ 
_LCF.max_value     . __doc__ += "\n" + LoKi.Dicts.GenAlgs.max_value    . __doc__ 
_LCF.min_element   . __doc__ += "\n" + LoKi.Dicts.GenAlgs.min_element  . __doc__ 
_LCF.max_element   . __doc__ += "\n" + LoKi.Dicts.GenAlgs.max_element  . __doc__ 

_LCF.printDecay    . __doc__ += "\n" + LoKi.PrintHepMC.printDecay      . __doc__ 


if not hasattr ( HepMC.GenParticle , 'ConstVector' ) :
    HepMC.GenParticle.ConstVector = std.vector ('const HepMC::GenParticle*')
if not hasattr ( HepMC.GenVertex   , 'ConstVector' ) :
    HepMC.GenVertex.ConstVector   = std.vector ('const HepMC::GenVertex*'  )

if not hasattr ( HepMC.GenParticle , 'Range' ) :
    HepMC.GenParticle.Range = Gaudi.NamedRange_( HepMC.GenParticle.ConstVector ) 
if not hasattr ( HepMC.GenVertex   , 'Range' ) :
    HepMC.GenVertex.Range   = Gaudi.NamedRange_( HepMC.GenVertex.ConstVector   ) 
    
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
