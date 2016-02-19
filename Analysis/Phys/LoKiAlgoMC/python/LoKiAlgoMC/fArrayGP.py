#!/usr/bin/env python
# =============================================================================
# $Id$
# =============================================================================
## @file
#  helper module for decoration of standard N-tuples
#
#        This file is a part of LoKi project - 
#    "C++ ToolKit  for Smart and Friendly Physics Analysis"
#
#  The package has been designed with the kind help from
#  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
#  contributions and advices from G.Raven, J.van Tilburg, 
#  A.Golutvin, P.Koppenburg have been used in the design.
#
#   By usage of this code one clearly states the disagreement 
#  with the campain of Dr.O.Callot et al.: 
#  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
#
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date 2011-02-13
#
#                    $Revision$
#  Last Modification $Date$
#                 by $Author$
#
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
with the campain of Dr.O.Callot et al.:
 ``No Vanya's lines are allowed in LHCb/Gaudi software.''

"""
# =============================================================================
__author__ = "Vanya BELYAEV Ivan.Belyaev@cern.ch "
__date__    = "????-??-??"
__version__ = "SVN $Revision$ "
# =============================================================================
from LoKiCore.basic import cpp

_Tuple = cpp.Tuples.Tuple
_Array = cpp.LoKi.Dicts.GenTupleArray
## simple function for decoration of "Tuples::Tuple::farray" method 
def _fArrayGP_ ( self , *args ) :
    """Simple function for decoration of 'Tuples::Tuple::farray' method
    
    >>> tup = ...
    >>> particles = ...            ## the range of particles 
    
    >>> tup.fArrayGP  ( 'p'  , GP  ,  ## column name and functor 
                         particles ,  ## source: the range of particles 
                         'Length'  ,  ## the name of 'length' column
                          100       )  ## maximal length 

    or
    >>> tup.fArrayGP  ( 'p'  , GP  ,  ## the first  column name and functor 
                        'pt' , GPT ,  ## the second column name and functor 
                         particles ,  ## source: the range of particles 
                        'Length'  ,  ## the name of 'length' column
                         100       )  ## maximal length 
    
    or
    >>> tup.fArrayGP  ( 'p'  , GP  ,  ## the first  column name and functor 
                        'pt' , GPT ,  ## the second column name and functor 
                        'm'  , GM  ,  ## the third  column name and functor 
                         particles ,  ## source: the range of particles 
                        'Length'  ,  ## the name of 'length' column
                        100       )  ## maximal length 
    
    or
    >>> tup.fArrayGP  ( 'p'  , GP  ,  ## the first  column name and functor 
                        'pt' , GPT ,  ## the second column name and functor 
                        'm'  , GM  ,  ## the third  column name and functor 
                        'pz' , GPZ ,  ## the fourth column name and functor 
                        particles ,  ## source: the range of particles 
                        'Length'  ,  ## the name of 'length' column
                         100       )  ## maximal length 
    """
    return _Array.farrayp ( self , *args )

_fArrayGP_ . __doc__ += '\n' + _Array.farrayp.__doc__

_Tuple.fArrayGP = _fArrayGP_ 
# =============================================================================
if '__main__' == __name__ :

    print 80*'*'
    print __doc__
    print ' Author  : ' , __author__
    print ' Version : ' , __version__
    print ' Date    : ' , __date__    
    print _Tuple.fArrayGP.__doc__
    
# =============================================================================
# The END 
# =============================================================================
