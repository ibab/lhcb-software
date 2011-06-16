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
    'makeGraph'            ## make ROOT Graph from input data
    )
# =============================================================================
import ROOT
from   GaudiPython.Bindings import gbl as cpp 
import LHCbMath.Types
VE             = cpp.Gaudi.Math.ValueWithError
ValueWithError = cpp.Gaudi.Math.ValueWithError

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
def _histo_get_item_ ( self , ibin ) :
    """
    ``Get-item'' for the histogram :
    
    >>> histo = ...
    >>> ve    = histo[ibin]
    """
    if not ibin in self : raise IndexError 
    val = self.GetBinContent ( ibin ) 
    err = self.GetBinError   ( ibin ) 
    return VE ( val , err * err ) 

# =============================================================================
## histogram as function 
def _histo_call_ ( self , x ) :
    """
    Histogram as function:
    
    >>> histo = ....
    >>> ve    = histo ( x ) 
    """    
    axis = self.GetXaxis()
    if not axis.GetXmin() <= x    <= axis.GetXmax  () : return VE(0,0)
    ##
    ibin = axis.FindBin ( x )
    if not 1              <= ibin <= axis.GetNbins () : return VE(0,0)
    ##
    val = self.GetBinContent ( ibin ) 
    err = self.GetBinError   ( ibin ) 
    return VE ( val , err * err ) 

ROOT.TH1   . __getitem__  = _histo_get_item_
ROOT.TH1F  . __getitem__  = _histo_get_item_
ROOT.TH1D  . __getitem__  = _histo_get_item_
ROOT.TH1   . __call__     = _histo_call_
ROOT.TH1   . __len__      = lambda s : s.GetNbinsX() * s.GetNbinsY() * s.GetNbinsZ()
ROOT.TH1   .   size       = lambda s : len ( s ) 
ROOT.TH1   . __contains__ = lambda s , i : 1<= i <= len ( s ) 

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
## use likelihood in histogram fit ? 
def useLL ( histo         ,
            minc  = 10    ,
            diff  = 1.e-4 ) :
    """
    Use Likelihood in histogram fit?
    """
    
    axis = histo.GetXaxis()
    bins = axis.GetNbins()
    
    diff = abs ( diff )
    
    minv = 1.e+9
    
    for ibin in histo :
        
        c = histo . GetBinContent ( ibin )
        e = histo . GetBinError   ( ibin )

        if c < 0                          : return False
        
        diff_ = max ( 1.0 , abs ( c ) ) * diff
        if abs ( c - long ( c ) ) > diff_ : return False 
        if abs ( c - e * e      ) > diff_ : return False 

        minv = min ( minv , c )

    if minv > abs ( minc )   : return False
    
    return True

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
        e = histo . GetBinError   ( ibin )
        if c < 0                            : return False

        diff_ = max ( 1.0 , abs ( c ) ) * diff
        if abs ( c   - long ( c ) ) > diff_ : return False
        
        if abs ( c - e * e ) > diff_        : return False 
        
    return True

ROOT.TH1.allInts = allInts

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
