#!/usr/bin/env python
# =============================================================================
# $Id$
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
#
#                    $Revision$
#  Last modification $Date$
#                 by $Author$
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
__author__  = 'Vanya BELYAEV ibelyaev@physics.syr.edu'
__date__    = '2007-08-11'
__version__ = 'Version $Revision$'
# =============================================================================

from   LoKiPhys.functions   import LoKi,LHCb,cpp

# =============================================================================
## Get number of child particles :
def nChildren ( p ) :
    """Get the number of child particles :

    >>> n = nChildren ( p )
    
    """
    return p.daughters().size()


LHCb.Particle .   nChildren    = nChildren
LHCb.Particle .   nDaughters   = nChildren
LHCb.Particle . __nChildren__  = nChildren
LHCb.Particle . __nDaughters__ = nChildren

_childSelector  = LoKi.Child.Selector 
# =============================================================================
## Get all children from the given LHCb::Particle 
def children ( o , *a ) :
    """
    Get all children from the given LHCb::Particle

    >>> o = ...
    >>> for p in children ( o ) :
    ...    print p
    
    """
    return LoKi.Child.children ( o , *a )

children . __doc__ += '\n' + LoKi.Child.children . __doc__

LHCb.Particle .   children    =  children
LHCb.Particle . __children__  =  children

## save/backup the original C++ method 
if not hasattr ( LHCb.Particle , 'orig_daughters' ) :
    LHCb.Particle.orig_daughters = LHCb.Particle.daughters
    
LHCb.Particle .   daughters   =  children
LHCb.Particle . __daughters__ =  children

LHCb.Particle.hex_id  = lambda s : LoKi.Particles.hex_id ( s ) 
# =============================================================================
## get the child
#  @code
#  p = ...
#  d1  = child ( p , 1 ) 
#  d12 = child ( p , 1 , 2 )
#  @code
#  @attention indices start from 1.
#  Index 0 corresponds to the particle itself. 
def child ( p , *a ) :
    """Get the child for 'particle'
    
    >>> p = ...
    >>> d1  = child ( p , 1 ) 
    >>> d12 = child ( p , 1 , 2 )
    
    ATTTENTION: indices start from 1.
    Index 0 corresponds to the particle itself.
    """
    return LoKi.Child.child ( p , *a )

child . __doc__  +=  "\n\n" + LoKi.Child.child    . __doc__ 

LHCb.Particle  .   child      = child 
LHCb.Particle  . __child__    = child 
LHCb.Particle  .   daughter   = child 
LHCb.Particle  . __daughter__ = child 

LHCb . Particle . __call__    = child


# =============================================================================
## get proper getitem method 
#  @code
#  p   = ...
#  d1  = p[1] 
#  d2  = p[2] 
#  @code
#  @attention indices start from 1.
#  Index 0 corresponds to the particle itself. 
def _p_getitem_ ( p , index ) :
    """ Get children particle by index
    >>> p   = ...
    >>> d1  = p[1] 
    >>> d2  = p[2]
    ATTTENTION: indices start from 1.
    Index 0 corresponds to the particle itself.    
    """
    nc = nChildren( p ) 
    if not 0 <= index <= nc :
        raise IndexError ('Index %s is outside the range [0,%d]' % ( index , nc ) )
    return child ( p , index ) 

LHCb . Particle . __getitem__ = _p_getitem_

# =============================================================================
## Get all descendants from the given LHCb::Particle 
def descendants ( o ) :
    """Get all descendants from the given LHCb::Particle
    >>> o = ...
    >>> for p in descendants ( o ) :
    ...    print p
    
    """
    return LoKi.Child.descendants ( o , *a )

descendants . __doc__ += "\n\n" + LoKi.Child.descendants . __doc__  

LHCb.Particle .   descendants    =  descendants
LHCb.Particle . __descendants__  =  descendants

# =============================================================================
## Get the particles from the decay tree, which satisfy certain criteria
def particles ( p , c ) :
    """Get the particles from the decay tree, which satisfy certrain criteria

    >>> p = ...
    >>> pions = p.particles ( 'pi+' == ABSID )
    >>> for pion in pions :
    ...   print pion

    """
    return LoKi.Dicts.Extract.particles ( p , c )

particles . __doc__ += "\n\n" + LoKi.Dicts.Extract.particles . __doc__

# =============================================================================
## Get the particles from the decay tree, which satisfy certain criteria
def protoparticles ( p , c ) :
    """Get the protoparticles from the decay tree, which satisfy certrain criteria

    >>> p = ...
    >>> pion_pps = p.protoparticles ( 'pi+' == ABSID )
    >>> for pp in pions_pps :
    ...   print pp

    """
    return LoKi.Dicts.Extract.protoParticles ( p , c )

# =============================================================================
## Get the tarcks from the decay tree, which satisfy certain criteria
def tracks ( p , *c ) :
    """Get the tracks from the decay tree, which satisfy certrain criteria

    >>> p = ...
    >>> pion_trks = p.tracks ( 'pi+' == ABSID )
    >>> for t in pions_trks :
    ...   print t

    """
    return LoKi.Dicts.Extract.tracks ( p , *c )


protoparticles . __doc__ += "\n\n" + LoKi.Dicts.Extract.particles . __doc__  


getParticles      = particles
getProtoParticles = protoparticles
getTracks         = tracks 

extract           = particles


LHCb.Particle .   particles      = particles 
LHCb.Particle . __particles__    =  particles 
LHCb.Particle . __getParticles   = particles 
LHCb.Particle . __getParticles__ =  particles 
LHCb.Particle .   extract        = particles 
LHCb.Particle . __extract__      =  particles 
LHCb.Particle .   vertex         =  LHCb.Particle . endVertex   
LHCb.Particle .   decayVertex    =  LHCb.Particle . endVertex   

LHCb.Particle .   protoparticles = protoparticles 
LHCb.Particle .   protoParticles = protoparticles
LHCb.Particle .getProtoParticles = protoparticles

LHCb.Particle .   tracks         = tracks 
LHCb.Particle .getTracks         = tracks 


# =============================================================================
## Count number of 'interesting' elements in the decay tree or container,
def count_if ( s , *a ) :
    """Count number of 'interesting' elements in the decay tree or container,
    which satisfy certain criteria

    >>> o = ...
    >>> number_of_pions = count_if ( o , 'pi+' == ABSID )
    
    """
    return LoKi.Dicts.Algs.count_if ( s , *a )

count_if . __doc__ += "\n\n" + LoKi.Dicts.Algs.count_if . __doc__  

# =============================================================================
## Check the presence of 'interesting' elements in the decay tree or container,
def found ( s , *a ) :
    """Check the presence of 'interesting' elements in the decay tree or container,
    which satisfy certain criteria

    >>> o = ...
    >>> good = found ( o , 'pi+' == ABSID )
    
    """
    return LoKi.Dicts.Algs.found ( s , *a )

found . __doc__ += "\n\n" + LoKi.Dicts.Algs.found . __doc__  

# =============================================================================
## Accumulate through the decay tree or container,
def accumulate  ( s , *a ) :
    """Accumulate through the decay tree or container,

    >>> o = ...
    >>> ptOfAllPions= accumulate ( o , PT , 'pi+' == ABSID )
    
    """
    return LoKi.Dicts.Algs.accumulate ( s , *a )

accumulate. __doc__ += "\n\n" + LoKi.Dicts.Algs.accumulate . __doc__  

# =============================================================================
## Find the minimal value through decay tree or  container  
def min_value ( s , *a ) :
    """Find the minimal value through decay tree or  container  

    >>> o = ...
    >>> minPT = min_value ( o , PT , 'pi+' == ABSID )
    
    """
    return LoKi.Dicts.Algs.min_value ( s , *a )

min_value. __doc__ += "\n\n" + LoKi.Dicts.Algs.min_value . __doc__  

# =============================================================================
## Find the maximal value through decay tree or  container  
def max_value ( s , *a ) :
    """Find the maximal value through decay tree or  container  

    >>> o = ...
    >>> maxPT = max_value ( o , PT , 'pi+' == ABSID )
    
    """
    return LoKi.Dicts.Algs.max_value ( s , *a )

max_value. __doc__ += "\n\n" + LoKi.Dicts.Algs.max_value . __doc__  

# =============================================================================
## Find the minimal element through decay tree or  container  
def min_element ( s , *a ) :
    """Find the minimal element through decay tree or  container  

    >>> o = ...
    >>> pion = min_element ( o , PT , 'pi+' == ABSID )
    
    """
    return LoKi.Dicts.Algs.min_element ( s , *a )

min_element. __doc__ += "\n\n" + LoKi.Dicts.Algs.min_element . __doc__  

# =============================================================================
## Find the maximal element through decay tree or  container  
def max_element ( s , *a ) :
    """Find the maximal element through decay tree or  container  

    >>> o = ...
    >>> pion = max_element ( o , PT , 'pi+' == ABSID )
    
    """
    return LoKi.Dicts.Algs.max_element ( s , *a )

max_element. __doc__ += "\n\n" + LoKi.Dicts.Algs.max_element . __doc__  

# =============================================================================

LHCb.Particle .   count_if      = count_if
LHCb.Particle . __count_if__    = count_if  
LHCb.Particle .   found         = found
LHCb.Particle . __found__       = found 
LHCb.Particle .   min_value     = min_value 
LHCb.Particle . __min_value__   = min_value 
LHCb.Particle .   max_value     = max_value 
LHCb.Particle . __max_value__   = max_value  
LHCb.Particle .   min_element   = min_element 
LHCb.Particle . __min_element__ = min_element
LHCb.Particle .   max_element   = max_element 
LHCb.Particle . __max_element__ = max_element
LHCb.Particle .   accumulate    = accumulate 
LHCb.Particle . __accumulate__  = accumulate 

# =============================================================================
## print the decay  
def printDecay ( s , *a ) :
    """Print the decay of LHCb::Particle

    >>> p = ...
    >>> printDecay ( p )
    
    """
    return LoKi.PrintPhys.printDecay ( s , *a )

printDecay. __doc__ += "\n\n" + LoKi.PrintPhys.printDecay . __doc__  

LHCb.Particle .  printDecay   =   printDecay
LHCb.Particle .       decay   =   printDecay
LHCb.Particle. __printDecay__ =   printDecay
LHCb.Particle.      __decay__ =   printDecay


## non-zero test for smart reference 
for t in ( LHCb.Particle    ,
           LHCb.Vertex      ,
           LHCb.VertexBase  ,
           LHCb.Track       ,
           LHCb.CaloDigit   ,
           LHCb.CaloCluster , 
           LHCb.CaloHypo    ) :
    
    _SR = cpp.SmartRef ( t )
    _SR .__nonzero__ = lambda s : bool ( s.target() )

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
            LHCb.VertexBase  ,
            LHCb.Track       ,
            LHCb.CaloDigit   ,
            LHCb.CaloHypo    ,
            LHCb.CaloCluster ) :
    _tt = cpp.SmartRefVector ( _t )
    # redefine the iterator 
    _tt.__iter__ = _LCF._iter_SRV_

    
LHCb.Particle.ConstVector   = cpp.std.vector ('const LHCb::Particle*')
LHCb.Particle.Range         = cpp.Gaudi.NamedRange_ ( LHCb.Particle.ConstVector )

LHCb.Vertex.ConstVector     = cpp.std.vector ('const LHCb::VertexBase*')
LHCb.Vertex.Range           = cpp.Gaudi.NamedRange_ ( LHCb.Vertex.ConstVector )

LHCb.VertexBase.ConstVector = cpp.std.vector ('const LHCb::VertexBase*')
LHCb.VertexBase.Range       = cpp.Gaudi.NamedRange_ ( LHCb.VertexBase.ConstVector )

for r in ( LHCb.Particle.Range     ,
           LHCb.VertexBase.Range   ,
           cpp.Gaudi.Range_ ( LHCb.Particle.ConstVector   ) ,
           cpp.Gaudi.Range_ ( LHCb.VertexBase.ConstVector ) ) :
    
     import LoKiCore.decorators as _LCD
     
     r.__iter__     = _LCD . _iter_1_
     r.__getslice__ = _LCD . _slice_
     r.__getitem__  =    r . __call__ 
     r.__setitem__  =          None


LHCb.Particle.Container = cpp.KeyedContainer(LHCb.Particle,'Containers::KeyedObjectManager<Containers::hashmap>')
LHCb.Particle.Selection = cpp.SharedObjectsContainer (LHCb.Particle) 

def _iter_LPS_ ( self ) :
    s = len( self )
    for i in range(0,s):
        yield self.at( i )
        
LHCb.Particle.Selection.__iter__ =  _iter_LPS_

# =============================================================================
## define various printers 
def _print_ ( self                                     ,
              accept   = None                          ,
              mark     = None                          ,
              maxDepth = 5                             ,
              vertex   = True                          , 
              mode     = LoKi.DecayChainBase.LV_WITHPT ,
              fg       = cpp.MSG.YELLOW                ,
              bg       = cpp.MSG.RED                   ) :
    
    """
    Define the print functions for some objects
    for details see LoKi::DecayChain 
    """
    _printer = LoKi.DecayChain ( maxDepth ,
                                 vertex   , 
                                 mode     ,
                                 fg       ,
                                 bg       ) 
    if    accept and not mark : 
        return _printer.print_ ( self , accept )
    elif  accept and     mark : 
        return _printer.print_ ( self , accept , mark )
    #
    return _printer.print_ ( self )

_print_ . __doc__ += "\n" + LoKi.DecayChain.print_ . __doc__ 

for t in ( LHCb.Particle             ,
           LHCb.Particle.Range       , 
           LHCb.Particle.ConstVector ,
           LHCb.Particle.Container   ,
           LHCb.Particle.Selection   ) :    
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
