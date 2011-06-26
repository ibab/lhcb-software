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
#                    $Revision$
#  Last modification $Date$
#  by                $Author$
# =============================================================================
"""

 Module with decoration of some ROOT objects for efficient use in PyROOT
 
"""
# =============================================================================
__version__ = "$Revision$"
__author__  = "Vanya BELYAEV Ivan.Belyaev@cern.ch"
__date__    = "2011-06-07"
# =============================================================================
__all__     = (
    #
    'rootID'         ,     ## construct the (global) unique ROOT identifier
    'funcID'         ,     ## construct the (global) unique ROOT identifier
    'funID'          ,     ## construct the (global) unique ROOT identifier
    'hID'            ,     ## construct the (global) unique ROOT identifier
    'histoID'        ,     ## construct the (global) unique ROOT identifier
    #
    'VE'             ,     ## Gaudi::Math::ValueWithError
    'ValueWithError' ,     ## Gaudi::Math::ValueWithError
    'histoGuess'     ,     ## guess the simple histo parameters
    'useLL'          ,     ## use LL for histogram fit?
    'allInts'        ,     ## natural histogram with natural entries?
    #
    'binomEff'       ,     ## calculate binomial efficiency 
    'binomEff_h1'    ,     ## calculate binomial efficiency for 1D-histos
    'binomEff_h2'    ,     ## calculate binomial efficiency for 2D-ihstos 
    'makeGraph'            ## make ROOT Graph from input data
    )
# =============================================================================
import ROOT
from   GaudiPython.Bindings import gbl as cpp 
import LHCbMath.Types
VE             = cpp.Gaudi.Math.ValueWithError
ValueWithError = cpp.Gaudi.Math.ValueWithError
binomEff       = cpp.Gaudi.Math.binomEff 

# =============================================================================
## global identifier for ROOT objects 
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
## global ROOT identified for histogram objects 
def histoID () : return rootID  ( 'h_' )
## global ROOT identified for histogram objects 
def hID     () : return histoID ( )
# =============================================================================
# Decorate histogram axis and iterators 
# =============================================================================

# =============================================================================
## iterator for histogram  axis 
def _axis_iter_1_ ( a ) :
    """
    Iterator for axis

    >>> axis = ...
    >>> for i in axis : 
    """
    i = 1
    s = a.GetNBins()
    while i <= s :
        yield i
        i+=1        
# =============================================================================
## iterator for histogram  axis 
def _axis_iter_2_ ( h ) :
    """
    Iterator for histogram axis
    
    >>> histo = ...
    >>> for i in histo : 
    """            
    i = 1
    s = h.GetXaxis().GetNbins()
    while i <= s :
        yield i
        i+=1
            
ROOT.TAxis . __iter__ = _axis_iter_1_
ROOT.TH1   . __iter__ = _axis_iter_2_

# =============================================================================
## get item for the histogram 
def _h_get_item_ ( h , ibin ) :
    """
    ``Get-item'' for the histogram :
    
    >>> histo = ...
    >>> ve    = histo[ibin]
    
    """
    #
    if not ibin in h  : raise IndexError 
    #
    val = h.GetBinContent ( ibin ) 
    err = h.GetBinError   ( ibin )
    #
    return VE ( val , err * err ) 


# =============================================================================
## histogram as function 
def _h1_call_ ( h1 , x ) :
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
    return VE ( val , err * err ) 

ROOT.TH1F  . __getitem__  = _h_get_item_
ROOT.TH1D  . __getitem__  = _h_get_item_
ROOT.TH1   . __call__     = _h1_call_

ROOT.TH1   . __len__      = lambda s : s.size() 
ROOT.TH1   .   size       = lambda s : s.GetNbinsX() * s.GetNbinsY() * s.GetBinsZ() 
ROOT.TH1   . __contains__ = lambda s , i : 1 <= i <= s.size() 

ROOT.TH2   . __contains__ = lambda s , i : 1 <= i <= s.size() 
ROOT.TH2   . __len__      = lambda s : s.size() 
ROOT.TH2   .   size       = lambda s : s.GetNbinsX() * s.GetNbinsY() * s.GetNbinsZ()


# =============================================================================
## histogram as function 
def _h2_call_ ( h2 , x , y ) :
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
    return VE ( val , err * err ) 

ROOT.TH2   . __call__     = _h2_call_
ROOT.TH2F  . __getitem__  = _h_get_item_
ROOT.TH2D  . __getitem__  = _h_get_item_

# =============================================================================
# iterate over items
# =============================================================================
## iterate over entries in 1D-histogram 
def _h1_iteritems_ ( h1 ) :
    """
    Iterate over histogram items:
    
    >>> h1 = ...
    >>> for i,x,y in h1 : ...
    
    """
    ax = h1.GetXAxis()
    sx = ax.GetNbins()
    ix = 1
    while ix <= sx :
        
        x   =       ax.GetBinCenter ( ix )
        xe  = 0.5 * ax.GetBinWidth  ( ix )
        
        y   =       h1.GetBinContent ( ix )
        ye  =       h1.GetBinError   ( ix )
        
        yield ix, VE(x,xe*xe) , VE ( y,ye*ye)
        
        ix += 1

# =============================================================================
## iterate over entries in 2D-histogram 
def _h2_iteritems_ ( h2 ) :
    """
    Iterate over histogram items:
    
    >>> h2 = ...
    >>> for ix,iy,x,y,z in h2 : ...
    
    """
    ax = h2.GetXaxis()
    sx = ax.GetNbins()

    ay = h2.GetYaxis()
    sy = ay.GetNbins()
    
    ix = 1
    while ix <= sx :        
        x   =       ax.GetBinCenter ( ix )
        xe  = 0.5 * ax.GetBinWidth  ( ix )
        #
        iy  = 1
        while iy <= sy :
            #
            y   =       ay.GetBinCenter  ( iy      )
            ye  = 0.5 * ay.GetBinWidth   ( iy      )
            #
            z   =       h2.GetBinContent ( ix , iy )
            ze  =       h2.GetBinError   ( ix , iy )
            #
            yield ix, iy, VE(x,xe*xe) , VE ( y,ye*ye) , VE( z,ze*ze) 
            #
            iy += 1
            
        ix += 1 
        
ROOT.TH2   . iteritems     = _h2_iteritems_
        
# =============================================================================
# Decorate fit results 
# =============================================================================

# =============================================================================
## representaion of TFitResult object 
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
#    - backhround level under the signal (per bin)
#    - background slope
def histoGuess ( histo , mass , sigma ) :
    """
    Get the guess for three major parameters of the histogram:
    - number of signal events
    - backhround level under the signal (per bin)
    - background slope
    - minimal and maximal slopes to ensure 'positive' background

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
    for ibin in histo :

        xbin = axis.GetBinCenter ( i )
        dx   = ( xbin - mass ) / sigma
        val  = histo.GetBinContent( i )
        
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


ROOT.TH1.histoGuess = histoGuess

# =============================================================================
def _int ( ve , precision = 1.e-5 ) :
    #
    if not hasattr ( ve , 'value' ) :
        return _int ( VE ( ve , abs ( ve ) ) , precision )  
    #
    diff = max ( 1 , abs ( ve.value() ) ) * precision 
    diff = min ( 0.1 , diff ) 
    # 
    if abs ( ve.value() - long ( ve.value() ) ) > diff : return False 
    if abs ( ve.value() -        ve.cov2 ()   ) > diff : return False
    #
    return True 


# =============================================================================
## use likelihood in histogram fit ? 
def useLL ( histo         ,
            minc  = 10    ,
            diff  = 1.e-5 ) :
    """
    Use Likelihood in histogram fit?
    """
    
    axis = histo.GetXaxis()
    bins = axis.GetNbins()
    
    minv = 1.e+9    
    for ibin in histo :
        
        c = histo . GetBinContent ( ibin )
        if c < 0                                : return False
        
        e = histo . GetBinError   ( ibin )
        if not _int ( VE ( c , e * e ) , diff ) : return False 
        
        minv = min ( minv , c )

    return  minv < abs ( minc ) 


ROOT.TH1.useLL = useLL

# =============================================================================
## Natural histiogram with all integer entries ?
def allInts ( histo         ,
              diff  = 1.e-4 ) :
    """
    Natural histiogram with all integer entries ?
    """
    
    axis = histo.GetXaxis()
    bins = axis.GetNbins()
    
    diff = abs ( diff )
    
    for ibin in histo : 
        
        c = histo . GetBinContent ( ibin )
        if c < 0                                : return False
        
        e = histo . GetBinError   ( ibin )
        if not _int ( VE ( c , e * e ) , diff ) : return False
        
    return True

ROOT.TH1.allInts = allInts

# =============================================================================
## calculate the efficiency histogram using the binomial erorrs 
def binomEff_h1 ( h1 , h2 ) :
    """
    Calculate the efficiency histogram using the binomial erorrs
    """
    func = binomEff
    #
    if                                 not h1.GetSumw2() : h1.Sumw2()
    if hasattr ( h1 , 'GetSumw2' ) and not h2.GetSumw2() : h2.Sumw2()
    #
    result = h1.Clone( hID () )
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
        v = func ( l1 , l2 )
        #
        result.SetBinContent ( i1 , v.value () ) 
        result.SetBinError   ( i1 , v.error () )
        
    return result 

ROOT.TH1.  binomEff    = binomEff_h1 

# =============================================================================
## calculate the efficiency histogram using the binomial erorrs 
def binomEff_h2 ( h1 , h2 ) :
    """
    Calculate the efficiency histogram using the binomial erorrs
    """
    func = binomEff
    #
    if                                 not h1.GetSumw2() : h1.Sumw2()
    if hasattr ( h1 , 'GetSumw2' ) and not h2.GetSumw2() : h2.Sumw2()
    #
    result = h1.Clone( hID () )
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
        v = func ( l1 , l2 )
        #
        result.SetBinContent ( ix1 , iy1 , v.value () ) 
        result.SetBinError   ( ix1 , iy1 , v.error () ) 
        
    return result 

ROOT.TH2.  binomEff    = binomEff_h2 


ROOT.TH1.__floordiv__  = binomEff_h1 
ROOT.TH2.__floordiv__  = binomEff_h2

# =============================================================================
## operation with the histograms 
def _h1_oper_ ( h1 , h2 , oper ) :
    """
    Operation with the histogram 
    """
    if                                 not h1.GetSumw2() : h1.Sumw2()
    if hasattr ( h1 , 'GetSumw2' ) and not h2.GetSumw2() : h2.Sumw2()
    #
    result = h1.Clone( hID() )
    #
    for i1,x1,y1 in h1.iteritems() :
        #
        result.SetBinContent ( i1 , 0 ) 
        result.SetBinError   ( i1 , 0 )
        #
        y2 = h2 ( x1.value() ) 
        #
        v = oper ( y1 , y2 ) 
        #
        result.SetBinContent ( i1 , v.value () ) 
        result.SetBinError   ( i1 , v.error () )
        
    return result

# =============================================================================
##  Division with the histograms 
def _h1_div_ ( h1 , h2 ) :
    """
    Divide the histograms 
    """
    return _h1_oper_ ( h1 , h2 , lambda x,y : x/y ) 
# =============================================================================
##  Division with the histograms 
def _h1_mul_ ( h1 , h2 ) :
    """
    Multiply the histograms 
    """
    return _h1_oper_ ( h1 , h2 , lambda x,y : x*y ) 
# =============================================================================
##  Addition with the histograms 
def _h1_add_ ( h1 , h2 ) :
    """
    Add the histograms 
    """
    return _h1_oper_ ( h1 , h2 , lambda x,y : x+y ) 
# =============================================================================
##  Subtraction of the histograms 
def _h1_sub_ ( h1 , h2 ) :
    """
    Subtract the histogram 
    """
    return _h1_oper_ ( h1 , h2 , lambda x,y : x-y ) 
# =============================================================================
##  Fraction of the histograms 
def _h1_frac_ ( h1 , h2 ) :
    """
    ``Fraction'' the histogram h1/(h1+h2)
    """
    return _h1_oper_ ( h1 , h2 , lambda x,y : x.frac(y) ) 
# =============================================================================
##  ``Asymmetry'' of the histograms 
def _h1_asym_ ( h1 , h2 ) :
    """
    ``Fraction'' the histogram (h1-h2)/(h1+h2)
    """
    return _h1_oper_ ( h1 , h2 , lambda x,y : x.asym(y) ) 
# =============================================================================
##  ``Chi2-tension'' of the histograms 
def _h1_chi2_ ( h1 , h2 ) :
    """
    ``Chi2-tension'' the histogram
    """
    return _h1_oper_ ( h1 , h2 , lambda x,y : VE ( x.chi2 ( y ) , 0 ) ) 
# =============================================================================
##  ``Average'' of the histograms 
def _h1_mean_ ( h1 , h2 ) :
    """
    ``Chi2-tension'' the histogram
    """
    return _h1_oper_ ( h1 , h2 , lambda x,y : x.mean ( y ) ) 

# =============================================================================
## 'pow' the histograms 
def _h1_pow_ ( h1 , val ) :
    """
    ``pow'' the histogram 
    """
    if not h1.GetSumw2() : h1.Sumw2()
    #
    result = h1.Clone( hID() )
    #
    for i1,x1,y1 in h1.iteritems() :
        #
        result.SetBinContent ( i1 , 0 ) 
        result.SetBinError   ( i1 , 0 )
        #
        v = pow ( y1 , val ) 
        #
        result.SetBinContent ( i1 , v.value () ) 
        result.SetBinError   ( i1 , v.error () )
        
    return result 

    
ROOT.TH1.__div__   = _h1_div_
ROOT.TH1.__mul__   = _h1_mul_
ROOT.TH1.__add__   = _h1_add_
ROOT.TH1.__sub__   = _h1_sub_
ROOT.TH1.__pow__   = _h1_pow_

ROOT.TH1.  frac    = _h1_frac_
ROOT.TH1.  asym    = _h1_asym_
ROOT.TH1.  chi2    = _h1_chi2_
ROOT.TH1.  average = _h1_chi2_

# =============================================================================
## operation with the histograms 
def _h2_oper_ ( h1 , h2 , oper ) :
    """
    Operation with the histogram 
    """
    if                                 not h1.GetSumw2() : h1.Sumw2()
    if hasattr ( h2 , 'GetSumw2' ) and not h2.GetSumw2() : h2.Sumw2()
    #
    result = h1.Clone( hID() )
    #
    for ix1,iy1,x1,y1,z1 in h1.iteritems() :
        #
        result.SetBinContent ( ix1 , iy1 , 0 ) 
        result.SetBinError   ( ix1 , iy1 , 0 )
        #
        z2 = h2 ( x1.value() , y1.value() ) 
        #
        v = oper ( z1 , z2 )
        print 'y1,y2,v :',  z1 , z2 , v 
        #
        result.SetBinContent ( ix1 , iy1 , v.value () ) 
        result.SetBinError   ( ix1 , iy1 , v.error () )
        
    return result


# =============================================================================
##  Division with the histograms 
def _h2_div_ ( h1 , h2 ) :
    """
    Divide the histograms 
    """
    return _h2_oper_ ( h1 , h2 , lambda x,y : x/y ) 
# =============================================================================
##  Division with the histograms 
def _h2_mul_ ( h1 , h2 ) :
    """
    Multiply the histograms 
    """
    return _h2_oper_ ( h1 , h2 , lambda x,y : x*y ) 
# =============================================================================
##  Addition with the histograms 
def _h2_add_ ( h1 , h2 ) :
    """
    Add the histograms 
    """
    return _h2_oper_ ( h1 , h2 , lambda x,y : x+y ) 
# =============================================================================
##  Subtraction of the histograms 
def _h2_sub_ ( h1 , h2 ) :
    """
    Subtract the histogram 
    """
    return _h2_oper_ ( h1 , h2 , lambda x,y : x-y ) 
# =============================================================================
##  ``Fraction'' of the histograms 
def _h2_frac_ ( h1 , h2 ) :
    """
    ``Fraction'' the histogram h1/(h1+h2)
    """
    return _h2_oper_ ( h1 , h2 , lambda x,y : x.frac(y) ) 
# =============================================================================
##  ``Asymmetry'' of the histograms 
def _h2_asym_ ( h1 , h2 ) :
    """
    ``Asymmetry'' the histogram (h1-h2)/(h1+h2)
    """
    return _h2_oper_ ( h1 , h2 , lambda x,y : x.asym(y) ) 
# =============================================================================
##  ``Chi2-tension'' the histograms 
def _h2_chi2_ ( h1 , h2 ) :
    """
    ``Chi2-tension'' for the histograms 
    """
    return _h2_oper_ ( h1 , h2 , lambda x,y : VE ( x.chi2 ( y ) , 0 ) ) 
# =============================================================================
##  ``Average'' the histograms 
def _h2_mean_ ( h1 , h2 ) :
    """
    ``Average'' for the histograms 
    """
    return _h2_oper_ ( h1 , h2 , lambda x,y : x.mean ( y ) )  

# =============================================================================
## 'pow' the histograms 
def _h2_pow_ ( h1 , val ) :
    """
    ``pow'' the histogram 
    """
    if not h1.GetSumw2() : h1.Sumw2()
    #
    result = h1.Clone( hID() )
    #
    for ix1,iy1,x1,y1,z1 in h1.iteritems() :
        #
        result.SetBinContent ( ix1 , iy1 , 0 ) 
        result.SetBinError   ( ix1 , iy1 , 0 )
        #
        v = pow ( y1 , val ) 
        #
        result.SetBinContent ( ix1 , iy1 , v.value () ) 
        result.SetBinError   ( ix1 , iy1 , v.error () )
        
    return result 
    
ROOT.TH2.__div__  = _h2_div_
ROOT.TH2.__mul__  = _h2_mul_
ROOT.TH2.__add__  = _h2_add_
ROOT.TH2.__sub__  = _h2_sub_
ROOT.TH2.__pow__  = _h2_pow_

ROOT.TH2.  frac    = _h2_frac_
ROOT.TH2.  asym    = _h2_asym_
ROOT.TH2.  chi2    = _h2_chi2_
ROOT.TH2.  average = _h2_mean_


def _h2_box_  ( self , opts = '' ) : return self.Draw( opts + 'box' )
def _h2_lego_ ( self , opts = '' ) : return self.Draw( opts + 'lego')

ROOT.TH2.  box  = _h2_box_
ROOT.TH2.  lego = _h2_lego_


# =============================================================================
## make graph from data 
def makeGraph ( x , y = []  , ex = [] , ey = [] ) :

    """
    Make graph using primitive data
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
if '__main__' == __name__ :
    
    print 80*'*'
    print __doc__
    print ' Author  : ' , __author__
    print ' Version : ' , __version__
    print ' Date    : ' , __date__    
    print 80*'*'
    
# =============================================================================
# The END 
# =============================================================================
