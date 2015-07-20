#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# @file
# Set of utilities for rounding according to PDG prescription
# @see http://pdg.lbl.gov/2010/reviews/rpp2010-rev-rpp-intro.pdf
# @see section 5.3 of doi:10.1088/0954-3899/33/1/001
# Quote:
# The basic rule states that if the three highest order digits of the error
# lie between 100 and 354, we round to two significant digits. If they lie between
# 355 and 949, we round to one significant digit. Finally,
# if they lie between 950 and 999, we round up to 1000 and keep two significant digits.
# In all cases, the central value is given with a precision that matches that of the error.
# 
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# @date 2015-07-20
#
# Version           $Revision:$
# Last modification $Date:$
#                by $Author:$
# 
# =============================================================================
""" Set of utilities for rounding according to PDG prescription
see http://pdg.lbl.gov/2010/reviews/rpp2010-rev-rpp-intro.pdf
see section 5.3 of doi:10.1088/0954-3899/33/1/001

Quote:
| The basic rule states that if the three highest order digits of the error
| lie between 100 and 354, we round to two significant digits. If they lie between
| 355 and 949, we round to one significant digit. Finally,
| if they lie between 950 and 999, we round up to 1000 and keep two significant digits.
| In all cases, the central value is given with a precision that matches that of the error.

"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2015-07-15"
__version__ = "Revision:"
__all__ = (
    'frexp10'     , ## similar to math.frexp but with radix=10
    'round_N'     , ## round floaing value to N-significant digits
    'pdg_round'   , ## round value,error-pair according to PDG prescription
    'pdg_format'  , ## format value&error according to PDF prescription
    'pdg_format2' , ## format value&error1&error2 structure accoring to PDF
    )
# ===============================================================================
import ROOT,          math
from   LHCbMath.Types import cpp 
VE = cpp.Gaudi.Math.ValueWithError
# =============================================================================

# =============================================================================
## round to nearest integer, rounds half integers to nearest even integer 
#  It is just a simple wrapper around boost::numeric::converter 
#  @see LHCb::Math::round 
cpp_round = cpp.LHCb.Math.round


# =============================================================================
## get the exponent and mantissa for radix10
#  similar for frexp, but use radix=10
#  @code
#  m,e = frexp10 ( value ) 
#  @endcode 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2015-07-20
#  @todo replace with proper math-calcualtions, withour trick with string formatting
def frexp10 ( value , precision = 12 ) :
    """
    Get the exponent and mantissa for radix10
    (similar for frexp, but use radix=10)
    
    >>> a,b = frexp10 ( value ) 
    """
    #
    value = float ( value )
    #
    ## adjust the precision 
    if not isinstance ( precision , ( int , long ) ) or 0 >= precision  : precision = 12
    #
    fmt = "%%.%de" % precision
    rep = ( fmt %  value ) .split('e')
    #
    return float ( rep[0] ) , int ( rep[1] )

    
# =============================================================================
## get three significant digits from the floating value
#  @code
#  nums = _3digits_ ( value ) 
#  @endcode 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2015-07-20
def _3digits_ ( value ) :
    """
    Get three first significant digits
    
    >>> nums = _3digits_ ( value ) 
    """
    #
    if abs ( value ) <   1.0 : value  = frexp10 ( value ) [0] 
    if abs ( value ) >= 10.0 : value  = frexp10 ( value ) [0]
    #
    return int ( round ( float ( value ) * 100 , 0 ) )

## ============================================================================
#  round value to to n-digits
#  @code
#  new_value = round_N ( value , 3 )
#  @endcode
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2015-07-20
def round_N ( value , n ) :
    """
    Round value to to n-digits
    
    >>> new_value = round_N ( value , 3 )    
    """
    m , e = frexp10 ( value )
    ##
    ni =  int ( n - 1 ) 
    f  = 10 ** ni  
    return 1.0 * cpp.Gaudi.Math.round ( 1.0 * m * f ) * 10 ** ( e - ni )

# ===============================================================================================
## make a rounding according to PDG prescription
#  @see http://pdg.lbl.gov/2010/reviews/rpp2010-rev-rpp-intro.pdf
#  @see section 5.3 of doi:10.1088/0954-3899/33/1/001
#  
#  The basic rule states that if the three highest order digits of the error
#  lie between 100 and 354, we round to two significant digits. If they lie between
#  355 and 949, we round to one significant digit. Finally,
#   if they lie between 950 and 999, we round up to 1000 and keep two significant digits.
#  In all cases, the central value is given with a precision that matches that of the error.
#
#  @code
#  v,e,f = pdg_round_ ( value , error )
#  print ' Rounded value +/- error is  (%s +/- %s)' % ( v , e )
#  @endcode
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2015-07-20 
def pdg_round_ ( value , error ) :
    """
    Make a rounding according to PDG prescription
    @see http://pdg.lbl.gov/2010/reviews/rpp2010-rev-rpp-intro.pdf
    @see section 5.3 of doi:10.1088/0954-3899/33/1/001
    
    Quote: 
    The basic rule states that if the three highest order digits of the error
    lie between 100 and 354, we round to two significant digits. If they lie between
    355 and 949, we round to one significant digit. Finally,
    if they lie between 950 and 999, we round up to 1000 and keep two significant digits.
    In all cases, the central value is given with a precision that matches that of the error.
    
    >>> v,e,f = pdg_round_ ( value , error )    
    >>> print ' Rounded value +/- error is  (%s +/- %s)' % ( v , e )
    """
    ##
    ne = _3digits_ ( error )
    ##
    if   abs ( ne ) <= 354 : pr,pe = 2,2
    elif abs ( ne ) <= 949 : pr,pe = 1,1
    else                   : pr,pe = 2,1
    ##
    ev , bv = frexp10 ( value )
    ee , be = frexp10 ( error )
    ##
    e = round_N ( error , pe )
    ##
    q  = be + 1 - pe 
    r  = 10**q
    v  = cpp.Gaudi.Math.round ( value / r ) * r
    ##
    return v , e , q 

# =============================================================================
## make a rounding according to PDG prescription
#  @see http://pdg.lbl.gov/2010/reviews/rpp2010-rev-rpp-intro.pdf
#  @see section 5.3 of doi:10.1088/0954-3899/33/1/001
#  
#  The basic rule states that if the three highest order digits of the error
#  lie between 100 and 354, we round to two significant digits. If they lie between
#  355 and 949, we round to one significant digit. Finally,
#   if they lie between 950 and 999, we round up to 1000 and keep two significant digits.
#  In all cases, the central value is given with a precision that matches that of the error.
#
#  @code
#  v,e = pdg_round ( value , error )
#  print ' Rounded value +/- error is  (%s +/- %s)' % ( v , e )
#  @endcode
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2015-07-20 
def pdg_round ( value , error ) :
    """
    Make a rounding according to PDG prescription
    @see http://pdg.lbl.gov/2010/reviews/rpp2010-rev-rpp-intro.pdf
    @see section 5.3 of doi:10.1088/0954-3899/33/1/001
    
    Quote: 
    The basic rule states that if the three highest order digits of the error
    lie between 100 and 354, we round to two significant digits. If they lie between
    355 and 949, we round to one significant digit. Finally,
    if they lie between 950 and 999, we round up to 1000 and keep two significant digits.
    In all cases, the central value is given with a precision that matches that of the error.
    
    >>> v,e = pdg_round ( value , error )    
    >>> print ' Rounded value +/- error is  (%s +/- %s)' % ( v , e )
    """
    ##
    v,e,f = pdg_round_ ( value , error )
    ##
    return v, e 

# =============================================================================
## Round value/error accoridng to PDG prescription and format it for print
#  @see http://pdg.lbl.gov/2010/reviews/rpp2010-rev-rpp-intro.pdf
#  @see section 5.3 of doi:10.1088/0954-3899/33/1/001
#  
#  The basic rule states that if the three highest order digits of the error
#  lie between 100 and 354, we round to two significant digits. If they lie between
#  355 and 949, we round to one significant digit. Finally,
#   if they lie between 950 and 999, we round up to 1000 and keep two significant digits.
#  In all cases, the central value is given with a precision that matches that of the error.
#
#  @code
#  print ' Rounded value/error is %s ' % pdg_format ( value , error , True ) 
#  @endcode
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2015-07-20 
def pdg_format ( value , error , latex = False ) :
    """
    Round value/error accoridng to PDG prescription and format it for print
    @see http://pdg.lbl.gov/2010/reviews/rpp2010-rev-rpp-intro.pdf
    @see section 5.3 of doi:10.1088/0954-3899/33/1/001
      
    Quote:
    The basic rule states that if the three highest order digits of the error
    lie between 100 and 354, we round to two significant digits. If they lie between
    355 and 949, we round to one significant digit. Finally,
    if they lie between 950 and 999, we round up to 1000 and keep two significant digits.
    In all cases, the central value is given with a precision that matches that of the error.
    
    >>> value, error = ...
    >>> print ' Rounded value/error is %s ' % pdg_format ( value , error , True ) 
    """

    ## round value, erorr and get the scaling factor
    v , e , q = pdg_round_  ( value , error )
    
    ## get scaling factors for printout 
    qv , bv   = frexp10 (  value  )
    n         = int     ( math.floor ( bv / 3. ) ) 
    
    
    q = abs ( min ( 0 , q ) ) 
    
    if 0 != n :
        
        q +=  3 * n 
        
        if latex : fmt = "$(%%.%df \pm %%.%df)\cdot10^{%%d}$" % ( q , q )
        else     : fmt = "$(%%.%df +/- %%.%df)*10^{%%d}"      % ( q , q )
        
        v  /= 10**(3*n)
        e  /= 10**(3*n)
        ##
        return fmt % ( v , e , n*3 )
        
    else :
            
        if latex : fmt = "$(%%.%df \pm %%.%df)$" % ( q , q )
        else     : fmt = "$(%%.%df +/- %%.%df)"  % ( q , q )
        ##
        return fmt % ( v , e )


# =============================================================================
## Round value/error accoridng to PDG prescription and format it for print
#  @see http://pdg.lbl.gov/2010/reviews/rpp2010-rev-rpp-intro.pdf
#  @see section 5.3 of doi:10.1088/0954-3899/33/1/001
#  
#  The basic rule states that if the three highest order digits of the error
#  lie between 100 and 354, we round to two significant digits. If they lie between
#  355 and 949, we round to one significant digit. Finally,
#   if they lie between 950 and 999, we round up to 1000 and keep two significant digits.
#  In all cases, the central value is given with a precision that matches that of the error.
#
#  @code
#  print ' Rounded value/error is %s ' % pdg_format2 ( value , error , error2 , True ) 
#  @endcode
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2015-07-20 
def pdg_format2( value , error1 , error2  , latex = False ) :
    """
    Round value/error accoridng to PDG prescription and format it for print
    @see http://pdg.lbl.gov/2010/reviews/rpp2010-rev-rpp-intro.pdf
    @see section 5.3 of doi:10.1088/0954-3899/33/1/001
      
    Quote:
    The basic rule states that if the three highest order digits of the error
    lie between 100 and 354, we round to two significant digits. If they lie between
    355 and 949, we round to one significant digit. Finally,
    if they lie between 950 and 999, we round up to 1000 and keep two significant digits.
    In all cases, the central value is given with a precision that matches that of the error.
    
    >>> value, error1, error2 = ...
    >>> print ' Rounded value/error is %s ' % pdg_format2 ( value , error1 , error2 , True ) 
    """

    error = min ( abs ( error1 ) , abs ( error2 ) )
    
    ## round value, error and get the scaling factor
    v  , e  , q  = pdg_round_  ( value , error )

    v_ , e1 , q_ = pdg_round_  ( value , error1 )
    v_ , e2 , q_ = pdg_round_  ( value , error2 )

    ## get scaling factors for printout 
    qv , bv   = frexp10 (  value  )
    n         = int     ( math.floor ( bv / 3. ) ) 
    
    
    q = abs ( min ( 0 , q ) ) 
    
    if 0 != n :
        
        q +=  3 * n 
        
        if latex : fmt = "$(%%.%df \pm %%.%df \pm %%.%df)\cdot10^{%%d}$" % ( q , q , q )
        else     : fmt = "$(%%.%df +/- %%.%df +/- %%.%df)*10^{%%d}"      % ( q , q , q )

        v  /= 10**(3*n)
        e1 /= 10**(3*n)
        e2 /= 10**(3*n)
        ##
        return fmt % ( v , e1 , e2 , n*3 )
        
    else :
            
        if latex : fmt = "$(%%.%df \pm %%.%df \pm %%.%df)$" % ( q , q , q )
        else     : fmt = "$(%%.%df +/- %%.%df +/- %%.%df)"  % ( q , q , q )
        ##
        return fmt % ( v , e1 , e2 )


# ====================================================================================
## make a rounding according to PDG prescription
#  @see http://pdg.lbl.gov/2010/reviews/rpp2010-rev-rpp-intro.pdf
#  @see section 5.3 of doi:10.1088/0954-3899/33/1/001
#  
#  The basic rule states that if the three highest order digits of the error
#  lie between 100 and 354, we round to two significant digits. If they lie between
#  355 and 949, we round to one significant digit. Finally,
#   if they lie between 950 and 999, we round up to 1000 and keep two significant digits.
#  In all cases, the central value is given with a precision that matches that of the error.
#
#  @code
#  ve  = VE( ...
#  vr  = ve.pdg()
#  print ' Rounded value with error is  %s ' % vr 
#  @endcode
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2015-07-20 
def _ve_pdg_ ( ve ) :
    """
    Make a rounding according to PDG prescription
    @see http://pdg.lbl.gov/2010/reviews/rpp2010-rev-rpp-intro.pdf
    @see section 5.3 of doi:10.1088/0954-3899/33/1/001
      
    Quote:
    The basic rule states that if the three highest order digits of the error
    lie between 100 and 354, we round to two significant digits. If they lie between
    355 and 949, we round to one significant digit. Finally,
    if they lie between 950 and 999, we round up to 1000 and keep two significant digits.
    In all cases, the central value is given with a precision that matches that of the error.

    >>> ve  = VE( ...
    >>> vr  = ve.pdg()
    >>> print ' Rounded value with error is  %s ' % vr
    """
    #
    v , e  = pdg_round  ( ve.value() , ve.error() )
    # 
    return VE ( v , e * e )

# =============================================================================
## Round value/error accoridng to PDG prescription and format it for print
#  @see http://pdg.lbl.gov/2010/reviews/rpp2010-rev-rpp-intro.pdf
#  @see section 5.3 of doi:10.1088/0954-3899/33/1/001
#  
#  The basic rule states that if the three highest order digits of the error
#  lie between 100 and 354, we round to two significant digits. If they lie between
#  355 and 949, we round to one significant digit. Finally,
#   if they lie between 950 and 999, we round up to 1000 and keep two significant digits.
#  In all cases, the central value is given with a precision that matches that of the error.
#
#  @code
#  ve = VE( ... ) 
#  print ' Rounded value/error is %s ' % ve.pdg_format ()
#  @endcode
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2015-07-20 
def _ve_pdg_format_ ( ve , latex = False ) :
    """
    Round value/error accoridng to PDG prescription and format it for print
    @see http://pdg.lbl.gov/2010/reviews/rpp2010-rev-rpp-intro.pdf
    @see section 5.3 of doi:10.1088/0954-3899/33/1/001
      
    Quote:
    The basic rule states that if the three highest order digits of the error
    lie between 100 and 354, we round to two significant digits. If they lie between
    355 and 949, we round to one significant digit. Finally,
    if they lie between 950 and 999, we round up to 1000 and keep two significant digits.
    In all cases, the central value is given with a precision that matches that of the error.
    
    >>> ve = VE(... ) 
    >>> print ' Rounded value/error is %s ' % ve.pdg_format ()
    """
    return pdg_format ( ve.value() , ve.error() , latex ) 


# =============================================================================
## finally decorate class ValueWith Error
# =============================================================================

VE.pdg        = _ve_pdg_
VE.pdg_format = _ve_pdg_format_ 
    
# =============================================================================
## insert it to math
if not hasattr ( math , 'frexp10' ) : math.frexp10 = frexp10
if not hasattr ( math , 'round_N' ) : math.round_N = round_N


if '__main__' == __name__ :
    
    v = 1./3 
    for s in ( 1.e-6 , 1.e-3 , 1.e3 , 1.e6 ) : 
        for e in ( 0.000123 , 0.000456 , 0.000900 , 0.000986 ) :
            ve = VE ( 1./3 , e*e ) * s 
            print ' Value %s,\tround: %s,\tTeX: %s' % ( ve , ve.pdg() , ve.pdg_format( True ) ) 

    print pdg_format2(  1./3 , 0.000012345 , 0.00500      )
    print pdg_format2(  1./3 , 0.0050      , 0.000012345  )
    print pdg_format2(  1./3 , 0.0020      , 0.0040       )
    print pdg_format2(  1./3 , 0.0099      , 0.0040       )
    print pdg_format2(  1./3 , 0.0099      , 0.0020       )
# =============================================================================
# The END 
# =============================================================================
