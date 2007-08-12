#!/usr/bin/env python
# =============================================================================
# $Id: Array.py,v 1.1 2007-08-12 18:57:09 ibelyaev Exp $ 
# =============================================================================
## @file
#  collection of utilities for useful 'decoration' of Array-objects
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2007-08-11 
# =============================================================================
"""
Collection of utilities for useful 'decoration' of Array-objects
"""
# =============================================================================
__author__ = 'Vanya BELYAEV ibelyaev@physics.syr.edu'
# =============================================================================

from   LoKiArrayFunctors.functions import LoKi,LHCb,std

_T = std.vector('const LHCb::Particle*')

# =============================================================================
## Get number of child particles :
def nChildren ( p ) :
    """
    Get the number of child particles :

    >>> n = nChildren ( p )
    
    """
    return p.size()

if not hasattr ( _T ,   'nChildren'    ) : _T .   nChildren    = nChildren
if not hasattr ( _T ,   'nDaughters'   ) : _T .   nDaughters   = nChildren
if not hasattr ( _T , '__nChildren__'  ) : _T . __nChildren__  = nChildren
if not hasattr ( _T , '__nDaughters__' ) : _T . __nDaughters__ = nChildren


# =============================================================================
## Get all children from the given ConstVector 
def children ( o ) :
    """
    Get all children from the given ConstVector

    >>> o = ...
    >>> for p in children ( o ) :
    ...    print p
    
    """
    return o   

if not hasattr ( _T ,   'children'    ) : _T .   children    =  children
if not hasattr ( _T , '__children__'  ) : _T . __children__  =  children
if not hasattr ( _T ,   'daughters'   ) : _T .   daughters   =  children
if not hasattr ( _T , '__daughters__' ) : _T . __daughters__ =  children


# =============================================================================
## get the child 
def child ( p , *a ) :
    """
    Get the child for particle combination
    
    >>> p = ...
    >>> d1  = child ( p , 1 ) 
    >>> d12 = child ( p , 1 , 2 )
    
    ATTTENTION: indices start from 1.
    Index 0 corresponds to the particle itself.
    """
    return LoKi.AChild.child ( p , *a )

child . __doc__  +=  "\n\n" + LoKi.AChild.child    . __doc__ 

if not hasattr ( _T ,   'child'      ) : _T .   child      = child 
if not hasattr ( _T , '__child__'    ) : _T . __child__    = child 
if not hasattr ( _T ,   'daughter'   ) : _T .   daughter   = child 
if not hasattr ( _T , '__daughter__' ) : _T . __daughter__ = child 

# =============================================================================
## Get all descendants from the given combination
def descendants ( o ) :
    """
    Get all descendants from the given combination 

    >>> o = ...
    >>> for p in descendants ( o ) :
    ...    print p
    
    """
    return LoKi.AChild.descendants ( o , *a )

descendants . __doc__ += "\n\n" + LoKi.AChild.descendants . __doc__  

if not hasattr ( _T ,   'descendants'   ) : _T .   descendants    =  descendants
if not hasattr ( _T , '__descendants__' ) : _T . __descendants__  =  descendants


import LoKiCore.functions  as _LCF 

_LCF.nChildren     . __doc__ += "\n" + LoKi.AChild.nChildren         . __doc__ 
_LCF.child         . __doc__ += "\n" + LoKi.AChild.child             . __doc__
_LCF.daughter      . __doc__ += "\n" + LoKi.AChild.child             . __doc__ 
_LCF.children      . __doc__ += "\n" + LoKi.AChild.children          . __doc__
_LCF.daughters     . __doc__ += "\n" + LoKi.AChild.children          . __doc__ 
_LCF.descendants   . __doc__ += "\n" + LoKi.AChild.descendants       . __doc__ 


# =============================================================================
if '__main__' == __name__ :
    print __doc__ , '\n' , __author__
    for i in dir() : print i 

# =============================================================================
# The END
# =============================================================================
