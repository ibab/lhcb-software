#!/usr/bin/env python
# =============================================================================
# $Id$
# =============================================================================
## @file
# helper module for decoration of standard N-tuples
#
#  This file is a part of LoKi project - 
#    "C++ ToolKit  for Smart and Friendly Physics Analysis"
#
#  The package has been designed with the kind help from
#  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
#  contributions and advices from G.Raven, J.van Tilburg, 
#  A.Golutvin, P.Koppenburg have been used in the design.
#
#  By usage of this code one clearly states the disagreement 
#  with the smear campaign of Dr.O.Callot et al.: 
#  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
#
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2008-06-11
#
#                   $Revision$
# Last modification $Date$
#                by $Author$
# =============================================================================
"""
Helper module for decoration of standard N-tuples

This file is a part of LoKi project - 
``C++ ToolKit  for Smart and Friendly Physics Analysis''

The package has been designed with the kind help from
Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
contributions and advices from G.Raven, J.van Tilburg, 
A.Golutvin, P.Koppenburg have been used in the design.

By usage of this code one clearly states the disagreement 
with the smear campaign of Dr.O.Callot et al.: 
``No Vanya's lines are allowed in LHCb/Gaudi software.''

"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl "
__date__    = "2008-06-11"
__version__ = "$Revision$"
# =============================================================================
from LoKiCore.basic import cpp

_Tuple = cpp.Tuples.Tuple
_Array = cpp.LoKi.Dicts.TupleArray

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

# =============================================================================
if '__main__' == __name__ :
    
    print 80*'*'
    print __doc__
    print ' Author  : ' , __author__
    print ' Version : ' , __version__
    print ' Date    : ' , __date__    
    print 80*'*'    
    print _Tuple.fArrayP .__doc__ 
    print _Tuple.fArrayV .__doc__ 
    print 80*'*'

# =============================================================================
# The END 
# =============================================================================
