#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id:$
# =============================================================================
## @file
#  Module with utilities for specific comparison of histograms/functions/shapes 
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-05-10
#  
#                    $Revision: 172094 $
#  Last modification $Date: 2014-04-29 16:02:06 +0200 (Tue, 29 Apr 2014) $
#  by                $Author: albarano $
# =============================================================================
"""
Module with utilities for specific comparion of histograms/functions/shapes 
"""
# =============================================================================
__version__ = "$Revision: 172094 $"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2011-06-07"
__all__     = () 
# =============================================================================
import ROOT             ## attention here!!
import PyPAW.HParamDeco 
from   PyPAW.PyRoUts     import hID,VE 
# 
# =============================================================================
# logging 
# =============================================================================
from AnalysisPython.Logger import getLogger 
logger = getLogger( __name__ )
# =============================================================================
logger.info ( 'Some specific comparison of histo-objects')
# =============================================================================
## Can 1D-histogram can be considered as ``constant'' ?  
def _h1_constant_ ( h1 , mn , mx , prob = 0.90 , delta = 0.001 ) :
    """
    Can  1D-historgam be considered as constant ? 
    """
    # 
    if not isinstance ( h1 , ( ROOT.TH1D , ROOT.TH1F ) ) : return False 
    #
    r  = h1.Fit ( 'pol0','S0Q' )
    if 0 != r.Status() : 
        logger.warning("Can't fit with constant function %s" % r.Status() )
        return False
    #
    for i in range ( 0 , 5 ) :
        ##
        v   = r[0]
        if  0 != v.value() :
            logger.warning("Can't fit with non-zero constant function" )
            return False
        ##
        h1 /= v
        r   = h1.Fit ( 'pol0','S0Q' )
        if 0 != r.Status() : 
            logger.warning("Can't fit with constant function %s" % r.Status() )
            return False
        ## compare!
        v = r[0]
        if abs ( v - 1.0 ) < delta and r.Prob() >= prob : return r
        
    return False

ROOT.TH1D.is_constant = _h1_constant_
ROOT.TH1F.is_constant = _h1_constant_


# =============================================================================
## compare the 1D-historgams trying to fit one with other
def _h1_cmp_fit_ ( h1              ,
                   h2              ,
                   rescale = False ,  
                   spline  = True  ,
                   opts    = ''    ) :

    """
    Compare histograms by refit of the first with functions,
    extracted from the second one

    >>> h1 = ... ## the first histo
    >>> h2 = ... ## the second histo
    >>> r  = h1.cmp_fit ( h2 )
    >>> if r : print r.Prob()
    
    """
    
    if rescale :
        h1 = h1.rescale_bins ( 1.0 ) 
        h2 = h2.rescale_bins ( 1.0 )

    f2 = h2.asTF ( spline = spline ) 
    f2.ReleaseParameter ( 0 ) 

    rf = h1.Fit ( f2 , 'S0Q' + opts ) 

    if 0 != rf.Status() :
        logger.warning("Can't fit with function " % rf.Status() )
        return None 
    
    return rf

ROOT.TH1D.cmp_fit = _h1_cmp_fit_
ROOT.TH1F.cmp_fit = _h1_cmp_fit_ 

# =============================================================================
## compare the 1D-historgams (as functions)
#  calculate
# \f$cos \theta = \frac{ f_1 \cdot f_2 } { \left|f_1\right|\left|f_2\right| }\f$
#  
def _h1_cmp_costheta_ ( h1              ,
                        h2              ,
                        rescale = False ,  
                        spline  = True  ) :
    """
    Compare the 1D-historgams (as functions)
    Calculate scalar product and get the angle form it
    
    >>> h1 = ... ## the first histo
    >>> h2 = ... ## the second histo
    >>> cos_theta  = h1.cmp_costheta ( h2 )
    
    """
    if rescale :
        h1 = h1.rescale_bins ( 1.0 )
        h2 = h2.rescale_bins ( 1.0 )

    if spline :
        f1 = h1.asSpline ()
        f2 = h2.asSpline ()
    else :
        f1 = h1.asFunc   ()
        f2 = h2.asFunc   ()

    from scipy.integrate import quad

    lims = h1.xminmax()
    
    r1   = quad ( lambda x : f1( x )**2    , lims[0] , lims[1] , limit = 200 )
    r2   = quad ( lambda x : f2( x )**2    , lims[0] , lims[1] , limit = 200 )
    r12  = quad ( lambda x : f1( x )*f2(x) , lims[0] , lims[1] , limit = 200 )
    
    from math import sqrt

    vr1   = VE ( r1 [0] , r1 [1]**2 )
    vr2   = VE ( r2 [0] , r2 [1]**2 )
    vr12  = VE ( r12[0] , r12[1]**2 )
    
    return vr12 / ( vr1 * vr2 ) ** 0.5 

ROOT.TH1D.cmp_cos = _h1_cmp_costheta_
ROOT.TH1F.cmp_cos = _h1_cmp_costheta_ 

# =============================================================================
## calculate the norm of difference of scaled histograms/functions 
#  \f$ d = \lef| f_1^{*} - f_2^{*}\right| \f$,
#  where \f$ f^* \f$-are scaled functions, such \f$ |left| f^*\right| = 1 \f$ 
def _h1_cmp_dist_ ( h1              ,
                    h2              ,
                    rescale = False ,  
                    spline  = True  ) :
    """
    Calculate the norm of difference of scaled histograms/functions 
    |f1-f2|, such |f1|=1 and |f2|=1
    
    >>> h1 = ... ## the first histo
    >>> h2 = ... ## the second histo
    >>> diff = h1.cmp_dist ( h2 )
    
    """
    if rescale :
        h1 = h1.rescale_bins ( 1.0 )
        h2 = h2.rescale_bins ( 1.0 )

    if spline :
        f1 = h1.asSpline ()
        f2 = h2.asSpline ()
    else :
        f1 = h1.asFunc   ()
        f2 = h2.asFunc   ()
        
    from scipy.integrate import quad

    lims = h1.xminmax()
    
    r1   = quad ( lambda x : f1 ( x )**2    , lims[0] , lims[1] , limit = 200 )
    r2   = quad ( lambda x : f2 ( x )**2    , lims[0] , lims[1] , limit = 200 )

    from math import sqrt

    sf1 = 1.0 / sqrt ( r1 [0] )
    sf2 = 1.0 / sqrt ( r2 [0] )

    d12  = quad ( lambda x : (sf1*f1(x)-sf2*f2(x))**2 , lims[0] , lims[1] , limit = 200 )

    
    return VE( d12[0] , d12[1]**2)


ROOT.TH1D.cmp_dist = _h1_cmp_dist_
ROOT.TH1F.cmp_dist = _h1_cmp_dist_ 

# =============================================================================
## calculate and print some statistic for comparison  
def _h1_cmp_prnt_ ( h1              ,
                    h2              ,
                    title   = ''    ) :
    """
    Calculate and print some statistic information for two histos 
    """
    
    logger.info ( ' %15s | -MEAN-     | %s | %s ' %
                  ( title    ,
                    h1  .mean     ().toString ('%+8.4g+-%-8.4g ') ,
                    h2  .mean     ().toString ('%+8.4g+-%-8.4g ') ) ) 
    logger.info ( ' %15s | -RMS-      | %s | %s ' %
                  ( title    ,
                    h1  .rms      ().toString ('%+8.4g+-%-8.4g ') ,
                    h2  .rms      ().toString ('%+8.4g+-%-8.4g ') ) )
    logger.info ( ' %15s | -SKEWNESS- | %s | %s ' %
                  ( title    ,
                    h1  .skewness ().toString ('%+8.4g+-%-8.4g ') ,
                    h2  .skewness ().toString ('%+8.4g+-%-8.4g ') ) )
    logger.info ( ' %15s | -KURTOSIS- | %s | %s ' %
                  ( title    ,
                    h1  .kurtosis ().toString ('%+8.4g+-%-8.4g ') ,
                    h2  .kurtosis ().toString ('%+8.4g+-%-8.4g ') ) )

    
ROOT.TH1D.cmp_prnt = _h1_cmp_prnt_
ROOT.TH1F.cmp_prnt = _h1_cmp_prnt_ 

# =============================================================================
if '__main__' == __name__ :
    
    print 80*'*'
    print __doc__
    print ' Author  : ' , __author__
    print ' Version : ' , __version__
    print ' Date    : ' , __date__    
    print ' Symbols : ' , __all__    
    print 80*'*'
    
# =============================================================================
# The END 
# =============================================================================
