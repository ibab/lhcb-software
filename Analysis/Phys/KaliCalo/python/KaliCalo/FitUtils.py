#!/usr/bin/env python
# =============================================================================
# @file FitUtils.py
# @author Albert Puig Navarro (albert.puig@cern.ch)
# @reshuffled from Vanya Belyaev's Pi0HistoFit.py
# @date 2010-11-13
# =============================================================================
"""A module for helping with the fitting of histograms with pi0-mass"""

__author__ = "Albert Puig (albert.puig@cern.ch)"
__date__ = "$Date$"
__version__ = "$Revision$"

# =============================================================================
from ROOT  import TH1F, TF1
from math  import sqrt, pi,exp
from array import array


## few helper math utilities 
import LHCbMath.Types
import GaudiPython.HistoUtils 
import KaliCalo.Kali as Kali

VE      = Kali.VE 
pi0Mass = Kale.pi0Mass

# =============================================================================
## The function to fit pi0-mass distribution
class Pi0Fit:
    """
    The function to fit pi0-mass distribution
    """
    ## Set the proper normalization 
    def __init__ ( self ) :
        """
        Set the proper normalization 
        """
        self.norm = sqrt(2.0*pi) 

    def __call__ ( self , x , par ) :

        num   = float ( abs ( par[0] ) )
        num   = abs   ( num    ) 
        m0    = float ( par[1] )
        sigma = float ( par[2] )

        x0    = float ( x[0] ) 

        dm      = ( x0 - m0 ) / sigma 
        signal  = num/self.norm/sigma*exp(-0.5*dm*dm)

        bck   = abs ( float ( par[3] ) ) 
        slope =       float ( par[4] )
        curv  =       float ( par[5] )
        dm0   = x0 - pi0Mass
        
        background  = bck * ( 1.  + slope * dm0 * ( 1.0 + curv * dm0 )  ) 
        background *= sqrt ( x0 / pi0Mass ) 
        
        return signal + background 

pi0Func = TF1('Pi0Fit', Pi0Fit() , 0.0 , 250.0 , 6 )

# =============================================================================
## The function to fit subtracted pi0-mass distribution
class Pi0FitSubtracted:
    """
    The function to fit subtracted pi0-mass distribution
    """
    ## Set the proper normalization 
    def __init__ ( self ) :
        """
        Set the proper normalization 
        """
        self.norm = sqrt(2.0*pi) 

    def __call__ ( self , x , par ) :

        num   = float ( abs ( par[0] ) )
        num   = abs   ( num    ) 
        m0    = float ( par[1] )
        sigma = float ( par[2] )

        x0    = float ( x[0] ) 

        dm      = ( x0 - m0 ) / sigma 
        signal  = num/self.norm/sigma*exp(-0.5*dm*dm)

        bck   = abs ( float ( par[3] ) ) 
        slope =       float ( par[4] )
        curv  =       0 # No curvature
        dm0   = x0 - pi0Mass
        
        background  = bck * ( 1.  + slope * dm0 * ( 1.0 + curv * dm0 )  ) 
        background *= sqrt ( x0 / pi0Mass ) 
        
        return signal + background 
        
pi0SubtractedFunc = TF1('Pi0FitSubtracted', Pi0FitSubtracted() , 0.0 , 250.0 , 6 )

for func in [ pi0Func , pi0SubtractedFunc ]:
    func.SetParNames  (
        "N_{#pi^{0}}"      ,
        "m_{#pi^{0}}"      ,
        "#sigma_{#pi^{0}}" ,
        "Background"       ,
        "Slope"            ,
        "Curvature"  
        )  

    func.SetParameter ( 0 , 100 )
    func.SetParameter ( 1 , pi0Mass )
    func.SetParameter ( 2 ,  10 )
    func.SetParameter ( 3 ,  10 )
    func.SetParameter ( 4 ,   0 )
    func.SetParameter ( 5 ,   0 )


## ============================================================================
## initialize parameters 
_low    =  50.000
_high   = 240.000
_mass0  = pi0Mass
_sigma0 =  13.001
_slope0 =   0.001 
_curve0 =   0.000

_good_l   = 120 
_good_r   = 150 
_weight_s = 0.4
_weight_b = 1.0 - _weight_s

_low_sigma  =   5.2
_high_sigma =  20.0
_low_mass   = 120.0 
_high_mass  = 150.0 

## ============================================================================
## get the fitted pi0 parameters 
def getPi0Params ( histo ) :
    """
    Get the fitted pi0 parameters 
    """
    if not hasattr ( histo , 'GetFunction' ) :
        if hasattr ( histo , 'toROOT' ) :
            histo = histo.toROOT()
            return getPi0Params ( histo )
        
    _func = histo.GetFunction('Pi0Fit')
    if not _func :
        _func = histo.GetFunction('Pi0FitSubtracted')
        if not _func :
            return None
    
    return ( VE ( _func.GetParameter ( 0 ) , _func.GetParError ( 0 ) ** 2 ) ,
             VE ( _func.GetParameter ( 1 ) , _func.GetParError ( 1 ) ** 2 ) ,
             VE ( _func.GetParameter ( 2 ) , _func.GetParError ( 2 ) ** 2 ) , 
             VE ( _func.GetParameter ( 3 ) , _func.GetParError ( 3 ) ** 2 ) , 
             VE ( _func.GetParameter ( 4 ) , _func.GetParError ( 4 ) ** 2 ) , 
             VE ( _func.GetParameter ( 5 ) , _func.GetParError ( 5 ) ** 2 ) ) 


# =============================================================================
## simple check if the parameter is fixed 
def isFixed ( func , par ) :
    """
    Simple check if the parametr is fixed
    """
    pare = func.GetParError( par )
    return True if 0 == pare else False
    
# =============================================================================
## adjust parameters 0, 2 &  3 
def adjust03 ( func ) :
    """
    adjust parameters 0 & 3
    """
    par0 = func.GetParameter( 0 )
    if 0 > par0 : func.SetParameter ( 0 , abs ( par0 ) )
    par2 = func.GetParameter( 2 )
    if 0 > par2 : func.SetParameter ( 2 , abs ( par2 ) )
    par3 = func.GetParameter( 3 )
    if 0 > par3 : func.SetParameter ( 3 , abs ( par3 ) )

# =============================================================================
## adjust parameters 1 , 2  
def adjust12 ( func , mass , sigma , fixMass = False , fixSigma = False ) :
    """
    adjust parameters 1 & 2
    """
    sigma = abs ( sigma )

    par1  = func.GetParameter( 1 )
    par1  = abs  ( par1  )

    par1  = min  ( par1  , _high_mass )
    par1  = max  ( par1  , _low_mass  )

    if fixMass : func.FixParameter ( 1 , par1 )
    else       : func.SetParameter ( 1 , par1 )

    par2  = func.GetParameter( 2 )
    par2  = abs  ( par2  )

    par2  = min  ( par2  , _high_sigma )
    par2  = max  ( par2  , _low_sigma  )

    if fixSigma : func.FixParameter ( 2 , par2 )
    else        : func.SetParameter ( 2 , par2 )

    return par1,par2 

# =============================================================================
## good sigma ? 
def goodSigma ( sigma ) :
    """ Good sigma ? """
    return _low_sigma <= sigma <= _high_sigma
# =============================================================================
## good mass? 
def goodMass  ( mass  ) :
    """ Good mass? """
    return _low_mass <= mass   <= _high_mass

# =============================================================================
## get the histo integral 
def sumHisto ( histo , iL = -2**64 , iH = 2**64 ) :
    """
    Get the histo integral
    """
    sum  = 0 
    for iB in range ( 1 , min ( iH + 1 , histo.GetNbinsX() + 1 ) ) :
        if iL <= iB <= iH :
            sum += histo.GetBinContent( iB )
    return sum 

# ==============================================================================
## get fraction of bins with 'small' content 
def smallBins ( histo , limit = 6 , low = _low , high = _high )  :
    """
    get fractions of bins with 'small' content
    """
    n0 = 0
    nb = 0
    nbins = histo.GetNbinsX() 
    for iB in range ( 1 , nbins + 1 ) :
        c = histo.GetBinContent ( iB )
        v = histo.GetBinCenter  ( iB ) 
        if not low <= v <= high : continue 
        nb +=1 
        if c < limit : n0 += 1

    nb = max ( nb , 1 ) 
    return float(n0)/nb


# =============================================================================
# get ``signal-to-backgrund'' ratio
def s2b ( histo , r = 2.5 ) :
    """
    get ``signal-to-background'' ratio in +-2.5 sigma interga
    """
    ## AIDA ??
    if hasattr ( histo , 'toROOT' ) : return s2b ( histo.toROOT() , r )
    #
    par = getPi0Params ( histo )
    #
    m = par[1].value()
    s = par[2].value()
    r = abs ( r )
    #
    iL     = histo.FindBin ( m - r * s )
    iR     = histo.FindBin ( m + r * s )
    #
    return par[0]/(par[3]*(iR-iL+1))

# =============================================================================
## bad parameters ? 
def badParam ( mass  , sigma , num , corr ) :
    """
    Bad parameters ?
    """

    # bad mass ?
    if 10 < abs ( mass.value() - pi0Mass ) : return True 

    # bad sigma? 
    if not 7 <= sigma.value() <= 18        : return True

    # bad number ?
    if  num.value() < 25                   : return True

    # bad number ?
    if  num.value() / num.error () < 3.5   : return True

    # bad precision ?
    if not 0.00001 < corr.error() <= 0.01  : return True

    return False

## ============================================================================
## pre-fit background histogram 
def _preFitBkg ( func , histo , background = None , options = ''  ) :
    """
    pre-fit background histogram
    """

    if not hasattr ( histo , 'Fit' ) :
        if hasattr ( histo , 'toROOT' ) :
            histo = histo.toROOT()
            return _preFitBkg  ( func , histo , background )
        
    if background :
        if hasattr ( background , 'toROOT' ) :
            background = background.toROOT()
            return _preFitBkg  ( func , histo , background )

    opts = '0QS'
    if 0.20 < smallBins ( histo ) : opts += 'L'
    
    entries = histo.GetEntries()
    
    func.FixParameter ( 0 , 0       )
    func.FixParameter ( 1 , _mass0  )
    func.FixParameter ( 2 , _sigma0 )
    ##
    func.ReleaseParameter(3)
    func.ReleaseParameter(4) 
    func.ReleaseParameter(5)
    ##
    slope0 = _slope0
    curve0 = _curve0 
    if background and 50 < sumHisto ( background ) : 
        b = getPi0Params ( background )
        if b : 
            slope0 = b[4].value()
            curve0 = b[5].value()
    ##
    if entries < 20 :        
        func.FixParameter ( 4 , slope0 )
        func.FixParameter ( 5 , curve0 )
    else :
        func.SetParameter ( 4 , slope0 )
        func.SetParameter ( 5 , curve0 )
    
    ##
    iL     = histo.FindBin ( _mass0 - 3.0 * _sigma0 )
    iR     = histo.FindBin ( _mass0 + 3.0 * _sigma0 )
    backg0 = 0.5 *  abs ( histo.GetBinContent ( iL ) +
                          histo.GetBinContent ( iR ) )
    
    nFit = 0 
    ##
    func.SetParameter ( 3 , backg0 )
    st = histo.Fit( func, opts ,'',_low,_high)
    nFit += 1 
    if 0 != st.Status() and not isFixed ( func , 5 ) : 
        func.FixParameter ( 5 , curve0 ) 
        func.SetParameter ( 3 , backg0 )
        adjust03 ( func ) 
        st = histo.Fit( func, opts ,'',_low,_high)
        nFit += 1 
    if 0 != st.Status() and not isFixed ( func , 4  ) : 
        func.FixParameter ( 4 , slope0 ) 
        func.SetParameter ( 3 , backg0 )
        adjust03 ( func ) 
        st = histo.Fit ( func, opts ,'',_low,_high )        
        nFit += 1 
    if 0 != st.Status() :
        pars = getPi0Params  ( histo )
        print 'BACKGROUND: backg =%20s slope=%20s curve=%20s %8d %25s ' % (
            pars[3] ,
            pars[4] ,
            pars[5] ,
            int ( histo.GetEntries ( ) ) , 
            histo.GetName  () 
            )
    if options :
        adjust03 ( func ) 
        st = histo.Fit ( func, opts.replace('0','').replace('Q','') + options ,'',_low,_high )
        nFit += 1 
            
    ##
    func.ReleaseParameter(3)
    func.ReleaseParameter(4) 
    func.ReleaseParameter(5)
    
    histo.GetFunction( func.GetName() ).ResetBit( 1<<9 )
        
    return  0 == st.Status() , nFit  

# =============================================================================
## pre-fit background histogram 
def fitBkg ( func , histo , background = None , options = ''  ) :
    """
    Fit background histogram
    """
    return _preFitBkg ( func       ,
                        histo      ,
                        background ,
                        options    ) 

# =============================================================================
## pre-fit signal histogram 
def _preFitSignal ( func              ,
                    histo             ,
                    background = None ,
                    signal     = None ,
                    options    = ''   ) :
    """
    pre-fit signal histogram
    """
    if not hasattr ( histo , 'Fit' ) :
        if hasattr ( histo , 'toROOT' ) :
            histo = histo.toROOT()
            return _preFitSignal  ( func , histo , background , signal )
        
    if background :
        if hasattr ( background , 'toROOT' ) :
            background = background.toROOT()
            return _preFitSignal  ( func , histo , background , signal )
        
    if signal     : 
        if hasattr ( signal , 'toROOT' ) :
            signal = signal.toROOT()
            return _preFitSignal  ( func , histo , background , signal )
    
    func.ReleaseParameter ( 0 )
    func.ReleaseParameter ( 1 )
    func.ReleaseParameter ( 2 )
    func.ReleaseParameter ( 3 )
    func.ReleaseParameter ( 4 )
    func.ReleaseParameter ( 5 )
    ##
    num0   = 100 
    mass0  = _mass0 
    sigma0 = _sigma0
    
    iL     = histo.FindBin ( mass0 - 3.0 * sigma0 )
    iR     = histo.FindBin ( mass0 + 3.0 * sigma0 )
    backg0 = 0.5 *  abs ( histo.GetBinContent ( iL ) +
                          histo.GetBinContent ( iR ) )  

    num0  = sumHisto ( histo , iL , iR ) - backg0 * ( iR - iL + 1 ) 
    num0  = abs ( num0 )

    opts = '0QS'
    if 0.20 < smallBins ( histo ) : opts += 'L'

    slope0 = _slope0 
    curve0 = _curve0

    if signal and background :

        
        s = getPi0Params ( signal     )
        b = getPi0Params ( background )

        if s and b :
            mass0   = s[1].value()
            sigma0  = s[2].value()
            slope0  = s[4].mean(b[4]).value() 
            curve0  = s[5].mean(b[5]).value() 
        elif s : 
            mass0   = s[1].value()
            sigma0  = s[2].value()
            slope0  = s[4].value() 
            curve0  = s[5].value()
        elif b : 
            slope0  = b[4].value() 
            curve0  = b[5].value()
        
    elif signal and 50 < sumHisto ( signal ) :
        
        s = getPi0Params( signal )
        mass0   = s[1] . value ()
        sigma0  = s[2] . value ()
        slope0  = s[4] . value ()
        curve0  = s[5] . value ()
        
    elif  background and 50 < sumHisto ( background ) :
        
        b = getPi0Params ( background )
        if b : 
            slope0  = b[4] . value () 
            curve0  = b[5] . value () 

    if not 7   <= sigma0 <=  20 : sigma0 = _sigma0
    if not 0   <= backg0        : backg0 = 0.0
    if not 120 <= mass0  <= 150 : mass0  = _mass0 
    
    ## number of fits
    nFit = 0 
    ##     
    func.SetParameter ( 0 , num0   )
    
    ## 1. fix signal & background
    func.FixParameter ( 1 , mass0  )
    func.FixParameter ( 2 , sigma0 )
    func.FixParameter ( 4 , slope0 )
    func.FixParameter ( 5 , curve0 )
    st    = histo.Fit ( func, opts ,'', _low , _high )
    nFit += 1 
    ## 2. Release signal parameter 
    func.ReleaseParameter ( 1 )
    func.ReleaseParameter ( 2 )
    ## adjust 
    adjust03 ( func )
    st    = histo.Fit ( func, opts ,'', _low , _high )
    nFit += 1 
    if 0 != st.Status() and not isFixed ( func , 2 )  : 
        adjust12 ( func , mass0 , sigma0 ) 
        func.FixParameter ( 2 , sigma0 )
        adjust03 ( func ) 
        st = histo.Fit ( func, opts ,'', _low , _high )
        nFit += 1 
    ##
    ## adjust 
    adjust12 ( func , mass0 , sigma0 ) 
    adjust03 ( func ) 
    st = histo.Fit ( func, opts ,'', _low , _high )
    nFit += 1 
    ##
    ## 3. fix the signal and fit for background 
    mass1  = func.GetParameter ( 1 )
    sigma1 = func.GetParameter ( 2 )
    ## adjustment 
    mass1,sigma1 = adjust12 ( func , mass0 , sigma0 ) 
    ##    
    func.FixParameter ( 1 , mass1  )
    func.FixParameter ( 2 , sigma1 )
    func.ReleaseParameter ( 4 ) 
    func.ReleaseParameter ( 5 )
    ## adjust
    adjust03 ( func ) 
    st = histo.Fit ( func, opts ,'', _low , _high )
    nFit += 1    
    if 0 != st.Status() and not isFixed ( func , 5 )  : 
        func.FixParameter ( 5 , curve0 )
        adjust03 ( func ) 
        mass1,sigma1 = adjust12 ( func , mass0 , sigma0 ) 
        st = histo.Fit ( func, opts ,'', _low , _high )
        nFit += 1
    if 0 != st.Status() and not isFixed ( func , 4 ) :
        func.FixParameter ( 4 , slope0 )
        mass1,sigma1 = adjust12 ( func , mass0 , sigma0 ) 
        adjust03 ( func ) 
        st = histo.Fit ( func, opts ,'', _low , _high )
        nFit += 1 
    ##
    slope1 =  func.GetParameter ( 4 )
    curve1 =  func.GetParameter ( 5 )
    ##
    func.ReleaseParameter ( 1 )
    func.ReleaseParameter ( 2 )
    func.FixParameter ( 4 , slope1 )
    func.FixParameter ( 5 , curve1  )
    ##
    mass1,sigma1 = adjust12 ( func , mass0 , sigma0 ) 
    adjust03 ( func )
    ##
    ## 4. fit it!
    st = histo.Fit ( func, opts ,'', _low , _high )
    nFit += 1 
    if 0 == st.Status() :
        adjust03 ( func ) 
        mass_x,sigma_x = adjust12 ( func , mass0 , sigma0 ) 
        func.ReleaseParameter ( 4 )
        func.ReleaseParameter ( 5 )
        st = histo.Fit ( func, opts ,'', _low , _high )
        nFit += 1
    if 0 != st.Status() and not isFixed ( func , 5 ) : 
        func.FixParameter ( 5 , curve1 )
        mass1,sigma1 = adjust12 ( func , mass0 , sigma0 ) 
        adjust03 ( func ) 
        st = histo.Fit ( func, opts ,'', _low , _high )
        nFit += 1
    if 0 != st.Status() and not isFixed ( func , 4 ) : 
        func.FixParameter ( 4 , slope1 )
        mass1,sigma1 = adjust12 ( func , mass0 , sigma0 ) 
        adjust03 ( func ) 
        st = histo.Fit ( func, opts ,'', _low , _high )
        nFit += 1
    ##
    par0 = func.GetParameter ( 0 ) 
    if abs ( par0 ) > histo.GetEntries () :
        func.SetParameter ( 0 , histo.GetEntries() ) 
        func.SetParameter ( 3 , backg0 ) 
        func.SetParameter ( 1 , mass1  )
        if not isFixed ( func , 4 ) : func.FixParameter ( 4 , slope0 )
        if not isFixed ( func , 5 ) : func.FixParameter ( 5 , curve0 )
        mass1,sigma1 = adjust12 ( func , mass0 , sigma0 ) 
        adjust03 ( func ) 
        st = histo.Fit ( func, opts ,'', _low , _high )
        nFit += 1
    ##
    if 0 != st.Status () : 
        mass1,sigma1 = adjust12 ( func , mass0 , sigma0 ) 
        adjust03 ( func ) 
        st = histo.Fit ( func, opts ,'', _low , _high )
        nFit += 1 
    ##
    if 0 != st.Status() and not isFixed ( func , 5 ) : 
        func.FixParameter ( 5 , curve1 ) 
        mass1,sigma1 = adjust12 ( func , mass0 , sigma0 ) 
        adjust03 ( func ) 
        func.SetParameter( 1 , mass0 )
        st = histo.Fit ( func, opts ,'', _low , _high )
        nFit += 1 
    if  0 != st.Status() and not isFixed ( func , 4 ) :
        func.FixParameter ( 4 , slope1 ) 
        mass1,sigma1 = adjust12 ( func , mass0 , sigma0 ) 
        adjust03 ( func ) 
        func.SetParameter( 1 , mass0 )
        st = histo.Fit ( func, opts ,'', _low , _high )
        nFit += 1 
        
##     if 0 != st.Status () :
##         if background :
##             b = getPi0Params( background )
##             if not b :
##                 ok, nfitsb = fitBkg ( func , background )
##                 nFit += nfitsb 
##                 if ok  :
##                     ok , nfits2 = _preFitSignal ( func       ,
##                                                   histo      ,
##                                                   background ,
##                                                   signal     ,
##                                                   options    )
##                     nFits += nfits2
##                     if ok : return ok , nFits 

    ## the last adjustment: sigma and width
    if 0 != st.Status() or not goodSigma ( func.GetParameter ( 2 ) ) : 
        mass1 , sigma1 = adjust12 ( func , mass0 , sigma0 , fixSigma = True )
        adjust03 ( func ) 
        func.SetParameter( 1 , mass0 )        
        st = histo.Fit ( func, opts ,'', _low , _high )
        nFit += 1

    if 0 != st.Status() or not goodSigma ( func.GetParameter ( 2 ) ) : 
        mass1 , sigma1 = adjust12 ( func , mass0 , sigma0 , fixMass = True )
        if not isFixed ( func , 2 ) : func.FixParameter ( 2 , sigma1 ) 
        if not isFixed ( func , 4 ) : func.FixParameter ( 4 , slope0 ) 
        if not isFixed ( func , 5 ) : func.FixParameter ( 5 , curve0 )
        adjust03 ( func )
        st = histo.Fit ( func, opts ,'', _low , _high )
        nFit += 1
        func.ReleaseParameter ( 1 )        
        st = histo.Fit ( func, opts ,'', _low , _high )
        nFit += 1
            
    ## the really last adjustment: adjust everything ...
    if 0 != st.Status() or not goodSigma ( func.GetParameter ( 2 ) ) : 
        mass1 , sigma1 = adjust12 ( func , mass0 , sigma0 , fixSigma = True )
        adjust03 ( func ) 
        if not isFixed ( func , 3 ) : func.FixParameter ( 3 , backg0 ) 
        if not isFixed ( func , 4 ) : func.FixParameter ( 4 , slope0 ) 
        if not isFixed ( func , 5 ) : func.FixParameter ( 5 , curve0 )
        func.FixParameter ( 2 , sigma0 )        
        func.SetParameter ( 1 , mass0  )        
        st = histo.Fit ( func, opts ,'', _low , _high )
        nFit += 1

    ## the really last adjustment: adjust everything ... 
    if 0 != st.Status() or not goodSigma ( func.GetParameter ( 2 ) ) : 
        func.FixParameter ( 2 , _sigma0 )
        func.SetParameter ( 1 ,   mass0 )
        st = histo.Fit ( func, opts ,'', _low , _high )
        nFit += 1
        
    if 0 != st.Status() : 
        pars = getPi0Params  ( histo )
        print 'SIGNAL    : signal=%20s mass =%20s sigma=%20s %8d %25s ' % (
            pars[0] ,
            pars[1] ,
            pars[2] ,
            int ( histo.GetEntries () ) , 
            histo.GetName  () 
            )
        print 'SIGNAL    : backgr=%20s slope=%20s curve=%20s ' % (
            pars[3] ,
            pars[4] ,
            pars[5] )
        print 'SIGNAL    : backgr=%20s slope=%20s curve=%20s ' % (
            backg0  ,
            slope0  ,
            curve0  )
        print ' FIXED?   : ' , ( isFixed ( func , 0 ) ,
                                 isFixed ( func , 1 ) ,
                                 isFixed ( func , 2 ) ,
                                 isFixed ( func , 3 ) ,
                                 isFixed ( func , 4 ) ,
                                 isFixed ( func , 5 ) )
                               
    if options :
        adjust03 ( func ) 
        st = histo.Fit ( func, opts.replace('0','').replace('Q','') + options ,'', _low , _high )
        nFit += 1 

    histo.GetFunction( func.GetName() ).ResetBit( 1<<9 )

    return  0 == st.Status() , nFit 

# =============================================================================
## Fit signal histogram 
def fitSignal ( func  ,
                histo ,
                background = None ,
                signal     = None ,
                options    = ''   ) :
    """
    Fit signal histogram
    """
    return _preFitSignal ( func       ,
                           histo      ,
                           background ,
                           signal     ,
                           options    )

## ============================================================================
## Fit signal histogram
def fitPi0 ( histo ,
             background = None ,
             signal     = None ,
             options    = ''   ) :
    """
    Fit signal histogram
    """
    _func = pi0Func
    
    return fitSignal ( _func , histo , background , signal , options ) 
    

# =============================================================================
## check if the fit results are ``reasonable''
def checkHisto ( histo ) :
    """
    check if the fit results are ``reasonable''
    """

    pars = getPi0Params ( histo )
    if pars[0].value() > histo.GetEntries()     : return False
    if pars[1].value() > _mass0 + 2.5 * _sigma0 : return False
    if pars[1].value() < _mass0 - 2.5 * _sigma0 : return False
    if pars[2].value() > _high_sigma            : return False
    if pars[2].value() < _low_sigma             : return False
    ##
    return True

# EOF
