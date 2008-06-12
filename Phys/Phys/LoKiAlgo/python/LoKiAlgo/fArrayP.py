#!/usr/bin/env python
# =============================================================================
# $Id: fArrayP.py,v 1.1 2008-06-12 08:28:54 ibelyaev Exp $
# =============================================================================
## @file
#  helper module for decoration of standrad N-tuples
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2008-06-11
# =============================================================================
"""
Helper module for decoration of standrad N-tuples
"""
# =============================================================================
__author__ = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl "
# =============================================================================

import PyCintex

# construct the global namespace 
_cpp = PyCintex.makeNamespace('')


_Tuple = _cpp.Tuples.Tuple
_Array = _cpp.LoKi.Dicts.TupleArray

## simple function for decoration of "Tuples::Tuple::farray" method 
def _fArrayP_ ( self , *args ) :
    """
    Simple function for decoration of 'Tuples::Tuple::farray' method
    
    
    >>> tup = ...
    >>> particles = ...            ## the range of particles 
    
    >>> tup.fArrayP ( 'p'  , P  ,  ## column name and functor 
                      particles ,  ## source: the range of particles 
                      'Length'  ,  ## the name of 'length' column
                      100       )  ## maximal length 

    or

    >>> tup.fArrayP ( 'p'  , P  ,  ## the first  column name and functor 
                      'pt' , PT ,  ## the second column name and functor 
                      particles ,  ## source: the range of particles 
                      'Length'  ,  ## the name of 'length' column
                      100       )  ## maximal length 
    
    or

    >>> tup.fArrayP ( 'p'  , P  ,  ## the first  column name and functor 
                      'pt' , PT ,  ## the second column name and functor 
                      'm'  , M  ,  ## the third  column name and functor 
                      particles ,  ## source: the range of particles 
                      'Length'  ,  ## the name of 'length' column
                      100       )  ## maximal length 
    
    or

    >>> tup.fArrayP ( 'p'  , P  ,  ## the first  column name and functor 
                      'pt' , PT ,  ## the second column name and functor 
                      'm'  , M  ,  ## the third  column name and functor 
                      'pz' , PZ ,  ## the fourth column name and functor 
                      particles ,  ## source: the range of particles 
                      'Length'  ,  ## the name of 'length' column
                      100       )  ## maximal length 
                      
    """
    return _Array.farrayp ( self , *args )


## simple function for decoration of "Tuples::Tuple::farray" method 
def _fArrayV_ ( self , *args ) :
    """
    Simple function for decoration of 'Tuples::Tuple::farray' method
    
    
    >>> tup = ...
    >>> vertices = ...             ## the range of vertices 
    
    >>> tup.fArrayP ( 'chi2' , VCHI2 ,  ## column name and functor 
                      vertices   ,  ## source: the range of vertices
                      'Length'   ,  ## the name of 'length' column
                      100        )  ## maximal length 

    or

    >>> tup.fArrayP ( 'chi2' , VCHI2  ,  ## the first  column name and functor 
                      'prob' , VPCHI2 ,  ## the second column name and functor 
                      vertices   ,  ## source: the range of vertices
                      'Length'  ,  ## the name of 'length' column
                      100       )  ## maximal length 
    
    or

    >>> tup.fArrayP ( 'chi2' , VCHI2  ,  ## the first  column name and functor 
                      'prob' , VPCHI2 ,  ## the second column name and functor 
                      'vx'   , VX     ,  ## the third  column name and functor 
                      vertices   ,  ## source: the range of vertices
                      'Length'  ,  ## the name of 'length' column
                      100       )  ## maximal length 
    
    or

    >>> tup.fArrayP ( 'chi2' , VCHI2  ,  ## the first  column name and functor 
                      'prob' , VPCHI2 ,  ## the second column name and functor 
                      'vx'   , VX     ,  ## the third  column name and functor 
                      'vy'   , VY     ,  ## the fourth column name and functor 
                      vertices   ,  ## source: the range of vertices
                      'Length'  ,  ## the name of 'length' column
                      100       )  ## maximal length 
                      
    """
    return _Array.farrayv ( self , *args )


_fArrayP_ . __doc__ += '\n' + _Array.farrayp.__doc__
_fArrayV_ . __doc__ += '\n' + _Array.farrayv.__doc__

_Tuple.fArrayP = _fArrayP_ 
_Tuple.fArrayV = _fArrayV_ 


if '__main__' == __name__ :

    print _Tuple.__doc__
    
# =============================================================================
# The END 
# =============================================================================
