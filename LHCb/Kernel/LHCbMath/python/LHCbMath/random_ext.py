#!/usr/bin/env python
# =============================================================================
# $Id:$
# =============================================================================
# @file LHCbMath/random.py
#
# The simple extention for the standard python module random
# 
# @author Vanya BELYAEV
# @date   2012-04-28
#
#                   $Revision:$
# Last modification $Date:$
#                by $Author:$
# =============================================================================
"""

The simple extension for the standard python module random

"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__version__ = "$Revision:$"
__date__    = "2012-04-28"
# =============================================================================
__all__ = (
    'bifur'  ,   ## bifurcated gaussian
    've_gau' ,   ## gaussian using ValueWithError construction 
    )
# =============================================================================

# =============================================================================
## generate bifurcated gaussian 
def _bifur_ ( self , mu , sigma1 , sigma2 ) :
    """
    Generate the bifurcated gaussian

    >>> value = bifur ( 0 , -1 , +2 )
    
    """
    if   sigma1 * sigma2 > 0.0 :
        raise ValueError, 'Lower and upper errors must have opposite signs'
        
    _as1  =  abs ( float ( sigma1 ) ) 
    _as2  =  abs ( float ( sigma2 ) ) 
    _frac = _as1 / (  _as1  +  _as2 )
    
    _aux  =       self.random ()
    _gau  = abs ( self.gauss  ( 0 , 1 ) ) 
    
    if _aux <= _frac : return mu + sigma1 * _gau
    else             : return mu + sigma2 * _gau 

# =============================================================================
## generate bifurcated gaussian using Value
#  @see Gaudi::Math::ValueWithError
def _ve_gau_ ( self , val ) :
    """
    Generate the gaussian 

    >>> value = ve_gau ( ... )
    
    """
    mean  = val.value ()
    sigma = val.error ()
    return self.gauss ( mean , sigma )
    
import random
if not hasattr ( random.Random , 'bifur'  ) : random.Random.bifur  = _bifur_
if not hasattr ( random        , 'bifur'  ) : random.bifur         = random._inst.bifur
if not hasattr ( random.Random , 've_gau' ) : random.Random.ve_gau = _ve_gau_
if not hasattr ( random        , 've_gau' ) : random.ve_gau        = random._inst.ve_gau

bifur  = random.bifur
ve_gau = random.ve_gau

# =============================================================================
if '__main__' == __name__  :
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print '*'*120
    
# =============================================================================
# The END
# =============================================================================
