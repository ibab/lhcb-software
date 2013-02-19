#!/usr/bin/env python
# =============================================================================
# $Id$
# =============================================================================
## @file
#  Module with decoration of some ROOT objects for efficient use in PyROOT
#
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
#
#  
#                    $Revision$
#  Last modification $Date$
#  by                $Author$
# =============================================================================
"""

Module with decoration of some ROOT objects for efficient use in PyROOT

Many native  root classes are equipped with new useful methods and operators,
in particular TH1(x) , TH2(x) , TAxis, TGraph(Errors), etc...

see also GaudiPython.HistoUtils 
"""
# =============================================================================
__version__ = "$Revision$"
__author__  = "Vanya BELYAEV Ivan.Belyaev@cern.ch"
__date__    = "2011-06-07"
# =============================================================================
__all__     = (
    #
    'rootID'         , ## construct the (global) unique ROOT identifier
    'funcID'         , ## construct the (global) unique ROOT identifier
    'funID'          , ## construct the (global) unique ROOT identifier
    'hID'            , ## construct the (global) unique ROOT identifier
    'histoID'        , ## construct the (global) unique ROOT identifier
    #
    'VE'             , ## Gaudi::Math::ValueWithError
    'ValueWithError' , ## Gaudi::Math::ValueWithError
    'histoGuess'     , ## guess the simple histo parameters
    'useLL'          , ## use LL for histogram fit?
    'allInts'        , ## natural histogram with natural entries?
    #
    'binomEff'       , ## calculate binomial efficiency 
    'binomEff_h1'    , ## calculate binomial efficiency for 1D-histos
    'binomEff_h2'    , ## calculate binomial efficiency for 2D-ihstos
    'binomEff_h3'    , ## calculate binomial efficiency for 3D-ihstos
    #
    'makeGraph'      , ## make ROOT Graph from input data
    'h2_axes'        , ## book 2D-histogram from axes
    'h1_axis'        , ## book 1D-histogram from axis 
    'axis_bins'      , ## convert list of bin edges to axis
    've_adjust'      , ## adjust the efficiency to be in physical range
    #
    # from Utils
    'memory'         , 
    'clocks'         , 
    'timing'         , 
    'mute'           , 
    #
    )
# =============================================================================
import ROOT
from   GaudiPython.Bindings   import gbl as cpp
import LHCbMath.Types
Gaudi          = cpp.Gaudi
VE             = Gaudi.Math.ValueWithError
SE             = cpp.StatEntity 
ValueWithError = Gaudi.Math.ValueWithError
binomEff       = Gaudi.Math.binomEff
import math
# =============================================================================
# logging 
# =============================================================================
from AnalysisPython.Logger import getLogger 
logger = getLogger( __name__ )
# =============================================================================
logger.info ( 'Zillions of decorations for ROOT   objects')
## ============================================================================
# global identifier for ROOT objects 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def rootID ( prefix = 'o_') :
    """
    Construct the unique ROOT-id 
    """
    _fun = lambda i : prefix + '%d'% i
    
    _root_ID = 1000
    
    _id = _fun ( _root_ID ) 
    while ROOT.gROOT.FindObject ( _id ) :
        _root_ID += 10 
        _id = _fun ( _root_ID ) 

    return _id                 ## RETURN
# =============================================================================
## global ROOT identified for function obejcts 
def funcID  () : return rootID  ( 'f_' )
## global ROOT identified for function obejcts 
def funID   () : return funcID  ( )
## global ROOT identified for function obejcts 
def fID     () : return funcID  ( )
## global ROOT identified for histogram objects 
def histoID () : return rootID  ( 'h_' )
## global ROOT identified for histogram objects 
def histID  () : return histoID ( )
## global ROOT identified for histogram objects 
def hID     () : return histoID ( )


# =============================================================================
# temporary trick, to be removed 
# =============================================================================

SE.__repr__ = lambda s : 'Stat: '+ s.toString()
SE.__str__  = lambda s : 'Stat: '+ s.toString()

# =============================================================================
def _int ( ve , precision = 1.e-5 ) :
    #
    if isinstance  ( ve , ( int , long ) ) : return True
    #
    if isinstance  ( ve , float ) :
        if Gaudi.Math.isint ( ve ) or Gaudi.Math.islong ( ve ) : return True 
        
    if not hasattr ( ve , 'value' ) :
        return _int ( VE ( ve , abs ( ve ) ) , precision )  
    #
    diff = max ( 1 , abs ( ve.value() ) ) * precision 
    diff = min ( 0.1 , diff ) 
    # 
    if abs ( ve.value () - long ( ve.value () ) ) > diff : return False 
    if abs ( ve.value () -        ve.cov2  ()   ) > diff : return False
    #
    return True 

# =============================================================================
## get the B/S estimate from the formula 
#  \f$ \sigma  = \fras{1}{S}\sqrt{1+\frac{B}{S}}\f$
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2012-10-15
def _b2s_ ( s )  :
    """
    Get B/S estimate from the equation:
    
       error(S) = 1/sqrt(S) * sqrt ( 1 + B/S)

    >>> v = ...
    >>> b2s = v.b2s() ## get B/S estimate
    
    """
    #
    v = s.value () 
    c = s.cov2  ()
    #
    if v <= 0  or c <= 0 : return -1
    #
    return c/v - 1

# =============================================================================
## get the precision 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2012-10-15
def _prec_ ( s )  :
    """
    Get precision 

    >>> v = ...
    >>> p = v.precision() 
    
    """
    if not hasattr ( s , 'value' ) :
        return _prec_ ( VE ( s , 0 ) )
    #
    c =       s.error ()
    v = abs ( s.value () ) 
    #
    if     c <  0 or v == 0  : return -1
    elif   c == 0            : return  0
    #
    return c/v


VE . b2s       = _b2s_
VE . prec      = _prec_
VE . precision = _prec_


# =============================================================================
# Decorate histogram axis and iterators 
# =============================================================================

# =============================================================================
## iterator for histogram  axis 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _axis_iter_1_ ( a ) :
    """
    Iterator for axis

    >>> axis = ...
    >>> for i in axis : 
    """
    i = 1
    s = a.GetNbins()
    while i <= s :
        yield i
        i+=1        

ROOT.TAxis . __iter__ = _axis_iter_1_

# =============================================================================
## get item for the 1-D histogram 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h1_get_item_ ( h1 , ibin ) :
    """
    ``Get-item'' for the 1D-histogram :
    
    >>> histo = ...
    >>> ve    = histo[ibin]
    
    """
    #
    if not ibin in h1.GetXaxis() : raise IndexError 
    #
    val = h1.GetBinContent ( ibin ) 
    err = h1.GetBinError   ( ibin )
    #
    return VE ( val , err * err )

# ==========================================================================
## get item for the 2D histogram 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h1_set_item_ ( h1 , ibin , v ) :
    """
    ``Set-item'' for the 1D-histogram :
    
    >>> histo[ibin] = value 
    
    """
    #
    if   isinstance ( v , ( int , long ) ) :
        
        if   0  < v   : return _h1_set_item_ ( h1 , ibin , VE   ( v , v ) )
        elif 0 == v   : return _h1_set_item_ ( h1 , ibin , VE   ( v , 1 ) )
        else          : return _h1_set_item_ ( h1 , ibin , VE   ( v , 0 ) )
        
    elif isinstance ( v , float ) :
        
        if _int ( v ) : return _h1_set_item_ ( h1 , ibin , long ( v     ) )
        else          : return _h1_set_item_ ( h1 , ibin , VE   ( v , 0 ) )

    ## check the validity of the bin 
    if not ibin in h1 : raise IndexError 
    #
    h1.SetBinContent ( ibin , v.value () )
    h1.SetBinError   ( ibin , v.error () )
    
ROOT.TH1F. __setitem__ = _h1_set_item_
ROOT.TH1D. __setitem__ = _h1_set_item_

# ==========================================================================
## get item for the 2D histogram 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h2_set_item_ ( h2 , ibin , v ) :
    """
    ``Set-item'' for the 2D-histogram :
    
    >>> histo[(ix,iy)] = value 
    
    """
    #
    if   isinstance ( v , ( int , long ) ) :
        
        if   0  < v   : return _h2_set_item_ ( h2 , ibin , VE ( v , v ) )
        elif 0 == v   : return _h2_set_item_ ( h2 , ibin , VE ( v , 1 ) )
        else          : return _h2_set_item_ ( h2 , ibin , VE ( v , 0 ) )
        
    elif isinstance ( v , float ) :
        
        if _int ( v ) : return _h2_set_item_ ( h2 , ibin , long ( v ) )
        else          : return _h2_set_item_ ( h2 , ibin , VE ( v , 0 ) )

    ## check the validity of the bin 
    if not ibin in h2 : raise IndexError 
    #
    h2.SetBinContent ( ibin[0] , ibin[1] , v.value () )
    h2.SetBinError   ( ibin[0] , ibin[1] , v.error () )
    
ROOT.TH2F. __setitem__ = _h2_set_item_
ROOT.TH2D. __setitem__ = _h2_set_item_


# ==========================================================================
## get item for the 3D histogram 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h3_set_item_ ( h3 , ibin , v ) :
    """
    ``Set-item'' for the 3D-histogram :
    
    >>> histo[(ix,iy,iz)] = value 
    
    """
    #
    if   isinstance ( v , ( int , long ) ) :
        
        if   0  < v   : return _h3_set_item_ ( h3 , ibin , VE ( v , v ) )
        elif 0 == v   : return _h3_set_item_ ( h3 , ibin , VE ( v , 1 ) )
        else          : return _h3_set_item_ ( h3 , ibin , VE ( v , 0 ) )
        
    elif isinstance ( v , float ) :
        
        if _int ( v ) : return _h3_set_item_ ( h3 , ibin , long ( v ) )
        else          : return _h3_set_item_ ( h3 , ibin , VE ( v , 0 ) )

    ## check the validity of the bin 
    if not ibin in h3 : raise IndexError 
    #
    h3.SetBinContent ( ibin[0] , ibin[1] , ibin[2] , v.value () )
    h3.SetBinError   ( ibin[0] , ibin[1] , ibin[2] , v.error () )
    
ROOT.TH3F. __setitem__ = _h3_set_item_
ROOT.TH3D. __setitem__ = _h3_set_item_

# =============================================================================
## get item for the 2D histogram 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h2_get_item_ ( h2 , ibin ) :
    """
    ``Get-item'' for the 2D-histogram :
    
    >>> histo = ...
    >>> ve    = histo[ (ix,iy) ]
    
    """
    #
    if not ibin[0] in h2.GetXaxis() : raise IndexError 
    if not ibin[1] in h2.GetYaxis() : raise IndexError 
    #
    val = h2.GetBinContent ( ibin[0] , ibin[1] )  
    err = h2.GetBinError   ( ibin[0] , ibin[1] ) 
    #
    return VE ( val , err * err ) 

# =============================================================================
## get item for the 3D histogram 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h3_get_item_ ( h3 , ibin ) :
    """
    ``Get-item'' for the 2D-histogram :
    
    >>> histo = ...
    >>> ve    = histo[ (ix,iy,iz) ]
    
    """
    #
    if not ibin[0] in h3.GetXaxis() : raise IndexError 
    if not ibin[1] in h3.GetYaxis() : raise IndexError 
    if not ibin[2] in h3.GetZaxis() : raise IndexError 
    #
    val = h3.GetBinContent ( ibin[0] , ibin[1] , ibin[2] )   
    err = h3.GetBinError   ( ibin[0] , ibin[1] , ibin[2] ) 
    #
    return VE ( val , err * err ) 

# =============================================================================
## iterator for 1D-histogram 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h1_iter_ ( h1 ) :
    """
    Iterator over 1D-histogram
    
    >>> for i in h1 : print i 
    """
    ax = h1.GetXaxis () 
    sx = ax.GetNbins ()
    for i in range ( 1 , sx + 1 ) : 
        yield i

ROOT.TH1  . __iter__ = _h1_iter_ 
ROOT.TH1F . __iter__ = _h1_iter_ 
ROOT.TH1D . __iter__ = _h1_iter_ 

# =============================================================================
## iterator for 2D-histogram 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h2_iter_ ( h2 ) :
    """
    Iterator over 2D-histogram
    
    >>> for i in h2 : print i 
    """
    #
    ax = h2.GetXaxis()
    ay = h2.GetYaxis()
    #
    sx = ax.GetNbins()
    sy = ay.GetNbins()
    #
    for ix in range ( 1 , sx + 1 ) : 
        for iy in range ( 1 , sy + 1 ) : 
            yield (ix,iy)


ROOT.TH2  . __iter__ = _h2_iter_ 
ROOT.TH2F . __iter__ = _h2_iter_ 
ROOT.TH2D . __iter__ = _h2_iter_ 

# =============================================================================
## iterator for 3D-histogram 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h3_iter_ ( h3 ) :
    """
    Iterator over 3D-histogram
    
    >>> for i in h3 : print i 
    """
    #
    ax = h3.GetXaxis()
    ay = h3.GetYaxis()
    az = h3.GetZaxis()
    #
    sx = ax.GetNbins()
    sy = ay.GetNbins()    
    sz = az.GetNbins()
    #
    for ix in range ( 1 , sx + 1 ) : 
        for iy in range ( 1 , sy + 1 ) : 
            for iz in range ( 1 , sz + 1 ) : 
                yield (ix,iy,iz)

                    
ROOT.TH3  . __iter__ = _h3_iter_ 
ROOT.TH3F . __iter__ = _h3_iter_ 
ROOT.TH3D . __iter__ = _h3_iter_ 

# =============================================================================
# interpolate 
# =============================================================================
## linear interpolation 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def interpolate_1D ( x       ,
                     x0 , v0 ,
                     x1 , v1 ) :
    """
    Linear interpolation 
    """
    if hasattr ( x  , 'value' ) : x  = x  . value()
    if hasattr ( x0 , 'value' ) : x0 = x0 . value()
    if hasattr ( x1 , 'value' ) : x1 = x1 . value()

    c1 = ( x - x0 ) / ( x1 - x0 )
    c0 = ( x - x1 ) / ( x0 - x1 )

    return c0 * v0 + c1 * v1

# ========================================================================
## bilinear interpolation 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def interpolate_2D ( x   , y   ,
                     x0  , x1  , 
                     y0  , y1  ,
                     v00 , v01 ,
                     v10 , v11 ) : 
    
    """
    bi-linear interpolation 
    """
    #
    if hasattr ( x  , 'value' ) : x  = x  . value()
    if hasattr ( y  , 'value' ) : y  = y  . value()
    #
    if hasattr ( x0 , 'value' ) : x0 = x0 . value()
    if hasattr ( x1 , 'value' ) : x1 = x1 . value()
    if hasattr ( y0 , 'value' ) : y0 = y0 . value()
    if hasattr ( y1 , 'value' ) : y1 = y1 . value()
    
    c00 =  ( x - x1 ) * ( y - y1 ) / ( x0 - x1 ) / ( y0 - y1 )
    c01 =  ( x - x1 ) * ( y - y0 ) / ( x0 - x1 ) / ( y1 - y0 )
    c10 =  ( x - x0 ) * ( y - y1 ) / ( x1 - x0 ) / ( y0 - y1 )
    c11 =  ( x - x0 ) * ( y - y0 ) / ( x1 - x0 ) / ( y1 - y0 )
    
    return c00 * v00 + c01 * v01 + c10 * v10 + c11 * v11 


# =============================================================================
## histogram as function 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h1_call_ ( h1 , x , func = lambda s : s , interpolate = True ) :
    """
    Histogram as function:
    
    >>> histo = ....
    >>> ve    = histo ( x ) 
    """
    #
    if hasattr ( x , 'value' )  : return _h1_call_ ( h1 ,  x.value() )
    #
    ax = h1.GetXaxis (   )
    ix = ax.FindBin  ( x )
    if not 1 <= ix <= ax.GetNbins () : return VE(0,0)
    #
    val = h1.GetBinContent ( ix ) 
    err = h1.GetBinError   ( ix )
    #
    value = VE ( val , err**2 )
    #
    if not interpolate : return func ( value )   ## RETURN 
    #
    ## make linear interpolation
    # 
    bc   = ax.GetBinCenter( ix )
    ibin = ix - 1 if x < bc else ix + 1
    #
    if ibin in h1  :
        #
        bx =      ax.GetBinCenter  ( ibin )
        bv = VE ( h1.GetBinContent ( ibin ) , h1.GetBinError ( ibin )**2 )
        #
        value = interpolate_1D (
            x  ,
            bc , value ,
            bx , bv    )
        #
    return func ( value )  ## RETURN 
        
ROOT.TH1F  . __getitem__  = _h1_get_item_
ROOT.TH1D  . __getitem__  = _h1_get_item_
ROOT.TH2F  . __getitem__  = _h2_get_item_
ROOT.TH2D  . __getitem__  = _h2_get_item_
ROOT.TH3F  . __getitem__  = _h3_get_item_
ROOT.TH3D  . __getitem__  = _h3_get_item_

ROOT.TH1   . __call__     = _h1_call_

ROOT.TH1   . __len__      = lambda s : s.size() 
ROOT.TH1   .   size       = lambda s : s.GetNbinsX() * s.GetNbinsY() * s.GetNbinsZ() 
ROOT.TH1   . __contains__ = lambda s , i : 1 <= i <= s.size() 

ROOT.TH2   . __len__      = lambda s : s.size() 
ROOT.TH2   .   size       = lambda s : s.GetNbinsX() * s.GetNbinsY() * s.GetNbinsZ()

ROOT.TH3   . __len__      = lambda s : s.size() 
ROOT.TH3   .   size       = lambda s : s.GetNbinsX() * s.GetNbinsY() * s.GetNbinsZ()

# =============================================================================
## check bin in 2D-histo 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h2_contains_ ( s , ibin ) :
    """
    Check if the bin contains in 3D-histogram:

    >>> (3,5) in h2
    
    """    
    return ibin[0] in s.GetXaxis() and ibin[1] in s.GetYaxis()


ROOT.TH2   . __contains__ = _h2_contains_
ROOT.TH2F  . __contains__ = _h2_contains_
ROOT.TH2D  . __contains__ = _h2_contains_

# ============================================================================
## check bin in 3D-histo 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h3_contains_ ( s , ibin ) :
    """
    Check if the bin contains in 3D-histogram:

    >>> (3,5,10) in h3
    
    """
    return ibin[0] in s.GetXaxis() and \
           ibin[1] in s.GetYaxis() and \
           ibin[2] in s.GetZaxis()


ROOT.TH3   . __contains__ = _h3_contains_
ROOT.TH3F  . __contains__ = _h3_contains_
ROOT.TH3D  . __contains__ = _h3_contains_


ROOT.TAxis . __contains__ = lambda s , i : 1 <= i <= s.GetNbins()


# =============================================================================
## find bin in 1D-histogram
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h1_find_ ( h1 , x ) :
    """
    Find the bin in 1D-histogram

    >>> ibin = h1.findBin ( x ) 
    """
    if hasattr ( x , 'value' ) : x = x.value()
    #
    ax = h1.GetXaxis()
    #
    return ax.FindBin ( x )
# =============================================================================
## find bin in 2D-histogram
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h2_find_ ( h2 , x , y ) :
    """
    Find the bin in 3D-histogram

    >>> ibin = h2.findBin ( x , y ) 
    """
    if hasattr ( x , 'value' ) : x = x.value()
    if hasattr ( y , 'value' ) : y = y.value()
    #
    ax = h2.GetXaxis()
    ay = h2.GetYaxis()
    #
    return  ( ax.FindBin ( x ) ,
              ay.FindBin ( y ) )
# =============================================================================
## find bin in 3D-histogram
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h3_find_ ( h3 , x , y , z ) :
    """
    Find the bin in 3D-histogram

    >>> ibin = h3.findBin ( x , y , z ) 
    """
    if hasattr ( x , 'value' ) : x = x.value()
    if hasattr ( y , 'value' ) : y = y.value()
    if hasattr ( z , 'value' ) : z = z.value()
    #
    ax = h3.GetXaxis()
    ay = h3.GetYaxis()
    az = h3.GetZaxis()
    #
    return  ( ax.FindBin ( x ) ,
              ay.FindBin ( y ) ,
              az.FindBin ( z ) )


ROOT.TH1F . findBin  = _h1_find_
ROOT.TH1D . findBin  = _h1_find_
ROOT.TH2F . findBin  = _h2_find_
ROOT.TH2D . findBin  = _h2_find_
ROOT.TH3F . findBin  = _h3_find_
ROOT.TH3D . findBin  = _h3_find_

# ============================================================================
## find the first X-value for the certain Y-value 
def _h1_find_X ( self             ,
                 v                ,
                 forward   = True ) : 
    
    ##
    if hasattr ( v , 'value' ) : v = v.value()
    ##
    mn , mx  = self.xminmax()
    ##
    if v < mn.value() :
        return hist.xmin() if forward else hist.xmax ()  
    if v > mx.value() :
        return hist.xmax() if forward else hist.xmin () 
    
    nb  = len ( hist ) 
    
    ax  = hist.GetXaxis()
    
    for i in hist :
        
        j  = i+1
        if not j in hist : continue
        
        ib = i if forward else nb + 1 - i 
        jb = j if forward else nb + 1 - j 
        
        vi = hist [ ib ].value () 
        vj = hist [ jb ].value ()
        
        if  vi <= v <= vj or vj <= v <= vi :
            
            xi = ax.GetBinCenter ( ib )
            xj = ax.GetBinCenter ( jb )
            
            if   vi == v             : return xi
            elif vj == v             : return xj 
            
            dv = vi - vj
            dx = xi - xj

            if   vi == vj or 0 == dv : return 0.5 * ( xi + xj ) 
            
            return (v*dx+vi*xj-vj*xi)/dv


# =============================================================================
## histogram as function 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h2_call_ ( h2 , x , y , func = lambda s : s , interpolate = True ) :
    """
    Histogram as function:
    
    >>> histo = ....
    >>> ve    = histo ( x , y ) 
    """    
    #
    if hasattr ( x , 'value' )  : return _h2_call_ ( h2 ,  x.value() , y          )
    if hasattr ( y , 'value' )  : return _h2_call_ ( h2 ,  x         , y.value () )
    #
    ax = h2.GetXaxis (   )
    ix = ax.FindBin  ( x )
    if not  1 <= ix <= ax.GetNbins() : return VE ( 0, 0 )
    #
    ay = h2.GetYaxis (   )
    iy = ay.FindBin  ( y )
    if not  1 <= iy <= ay.GetNbins() : return VE ( 0, 0 )
    ##
    val = h2.GetBinContent ( ix , iy ) 
    err = h2.GetBinError   ( ix , iy )
    #
    value = VE ( val , err*err )
    ##
    if not interpolate : return func ( value )
    ##
    bcx = ax.GetBinCenter ( ix )
    ibx = ix - 1 if x < bcx else ix + 1
    #
    bcy = ay.GetBinCenter ( iy )
    iby = iy - 1 if y < bcy else iy + 1
    #
    if ibx in ax and iby in ay :  ## regular interpolation
        
        bx = ax.GetBinCenter  ( ibx )
        by = ay.GetBinCenter  ( iby )

        value = interpolate_2D (
            x   , y  ,
            bcx , bx ,
            bcy , by ,
            ##
            value    , 
            VE ( h2.GetBinContent (  ix , iby ) , h2.GetBinError (  ix , iby )**2 ) ,
            VE ( h2.GetBinContent ( ibx ,  iy ) , h2.GetBinError ( ibx ,  iy )**2 ) ,
            VE ( h2.GetBinContent ( ibx , iby ) , h2.GetBinError ( ibx , iby )**2 ) )
        
    elif ibx in ax : ## interpolation in X-direction 

        bx = ax.GetBinCenter  ( ibx )
        
        value = interpolate_1D (
            x   ,
            bcx , value , 
            bx  ,
            VE ( h2.GetBinContent ( ibx ,  iy ) , h2.GetBinError ( ibx ,  iy )**2 )
            )

    elif iby in ay : ## interpolation in Y-direction 

        by = ay.GetBinCenter  ( iby )
        
        value = interpolate_1D (
            y   ,
            bcy , value , 
            by  ,
            VE ( h2.GetBinContent ( ix , iby ) , h2.GetBinError ( ix , iby )**2 )
            )

    
    return func ( value )


ROOT.TH2   . __call__     = _h2_call_
ROOT.TH2F  . __getitem__  = _h2_get_item_
ROOT.TH2D  . __getitem__  = _h2_get_item_

# =============================================================================
## histogram as function 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h3_call_ ( h3 , x , y , z , func = lambda s : s ) :
    """
    Histogram as function:
    
    >>> histo = ....
    >>> ve    = histo ( x , y , z ) 
    """    
    #
    if hasattr ( x , 'value' )  : return _h3_call_ ( h3 ,  x.value() , y          , z          )
    if hasattr ( y , 'value' )  : return _h3_call_ ( h3 ,  x         , y.value () , z          )
    if hasattr ( z , 'value' )  : return _h3_call_ ( h3 ,  x         , y          , z.value () )
    #
    ax = h3.GetXaxis (   )
    ix = ax.FindBin  ( x )
    if not  1 <= ix <= ax.GetNbins() : return VE ( 0, 0 )
    #
    ay = h3.GetYaxis (   )
    iy = ay.FindBin  ( y )
    if not  1 <= iy <= ay.GetNbins() : return VE ( 0, 0 )
    ##
    az = h3.GetZaxis (   )
    iz = az.FindBin  ( z )
    if not  1 <= iz <= az.GetNbins() : return VE ( 0, 0 )
    ##
    ##
    val = h3.GetBinContent ( ix , iy , iz ) 
    err = h3.GetBinError   ( ix , iy , iz )
    #
    return func ( VE ( val , err * err ) ) 

ROOT.TH3   . __call__     = _h3_call_
ROOT.TH3F  . __getitem__  = _h3_get_item_
ROOT.TH3D  . __getitem__  = _h3_get_item_

# =============================================================================
# iterate over items
# =============================================================================
## iterate over entries in 1D-histogram 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h1_iteritems_ ( h1 ) :
    """
    Iterate over histogram items:
    
    >>> h1 = ...
    >>> for i,x,y in h1.iteritems()  : ...
    
    """
    ax = h1.GetXaxis()
    sx = ax.GetNbins()

    for ix in range ( 1  , sx + 1 ) : 
        
        x   =       ax.GetBinCenter ( ix )
        xe  = 0.5 * ax.GetBinWidth  ( ix )
        
        y   =       h1.GetBinContent ( ix )
        ye  =       h1.GetBinError   ( ix )
        
        yield ix, VE(x,xe*xe) , VE ( y,ye*ye)
        

ROOT.TH1F  . iteritems     = _h1_iteritems_
ROOT.TH1D  . iteritems     = _h1_iteritems_

# =============================================================================
## iterate over entries in 2D-histogram 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h2_iteritems_ ( h2 ) :
    """
    Iterate over histogram items:
    
    >>> h2 = ...
    >>> for ix,iy,x,y,z in h2.iteritems() : ...
    
    """
    ax = h2.GetXaxis()
    sx = ax.GetNbins()

    ay = h2.GetYaxis()
    sy = ay.GetNbins()
    
    for ix in range( 1 , sx + 1 ) :  
        x   =       ax.GetBinCenter ( ix )
        xe  = 0.5 * ax.GetBinWidth  ( ix )
        #
        for iy in range ( 1 , sy + 1 ) : 
            #
            y   =       ay.GetBinCenter  ( iy      )
            ye  = 0.5 * ay.GetBinWidth   ( iy      )
            #
            z   =       h2.GetBinContent ( ix , iy )
            ze  =       h2.GetBinError   ( ix , iy )
            #
            yield ix, iy, VE(x,xe*xe) , VE ( y,ye*ye) , VE( z,ze*ze) 
            #
            
        
ROOT.TH2F  . iteritems     = _h2_iteritems_
ROOT.TH2D  . iteritems     = _h2_iteritems_

# =============================================================================
## iterate over entries in 3D-histogram 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h3_iteritems_ ( h3 ) :
    """
    Iterate over histogram items:
    
    >>> h3 = ...
    >>> for ix,iy,iz,x,y,z,V in h3 : ...
    
    """
    ax = h3.GetXaxis()
    sx = ax.GetNbins()

    ay = h3.GetYaxis()
    sy = ay.GetNbins()

    az = h3.GetZaxis()
    sz = ay.GetNbins()
    
    for ix in range( 1 , sx + 1 ) :
        #
        x   =       ax.GetBinCenter ( ix )
        xe  = 0.5 * ax.GetBinWidth  ( ix )
        #
        for iy in range ( 1 , sy + 1 ) : 
            #
            y   =       ay.GetBinCenter  ( iy      )
            ye  = 0.5 * ay.GetBinWidth   ( iy      )
            #
            for iz in range ( 1 , sz + 1 ) :
                #
                z   =       az.GetBinCenter  ( iz      )
                ze  = 0.5 * az.GetBinWidth   ( iz      )
                #
                
                v   =       h2.GetBinContent ( ix , iy , iz )
                ve  =       h2.GetBinError   ( ix , iy , iz )
                #
                yield ix, iy, iz , VE(x,xe*xe) , VE ( y,ye*ye) , VE( z,ze*ze) , VE( v, ve ) 
                #

            
ROOT.TH3F  . iteritems     = _h3_iteritems_
ROOT.TH3D  . iteritems     = _h3_iteritems_


# =============================================================================
## iterate over items in TAxis
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _a_iteritems_ ( axis ) :
    """
    Iterate over items in axis 
    
    >>> axis = ...
    >>> for ibin,low,center,high in axis.iteritems() : 
    
    """
    for i in axis :

        l = axis.GetBinLowEdge ( i )
        c = axis.GetBinCenter  ( i )
        u = axis.GetBinUpEdge  ( i )

        yield i,l,c,u

ROOT.TAxis. iteritems     = _a_iteritems_

# =============================================================================
# soem minor decoration for 2D-histos 
# =============================================================================

# =============================================================================
## transpose 2D-ihstogram 
def _h2_transpose_ ( h2 ) :
    """
    Transpose 2D-histogram

    >>> h2 = ...
    >>> ht = h2.T()
    
    """
    if hasattr ( h2 , 'GetSumw2' ) and not h2.GetSumw2() : h2.Sumw2()
    #
    xa = h2.GetXaxis()
    ya = h2.GetYaxis()
    #
    hn = h2_axes ( ya , xa )  
    ##
    for i in h2 :
        hn[ (i[1],i[0]) ] = h2[ i ] 

    return hn


ROOT.TH2F.T         = _h2_transpose_
ROOT.TH2D.T         = _h2_transpose_
ROOT.TH2F.Transpose = _h2_transpose_
ROOT.TH2D.Transpone = _h2_transpose_
ROOT.TH2F.transpose = _h2_transpose_
ROOT.TH2D.transpone = _h2_transpose_

# =============================================================================
## Draw 2D-histogram as 'colz'
def _h2_colz_ ( h2 , opts = '' ) :
    """
    Draw 2D-histogram as 'colz'
    
    >>> h2.colz()
    
    """
    return h2.Draw ( 'colz ' + opts )


ROOT.TH2F . colz = _h2_colz_
ROOT.TH2D . colz = _h2_colz_
ROOT.TH2F . Colz = _h2_colz_
ROOT.TH2D . Colz = _h2_colz_

# =============================================================================
# Decorate fit results 
# =============================================================================

# =============================================================================
## representation of TFitResult object 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _fit_repr_ ( self ) :
    """
    Representaion of TFitResult object
    """
    _r  = ''
    _r += "\n Status      = %s "    %   self.Status ()
    _r += "\n Chi2/nDoF   = %s/%s " % ( self.Chi2   () , self.Ndf() ) 
    _r += "\n Probability = %s "    %   self.Prob   () 
    _p = self.Parameters ()
    _e = self.Errors     ()
    for i in range( 0 , len(_p) ) :
        v = _p[i]
        e = _e[i]
        a = VE ( v ,e*e )
        _r  += " \n %s " % a 
    return _r
# =============================================================================
## iterator over fit-result object 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _fit_iter_ ( r ) :
    """
    Iterator over fit-result object 
    """
    _i = 0
    _p = r.Parameters ()
    _e = r.Errors     ()
    _l = len(_p)
    while _i < _l :
        a = VE ( _p[_i] , _e[_i]**2 )
        yield a
        _i += 1
        
# =============================================================================
## getitem for fit-result-object            
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _fit_get_item_ ( self , i ) :
    """
    Getitem for fit-result-object            
    """
    _p = self.Parameters ()
    _e = self.Errors     ()
    _l = len(_p)
    if 0<= i < _l :
        return VE ( _p[i]  , _e[i]**2 )
    raise IndexError 

ROOT.TFitResultPtr.__repr__     = _fit_repr_ 
ROOT.TFitResultPtr.__str__      = _fit_repr_ 
ROOT.TFitResultPtr.__iter__     = _fit_iter_ 
ROOT.TFitResultPtr.__getitem__  = _fit_get_item_ 
ROOT.TFitResultPtr.__call__     = _fit_get_item_ 
ROOT.TFitResultPtr.__len__      = lambda s : len( s.Parameters() ) 

# =============================================================================
## get the guess for three major parameters of the histogram:
#    - number of signal events
#    - background level under the signal (per bin)
#    - background slope
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def histoGuess ( histo , mass , sigma ) :
    """
    Get the guess for three major parameters of the histogram:
    - number of signal events
    - background level under the signal (per bin)
    - background slope
    - (minimal and maximal slopes to ensure 'positive' background)

    >>> histo = ...
    >>> signal, background, slope, slope_min, slope_max = histoGuess ( histo , mass , sigma )
    
    """
    tot0  = 0
    bin0  = 0
    tot3r = 0
    bin3r = 0
    tot3l = 0
    bin3l = 0
    tot4  = 0
    bin4  = 0

    axis  = histo.GetXaxis()   
    for ibin in axis:

        xbin = axis.GetBinCenter   ( ibin )
        dx   = ( xbin - mass ) / sigma
        val  = histo.GetBinContent ( ibin )
        
        if   abs ( dx ) < 2 :  ## +-2sigma
            tot0  += val 
            bin0  += 1
        elif   2 < dx  <  4 :  ## 'near' right sideband: 2 4 sigma 
            tot3r += val 
            bin3r += 1
        elif  -4 < dx  < -2 :  ## 'near' left sideband: -4 -2 sigma 
            tot3l += val 
            bin3l += 1
        else :
            tot4  += val 
            bin4  += 1

    bin3 = bin3r + bin3l + bin4
    tot3 = tot3r + tot3l + tot4 

    p00 = 0
    p03 = 0
    p04 = 0
    
    if bin3          : p03 = float(tot3)/bin3 
    if bin0 and bin3 : p00 = max ( float(tot0)-bin0*p03 , 0 ) 

    if bin3r and bin3l and tot3r and tot3l : 
        p04 = ( tot3r - tot3l ) / ( tot3r + tot3l ) / 3 / sigma 
    
    _xmin = axis.GetXmin() - 0.5* axis.GetBinWidth ( 1               )
    _xmax = axis.GetXmax() + 0.5* axis.GetBinWidth ( axis.GetNbins() )
    
    s1   = -1.0 / ( _xmin - mass )
    s2   = -1.0 / ( _xmax - mass )
    
    smin = min ( s1 , s2 )
    smax = max ( s1 , s2 )
    
    # if   p04 < smin : p04 = smin
    # elif p04 > smax : p04 = smax 
    
    return p00, p03, p04 , smin , smax  


ROOT.TH1F . histoGuess = histoGuess
ROOT.TH1D . histoGuess = histoGuess

# =============================================================================
## use likelihood in histogram fit ? 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def useLL ( histo         ,
            minc  = 10    ,
            diff  = 1.e-5 ) :
    """
    Use Likelihood in histogram fit?
    """    
    minv = 1.e+9    
    for ibin in histo : 

        v = histo [ ibin ]
        
        if 0 > v.value()         : return False
        if not _int ( v , diff ) : return False 
        
        minv = min ( minv , v.value() )
        
    return  minv < abs ( minc ) 


ROOT.TH1.useLL = useLL

# =============================================================================
## Natural histogram with all integer entries ?
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def allInts ( histo         ,
              diff  = 1.e-4 ) :
    """
    Natural histogram with all integer entries ?
    """
    
    diff = abs ( diff )
    
    for ibin in histo : 
        
        v = histo [ ibin ]
        
        if       0 >  v.value()  : return False
        if not _int ( v , diff ) : return False
        
    return True

ROOT.TH1.allInts = allInts

# =============================================================================
## calculate the efficiency histogram using the binomial erorrs 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def binomEff_h1 ( h1 , h2 ) :
    """
    Calculate the efficiency histogram using the binomial erorrs
    
    >>> h1 = ...
    >>> h2 = ...
    >>> h3 = h1 // h2
    
    """
    func = binomEff
    #
    if                                 not h1.GetSumw2() : h1.Sumw2()
    if hasattr ( h2 , 'GetSumw2' ) and not h2.GetSumw2() : h2.Sumw2()
    #
    result = h1.Clone( hID () )
    if not result.GetSumw2() : result.Sumw2()
    #
    for i1,x1,y1 in h1.iteritems() :
        #
        assert ( _int ( y1 ) )
        #
        y2 = h2 ( x1.value() ) 
        assert ( _int ( y2 ) )
        #
        l1 = long ( y1.value () )
        l2 = long ( y2.value () )
        #
        assert ( l1 <= l2 )
        #
        v = VE ( func ( l1 , l2 ) ) 
        #
        if not v.isfinite() : continue 
        #
        result.SetBinContent ( i1 , v.value () ) 
        result.SetBinError   ( i1 , v.error () )
        
    return result 

ROOT.TH1.  binomEff    = binomEff_h1 

# =============================================================================
## calculate the efficiency histogram using the binomial erorrs 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def binomEff_h2 ( h1 , h2 ) :
    """
    Calculate the efficiency histogram using the binomial erorrs

    >>> h1 = ...
    >>> h2 = ...
    >>> h3 = h1 // h2
    
    """
    func = binomEff
    #
    if                                 not h1.GetSumw2() : h1.Sumw2()
    if hasattr ( h2 , 'GetSumw2' ) and not h2.GetSumw2() : h2.Sumw2()
    #
    result = h1.Clone( hID () )
    if not result.GetSumw2() : result.Sumw2()
    #
    for ix1,iy1,x1,y1,z1 in h1.iteritems() :
        #
        assert ( _int ( z1 ) )
        #
        z2 = h2 ( x1.value() , y1.value() ) 
        assert ( _int ( z2 ) )
        #
        l1 = long ( z1.value () )
        l2 = long ( z2.value ()  )
        #
        assert ( l1 <= l2 )
        #
        v = VE ( func ( l1 , l2 ) ) 
        #
        if not v.isfinite() : continue 
        #
        result.SetBinContent ( ix1 , iy1 , v.value () ) 
        result.SetBinError   ( ix1 , iy1 , v.error () ) 
        
    return result 

ROOT.TH2.  binomEff    = binomEff_h2 

# =============================================================================
## calculate the efficiency histogram using the binomial erorrs 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def binomEff_h3 ( h1 , h2 ) :
    """
    Calculate the efficiency histogram using the binomial erorrs

    >>> h1 = ...
    >>> h2 = ...
    >>> h3 = h1 // h2
    
    """
    func = binomEff
    #
    if                                 not h1.GetSumw2() : h1.Sumw2()
    if hasattr ( h2 , 'GetSumw2' ) and not h2.GetSumw2() : h2.Sumw2()
    #
    result = h1.Clone( hID () )
    if not result.GetSumw2() : result.Sumw2()
    #
    for ix1,iy1,iz1,x1,y1,z1,v1 in h1.iteritems() :
        #
        assert ( _int ( v1 ) )
        #
        v2 = h2 ( x1.value() , y1.value() , z1.value() ) 
        assert ( _int ( v2 ) )
        #
        l1 = long ( v1.value () )
        l2 = long ( v2.value ()  )
        #
        assert ( l1 <= l2 )
        #
        v = VE ( func ( l1 , l2 ) ) 
        #
        if not v.isfinite() : continue 
        #
        result.SetBinContent ( ix1 , iy1 , iz1 , v.value () ) 
        result.SetBinError   ( ix1 , iy1 , iz1 , v.error () ) 
        
    return result 

ROOT.TH3.  binomEff    = binomEff_h3 


ROOT.TH1F . __floordiv__  = binomEff_h1 
ROOT.TH1D . __floordiv__  = binomEff_h1 
ROOT.TH2F . __floordiv__  = binomEff_h2
ROOT.TH2D . __floordiv__  = binomEff_h2
ROOT.TH3  . __floordiv__  = binomEff_h3

# =============================================================================
## operation with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h1_oper_ ( h1 , h2 , oper ) :
    """
    Operation with the histogram

    >>> h1     = ...
    >>> h2     = ...
    >>> result = h1 (oper) h2 
    """
    if                                 not h1.GetSumw2() : h1.Sumw2()
    if hasattr ( h2 , 'GetSumw2' ) and not h2.GetSumw2() : h2.Sumw2()
    #
    result = h1.Clone( hID() )
    if not result.GetSumw2() : result.Sumw2()
    #
    if   isinstance ( h2 , ( int , long , float ) ) :
        v1 = float  ( h2 ) 
        h2 = lambda s : VE ( v1 , 0 )
    elif isinstance ( h2 ,    VE ) :
        v1 =     VE ( h2 )
        h2 = lambda s : v1  
    elif isinstance ( h2 ,   ROOT.TF1 ) :
        v1 =          h2  
        h2 = lambda s : VE ( v1 ( float ( s ) , 0 ) ) 
    #
    for i1,x1,y1 in h1.iteritems() :
        #
        result.SetBinContent ( i1 , 0 ) 
        result.SetBinError   ( i1 , 0 )
        #
        y2 = h2 ( x1.value() ) 
        #
        v = VE ( oper ( y1 , y2 ) ) 
        #
        if not v.isfinite() : continue 
        #
        result.SetBinContent ( i1 , v.value () ) 
        result.SetBinError   ( i1 , v.error () )
        
    return result

# =============================================================================
## operation with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h1_ioper_ ( h1 , h2 , oper ) :
    """
    Operation with the histogram 
    """
    if                                 not h1.GetSumw2() : h1.Sumw2()
    if hasattr ( h2 , 'GetSumw2' ) and not h2.GetSumw2() : h2.Sumw2()
    #
    if   isinstance ( h2 , ( int , long , float ) ) :
        v1 = float  ( h2 ) 
        h2 = lambda s : VE ( v1 , 0 )
    elif isinstance ( h2 ,    VE ) :
        v1 =          h2  
        h2 = lambda s : v1  
    elif isinstance ( h2 ,   ROOT.TF1 ) :
        v1 =          h2  
        h2 = lambda s : VE ( v1 ( float ( s ) , 0 ) )    
    #
    for i1,x1,y1 in h1.iteritems() :
        #
        y2 = h2 ( x1.value() ) 
        #
        v = VE ( oper ( y1 , y2 ) ) 
        #
        if not v.isfinite() : continue 
        #
        h1.SetBinContent ( i1 , v.value () ) 
        h1.SetBinError   ( i1 , v.error () )
        
    return h1 

# =============================================================================
##  Division with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h1_div_ ( h1 , h2 ) :
    """
    Divide the histograms 
    
    >>> h1     = ...
    >>> h2     = ...
    >>> result = h1 / h2  
    """
    return _h1_oper_ ( h1 , h2 , lambda x,y : x/y ) 
# =============================================================================
##  Division with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h1_mul_ ( h1 , h2 ) :
    """
    Multiply the histograms 
    
    >>> h1     = ...
    >>> h2     = ...
    >>> result = h1 * h2  
    """
    return _h1_oper_ ( h1 , h2 , lambda x,y : x*y ) 
# =============================================================================
##  Addition with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h1_add_ ( h1 , h2 ) :
    """
    Add the histograms 
    
    >>> h1     = ...
    >>> h2     = ...
    >>> result = h1 + h2  
    """
    return _h1_oper_ ( h1 , h2 , lambda x,y : x+y ) 
# =============================================================================
##  Subtraction of the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h1_sub_ ( h1 , h2 ) :
    """
    Subtract the histogram 
    
    >>> h1     = ...
    >>> h2     = ...
    >>> result = h1 - h2  
    """
    return _h1_oper_ ( h1 , h2 , lambda x,y : x-y ) 
# =============================================================================
##  Fraction of the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h1_frac_ ( h1 , h2 ) :
    """
    ``Fraction'' the histogram h1/(h1+h2)
    
    >>> h1     = ...
    >>> h2     = ...
    >>> result = h1.frac  ( h2 ) 
    """
    return _h1_oper_ ( h1 , h2 , lambda x,y : x.frac(y) ) 
# =============================================================================
##  ``Asymmetry'' of the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h1_asym_ ( h1 , h2 ) :
    """
    ``Asymmetry'' the histogram (h1-h2)/(h1+h2)
    
    >>> h1     = ...
    >>> h2     = ...
    >>> result = h1.asym ( h2 ) 
    
    """
    return _h1_oper_ ( h1 , h2 , lambda x,y : x.asym(y) ) 
# =============================================================================
## ``Difference'' of the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h1_diff_ ( h1 , h2 ) :
    """
    ``Difference'' the histogram 2*(h1-h2)/(h1+h2)
    
    >>> h1     = ...
    >>> h2     = ...
    >>> result = h1.diff ( h2 ) 
    
    """
    return _h1_oper_ ( h1 , h2 , lambda x,y : 2*x.asym(y) ) 
# =============================================================================
##  ``Chi2-tension'' of the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h1_chi2_ ( h1 , h2 ) :
    """
    ``Chi2-tension'' the histogram
    
    >>> h1     = ...
    >>> h2     = ...
    >>> result = h1.chi2  ( h2 ) 
    
    """
    return _h1_oper_ ( h1 , h2 , lambda x,y : VE ( x.chi2 ( y ) , 0 ) ) 
# =============================================================================
##  ``Average'' of the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h1_mean_ ( h1 , h2 ) :
    """
    ``Mean'' the histograms
    
    >>> h1     = ...
    >>> h2     = ...
    >>> result = h1.average  ( h2 ) 
    
    """
    return _h1_oper_ ( h1 , h2 , lambda x,y : x.mean ( y ) ) 

# =============================================================================
## 'pow' the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h1_pow_ ( h1 , val ) :
    """
    ``pow'' the histogram 
    """
    if not h1.GetSumw2() : h1.Sumw2()
    #
    result = h1.Clone( hID() )
    if not result.GetSumw2() : result.Sumw2()
    #
    for i1,x1,y1 in h1.iteritems() :
        #
        result.SetBinContent ( i1 , 0 ) 
        result.SetBinError   ( i1 , 0 )
        #
        v = VE ( pow ( y1 , val ) ) 
        #
        if not v.isfinite() : continue 
        #
        result.SetBinContent ( i1 , v.value () ) 
        result.SetBinError   ( i1 , v.error () )
        
    return result 


# =============================================================================
## 'abs' the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-04-29
def _h1_abs_ ( h1 , val ) :
    """
    ``abs'' the histogram

    >>> h      = ...
    >>> result = abs ( h )
    """
    if not h1.GetSumw2() : h1.Sumw2()
    #
    result = h1.Clone( hID() )
    if not result.GetSumw2() : result.Sumw2()
    #
    for i1,x1,y1 in h1.iteritems() :
        #
        result.SetBinContent ( i1 , 0 ) 
        result.SetBinError   ( i1 , 0 )
        #
        v = abs ( y1 )  
        #
        result.SetBinContent ( i1 , v.value () ) 
        result.SetBinError   ( i1 , v.error () )
        
    return result 


# =============================================================================
## Division with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h1_idiv_ ( h1 , h2 ) :
    """
    Divide the histograms 
    
    >>> h1  = ...
    >>> h2  = ...
    >>> h1 /=  h2 
    
    """
    return _h1_ioper_ ( h1 , h2 , lambda x,y : x/y ) 
# =============================================================================
##  Division with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h1_imul_ ( h1 , h2 ) :
    """
    Multiply the histograms 
    
    >>> h1  = ...
    >>> h2  = ...
    >>> h1 *=  h2 
    
    """
    return _h1_ioper_ ( h1 , h2 , lambda x,y : x*y ) 
# =============================================================================
##  Addition with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h1_iadd_ ( h1 , h2 ) :
    """
    Add the histograms
    
    >>> h1  = ...
    >>> h2  = ...
    >>> h1 +=  h2 
    
    """
    return _h1_ioper_ ( h1 , h2 , lambda x,y : x+y ) 
# =============================================================================
##  Subtraction of the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h1_isub_ ( h1 , h2 ) :
    """
    Subtract the histogram
    
    >>> h1  = ...
    >>> h2  = ...
    >>> h1 -=  h2 
    
    """
    return _h1_ioper_ ( h1 , h2 , lambda x,y : x-y ) 

# =============================================================================
## Division with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h1_rdiv_ ( h1 , h2 ) :
    """
    Divide the histograms

    >>> h1     = ...
    >>> obj    = ...
    >>> result = obj / h1 
    """
    return _h1_oper_ ( h1 , h2 , lambda x,y : y/x ) 
# =============================================================================
##  Division with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h1_rmul_ ( h1 , h2 ) :
    """
    Multiply the histograms 

    >>> h1     = ...
    >>> obj    = ...
    >>> result = obj * h1 
    """
    return _h1_oper_ ( h1 , h2 , lambda x,y : y*x ) 
# =============================================================================
##  Addition with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h1_radd_ ( h1 , h2 ) :
    """
    Add the histograms
    
    >>> h1     = ...
    >>> obj    = ...
    >>> result = obj + h1 
    """
    return _h1_oper_ ( h1 , h2 , lambda x,y : y+x ) 
# =============================================================================
##  Subtraction of the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h1_rsub_ ( h1 , h2 ) :
    """
    Subtract the histogram
            
    >>> h1     = ...
    >>> obj    = ...
    >>> result = obj - h1 
    """
    return _h1_oper_ ( h1 , h2 , lambda x,y : y-x ) 

# =============================================================================

for t in ( ROOT.TH1F , ROOT.TH1D ) : 
    
    t . _oper_    = _h1_oper_
    t . _ioper_   = _h1_ioper_
    t . __div__   = _h1_div_
    t . __mul__   = _h1_mul_
    t . __add__   = _h1_add_
    t . __sub__   = _h1_sub_
    t . __pow__   = _h1_pow_
    
    t . __idiv__  = _h1_idiv_
    t . __imul__  = _h1_imul_
    t . __iadd__  = _h1_iadd_
    t . __isub__  = _h1_isub_
    
    t . __rdiv__  = _h1_rdiv_
    t . __rmul__  = _h1_rmul_
    t . __radd__  = _h1_radd_
    t . __rsub__  = _h1_rsub_
    
    t . __abs__   = _h1_abs_
    t .  frac     = _h1_frac_
    t .  asym     = _h1_asym_
    t .  diff     = _h1_diff_
    t .  chi2     = _h1_chi2_
    t .  average  = _h1_mean_




# =============================================================================
## find the first X-value for the given Y-value 
def _h1_xfind_ ( self           ,
                 v              ,
                 forward = True ) :
    """
    
    Find the first X-value for the given Y-value

    >>> h1 = ...
    >>> x = h1.find_X ( 1000 )

    >>> h1 = ...
    >>> x = h1.find_X ( 1000 , False )
    
    """
    #
    if hasattr (  v , 'value' ) : v = v.value()
    #
    mn = self [ self.GetMinimumBin() ]
    mx = self [ self.GetMaximumBin() ]

    #
    if v < mn.value() :
        return self.xmin() if forward else self.xmax ()
    #
    if v > mx.value() :
        return self.xmax() if forward else self.xmin () 
    
    nb = len ( self )
    ax = self.GetXaxis () 
    
    for i in self :
        
        j  = i+1
        if not j in self : continue
        
        ib = i if forward else nb + 1 - i 
        jb = j if forward else nb + 1 - j 
        
        vi = self [ ib ].value () 
        vj = self [ jb ].value ()
        
        if  vi <= v <= vj or vj <= v <= vi :
            
            xi = ax.GetBinCenter ( ib )
            xj = ax.GetBinCenter ( jb )
            
            if   vi == v             : return xi   ## RETURN 
            elif vj == v             : return xj   ## RETURN 
            
            dv = vi - vj
            dx = xi - xj
            
            if   vi == vj or 0 == dv : return 0.5 * ( xi + xj )  ## RETURN
            
            return (v*dx+vi*xj-vj*xi)/dv                         ## RETURN
        

ROOT.TH1F . find_X = _h1_xfind_
ROOT.TH1D . find_X = _h1_xfind_


# =======================================================================
## get "n-sigma" interval assuming the presence of peak
def _h1_CL_interval_ ( self       ,
                       nsigma = 1 ) : 
    """
    Get ``n-sigma'' interval for the distribution.

    >>> h = ...
    >>> xlow,xhigh = h.cl_interval( 1 ) 
    """
    mv   = self.maxv()
    import math 
    
    try :
        #
        f    = math.exp    ( -0.5 * nsigma * nsigma )
        x1   = self.find_X ( mv * f , True  )
        x2   = self.find_X ( mv * f , False )
        #
    except :
        #
        x1,x2 = self.xminmax () 
        
    return x1,x2 
    
ROOT.TH1F . cl_interval = _h1_CL_interval_
ROOT.TH1D . cl_interval = _h1_CL_interval_
            
# =============================================================================
## get the minumum value for the histogram 
def _h_minv_ ( self ) :
    """
    Get the minimum value for the histogram

    >>> h  = ...
    >>> mv = h.minv ()
    """
    mv = VE ( 1.e+100 , -1 ) 
    for ibin in self :
        v = self[ibin]
        if v.value() <= mv.value() or mv.error() < 0 : mv = v
    return mv 

# =============================================================================
## get the maximum value for the histogram 
def _h_maxv_ ( self ) :
    """
    Get the minimum value for the histogram

    >>> h  = ...
    >>> mv = h.maxv ()
    """
    mv = VE ( -1.e+100 , -1 ) 
    for ibin in self :
        v = self[ibin]
        if v.value() >= mv.value() or mv.error() < 0 : mv = v
    return mv 
        
# =============================================================================
## get the minmaximum values for the histogram 
def _h_minmax_ ( self ) :
    """
    Get the minmax pair for the histogram
    
    >>> h     = ...
    >>> mn,mx = h.minmax ()
    """
    return self.minv() , self.maxv() 

ROOT.TH1 . minv    = _h_minv_
ROOT.TH1 . maxv    = _h_maxv_
ROOT.TH1 . minmax  = _h_minmax_

# ============================================================================
## get the minimum value for X-axis 
def _ax_min_ ( self ) :
    """
    Get the minimum value for X-axis

    >>> xmin = h.xmin()
    """
    ax = self.GetXaxis () 
    return ax.GetXmin()
# ============================================================================
## get the minimum value for y-axis 
def _ay_min_ ( self ) :
    """
    Get the minimum value for Y-axis

    >>> ymin = h.ymin()
    """
    ay = self.GetYaxis () 
    return ay.GetXmin()
# ============================================================================
## get the minimum value for z-axis 
def _az_min_ ( self ) :
    """
    Get the minimum value for Z-axis

    >>> zmin = h.zmin()
    """
    az = self.GetZaxis () 
    return az.GetXmin()

# ============================================================================
## get the maximum value for X-axis 
def _ax_max_ ( self ) :
    """
    Get the maximum value for X-axis

    >>> xmax = h.xmax()
    """
    ax = self.GetXaxis () 
    return ax.GetXmax()
# ============================================================================
## get the maximum value for y-axis 
def _ay_max_ ( self ) :
    """
    Get the maximum value for Y-axis

    >>> ymax = h.ymax()
    """
    ay = self.GetYaxis () 
    return ay.GetXmax()
# ============================================================================
## get the maximum value for z-axis 
def _az_max_ ( self ) :
    """
    Get the maximum value for Z-axis

    >>> zmax = h.zmax()
    """
    az = self.GetZaxis () 
    return az.GetXmax()

ROOT.TH1D. xmin    = _ax_min_
ROOT.TH1D. xmax    = _ax_max_
ROOT.TH1D. ymin    = _h_minv_
ROOT.TH1D. ymax    = _h_maxv_

ROOT.TH1F. xmin    = _ax_min_
ROOT.TH1F. xmax    = _ax_max_
ROOT.TH1F. ymin    = _h_minv_
ROOT.TH1F. ymax    = _h_maxv_

ROOT.TH2 . xmin    = _ax_min_
ROOT.TH2 . xmax    = _ax_max_
ROOT.TH2 . ymin    = _ay_min_
ROOT.TH2 . ymax    = _ay_max_
ROOT.TH2 . zmin    = _h_minv_
ROOT.TH2 . zmax    = _h_maxv_

ROOT.TH3 . xmin    = _ax_min_
ROOT.TH3 . xmax    = _ax_max_
ROOT.TH3 . ymin    = _ay_min_
ROOT.TH3 . ymax    = _ay_max_
ROOT.TH3 . zmin    = _az_min_
ROOT.TH3 . zmax    = _az_max_

ROOT.TH1D. xminmax = lambda s : ( s.xmin() , s.xmax() )
ROOT.TH1D. yminmax = lambda s : ( s.ymin() , s.ymax() )
ROOT.TH1F. xminmax = lambda s : ( s.xmin() , s.xmax() )
ROOT.TH1F. yminmax = lambda s : ( s.ymin() , s.ymax() )

ROOT.TH2 . xminmax = lambda s : ( s.xmin() , s.xmax() )
ROOT.TH2 . yminmax = lambda s : ( s.ymin() , s.ymax() )
ROOT.TH2 . zminmax = lambda s : ( s.zmin() , s.zmax() )

ROOT.TH3 . xminmax = lambda s : ( s.xmin() , s.xmax() )
ROOT.TH3 . yminmax = lambda s : ( s.ymin() , s.ymax() )
ROOT.TH3 . zminmax = lambda s : ( s.zmin() , s.zmax() )


    
# =============================================================================
## operation with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h2_oper_ ( h1 , h2 , oper ) :
    """
    Operation with the histogram
        
    >>> h1     = ...
    >>> h2     = ...
    >>> result = h1 (oper) h2
    
    """
    if                                 not h1.GetSumw2() : h1.Sumw2()
    if hasattr ( h2 , 'GetSumw2' ) and not h2.GetSumw2() : h2.Sumw2()
    #
    result = h1.Clone( hID() )
    if not result.GetSumw2() : result.Sumw2()
    #
    if   isinstance ( h2 , (int,long,float) ) :
        v1 = float ( h2 )  
        h2 = lambda x,y : VE(v1,0)
    elif isinstance ( h2 ,  VE              ) :
        v1 =     VE ( h2 )
        h2 = lambda x,y : v1
    #
    for ix1,iy1,x1,y1,z1 in h1.iteritems() :
        #
        result.SetBinContent ( ix1 , iy1 , 0 ) 
        result.SetBinError   ( ix1 , iy1 , 0 )
        #
        z2 = h2 ( x1.value() , y1.value() ) 
        #
        v = VE ( oper ( z1 , z2 ) ) 
        #
        if not v.isfinite() : continue 
        #
        result.SetBinContent ( ix1 , iy1 , v.value () ) 
        result.SetBinError   ( ix1 , iy1 , v.error () )
        
    return result

# =============================================================================
## operation with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-06-03
def _h2_ioper_ ( h1 , h2 , oper ) :
    """
    Operation with the histogram 
    """
    if                                 not h1.GetSumw2() : h1.Sumw2()
    if hasattr ( h2 , 'GetSumw2' ) and not h2.GetSumw2() : h2.Sumw2()
    #
    if   isinstance ( h2 , ( int , long , float ) ) :
        v1 = float  ( h2 ) 
        h2 = lambda x,y : VE ( v1 , 0 )
    elif isinstance ( h2 ,    VE ) :
        v1 =          h2  
        h2 = lambda x,y : v1  
    #
    for ix1,iy1,x1,y1,z1 in h1.iteritems() :
        #
        h1.SetBinContent ( ix1 , iy1 , 0 ) 
        h1.SetBinError   ( ix1 , iy1 , 0 )
        #
        z2 = h2 ( x1.value() , y1.value() ) 
        #
        v = VE ( oper ( z1 , z2 ) ) 
        #
        if not v.isfinite() : continue 
        #
        h1.SetBinContent ( ix1 , iy1 , v.value () ) 
        h1.SetBinError   ( ix1 , iy1 , v.error () )

    return h1

# =============================================================================
## Division with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h2_div_ ( h1 , h2 ) :
    """
    Divide the histograms
    
    >>> h1     = ...
    >>> h2     = ...
    >>> result = h1 / h2
    
    """
    return _h2_oper_ ( h1 , h2 , lambda x,y : x/y ) 
# =============================================================================
## Division with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h2_mul_ ( h1 , h2 ) :
    """
    Multiply the histograms 
    
    >>> h1     = ...
    >>> h2     = ...
    >>> result = h1 * h2 
    """
    return _h2_oper_ ( h1 , h2 , lambda x,y : x*y ) 
# =============================================================================
## Addition with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h2_add_ ( h1 , h2 ) :
    """
    Add the histograms 
    
    >>> h1     = ...
    >>> h2     = ...
    >>> result = h1 + h2 
    """
    return _h2_oper_ ( h1 , h2 , lambda x,y : x+y ) 
# =============================================================================
## Subtraction of the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h2_sub_ ( h1 , h2 ) :
    """
    Subtract the histogram
    
    >>> h1     = ...
    >>> h2     = ...
    >>> result = h1 - h2 
    """
    return _h2_oper_ ( h1 , h2 , lambda x,y : x-y ) 

# =============================================================================
## Division with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h2_rdiv_ ( h1 , h2 ) :
    """
    Divide the histograms
    
    >>> h1     = ...
    >>> h2     = ...
    >>> result = h1 / h2
    
    """
    return _h2_oper_ ( h1 , h2 , lambda x,y : y/x ) 
# =============================================================================
## Division with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h2_rmul_ ( h1 , h2 ) :
    """
    Multiply the histograms 
    
    >>> h1     = ...
    >>> h2     = ...
    >>> result = h1 * h2 
    """
    return _h2_oper_ ( h1 , h2 , lambda x,y : y*x ) 
# =============================================================================
## Addition with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h2_radd_ ( h1 , h2 ) :
    """
    Add the histograms 
    
    >>> h1     = ...
    >>> h2     = ...
    >>> result = h1 + h2 
    """
    return _h2_oper_ ( h1 , h2 , lambda x,y : y+x ) 
# =============================================================================
## Subtraction of the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h2_rsub_ ( h1 , h2 ) :
    """
    Subtract the histogram
    
    >>> h1     = ...
    >>> h2     = ...
    >>> result = h1 - h2 
    """
    return _h2_oper_ ( h1 , h2 , lambda x,y : y-x ) 

# =============================================================================
##  ``Fraction'' of the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h2_frac_ ( h1 , h2 ) :
    """
    ``Fraction'' the histogram h1/(h1+h2)
    
    >>> h1     = ...
    >>> h2     = ...
    >>> frac   = h1.frac ( h2 )

    """
    return _h2_oper_ ( h1 , h2 , lambda x,y : x.frac(y) ) 
# =============================================================================
## ``Asymmetry'' of the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h2_asym_ ( h1 , h2 ) :
    """
    ``Asymmetry'' the histogram (h1-h2)/(h1+h2)
    
    >>> h1     = ...
    >>> h2     = ...
    >>> asym   = h1.asym ( h2 )
    """
    return _h2_oper_ ( h1 , h2 , lambda x,y : x.asym(y) ) 
# =============================================================================
## ``Difference'' of the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2012-06-03
def _h2_diff_ ( h1 , h2 ) :
    """
    ``Difference'' the histogram 2*(h1-h2)/(h1+h2)
    
    >>> h1     = ...
    >>> h2     = ...
    >>> diff   = h1.diff ( h2 )
    """
    return _h2_oper_ ( h1 , h2 , lambda x,y : 2*x.asym(y) ) 
# =============================================================================
##  ``Chi2-tension'' the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h2_chi2_ ( h1 , h2 ) :
    """
    ``Chi2-tension'' for the histograms
    
    >>> h1     = ...
    >>> h2     = ...
    >>> chi2   = h1.chi2 ( h2 ) 
    """
    return _h2_oper_ ( h1 , h2 , lambda x,y : VE ( x.chi2 ( y ) , 0 ) ) 
# =============================================================================
##  ``Average'' the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h2_mean_ ( h1 , h2 ) :
    """
    ``Average'' for the histograms
    
    >>> h1     = ...
    >>> h2     = ...
    >>> mean   = h1.average ( h2 ) 
    """
    return _h2_oper_ ( h1 , h2 , lambda x,y : x.mean ( y ) )  

# =============================================================================
## 'pow' the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h2_pow_ ( h1 , val ) :
    """
    ``pow'' the histogram
    
    >>> h2     = ...
    >>> result = h2 ** 2 

    """
    if not h1.GetSumw2() : h1.Sumw2()
    #
    result = h1.Clone( hID() )
    if not result.GetSumw2() : result.Sumw2()
    #
    for ix1,iy1,x1,y1,z1 in h1.iteritems() :
        #
        result.SetBinContent ( ix1 , iy1 , 0 ) 
        result.SetBinError   ( ix1 , iy1 , 0 )
        #
        v = VE ( pow ( z1 , val ) ) 
        #
        if not v.isfinite() : continue 
        #
        result.SetBinContent ( ix1 , iy1 , v.value () ) 
        result.SetBinError   ( ix1 , iy1 , v.error () )
        
    return result 

# =============================================================================
## 'abs' the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h2_abs_ ( h1 ) :
    """
    ``abs'' the histogram

    >>> h2     = ...
    >>> result = abs ( h2 ) 
    """
    if not h1.GetSumw2() : h1.Sumw2()
    #
    result = h1.Clone( hID() )
    if not result.GetSumw2() : result.Sumw2()
    #
    for ix1,iy1,x1,y1,z1 in h1.iteritems() :
        #
        result.SetBinContent ( ix1 , iy1 , 0 ) 
        result.SetBinError   ( ix1 , iy1 , 0 )
        #
        v = abs  ( z1 ) 
        #
        result.SetBinContent ( ix1 , iy1 , v.value () ) 
        result.SetBinError   ( ix1 , iy1 , v.error () )
        
    return result 

# =============================================================================
## Division with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-06-03
def _h2_idiv_ ( h1 , h2 ) :
    """
    Divide the histograms 
    
    >>> h1  = ...
    >>> h2  = ...
    >>> h1 /=  h2 
    
    """
    return _h2_ioper_ ( h1 , h2 , lambda x,y : x/y ) 
# =============================================================================
## Division with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-06-03
def _h2_imul_ ( h1 , h2 ) :
    """
    Multiply the histograms 
    
    >>> h1  = ...
    >>> h2  = ...
    >>> h1 *=  h2 
    
    """
    return _h2_ioper_ ( h1 , h2 , lambda x,y : x*y ) 
# =============================================================================
## Addition with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-06-03
def _h2_iadd_ ( h1 , h2 ) :
    """
    Add the histograms
    
    >>> h1  = ...
    >>> h2  = ...
    >>> h1 +=  h2 
    
    """
    return _h2_ioper_ ( h1 , h2 , lambda x,y : x+y ) 
# =============================================================================
## Subtraction of the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-06-03
def _h2_isub_ ( h1 , h2 ) :
    """
    Subtract the histogram
    
    >>> h1  = ...
    >>> h2  = ...
    >>> h1 -=  h2 
    
    """
    return _h2_ioper_ ( h1 , h2 , lambda x,y : x-y ) 
# =============================================================================


def _h2_box_  ( self , opts = '' ) : return self.Draw( opts + 'box' )
def _h2_lego_ ( self , opts = '' ) : return self.Draw( opts + 'lego')

for t in ( ROOT.TH2F , ROOT.TH2D ) : 
    
    t . _oper_   = _h2_oper_
    t . __div__  = _h2_div_
    t . __mul__  = _h2_mul_
    t . __add__  = _h2_add_
    t . __sub__  = _h2_sub_
    t . __pow__  = _h2_pow_
    t . __abs__  = _h2_abs_

    t . __rdiv__ = _h2_rdiv_
    t . __rmul__ = _h2_rmul_
    t . __radd__ = _h2_radd_
    t . __rsub__ = _h2_rsub_

    t . __idiv__ = _h2_idiv_
    t . __imul__ = _h2_imul_
    t . __iadd__ = _h2_iadd_
    t . __isub__ = _h2_isub_
    
    t .  frac    = _h2_frac_
    t .  asym    = _h2_asym_
    t .  diff    = _h2_diff_
    t .  chi2    = _h2_chi2_
    t .  average = _h2_mean_

    t .  box     = _h2_box_
    t .  lego    = _h2_lego_


# =============================================================================
## operation with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h3_oper_ ( h1 , h2 , oper ) :
    """
    Operation with the histogram 
    
    >>> h1 = ...
    >>> h2 = ...
    >>> h3 = h1 (oper) h2
    
    """
    if                                 not h1.GetSumw2() : h1.Sumw2()
    if hasattr ( h2 , 'GetSumw2' ) and not h2.GetSumw2() : h2.Sumw2()
    #
    result = h1.Clone( hID() )
    if not result.GetSumw2() : result.Sumw2()
    #
    for ix1,iy1,iz1,x1,y1,z1,v1 in h1.iteritems() :
        #
        result.SetBinContent ( ix1 , iy1 , iz1 , 0 ) 
        result.SetBinError   ( ix1 , iy1 , iz1 , 0 )
        #
        v2 = h2 ( x1.value() , y1.value() , z1.value() ) 
        #
        v = VE ( oper ( v1 , v2 ) ) 
        #
        if not v.isfinite() : continue 
        #
        result.SetBinContent ( ix1 , iy1 , iz1 , v.value () ) 
        result.SetBinError   ( ix1 , iy1 , iz1 , v.error () )
        
    return result


# =============================================================================
##  Division with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h3_div_ ( h1 , h2 ) :
    """
    Divide the histograms 
    
    >>> h1 = ...
    >>> h2 = ...
    >>> h3 = h1 / h2 
    """
    return _h3_oper_ ( h1 , h2 , lambda x,y : x/y ) 
# =============================================================================
##  Division with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h3_mul_ ( h1 , h2 ) :
    """
    Multiply the histograms
    
    >>> h1 = ...
    >>> h2 = ...
    >>> h3 = h1 * h2 
    """
    return _h3_oper_ ( h1 , h2 , lambda x,y : x*y ) 
# =============================================================================
##  Addition with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h3_add_ ( h1 , h2 ) :
    """
    Add the histograms 
    
    >>> h1 = ...
    >>> h2 = ...
    >>> h3 = h1 + h2 
    """
    return _h3_oper_ ( h1 , h2 , lambda x,y : x+y ) 
# =============================================================================
##  Subtraction of the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h3_sub_ ( h1 , h2 ) :
    """
    Subtract the histogram
    
    >>> h1 = ...
    >>> h2 = ...
    >>> h3 = h1 - h2 
    """
    return _h3_oper_ ( h1 , h2 , lambda x,y : x-y ) 
# =============================================================================
##  ``Fraction'' of the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h3_frac_ ( h1 , h2 ) :
    """
    ``Fraction'' the histogram h1/(h1+h2)
    
    >>> h1 = ...
    >>> h2 = ...
    >>> h3 = h1.frac ( h2 )
    
    """
    return _h3_oper_ ( h1 , h2 , lambda x,y : x.frac(y) ) 
# =============================================================================
##  ``Asymmetry'' of the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h3_asym_ ( h1 , h2 ) :
    """
    ``Asymmetry'' the histogram (h1-h2)/(h1+h2)
    
    >>> h1 = ...
    >>> h2 = ...
    >>> h3 = h1.asym ( h2 )
    
    """
    return _h3_oper_ ( h1 , h2 , lambda x,y : x.asym(y) ) 
# =============================================================================
##  ``Chi2-tension'' the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h3_chi2_ ( h1 , h2 ) :
    """
    ``Chi2-tension'' for the histograms
    
    >>> h1 = ...
    >>> h2 = ...
    >>> h3 = h1.chi2 ( h2 )
    
    """
    return _h3_oper_ ( h1 , h2 , lambda x,y : VE ( x.chi2 ( y ) , 0 ) ) 
# =============================================================================
##  ``Average'' the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h3_mean_ ( h1 , h2 ) :
    """
    ``Average'' for the histograms

    >>> h1 = ...
    >>> h2 = ...
    >>> h3 = h1.average ( h2 ) 
    """
    return _h3_oper_ ( h1 , h2 , lambda x,y : x.mean ( y ) )  

# =============================================================================
## 'pow' the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h3_pow_ ( h1 , val ) :
    """
    ``pow'' the histogram 
    
    >>> h1      = ...
    >>> result  = h1 ** 3 
    """
    if not h1.GetSumw2() : h1.Sumw2()
    #
    result = h1.Clone( hID() )
    if not result.GetSumw2() : result.Sumw2()
    #
    for ix1,iy1,iz1,x1,y1,z1,v1 in h1.iteritems() :
        #
        result.SetBinContent ( ix1 , iy1 , iz1 , 0 ) 
        result.SetBinError   ( ix1 , iy1 , iz2 , 0 )
        #
        v = VE ( pow ( v1 , val ) ) 
        #
        if not v.isfinite() : continue 
        #
        result.SetBinContent ( ix1 , iy1 , iz1 , v.value () ) 
        result.SetBinError   ( ix1 , iy1 , iz1 , v.error () )
        
    return result 


ROOT.TH3._oper_    = _h3_oper_
    
ROOT.TH3.__div__   = _h3_div_
ROOT.TH3.__mul__   = _h3_mul_
ROOT.TH3.__add__   = _h3_add_
ROOT.TH3.__sub__   = _h3_sub_
ROOT.TH3.__pow__   = _h3_pow_

ROOT.TH3.  frac    = _h3_frac_
ROOT.TH3.  asym    = _h3_asym_
ROOT.TH3.  chi2    = _h3_chi2_
ROOT.TH3.  average = _h3_mean_

# =============================================================================
## get the runnig sum over the histogram
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h1_sumv_ ( h , increasing = True ) :
    """
    Create the ``runnig sum'' over the histogram 

    >>> h   = ...
    >>> h1 = h.sumv()
    
    """
    result = h.Clone ( hID() )
    result.Reset() 
    if not result.GetSumw2() : result.Sumw2()
    
    if increasing :
        
        _s = VE ( 0 , 0 ) 
        for ibin in h :
            _s            += h [ibin]
            result [ibin]  = VE( _s ) 
    else :
        
        for ibin in h :
            _s  = VE(0,0) 
            for jbin in h :
                if jbin < ibin : continue
                _s += h [jbin]
                
            result [ibin]  = VE( _s ) 
        
    return result 

# ==============================================================================
for t in  (ROOT.TH1F , ROOT.TH1D ) : 
    t . sumv   = _h1_sumv_ 
        

# =============================================================================
## Calculate the "cut-efficiency from the histogram
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h1_effic_ ( h , increasing = True ) :
    """
    Calculate the cut efficiency for the histogram

    >>> h  = ...
    >>> he = h.effic ( 14.2 )
    
    """
    
    result = h.Clone ( hID() )
    result.Reset() 
    if not result.GetSumw2() : result.Sumw2()

    for ibin in h :

        s1 = VE(0,0)
        s2 = VE(0,0)
        
        for jbin in h :
            
            if jbin < ibin : s1 += h [ jbin ]
            else           : s2 += h [ jbin ]

        result [ibin] = s1.frac( s2 ) if increasing else s2.frac( s1 ) 

    return result 


# =============================================================================
## Calculate the "cut-efficiency from the histogram
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h1_effic2_ ( h , value , increasing = True ) :
    """
    Calculate the cut efficiency for the histogram

    >>> h  = ...
    >>> he = h.efficiency ( 14.2 )
    
    """

    s1 = VE(0,0)
    s2 = VE(0,0)

    for i,x,y in h.iteritems () :

        if x.value() < value : s1 += y 
        else                 : s2 += y 
        
    return s1.frac( s2 ) if increasing else s2.frac ( s1 ) 

ROOT.TH1F.effic      = _h1_effic_ 
ROOT.TH1D.effic      = _h1_effic_ 
ROOT.TH1F.efficiency = _h1_effic2_ 
ROOT.TH1D.efficiency = _h1_effic2_ 

# ================================================================================
_sqrt_2_ = math.sqrt( 2.0 ) 
## helper function : convolution of gaussian with the single pulse 
def _cnv_ ( x , x0 , dx , sigma ) :
    """
    Simple wrapper over error-function:
    convolution of gaussian with the single pulse 
    """
    _erf_ = ROOT.Math.erfc
    
    s = abs ( sigma )
    if hasattr ( s , 'value' ) : s = s.value()
    
    h = ( x - ( x0 + 0.5 * dx ) ) / _sqrt_2_ / s 
    l = ( x - ( x0 - 0.5 * dx ) ) / _sqrt_2_ / s 
    
    return 0.5 * ( _erf_ ( h ) - _erf_ ( l ) ) / dx 

# =============================================================================
## "Smear" : make a convolution of the histogram with gaussian function
#  @param sigma     the gaussian resolutuon
#  @param addsigmas the parameter to treat the bounary conditions 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-09-20
def _smear_ ( h1 , sigma , addsigmas = 5 ) :
    """
    Smear the histogram through the convolution with the gaussian function:

    >>> histo   = ...
    >>> smeared = h.smear ( sigma = 0.01 )
    """
    #
    ## clone the input histogram
    #
    h2  = h1.Clone ( hID () ) ; h2.Reset() ;
    if not h2.GetSumw2() : h2.Sumw2()

    first_bin = None
    last_bin  = None

    ## collect all bins
    bins = []

    for ibin in h1.iteritems() : bins.append ( ibin[1:] )

    ## add few artificially replicated bins
    fb    = bins [  0 ]
    lb    = bins [ -1 ]
    xmin  = fb[1].value() - fb[1].error()
    xmax  = lb[1].value() + lb[1].error()    
    x_min = xmin - abs ( addsigmas * sigma ) 
    x_max = xmax + abs ( addsigmas * sigma ) 

    ## add few fictive bins 
    while  xmin > x_min :
        
        bin0 = bins[0]
        
        xc   = bin0[0]
        val  = bin0[1]
        
        bin  = (xc-2*xc.error(),val)
        bins.insert (  0, bin )
        fb    = bins [  0 ]
        
        xmin  = bin[0].value()-bin[0].error()

    ## add few fictive bins 
    while  xmax < x_max :
        
        bin0 = bins[-1]
        
        xc   = bin0[0]
        val  = bin0[1]
        
        bin  = (xc+2*xc.error(),val)
        bins.append ( bin )
        fb    = bins [  0 ]
        
        xmax  = bin[0].value()+bin[0].error()
    

    for ibin1 in bins :
        
        x1c  =     ibin1 [0].value () 
        x1w  = 2 * ibin1 [0].error () 
        val1 =     ibin1 [1]
        
        if 0 == val1.value() and 0 == val1.cov2() : continue
        
        for ibin2 in h2.iteritems() :

            i2      =     ibin2 [0]
            x2c     =     ibin2 [1].value () 
            x2w     = 2 * ibin2 [1].error () 

            val2    = VE ( val1 )
            val2   *= x2w  
            val2   *= _cnv_ ( x2c , x1c , x1w , sigma ) 
            
            h2[i2] += val2

    return h2


ROOT.TH1F. smear = _smear_
ROOT.TH1D. smear = _smear_


# =============================================================================
## make transformation of histogram content 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2012-10-23  
def _transform_ ( h1 , func ) :
    """
    
    Make the transformation of the histogram content 
    
    >>> func = lambda x,y: y   ## identical transformation/copy
    >>> h1 = ...
    >>> h2 = h1.fransform ( func ) 
    
    """
    #
    if not h1.GetSumw2() : h1.Sumw2()
    h2 = h1.Clone( hID() )
    if not h2.GetSumw2() : h2.Sumw2()
    #
    for i,x,y in h1.iteritems() :
        
        h2 [ i ] = func ( x, y ) 
        
    return h2 

ROOT.TH1F. transform = _transform_ 
ROOT.TH1D. transform = _transform_ 

# =============================================================================
## Get the Figure-of-Merit (FoM) for the pure signal distribution,
#  e.g. from sPlot)
#  the FoM is defined from the relative precision of the signal yield
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2012-10-23  
def _fom_2_ ( h1 , increase = True ) :
    """
    Get figure-of-merit (FOM) distribution for signal

    >>> h1 = ...  ## signal distribution
    >>> f1 = h1.FoM2 () 
    """
    #
    h = h1.sumv( increase )
    #
    return _transform_ ( h , func = lambda x,y : y.precision() ) 

# =============================================================================
## Calculate S/sqrt(S+a*B)
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2012-10-23   
def _sb_ ( s , b  , a = 1 ) :
    """
    Calculate S/sqrt(S+a*B) 
    """
    #
    v     = ( s + a * b ).value()
    if 0 >= v : return VE ( 0 , 0 )
    #
    F     = s.value() / pow ( v , 0.5 )
    #
    # (dF/dS)**2 
    dFdS2 = ( 0.5 * s  + a * b ).value() 
    dFdS2 = dFdS2**2  / v**3
    #
    # (dR/dB)**2 
    dFdB2 = ( -0.5 * a * s  ).value() 
    dFdB2 = dFdB2**2  / v**3
    #
    return VE ( F , dFdS2 * s.cov2() + dFdB2 * b.cov2() ) 

# =============================================================================
## Get the figure-of-merit (FoM) for the signal and background distributions 
#  the FoM is defined as S/sqrt(S+alpha*B) 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2012-10-23  
def _fom_1_ ( s , b , alpha = 1 , increase = True ) :
    """
    Get figure-of-merit FoM = S/sqrt(S+a*B)
    
    >>> s = ... ## signal distributions
    >>> b = ... ## background distributions
    >>> fom = s.FoM1( b , alpha = 1.0 )  
    
    """
    #
    if not s.GetSumw2() : s.Sumw2()
    h  = s.Clone( hID() )
    if not h.GetSumw2() : h.Sumw2()
    #
    hs = s.sumv ( increase )
    hb = b.sumv ( increase )
    #
    from math import sqrt, pow 
    #
    for i,x,y in hs.iteritems() :
        
        ## the signal 
        si = y
        
        ## the background 
        bi = hb ( x ) 
        
        h [i] = _sb_ ( si , bi , alpha ) 
        
    return h 

ROOT.TH1D . fom_1 = _fom_1_ 
ROOT.TH1D . fom_2 = _fom_2_ 
ROOT.TH1F . fom_1 = _fom_1_ 
ROOT.TH1F . fom_2 = _fom_2_

ROOT.TH1D . FoM_1 = _fom_1_ 
ROOT.TH1D . FoM_2 = _fom_2_ 
ROOT.TH1F . FoM_1 = _fom_1_ 
ROOT.TH1F . FoM_2 = _fom_2_ 

# =============================================================================
## make graph from data 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def makeGraph ( x , y = []  , ex = [] , ey = [] ) :

    """
    Make graph using the primitive data
    """
    if  isinstance ( x , dict ) and not y and not ex and not ey : 
        _x = []
        _y = []
        keys = x.keys()
        keys.sort()
        for k in keys :
            _x += [   k  ]
            _y += [ x[k] ] 
        return makeGraph ( _x , _y )
        
    if  not x : raise TypeError, "X is not a proper vector!"
    if  not y : raise TypeError, "Y is not a proper vector!"
    if len( x ) != len ( y ) :
        raise TypeError, "Mismatch X/Y-lengths"

    if ex and len(ex) != len(x) : raise TypeError, "Mismatch X/eX-lengths"
    if ey and len(ey) != len(y) : raise TypeError, "Mismatch Y/eY-lengths"

    gr = ROOT.TGraphErrors ( len(x) ) 
        
    for i in range ( 0 , len(x) ) :
        
        if ex : _ex = ex[i]
        else  : _ex = 0.0
        if ey : _ey = ey[i]
        else  : _ey = 0.0

        _x = x[i]
        if hasattr ( x[i] , 'value' ) : _x  = x[i].value ()        
        if hasattr ( x[i] , 'error' ) : _ex = x[i].error ()

        _y = y[i]
        if hasattr ( y[i] , 'value' ) : _y  = y[i].value ()        
        if hasattr ( y[i] , 'error' ) : _ey = y[i].error ()
                    
        gr .SetPoint      ( i ,  _x ,  _y )
        gr .SetPointError ( i , _ex , _ey )
        
    return gr

# =============================================================================
# rebin the histograms
# =============================================================================
## get the overlap for 1D-bins 
def _bin_overlap_1D_ ( x1 , x2 ) :
    """
    """
    #
    xmin_1 = x1.value() - x1.error () 
    xmax_1 = x1.value() + x1.error ()
    if xmin_1 >= xmax_1  : return 0         ## RETURN
    #
    xmin_2 = x2.value() - x2.error () 
    xmax_2 = x2.value() + x2.error ()
    if xmin_2 >= xmax_2  : return 0         ## RETURN
    #
    xmin = max ( xmin_1 , xmin_2 )
    xmax = min ( xmax_1 , xmax_2 )
    #
    if xmin >= xmax      : return 0         ## RETURN 
    #
    return ( xmax - xmin ) / 2.0 / x1.error()
# =============================================================================
## get the overlap for 2D-bins 
def _bin_overlap_2D_ ( x1 , y1 , x2 , y2 ) :
    """
    """
    #
    xmin_1 = x1.value() - x1.error () 
    xmax_1 = x1.value() + x1.error ()
    if xmin_1 >= xmax_1  : return 0         ## RETURN
    #
    xmin_2 = x2.value() - x2.error () 
    xmax_2 = x2.value() + x2.error ()
    if xmin_2 >= xmax_2  : return 0         ## RETURN
    #
    ymin_1 = y1.value() - y1.error () 
    ymax_1 = y1.value() + y1.error ()
    if ymin_1 >= ymax_1  : return 0         ## RETURN
    #
    ymin_2 = y2.value() - y2.error () 
    ymax_2 = y2.value() + y2.error ()
    if ymin_2 >= ymax_2  : return 0         ## RETURN
    #
    xmin = max ( xmin_1 , xmin_2 )
    xmax = min ( xmax_1 , xmax_2 )
    if xmin >= xmax      : return 0         ## RETURN 
    #
    ymin = max ( ymin_1 , xmin_2 )
    ymax = min ( ymax_1 , xmax_2 )
    if ymin >= ymax      : return 0         ## RETURN 
    #
    #
    return ( xmax - xmin ) * ( ymax - ymin ) / 4.0 / x1.error() / y1.error()  

# ==============================================================================
## rebin 1D-histogram with NUMBERS 
def _rebin_nums_1D_ ( h1 , template ) :
    """
    Rebin 1D-histogram assuming it is a histogram with *NUMBERS*

    >>> horig    = ...  ## the original historgam 
    >>> template = ...  ## the template with binnings

    >>> h = horig.rebinNumbers ( template ) 
    """
    ##
    # clone it!
    h2 = template.Clone( hID() )
    if not h2.GetSumw2() : h2.Sumw2()
    #
    ## reset the histogram 
    for i2 in h2 : h2[i2] = VE(0,0)
    #
    for i2 in h2.iteritems() :
        
        for i1 in h1.iteritems() :
            
            o = _bin_overlap_1D_ ( i1[1] , i2[1] )
            
            h2 [ i2[0] ] +=  o * i1[2] 
            
    return h2 
# =============================================================================
## rebin 1D-histogram as FUNCTION 
def _rebin_func_1D_ ( h1 , template ) :
    """
    Rebin 1D-histogram assuming it is a FUNCTION

    >>> horig    = ...  ## the original historgam 
    >>> template = ...  ## the template with binnings

    >>> h = horig.rebinFunction ( template ) 
    """
    # clone it!
    h2 = template.Clone( hID() )
    if not h2.GetSumw2() : h2.Sumw2()
    ## reset the histogram 
    for i2 in h2 : h2[i2] = VE(0,0)
    #
    for i2 in h2.iteritems() :
        
        for i1 in h1.iteritems() :
            
            o = _bin_overlap_1D_ ( i2[1] , i1[1] ) ## NOTE THE ORDER!!! 
            
            h2 [ i2[0] ] +=  o * i1[2]
            
    return h2 

# ==============================================================================
## rebin 2D-histogram with NUMBERS 
def _rebin_nums_2D_ ( h1 , template ) :
    """
    Rebin 2D-histogram assuming it is a histogram with *NUMBERS*

    >>> horig    = ...  ## the original historgam 
    >>> template = ...  ## the template with binnings

    >>> h = horig.rebinNumbers ( template ) 
    """
    #
    # clone it!
    h2 = template.Clone( hID() )
    if not h2.GetSumw2() : h2.Sumw2()
    ## reset the histogram 
    for i2 in h2 : h2[i2] = VE(0,0)
    #
    for i2 in h2.iteritems() :
        
        for i1 in h1.iteritems() :
            
            o = _bin_overlap_2D_ ( i1[2] , i1[3] , i2[2] , i2[3] )
            
            h2 [ (i2[0],i2[1]) ] +=  o * i1[4] 
            
    return h2 
# =============================================================================
## rebin 2D-histogram as FUNCTION 
def _rebin_func_2D_ ( h1 , template ) :
    """
    Rebin 2D-histogram assuming it is a FUNCTION

    >>> horig    = ...  ## the original historgam 
    >>> template = ...  ## the template with binnings

    >>> h = horig.rebinFunction ( template ) 
    """
    # clone it!
    h2 = template.Clone( hID() )
    if not h2.GetSumw2() : h2.Sumw2()
    ## reset the histogram 
    for i2 in h2 : h2[i2] = VE(0,0)
    #
    for i2 in h2.iteritems() :
        
        for i1 in h1.iteritems() :
            
            o = _bin_overlap_1D_ ( i2[2] , i2[3] , i1[2] , i2[3] ) ## NOTE THE ORDER!!! 
            
            h2 [ i2[0] ] +=  o * i1[4]
            
    return h2 

for t in ( ROOT.TH1F , ROOT.TH1D ) :
    t.rebinNumbers  = _rebin_nums_1D_
    t.rebinFunction = _rebin_func_1D_
    
for t in ( ROOT.TH2F , ROOT.TH2D ) :
    t.rebinNumbers  = _rebin_nums_2D_
    t.rebinFunction = _rebin_func_2D_

# =============================================================================
## Create NULL-line fo rthe histogram and (optionally) draw it
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-11-01
def _h1_null_ ( h1 , draw = False , style = 1 ) :
    """
    Create NULL-line for the histogram and (optionally) draw it
    
    """
    axis = h1.GetXaxis()
    line = ROOT.TLine ( axis.GetXmin() , 0 ,
                        axis.GetXmax() , 0 )
    
    line.SetLineStyle ( style )
    
    if draw :
        line.Draw()

    return line 

ROOT.TH1D.null = _h1_null_
ROOT.TH1F.null = _h1_null_

# =============================================================================
## convert histogram to graph
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def hToGraph ( h1                   ,
               funcx = lambda s : s ,
               funcv = lambda s : s ) :
    """
    Convert  1D-histogram into graph 
    """
    #
    ## book graph
    #
    graph = ROOT.TGraphErrors( len( h1 )  )
    
    #
    ## copy attributes
    #
    graph.SetLineColor   ( h1.GetLineColor   () )
    graph.SetLineWidth   ( h1.GetLineWidth   () )
    graph.SetMarkerColor ( h1.GetMarkerColor () )
    graph.SetMarkerStyle ( h1.GetMarkerStyle () )
    graph.SetMarkerSize  ( h1.GetMarkerSize  () )

    for i in h1.iteritems () :

        x = funcx  ( i[1] ) 
        v = funcv  ( i[2] )

        ## note the different convention 
        graph [ i[0] - 1 ] = (x,v)  
        
    return graph


# =============================================================================
# iterate over graph items
# =============================================================================
## iterate over points in TGraphError
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _gr_iter_ ( graph ) :
    """
    Iterate over graph points 
    
    >>> gr = ...
    >>> for i in gr : ...
    
    """
    
    for ip in range ( 0 , len ( graph ) ) :
        yield ip
        
# =============================================================================
## iterate over points in TGraphError
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _gr_iteritems_ ( graph ) :
    """
    Iterate over graph points 
    
    >>> gr = ...
    >>> for i,x,v in gr.iteritems(): ...
    
    """
    for ip in graph :

        point = graph[ ip ] 
        
        yield ip , point[0] , point[1] 
        
# =============================================================================
## get the point in TGraphError
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _gr_getitem_ ( graph , ipoint )  :
    """
    Get the point from the Graph
    """
    if not ipoint in graph : raise IndexError 
    #
    
    x_ = ROOT.Double(0)
    v_ = ROOT.Double(0)
    
    graph.GetPoint ( ipoint , x_ , v_ )
    
    x = VE ( x_ , graph.GetErrorX ( ipoint ) )
    v = VE ( v_ , graph.GetErrorY ( ipoint ) )
    
    return x,v

# =============================================================================
## set the point in TGraphError
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _gr_setitem_ ( graph , ipoint , point )  :
    """
    Get the point from the Graph
    """
    #
    if not ipoint in graph : raise IndexError 
    #
    
    x = point[0]
    v = point[1]

    graph.SetPoint      ( ipoint , x . value () , v . value () )
    graph.SetPointError ( ipoint , x . error () , v . error () )

# =============================================================================
ROOT.TGraphErrors.__len__       = ROOT.TGraphErrors . GetN 
ROOT.TGraphErrors.__contains__  = lambda s,i : i in range(0,len(s))
ROOT.TGraphErrors.__iter__      = _gr_iter_ 
ROOT.TGraphErrors.__iteritems__ = _gr_iteritems_ 
ROOT.TGraphErrors.__getitem__   = _gr_getitem_ 
ROOT.TGraphErrors.__setitem__   = _gr_setitem_ 

ROOT.TH1F.asGraph = hToGraph
ROOT.TH1D.asGraph = hToGraph
ROOT.TH1F.toGraph = hToGraph
ROOT.TH1D.toGraph = hToGraph

# =============================================================================
## convert histogram to graph
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def hToGraph_ ( h1 , funcx , funcy ) :
    """
    Convert  1D-histogram into TGraphAsymmErorr 

    """
    #
    ## book graph
    #
    graph = ROOT.TGraphAsymmErrors( len( h1 )  )
    
    #
    ## copy attributes
    #
    graph.SetLineColor   ( h1.GetLineColor   () )
    graph.SetLineWidth   ( h1.GetLineWidth   () )
    graph.SetMarkerColor ( h1.GetMarkerColor () )
    graph.SetMarkerStyle ( h1.GetMarkerStyle () )
    graph.SetMarkerSize  ( h1.GetMarkerSize  () )

    for i in h1.iteritems () :
        
        ip = i[0] - 1 ## different convention for TH1 and TGraph
        x  = i[1] 
        y  = i[2]

        x0 , xep , xen = funcx ( x , y )
        y0 , yep , yen = funcy ( x , y )
            
        graph.SetPoint      ( ip , x0  , y0  ) 
        graph.SetPointError ( ip , xep , xen , yep , yen ) 

    return graph


# =============================================================================
## convert histogram to graph
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def hToGraph2 ( h1 , bias ) :
    """
    Convert  1D-histogram into graph with small shift in x
    Useful for overlay of very similar plots

    >>> h1 = ....
    >>> g2 = h1.asGraph2 ( 0.1 ) ## shift for 10% of bin width
    
    """
    if abs ( bias ) > 1 :
        raise ValueErorr, ' Illegal value for "bias" parameter '
    
    funcx = lambda x,y : ( x.value() , x.error()*(1+bias) , x.error()*(1-bias) ) 
    funcy = lambda x,y : ( y.value() , y.error()          , y.error()          ) 
        
    return hToGraph_ ( h1 , funcx , funcy ) 


# =============================================================================
## convert histogram to graph
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def hToGraph3 ( h1 , bias ) :
    """
    Convert  1D-histogram into graph with small shift in x
    Useful for overlay of very similar plots

    >>> h1 = ....
    >>> g2 = h1.asGraph2 ( 0.1 ) ## shift for 10% of bin width
    
    """
    for p in h1.iteritems() :
        x = p[1]
        if x.error() < abs ( bias ) :
            raise ValueErorr, ' Illegal value for "bias" parameter '
        
    funcx = lambda x,y : ( x.value() , x.error()+bias , x.error()-bias )
    funcy = lambda x,y : ( y.value() , y.error()      , y.error()      ) 
        
    return hToGraph_ ( h1 , funcx , funcy ) 

ROOT.TGraphAsymmErrors.__len__       = ROOT.TGraphAsymmErrors . GetN 
ROOT.TGraphAsymmErrors.__contains__  = lambda s,i : i in range(0,len(s))
ROOT.TGraphAsymmErrors.__iter__      = _gr_iter_ 

ROOT.TH1F.asGraph2 = hToGraph2
ROOT.TH1D.asGraph2 = hToGraph2
ROOT.TH1F.toGraph2 = hToGraph2
ROOT.TH1D.toGraph2 = hToGraph2
ROOT.TH1F.asGraph3 = hToGraph3
ROOT.TH1D.asGraph3 = hToGraph3
ROOT.TH1F.toGraph3 = hToGraph3
ROOT.TH1D.toGraph3 = hToGraph3

# =============================================================================
## get edges from the axis:
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _edges_ ( axis ) :
    """
    Get list of edges form the TAxis

    >>> axis
    >>> edges = axis.edges() 
    """
    #
    bins  = [ axis.GetBinLowEdge ( i ) for i in axis ]
    bins += [ axis.GetXmax() ]
    #
    return tuple( bins )

# =============================================================================
ROOT.TAxis.edges = _edges_

# =============================================================================
## make axis from bin-edges 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def axis_bins ( bins         ) :
    """
    Make axis according to the binning 
    
    >>> bins = [ ... ] 
    >>> axis = axis_bins ( bins )  
    
    """
    #
    bins = set  ( bins )
    bins = [ i for i in bins ]
    bins.sort()
    # 
    assert ( 1 < len ( bins ) )
    #
    from numpy import array
    #
    return ROOT.TAxis ( 
        len ( bins ) - 1 , array ( bins , dtype='d' )
        ) 

# =============================================================================
## make 2D-histogram from axes
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def h2_axes ( x_axis            ,
              y_axis            ,
              title  = '2D'     , 
              name   = None     ,
              double = False    ) :
    """
    Make 2D-histogram with binning deifned by already created axes
    
    >>> x_axis = ...
    >>> y_axis = ...
    >>> h2 = h2_axes ( x_axis , y_axis , title = 'MyHisto' ) 
    
    """
    #
    if not name : name = hID() 
    #
    if not issubclass ( type ( x_axis ) , ROOT.TAxis ) : x_axis = axis_bins   ( x_axis )
    if not issubclass ( type ( y_axis ) , ROOT.TAxis ) : y_axis = axis_bins   ( y_axis )
    #
    # 
    x_bins  = x_axis.edges()
    y_bins  = y_axis.edges()
    #
    from numpy import array
    #
    typ = ROOT.TH2D if double else ROOT.TH2F
    return typ ( name  ,
                 title ,
                 len ( x_bins ) - 1 , array ( x_bins , dtype='d' ) ,
                 len ( y_bins ) - 1 , array ( y_bins , dtype='d' ) ) 


# =============================================================================
## make 1D-histogram from axis
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def h1_axis ( axis           ,
              title  = '1D'  , 
              name   = None  ,
              double = False ) :
    """
    Make 1D-histogram with binning deifned by already created axes
    
    >>> axis = ...
    >>> h1 = h1_axes ( axis , title = 'MyHisto' ) 
    
    """
    #
    if not name : name = hID()
    #
    if not issubclass ( type ( axis ) , ROOT.TAxis ) : axis = axis_bins   ( axis )
    # 
    bins  = axis.edges()
    #
    from numpy import array
    #
    typ = ROOT.TH1D if double else ROOT.TH1F
    return typ ( name  ,
                 title ,
                 len ( bins ) - 1 , array ( bins , dtype='d' ) ) 

# =============================================================================
## helper class to wrap 1D-histogram as function 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
class _H1Func(object) :
    """
    Helper class to Wrap 1D-histogram as function 
    """
    def __init__ ( self , histo , func = lambda s : s.value() ) :
        self._histo = histo
        self._func  = func
        
    ## evaluate the function 
    def __call__ ( self , x ) :
        """
        Evaluate the function 
        """
        #
        x0 = x if isinstance ( x , (int,long,float) ) else x[0]
        #
        return self._func ( self._histo ( x0 , interpolate = True ) )

# ==============================================================================
## helper class to wrap 2D-histogram as function 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
class _H2Func(object) :
    """
    Helper class to Wrap 2D-histogram as function 
    """
    def __init__ ( self , histo , func = lambda s : s.value() ) :
        self._histo = histo    
        self._func  = func
        
    ## evaluate the function 
    def __call__ ( self , x ) :
        """
        Evaluate the function 
        """
        x0 = x[0]
        y0 = x[1]
        return self._func ( self._histo ( x0 , y0 , interpolate = True ) )

# =============================================================================
## construct helper class 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h1_as_fun_ ( self , func = lambda s : s.value () ) :
    """
    construct the function fomr the histogram 
    """
    return _H1Func ( self , func )
# =============================================================================
## construct helper class 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h2_as_fun_ ( self , func = lambda s : s.value () ) :
    """
    construct the function fomr the histogram 
    """
    return _H2Func ( self , func )
# =============================================================================
## construct function 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h1_as_tf1_ ( self , func = lambda s : s.value () ) :
    """
    Construct the function from the 1D-histogram

    >>> fun = h1.asFunc()
    
    """
    ax  = self.GetXaxis()
    fun = _h1_as_fun_ ( self , func )
    #
    f1  = ROOT.TF1  ( funID()       ,
                      fun           ,
                      ax.GetXmin () ,
                      ax.GetXmax () )
    
    f1.SetNpx  ( 10 * ax.GetNbins() )

    f1._funobj = fun  
    f1._histo  = fun._histo
    f1._func   = fun._func
    
    return f1 
    
# =============================================================================
## construct function 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h2_as_tf2_ ( self , func = lambda s : s.value () ) :
    """
    Construct the function from the histogram

    >>> fun = h2.asFunc()
    
    """
    ax  = self.GetXaxis()
    ay  = self.GetYaxis()
    #
    fun = _h2_as_fun_ ( self , func )
    #
    f2  = ROOT.TF2  ( funID()       ,
                      fun           ,
                      ax.GetXmin () ,
                      ax.GetXmax () ,
                      ay.GetXmin () ,
                      ay.GetXmax () ) 
    
    f2.SetNpx  ( 10 * ax.GetNbins() )
    f2.SetNpy  ( 10 * ay.GetNbins() )
    
    f2._funobj = fun  
    f2._histo  = fun._histo
    f2._func   = fun._func
    
    return f2

    
ROOT.TH1F . asTF   = _h1_as_tf1_ 
ROOT.TH1D . asTF   = _h1_as_tf1_ 
ROOT.TH2F . asTF   = _h2_as_tf2_ 
ROOT.TH2D . asTF   = _h2_as_tf2_ 
ROOT.TH1F . asTF1  = _h1_as_tf1_ 
ROOT.TH1D . asTF1  = _h1_as_tf1_ 
ROOT.TH2F . asTF2  = _h2_as_tf2_ 
ROOT.TH2D . asTF2  = _h2_as_tf2_ 
ROOT.TH1F . asFunc = _h1_as_fun_ 
ROOT.TH1D . asFunc = _h1_as_fun_ 
ROOT.TH2F . asFunc = _h2_as_fun_ 
ROOT.TH2D . asFunc = _h2_as_fun_ 

# =======================================================================
## calculate the ``difference'' between two histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h_diff_ ( h1 , h2 , func = lambda s1,s2 : (s1/s2).value() ) :
    """
    Estimate the ``difference'' between two histograms

    """
    
    se = SE()
    
    for bin in h1 :
        
        v1 = h1 [ bin ] 
        v2 = h2 [ bin ] 
        
        se += func ( v1 , v2 )
        
    return se 


ROOT.TH1F.histoDiff = _h_diff_
ROOT.TH1D.histoDiff = _h_diff_
ROOT.TH2F.histoDiff = _h_diff_
ROOT.TH2D.histoDiff = _h_diff_
ROOT.TH3F.histoDiff = _h_diff_
ROOT.TH3D.histoDiff = _h_diff_


# =============================================================================
## perform some accumulation for the histogram 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h1_accumulate_ ( h                         ,
                     func = lambda s,v : s + v ,
                     cut  = lambda s   : True  , 
                     init = VE ()              ) :
    """
    Accumulate the function value over the histogram

    >>> h =...
    >>> sum = h.accumulate() 
    """
    result = init
    for i in h.iteritems() :
        if cut ( i ) : result = func ( result , i[2] )
    return result 

# =============================================================================
## perform some accumulation for the histogram 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h2_accumulate_ ( h                         ,
                      func = lambda s,v : s + v ,
                      cut  = lambda s   : True  , 
                      init = VE ()              ) :
    """
    Accumulate the function value over the histogram

    >>> h =...
    >>> sum = h.accumulate() 
    """
    result = init
    for i in h.iteritems() :
        if cut ( i ) : result = func ( result , i[4] )
    return result 

# =============================================================================
## get the sum of entries 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h1_sum_ ( h    ,
              low  ,
              high ) :
    """
    Get the histogram integral  over the specified range:
    
    >>> h = ....
    >>> h.sum ( 1 , 20 )
    
    """
    return _h1_accumulate_ ( h , cut = lambda s : low<=s[1].value()<=high ) 

# =============================================================================
## simple scaling
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h_scale_ ( histo , val = 1.0 ) :
    """
    Scale the histogram to certain integral

    >>> h = ...
    >>> h.scale ( 15 )
    
    """
    factor  = 0.0
    val     = float(val)
    if 0   != val :

        total = VE()
        for ibin in histo :
            total += histo[ ibin ]
        total = total.value()
        
        if 0 != total :
            factor = val/total

    if not histo.GetSumw2() : histo.Sumw2()

    for ibin in histo :

        value          = histo [ ibin ]
        value         *= factor
        histo[ ibin ]  = value 

    return histo

# =============================================================================
## simple shift of the histogram
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h1_shift_ ( h , bias ) :
    """
    Simple shift of the historgam :
    
    >>> h = ... # the histogram
    >>> h2 = h.shift ( -5 * MeV )
    
    """
    #
    if not h     .GetSumw2()  : h    .Sumw2()
    result = h.Clone( hID() ) ;
    result.Reset() ;
    if not result.GetSumw2() : result.Sumw2()
    #
    for i,x,y in result.iteritems() :
        
        y         += bias
        result[i]  = h ( y )
        
    return result      

# =============================================================================
## simple shift of the histogram
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h1_ishift_ ( h , ibias ) :
    """
    Simple shift of the historgam :
    
    >>> h = ...      # the histogram
    >>> h2 = h >> 5  # shift for 5 bins right 
    >>> h2 = h << 5  # shift for 3 bins left
    
    """
    #
    if not h     .GetSumw2()  : h    .Sumw2()
    result = h.Clone( hID() ) ;
    result.Reset() ;
    if not result.GetSumw2() : result.Sumw2()
    #
    for i in result :
        j = i - ibias
        if j in h :  result[i] = h[ j ] 
        
    return result      


ROOT.TH1F .   shift    = _h1_shift_
ROOT.TH1D .   shift    = _h1_shift_
ROOT.TH1D . __rshift__ = _h1_ishift_
ROOT.TH1F . __rshift__ = _h1_ishift_
ROOT.TH1D . __lshift__ = lambda s,i : _h1_ishift_ ( s , -1 * i ) 
ROOT.TH1F . __lshift__ = lambda s,i : _h1_ishift_ ( s , -1 * i ) 


# =============================================================================    
for t in ( ROOT.TH1F , ROOT.TH1D ) :    
    t . accumulate = _h1_accumulate_ 
    t . sum        = _h1_sum_ 
    t . integral   = _h1_sum_ 

for t in ( ROOT.TH2F , ROOT.TH2D ) :    
    t . accumulate = _h2_accumulate_ 
    t . sum        = _h2_accumulate_
    t . integral   = _h2_accumulate_ 

## generic
ROOT.TH1 . scale      = _h_scale_

    
HStats   = cpp.Gaudi.Utils.HStats

# =============================================================================
## calculate bin-by-bin momenta 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h1_moment_ ( h1 , order ) :
    """
    Get ``bin-by-bin''-moment around the specified value
    
    >>> histo = ...
    >>> mom   = histo.moment ( 4 , 0 ) 
    """
    #
    m = HStats.moment    ( h1 , order )
    e = HStats.momentErr ( h1 , order )
    #
    return VE ( m , e*e )  if 0<= e  else VE ( m , -e*e )

_h1_moment_ .__doc__ += '\n' + HStats.moment    .__doc__
_h1_moment_ .__doc__ += '\n' + HStats.momentErr .__doc__

# =============================================================================
## calculate bin-by-bin central momenta 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h1_central_moment_ ( h1 , order ) :
    """
    Get ``bin-by-bin'' central moment
    
    >>> histo = ...
    >>> cmom  = histo.centralMoment ( 4 ) 
    """
    #
    m = HStats.centralMoment    ( h1 , order )
    e = HStats.centralMomentErr ( h1 , order )
    #
    return VE ( m , e*e )  if 0<= e  else VE ( m , -e*e ) 

_h1_central_moment_ .__doc__ += '\n' + HStats.centralMoment    .__doc__
_h1_central_moment_ .__doc__ += '\n' + HStats.centralMomentErr .__doc__

# =============================================================================
## get skewness
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h1_skewness_ ( h1 ) :
    """
    Get the skewness

    >>> histo = ...
    >>> skew  = histo.skewness () 
    """
    m = HStats.skewness    ( h1 )
    e = HStats.skewnessErr ( h1 )
    #
    return VE ( m , e*e )  if 0<= e  else VE ( m , -e*e ) 

_h1_skewness_ .__doc__ += '\n' + HStats.skewness    .__doc__
_h1_skewness_ .__doc__ += '\n' + HStats.skewnessErr .__doc__

# =============================================================================
## get kurtosis
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h1_kurtosis_ ( h1 ) :
    """
    Get the kurtosis

    >>> histo = ...
    >>> k     = histo.kurtosis () 
    """
    m = HStats.kurtosis    ( h1 )
    e = HStats.kurtosisErr ( h1 )
    #
    return VE ( m , e*e )  if 0<= e  else VE ( m , -e*e ) 

_h1_kurtosis_ .__doc__ += '\n' + HStats.kurtosis    .__doc__
_h1_kurtosis_ .__doc__ += '\n' + HStats.kurtosisErr .__doc__

# =============================================================================
## get mean
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h1_mean_ ( h1 ) :
    """
    Get the mean

    >>> histo = ...
    >>> k     = histo.mean () 
    """
    m = HStats.mean    ( h1 )
    e = HStats.meanErr ( h1 )
    #
    return VE ( m , e*e )  if 0<= e  else VE ( m , -e*e ) 

_h1_mean_ .__doc__ += '\n' + HStats.mean    .__doc__
_h1_mean_ .__doc__ += '\n' + HStats.meanErr .__doc__

# =============================================================================
## get RMS
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _h1_rms_ ( h1 ) :
    """
    Get the rms
    
    >>> histo = ...
    >>> s     = histo.rms () 
    """
    m = HStats.rms    ( h1 )
    e = HStats.rmsErr ( h1 )
    #
    return VE ( m , e*e )  if 0<= e  else VE ( m , -e*e ) 

_h1_rms_ .__doc__ += '\n' + HStats.rms    .__doc__
_h1_rms_ .__doc__ += '\n' + HStats.rmsErr .__doc__

for h in ( ROOT.TH1F , ROOT.TH1D ) :
    
    h.mean           = _h1_mean_
    h.rms            = _h1_rms_
    h.skewness       = _h1_skewness_
    h.kurtosis       = _h1_kurtosis_
    h.moment         = _h1_moment_
    h.centralMoment  = _h1_central_moment_
    #
    h.nEff           = h.GetEffectiveEntries 

# =============================================================================
## adjust the "efficiency"
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def ve_adjust ( ve , mn = 0 , mx = 1.0 ) :
    """
    Adjust the efficiency
    """
    if ve.value() < mn : ve.setValue ( mn )
    if ve.value() > mx : ve.setValue ( mx )
    #
    return ve

# =============================================================================
## represent 1D-histo as Bernstein polynomial
def _h1_bernstein_ ( h1 , N , interpolate = True ) :
    """
    Represent histo as Bernstein polynomial
    
    >>> h = ... # the historgam
    >>> b = h.bernstein ( 5 )
    
    """
    mn,mx = h1.xminmax ()
    #
    ## N = min ( N ,  len ( h1 ) - 1 ) 
    b  = cpp.Gaudi.Math.Bernstein ( N , mn , mx )
    #
    for i in range ( 0 , N + 1 ) :

        if   i == 0   :
        
            x = mn 
            v = h1 [ 1       ].value()

        elif i == N   :
            
            x = mx 
            v = h1 [ len(h1) ].value()
            
        else :
            
            x = mn + ( mx - mn ) * float( i ) / float ( N )
            v = h1 ( x , interpolate = interpolate ).value()
            
        b.setPar ( i , v )

    #
    return b 

# =============================================================================
## represent 1D-histo as POSITIVE Bernstein polynomial
def _h1_positive_ ( h1 , N , interpolate = True ) :
    """
    Represent histo as Positive Bernstein polynomial
    
    >>> h = ... # the historgam
    >>> b = h.positive ( 5 )
    
    """
    mn,mx = h1.xminmax ()
    b = cpp.Gaudi.Math.Positive ( N , mn , mx )
    #
    for i in range ( 0 , N + 1 ) :

        if   i == 0   :
            
            x = mn 
            v = h1 [ 1       ].value()

        elif i == N   :
            
            x = mx 
            v = h1 [ len(h1) ].value()
            
        else :
            
            x = mn + ( mx - mn ) * float( i ) / float ( N )
            v = h1 ( x , interpolate = interpolate ).value()
            
        b.setPar ( i , abs ( v ) )

    #
    return b 

for t in ( ROOT.TH1D , ROOT.TH1F ) :
    t.bernstein  = _h1_bernstein_
    t.positive   = _h1_positive_
    
# =============================================================================


# =============================================================================
## fit the histogram by sum of components
def _h_Fit_ ( self                              ,
              components                        ,
              draw = False                      ,
              interpolate = True                ,
              selector    = lambda i,x,y : True ) :
    """
    (Chi_2)-fit the histogram with the set of ``components''
    
    The ``components'' could be histograms, functions and other
    callable object :
    
    >>> h0 = ...
    >>> h .hFit ( h0 )
    
    >>> h0 = ...
    >>> h1 = ...
    >>> h .hFit ( [ h0 , h1 ] )
    
    """
    DATA =   VE.Vector
    CMPS = DATA.Vector

    if   isinstance ( components , ROOT.TH1D ) : components = [ components ]
    elif isinstance ( components , ROOT.TH1F ) : components = [ components ]
    elif not isinstance ( components , ( tuple , list ) ) :
        components = [ components ]

    data = DATA ()
    cmps = CMPS ()
    
    while len ( cmps ) < len ( components )  :
        cmps.push_back( DATA() )

    for i,x,y in self.iteritems () :

        if not selector ( i , x , y ) : continue
        
        dp = VE ( y )
        data.push_back ( dp )
        
        for j in range ( 0 , len ( components ) ) :

            cmp = components[j]
            if isinstance ( cmp , ( ROOT.TH1F , ROOT.TH1D )) :
                cp = cmp ( x , interpolate   )
            else :
                cp =  VE ( cmp ( x.value() ) )  ## CALLABLE !!! 
            
            cmps[ j ].push_back ( cp ) 
            

    _c2Fit = cpp.Gaudi.Math.Chi2Fit ( data , cmps )

    if draw :
        
        if not hasattr ( self , '_histos_' ) :
            self._histos_ = []
            
        self.Draw( 'e1' ) 
        for j in range ( 0 , len ( components ) ) :
            cmpj = components [j]
            if not isinstance ( cmpj , ( ROOT.TH1F , ROOT.TH1D ) ) : continue
            sc = _c2Fit[j].value ()
            nh = cmpj * sc 
            nh.Draw ( 'same' )
            self._histos_.append ( nh ) 

    return _c2Fit


ROOT.TH1F. hFit = _h_Fit_ 
ROOT.TH1D. hFit = _h_Fit_ 

# =============================================================================
## fit histo
#  @see TH1::Fit 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-09-28
def _f_fit_ ( func , histo , *args ) :
    """
    Fit histogram (Actially delegate to TH1::Fit method)
    
    >>> func  = ...
    >>> histo = ...
    >>> func.Fit ( histo , .... )
    
    """
    return histo.Fit( func , *args )

ROOT.TF1 . Fit      = _f_fit_ 
ROOT.TF1 . fitHisto = _f_fit_ 
# =============================================================================
## draw the line for the histogram 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-01-21 
def _level_ ( self , level = 0 , linestyle = 2 ) :
    """
    Draw NULL-line for the histogram

    >>> h.level ( 5 )
    
    """
    mn,mx = self.xminmax() 
    line = ROOT.TLine ( mn , level , mx , level )
    line.SetLineStyle ( linestyle )
    self._line_ = line
    self._line_.Draw() 
    return self._line_
# =============================================================================
## draw null-level for histogram  
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-01-21 
def _null_ ( self , linestyle = 2 ) :
    """
    Draw NULL-line for the histogram
    
    >>> h.null() 
    """
    return _level_ ( self , 0 , linestyle ) 
# =============================================================================
## set color attributes  
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-01-21 
def _color_ ( self , color = 2 ) :
    """
    Set color attributes

    >>> h.color ( 3 ) 
    """
    #
    if hasattr ( self , 'SetLineColor'   ) : self.SetLineColor   ( color )
    if hasattr ( self , 'SetMarkerColor' ) : self.SetMarkerColor ( color )
    #
    return self
# =============================================================================
## set color attributes  
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-01-21 
def _red_  ( self ) : return _color_( self , 2 ) 
# =============================================================================
## set color attributes  
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-01-21 
def _blue_ ( self ) : return _color_( self , 4 ) 

ROOT.TH1D. level = _level_
ROOT.TH1F. level = _level_
ROOT.TH1D. null  = _null_
ROOT.TH1F. null  = _null_

ROOT.TH1D. color  = _color_
ROOT.TH1D. red    = _red_
ROOT.TH1D. blue   = _blue_

ROOT.TH1F. color  = _color_
ROOT.TH1F. red    = _red_
ROOT.TH1F. blue   = _blue_

# =============================================================================
logger.info ( 'Some useful decorations for TMinuit objects')
# ==============================================================================
## get the parameter form minuit 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-09-28
def _mn_par_ ( self , i ) :
    """
    Get the paraeter fomr minuit

    >>> mn = ...             # TMinuit object
    >>> p1 = mn[0]           # get the parameter 
    >>> p1 = mn.par(0)       # ditto 
    >>> p1 = mn.parameter(0) # ditto 
    """
    if not i in self : raise IndexError
    #
    ip  = ROOT.Long   ( i )
    val = ROOT.Double ( 0 )
    err = ROOT.Double ( 0 )
    #
    res = self.GetParameter ( ip , val , err )
    #
    return VE ( val , err*err )

ROOT.TMinuit . __contains__ = lambda s,i : isinstance(i,(int,long,ROOT.Long)) and 0<=i<s.GetNumPars() 
ROOT.TMinuit . __len__      = lambda s : s.GetNumPars() 

ROOT.TMinuit . par         = _mn_par_
ROOT.TMinuit . parameter   = _mn_par_
ROOT.TMinuit . __getitem__ = _mn_par_

# =============================================================================
## set the parameter 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-09-28
def _mn_set_par_ ( self , i , val , fix = False ) :
    """
    Set MINUIT parameter for some value
    """
    if not i in self : raise IndexError
    #
    ip = ROOT.Long ( i )
    if hasattr ( val , 'value' ) : val = val.value()
    #
    import array
    arglist = array.array('d',2*[0.])
    ierr    = ROOT.Long  ( -1 )
    #
    arglist[0] = i+1 
    arglist[1] = val
    self.mnexcm ( "SET PAR" , arglist , 2 , ierr )
    #
    if fix : self.FixParameter ( ip ) 
    #
    return ierr 

ROOT.TMinuit . setPar       = _mn_set_par_
ROOT.TMinuit . setParameter = _mn_set_par_

ROOT.TMinuit . fixPar       = lambda s,i,v: _mn_set_par_ ( s , i , v , True )
ROOT.TMinuit . fixParameter = lambda s,i,v: _mn_set_par_ ( s , i , v , True )

ROOT.TMinuit . __setitem__ = _mn_set_par_ 

# ===========================================================
## set the parameter 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-09-28
def _mn_min_ ( self                  ,
               maxcalls  = 5000      ,
               tolerance = 0.1       ,
               method    = 'MIGRADE' ) :
    """
    Perform the actual MINUIT minimization:

    >>> m = ... #
    >>> m.fit()       ## run migrade! 
    >>> m.migrade ()  ## ditto
    >>> m.fit ( method = 'MIN' ) 
    
    """
    #
    import array
    arglist = array.array ( 'd' , 2*[0.] )
    ierr    = ROOT.Long   (  -1          )
    #
    arglist[0] = maxcalls
    arglist[1] = tolerance
    #
    self.mnexcm ( method , arglist , 2 , ierr )
    #
    return ierr

ROOT.TMinuit . migrade  = _mn_min_
ROOT.TMinuit . fit      = _mn_min_
       
# =============================================================================
## set the parameter 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-09-28
def _mn_str_ ( self , l = 3 , v = 0.0 ) :
    """
    Print MINUIT information:

    >>> m = ...
    >>> print m
    
    """
    #
    self.mnprin ( l , v )
    return '\n'

ROOT.TMinuit . Print     =  _mn_str_ 
ROOT.TMinuit . __str__   =  _mn_str_ 
ROOT.TMinuit . __repr__  =  _mn_str_ 

# =============================================================================
## define/add parameter to TMinuit 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-09-28
def _mn_add_par_ ( self    , name      ,
                   start   , step = -1 ,
                   low = 0 , high = 0  ) :
    """
    Define/add parameter to MUNUIT

    >>> m.addPar ( 'ququ' , 10 , 0.1 )
    
    """
    if hasattr ( start , 'value' ) : start = start . value()
    if hasattr ( step  , 'value' ) : step  = step  . value()
    ## 
    if step < 0 : step = abs ( 0.01 * start ) 
    ##
    import array
    starts  = array.array ( 'd' , 1 * [ start ] )
    steps   = array.array ( 'd' , 1 * [ step  ] )
    #
    ipar    = len ( self ) 
    ierr    = ROOT.Long   ( 0 )
    self.mnparm ( ipar , name ,  start , step , low , high , ierr )
    #
    return ierr 

ROOT.TMinuit . addpar = _mn_add_par_
ROOT.TMinuit . addPar = _mn_add_par_
ROOT.TMinuit . defpar = _mn_add_par_
ROOT.TMinuit . defPar = _mn_add_par_
ROOT.TMinuit . newpar = _mn_add_par_
ROOT.TMinuit . newPar = _mn_add_par_

# =============================================================================
## get MINOS errors
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-09-28 
def _mn_minerr_ ( self , i ) :
    """
    Get MINOS errors for parameter:

    >>> m = ...       # TMinuit object
    >>> pos,neg = m.minosErr( 0 )
    
    """
    #
    if not i in self : raise IndexError
    #
    eplus  = ROOT.Double ( 0 ) 
    eminus = ROOT.Double ( 0 ) 
    epara  = ROOT.Double ( 0 ) 
    gcc    = ROOT.Double ( 0 ) 
    #
    self.mnerrs ( i , eplus , eminus , epara , gcc )
    #
    return eplus,eminus 

ROOT.TMinuit .   minErr  = _mn_minerr_ 
ROOT.TMinuit . minosErr  = _mn_minerr_ 
ROOT.TMinuit .   minErrs = _mn_minerr_ 
ROOT.TMinuit . minosErrs = _mn_minerr_ 

# =============================================================================
## Run MINOS
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-09-28 
def _mn_minos_ ( self , *args ) :
    """
    Get MINOS errors for parameter:
    
    >>> m = ...       # TMinuit object
    >>> pos,neg = m.minosErr( 0 )
    
    """
    ipars  = []
    for i in args :
        if not i in self : raise IndexError
        ipars.append ( i )
    #
    import array
    arglist = array.array ( 'd' , [ 200 ] + ipars  )
    ierr    = ROOT.Long   ( 0 )
    #
    self.mnexcm ( "MINOS" , arglist , len(arglist) , ierr )
    #
    return ierr

ROOT.TMinuit . minos = _mn_minos_


# =============================================================================
logger.info ( 'Some useful decorations for RooFit objects')
# =============================================================================
## iterator for RooArgList 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _ral_iter_ ( self ) :
    """
    Iterator for RooArgList:

    >>> arg_list = ...
    >>> for p in arg_list : print p
    
    """
    l = len ( self )
    for i in range ( 0 , l )  : yield self[i]

## some decoration over RooArgList 
ROOT.RooArgList . __len__       = lambda s   : s.getSize()
ROOT.RooArgList . __contains__  = lambda s,i :  0<= i < len(s)
ROOT.RooArgList . __iter__      = _ral_iter_ 


# =============================================================================
## iterator for RooArgSet
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _ras_iter_ ( self ) :
    """
    Simple iterator fior RotoArgSet:

    >>> arg_set = ...
    >>> for i in arg_set : print i
    
    """
    
    it  = self.createIterator()
    val = it.Next()
    while val :
        yield val 
        val = it.Next()
        
    del it
    
## some decoration over RooArgSet 
ROOT.RooArgSet . __len__       = lambda s   : s.getSize()
ROOT.RooArgSet . __iter__      = _ras_iter_ 


# =============================================================================
## iterator for RooDataSet
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _rds_iter_ ( self ) :
    """
    Iterator for RooDataSet 
    """
    _l = len ( self )
    for i in xrange ( 0 , _l ) : yield self.get ( i )

## some decoration over RooDataSet 
ROOT.RooDataSet . __len__       = lambda s   : s.numEntries()
ROOT.RooDataSet . __iter__      = _ras_iter_ 
        
        
# =============================================================================
## ``easy'' print of RooFitResult
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _rfr_print_ ( self , opts = 'v' ) :
    """
    Easy print of RooFitResult

    >>> result = ...
    >>> print result
    
    """
    self.Print( opts )
    return 'RooFitResult'

# =============================================================================
## get parameters from RooFitResult
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _rfr_params_ ( self ) :
    """
    GetParameters from RooFitResult:

    >>> result = ...
    >>> params = results
    >>> print params
    
    """
    pars  = self.floatParsFinal()
    pars_ = {}
    for p in pars :
        pars_ [ p.GetName() ] = p.as_VE(), p
    return pars_

# =============================================================================
## get parameter by name  from RooFitResult
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _rfr_param_  ( self , pname ) :
    """
    Get Parameter from RooFitResult by name 

    >>> result = ...
    >>> signal = results.param('Signal')
    >>> print signal
    """
    p = self.parameters()[ pname ] 
    return p 

# =============================================================================
## get the correlation coefficient
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _rfr_corr_  ( self , name1 , name2 ) :
    """
    Get correlation coefficient for two parameter 

    >>> result = ...
    >>> corr = results.corr('Signal', 'Background')
    >>> print corr
    """
    p1 = self.parameters()[ name1 ]
    p2 = self.parameters()[ name2 ]
    #
    return self.correlation ( p1[1] , p2[1] ) 

# =============================================================================
## get the covariance (sub) matrix 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
def _rfr_cov_  ( self , name1 , name2 ) :
    """
    Get covariance (sub) matrix 

    >>> result = ...
    >>> cov = results.cov('Signal', 'Background')
    >>> print corr
    """
    p1 = self.parameters()[ name1 ]
    p2 = self.parameters()[ name2 ]
    args = ROOT.RooArgList ( p1[1] , p2[1] ) 
    return self.reducedCovarianceMatrix (  args ) 


# =============================================================================

## some decoration over RooFitResult
ROOT.RooFitResult . __repr__   = _rfr_print_
ROOT.RooFitResult . __str__    = _rfr_print_
ROOT.RooFitResult . __call__   = _rfr_param_
ROOT.RooFitResult . parameters = _rfr_params_
ROOT.RooFitResult . params     = _rfr_params_
ROOT.RooFitResult . param      = _rfr_param_
ROOT.RooFitResult . parameter  = _rfr_param_
ROOT.RooFitResult . corr       = _rfr_corr_
ROOT.RooFitResult . cor        = _rfr_corr_
ROOT.RooFitResult . cov        = _rfr_cov_
ROOT.RooFitResult . parValue   = lambda s,n : s.parameter(n)[0]

# =============================================================================
## fix parameter at some value
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-09-20
def _fix_par_ ( var , value ) :
    """
    Fix parameter at some value :

    >>> par = ...
    >>> par.fix ( 10 ) 
    
    """
    #
    if hasattr ( value , 'value' ) : value = value.value()
    #
    var.setVal      ( value )
    var.setConstant ( True  )
    #
    return var.ve() 

# =============================================================================
## release the parameter
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-09-20
def _rel_par_ ( var )  :
    """
    Release the parameters

    >>> par = ...
    >>> par.release () 
    
    """
    var.setConstant ( False )
    #
    return var.ve()

# =============================================================================
## decorate RooRealVar:
ROOT.RooRealVar   . as_VE   = lambda s :  VE( s.getVal () , s.getError()**2 )
ROOT.RooRealVar   . ve      = lambda s :      s.as_VE  () 
ROOT.RooRealVar   . fix     = _fix_par_
ROOT.RooRealVar   . Fix     = _fix_par_
ROOT.RooRealVar   . release = _rel_par_
ROOT.RooRealVar   . Release = _rel_par_


# =============================================================================
## further decoration
import GaudiPython.HistoUtils
logger.info ( 'Histogram utilities from GaudiPython.HistoUtils' )

# =============================================================================
_HS = cpp.Gaudi.Utils.Histos.HistoStrings
# =============================================================================
## convert histogram to string
def _h_toString_ ( h , asXml = False  ) :
    """
    Convert histogram to string (or XML)

    >>> h = ... # the histo
    >>> s = h.toString()
    >>> print s 
    
    """
    return _HS.toString( h , asXml )

_h_toString_ . __doc__  += '\n' + _HS.toString. __doc__

# =============================================================================
## convert histogram to XML
def _h_toXml_ ( h ) :
    """
    Convert histogram to XML

    >>> h = ... # the histo
    >>> s = h.toXml()
    >>> print s 
    
    """
    return _HS.toXml( h )

_h_toXml_ . __doc__  += '\n' + _HS.toXml. __doc__

# =============================================================================
## convert XML to histogram 
def _h_fromString_ ( h , input  ) :
    """
    Convert string (or XLM) to histogram 

    >>> input  = ... # the string(or XML)
    >>> histo  = ... # book some the histo
    >>> status = histo.fromString ( input ) 

    """
    return _HS.fromString ( h , input )

_h_fromString_ . __doc__  += '\n' + _HS.fromString. __doc__

# =============================================================================
## convert XML to histogram 
def _h_fromXml_ ( h , xml ) :
    """
    Convert XML to histogram 

    >>> xml   = ... # the XML-string
    >>> histo = ... # book some the histo
    >>> status = histo.fromXml ( xml ) 

    """
    return _HS.fromXml ( h , xml )

_h_fromXml_ . __doc__  += '\n' + _HS.fromXml. __doc__


for t in ( ROOT.TH1D       ,
           ROOT.TH1F       ,
           ROOT.TH2D       ,
           ROOT.TH2F       ,     
           ROOT.TH3D       ,     
           ROOT.TProfile   ,
           ROOT.TProfile2D ) : 
    
    t.toString   = _h_toString_
    t.fromString = _h_fromString_
    t.toXml      = _h_toXml_
    t.fromXml    = _h_fromXml_

# =============================================================================
## define simplified print for TCanvas 
def _cnv_print_ ( cnv , fname , exts = [ 'eps' , 'pdf' , 'png' ] ) :
    """
    A bit simplified version for TCanvas print

    >>> canvas.print ( 'fig' )    
    """
    p = fname.rfind ('.')
    if    0 < p and p+4   == len ( fname ) : cnv.Print ( fname )
    elif  0 < p and p[p:] == '.ps'         : cnv.Print ( fname )
    else :
        for e in exts :
            cnv.Print ( fname + '.' + e )
            
    return cnv 

# =============================================================================
## define streamer for canvas 
def _cnv_rshift_ ( cnv , fname ) :
    """
    very simple print for canvas:
    
    >>> canvas >> 'a'
    
    """
    return _cnv_print_ ( cnv , fname )

ROOT.TCanvas.print_     = _cnv_print_
ROOT.TCanvas.__rshift__ = _cnv_rshift_

# =============================================================================
## HEPDATA format
# 
# @code 
# The format we accept data in is very wide and generally we require only
# a flat file containing the numerical values.
# Postscript and pdf figures are not suitable.
# 
# Ideally the format should be:
#
# xlow xhigh y +stat -stat +sys1 -sys1 +sys2 -sys2 ......
# where:
#  xlow and xhigh are the xbin edges
#  y is the measured quantity
# +stat and -stat are the positive and negative statistical errors (could also be +-stat)
# +sysn and -sysn are any number of positive and negative systematic errors (again could be +-sysn)
# @endcode 
# 
# =============================================================================
def _h1_hepdata_ ( h1 , fmt = " %13.6g %-13.6g   %13.6g +-%-12.5g \n" ) :
    '''
    Dump 1D-histogram in HepData -compatible format
    
    >>> h = ... # the histogram
    >>> data = h.toHepDATA ()
    
    """
    The format we accept data in is very wide and generally we require only
    a flat file containing the numerical values.
    Postscript and pdf figures are not suitable.
    
    Ideally the format should be:
    
    xlow xhigh y +stat -stat +sys1 -sys1 +sys2 -sys2 ......
    
    where:
    xlow and xhigh are the xbin edges
    y is the measured quantity
    +stat and -stat are the positive and negative statistical errors (could also be +-stat)
    +sysn and -sysn are any number of positive and negative systematic errors (again could be +-sysn)
    """
    '''    
    data = ''
    for  ibin in h1.iteritems () :
        
        x = ibin[1]
        y = ibin[2]
        
        x_low  = x.value() - x.error()
        x_high = x.value() + x.error()
        
        data += fmt  % ( x_low      ,
                         x_high     ,
                         y.value () ,
                         y.error () )
        
    return data

# ================================================================================================
def _h2_hepdata_ ( h2 ,
                   fmt = " %13.6g %-13.6g  %13.6g %-13.6g  %13.6g +-%-12.5g \n" )  : 
    """
    Dump 2D-histogram in HepData -compatible format
    
    >>> h = ... # the histogram
    >>> data = h.toHepDATA ()
    
    """
    data = ''
    for  ibin in h2.iteritems () :

        x = ibin[2]
        y = ibin[3]
        z = ibin[4]
        
        x_low  = x.value () - x.error ()
        x_high = x.value () + x.error ()
        y_low  = y.value () - y.error ()
        y_high = y.value () + y.error ()
        
        data += fmt  % ( x_low      ,
                         x_high     ,
                         y_low      ,
                         y_high     ,
                         z.value () ,
                         z.error () )
        
        
    return data
    
    
for t in ( ROOT.TH1D ,
           ROOT.TH1F ) :
    
    t . toHepDATA = _h1_hepdata_
    t . toHepData = _h1_hepdata_
    
for t in ( ROOT.TH2D ,
           ROOT.TH2F ) :
    
    t . toHepDATA = _h2_hepdata_
    t . toHepData = _h2_hepdata_
    
# =============================================================================
## import useful contetx managers
from AnalysisPython.Utils import *

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
