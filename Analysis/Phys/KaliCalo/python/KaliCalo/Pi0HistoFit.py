#!/usr/bin/env python
# =============================================================================
# $Id: Pi0HistoFit.py,v 1.5 2010-03-22 18:24:00 ibelyaev Exp $ 
# =============================================================================
"""
A module for fitting the histograms with pi0-mass

"""
# =============================================================================
__author__  = " ??? "
__date__    = " 2009-12-?? "
__version__ = " CVS Tag $Name: not supported by cvs2svn $, version $Revision: 1.5 $ "
# =============================================================================
from ROOT  import TH1F, TF1
from math  import sqrt, pi,exp
from array import array


## few helper math utilities 
import LHCbMath.Types
from   GaudiPython.Bindings import gbl as cpp
import GaudiPython.HistoUtils 
VE = cpp.Gaudi.Math.ValueWithError


def HiFit(FilledHistos, alam):
    """
    Module for running over all the histograms and calculation of the
    improved correction coefficients. Returnes a dictionary of the
    coefficients.
    """
    # loop over all the cells
    for i in alam.keys():
        # fitting and calculating the new correction coefficient
        alam[i]=alam[i]*0.135/FitProcess(FilledHistos[i])
    return alam

def FitProcess(histo):
    """
    A module for performing a fit of the histogram and return the pi0 mass
    Fit function is a sum of the Gauss and the second order polinom. Gauss mean
    value is returned as the "pi0 mass".
    """
    #== extracting the histogram parameters
    left=histo.GetXaxis().GetXmin()        # left limit
    right=histo.GetXaxis().GetXmax()       # right limit
    binwidth = histo.GetBinWidth(1)        # width of the bin

    #== true pi0 peak parameters
    trueMass = 0.135                       # initial gauss mean
    trueSigm = 0.0094                      # initial gauss sigma
    norm     = 1.0/(sqrt(2.0*pi)*trueSigm) # gauss constant

    #== check if there are enough events in the histogram
    if(histo.GetEntries()<=(right-left)/binwidth):
        print "Bad fit, returned coefficient = 1"
        return trueMass

    #== array of the fit parameters
    npar     = 12                          # number of fit parameters
    par      = array("d",npar*[0.0])       # par[0-5] are fit parameters
                                           # par[6-11] are the errors of fit parameters

    #== fit parameters bounds
    massmin = 0.120                        # min mean value
    massmax = 0.150                        # max mean value
    sigmmin = 0.005                        # min sigma value
    sigmmax = 0.015                        # max sigma value

    #== gauss fit limits
    gaussleft  = trueMass-2*trueSigm       # (mean+/-2sigma)
    gaussright = trueMass+2*trueSigm 

    #== the functions
    polinom = TF1("polinom","1.0+[1]*(x-[0])+[2]*(x-[0])*(x-[0])") # 2nd order polinom
    gauss = TF1("gauss","[0]*exp(-0.5*((x-[1])**2)/([2]**2))")     # gauss function
    fitfunc = TF1("fitfunc","gauss+polinom")                       # sum of the gauss and the polinom
    gauss.SetParameter(1,trueMass)
    gauss.SetParameter(2,trueSigm)
    
    #== fitting
    histo.Fit("gauss","Q","",gaussleft,gaussright)
    parGaus=gauss.GetParameters()

    histo.Fit("polinom","Q","",left,right)
    parPol=polinom.GetParameters()

    #== setting the parameters of the total fit function
    par[0],par[1],par[2]=parGaus[0],parGaus[1],parGaus[2]
    par[3],par[4],par[5]=parPol[0],parPol[1],parPol[2]

    fitfunc.SetParameters(par)
    fitfunc.SetParLimits(1,massmin,massmax)
    fitfunc.SetParLimits(2,sigmmin,sigmmax)
    
    #== fitting
    histo.Fit("fitfunc","Q")

    #== getting the fit parameters and their errors
    for i in range(npar/2):
        par[i]=fitfunc.GetParameter(i)   # parameters
        par[i+6]=fitfunc.GetParError(i)  # errors

    #== return the gauss mean value and it's error
    return par[1]


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
        dm0   = x0 - 135.0
        
        background  = bck * ( 1.  + slope * dm0 * ( 1.0 + curv * dm0 )  ) 
        background *= sqrt ( x0 / 135.0 ) 
        
        return signal + background 

pi0Func = TF1('Pi0Fit', Pi0Fit() , 0.0 , 250.0 , 6 )
    
pi0Func.SetParNames  (
    "Signal"     ,
    "Mass"       ,
    "Sigma"      ,
    "Background" ,
    "Slope"      ,
    "Curvature"  
    )  

pi0Func.SetParameter ( 0 , 100 )
pi0Func.SetParameter ( 1 , 135 )
pi0Func.SetParameter ( 2 ,  10 )
pi0Func.SetParameter ( 3 ,  10 )
pi0Func.SetParameter ( 4 ,   0 )
pi0Func.SetParameter ( 5 ,   0 )

_low    =  50.000
_high   = 250.000
_mass0  = 135.000
_sigma0 =  13.001
_slope0 =   0.001 
_curve0 =   0.000

_good_l   = 120 
_good_r   = 150 
_weight_s = 0.4
_weight_b = 1.0 - _weight_s

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
        print 'No function has been found', histo.GetName()
        return ( VE ( 100     , 100     ** 2 ) ,
                 VE ( _mass0  , _mass0  ** 2 ) ,  
                 VE ( _sigma0 , _sigma0 ** 2 ) ,  
                 VE ( 100.0   , 100     ** 2 ) , 
                 VE ( _slope0 , _slope0 ** 2 ) ,
                 VE ( _curve0 , _curve0 ** 2 ) ) 
    return ( VE ( _func.GetParameter ( 0 ) , _func.GetParError ( 0 ) ** 2 ) ,
             VE ( _func.GetParameter ( 1 ) , _func.GetParError ( 1 ) ** 2 ) ,
             VE ( _func.GetParameter ( 2 ) , _func.GetParError ( 2 ) ** 2 ) , 
             VE ( _func.GetParameter ( 3 ) , _func.GetParError ( 3 ) ** 2 ) , 
             VE ( _func.GetParameter ( 4 ) , _func.GetParError ( 4 ) ** 2 ) , 
             VE ( _func.GetParameter ( 5 ) , _func.GetParError ( 5 ) ** 2 ) )

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
def smallBins ( histo , limit = 9 , low = -1.e+100 , high = 1.e+100 )  :
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
        if c < limit :
            n0 += 1

    nb = max ( nb , 1 ) 
    return float(n0)/nb


# =============================================================================
# get ``signal-to-backgrund'' ratio
def s2b ( histo , r = 2.5 ) :
    """
    get ``signal-to-background'' ratio in +-2.5 sigma interga
    """
    par = getPi0Params ( histo )
    m = par[1].value()
    s = par[2].value()
    r = abs ( r ) 
    iL     = histo.FindBin ( m - r * s )
    iR     = histo.FindBin ( m + r * s )
    return par[0]/(par[3]*(iR-iL))
    
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

    opts = '0QSL'    
    if 0.10 > smallBins ( histo ) : opts = opts.replace('L','')
    
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
    backg0 = 0.5 *  ( histo.GetBinContent ( iL ) +
                      histo.GetBinContent ( iR ) ) 
    ##
    func.SetParameter ( 3 , backg0 )
    st = histo.Fit( func, opts ,'',_low,_high)
    if 0 != st.Status() :
        func.FixParameter ( 5 , curve0 ) 
        func.SetParameter ( 3 , backg0 )
        par3 = func.GetParameter ( 3 ) 
        if 0 > par3 : func.SetParameter( 3 , abs ( par3 ) )
        st = histo.Fit( func, opts ,'',_low,_high)
        if 0 != st.Status() :
            func.FixParameter ( 4 , slope0 ) 
            func.SetParameter ( 3 , backg0 )
            par3 = func.GetParameter ( 3 ) 
            if 0 > par3 : func.SetParameter( 3 , abs ( par3 ) )
            st = histo.Fit ( func, opts ,'',_low,_high )        
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
        par3 = func.GetParameter ( 3 ) 
        if 0 > par3 : func.SetParameter( 3 , abs ( par3 ) )
        st = histo.Fit ( func, opts.replace('0','').replace('Q','') + options ,'',_low,_high )
        
    ##
    func.ReleaseParameter(3)
    func.ReleaseParameter(4) 
    func.ReleaseParameter(5)
        
    return  0 == st.Status()

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
    backg0 = 0.5 *  ( histo.GetBinContent ( iL ) -
                      histo.GetBinContent ( iR ) )  

    num0  = sumHisto ( histo , iL , iR ) - backg0 * ( iR - iL + 1 ) 
    num0  = abs ( num0 )

    opts = '0QSL'    
    if 0.10 > smallBins ( histo ) : opts = opts.replace('L','')

    slope0 = _slope0 
    curve0 = _curve0

    if signal and background :

        s = getPi0Params ( signal     )
        b = getPi0Params ( background )
        
        mass0   = s[1].value()
        sigma0  = s[2].value()
        
        s4 = VE ( s[4] ) 
        b4 = VE ( b[4] ) 
        s4.setCovariance ( s4.cov2() / ( _weight_s**2 ) )
        b4.setCovariance ( b4.cov2() / ( _weight_b**2 ) )        
        slope0  = s4.mean(b4).value()
        
        s5 = VE ( s[5] ) 
        b5 = VE ( b[5] ) 
        s5.setCovariance ( s5.cov2() / ( _weight_s**2 ) )
        b5.setCovariance ( b5.cov2() / ( _weight_b**2 ) )
        curve0  = s5.mean(b5).value()
        
    elif signal and 50 < sumHisto ( signal ) :
        
        s = getPi0Params( signal )
        mass0   = s[1] . value ()
        sigma0  = s[2] . value ()
        slope0  = s[4] . value ()
        curve0  = s[5] . value ()
        
    elif  background and 50 < sumHisto ( background ) :
        b = getPi0Params ( background )
        slope0  = b[4] . value () 
        curve0  = b[5] . value () 
        
    ##     
    func.SetParameter ( 0 , num0   )
    
    ## 1. fix signal & background
    func.FixParameter ( 1 , mass0  )
    func.FixParameter ( 2 , sigma0 )
    func.FixParameter ( 4 , slope0 )
    func.FixParameter ( 5 , curve0 )
    st = histo.Fit ( func, opts ,'', _low , _high )
    ## 2. Release signal parameter 
    func.ReleaseParameter ( 1 )
    func.ReleaseParameter ( 2 )
    par0 = func.GetParameter ( 0 ) 
    if 0 > par0 : func.SetParameter( 0 , abs ( par0 ) )
    par3 = func.GetParameter ( 3 ) 
    if 0 > par3 : func.SetParameter( 3 , abs ( par3 ) )
    st = histo.Fit ( func, opts ,'', _low , _high )
    if 0 != st.Status() :
        func.FixParameter ( 2 , sigma0 )
        par0 = func.GetParameter ( 0 ) 
        if 0 > par0 : func.SetParameter( 0 , abs ( par0 ) )
        par3 = func.GetParameter ( 3 ) 
        if 0 > par3 : func.SetParameter( 3 , abs ( par3 ) )
        st = histo.Fit ( func, opts ,'', _low , _high )
    ##
    par1 = func.GetParameter ( 1 )
    if   par1 < mass0 - 2 * sigma0 :
        func.SetParameter ( 1 , mass0 - 2*sigma0 )    
    elif par1 > mass0 + 2 * sigma0 :
        func.SetParameter ( 1 , mass0 + 2*sigma0 )
    par0 = func.GetParameter ( 0 ) 
    if 0 > par0 : func.SetParameter( 0 , abs ( par0 ) )
    par3 = func.GetParameter ( 3 ) 
    if 0 > par3 : func.SetParameter( 3 , abs ( par3 ) )
    st = histo.Fit ( func, opts ,'', _low , _high )
    ##
    par0 = func.GetParameter ( 0 ) 
    if 0 > par0 : func.SetParameter( 0 , abs ( par0 ) ) 
    ## 3. fix the background:
    mass1  = func.GetParameter ( 1 )
    sigma1 = func.GetParameter ( 2 )
    ## adjustment 
    mass1  = min ( mass1  , mass0 + 2  * sigma0 ) 
    mass1  = max ( mass1  , mass0 - 2  * sigma0 ) 
    sigma1 = min ( sigma1 ,   2  * sigma0 ) 
    sigma1 = max ( sigma1 , 0.5  * sigma0 ) 
    ##
    func.FixParameter ( 1 , mass1  )
    func.FixParameter ( 2 , sigma1 )
    func.ReleaseParameter ( 4 ) 
    func.ReleaseParameter ( 5 ) 
    par0 = func.GetParameter ( 0 ) 
    if 0 > par0 : func.SetParameter( 0 , abs ( par0 ) )
    par3 = func.GetParameter ( 3 ) 
    if 0 > par3 : func.SetParameter( 3 , abs ( par3 ) )
    st = histo.Fit ( func, opts ,'', _low , _high )
    if 0 != st.Status() :
        func.FixParameter ( 5 , curve0 )
        par0 = func.GetParameter ( 0 ) 
        if 0 > par0 : func.SetParameter( 0 , abs ( par0 ) )
        par3 = func.GetParameter ( 3 ) 
        if 0 > par3 : func.SetParameter( 3 , abs ( par3 ) )
        st = histo.Fit ( func, opts ,'', _low , _high )
        if 0 != st.Status() :
            func.FixParameter ( 4 , slope0 )
            par0 = func.GetParameter ( 0 ) 
            if 0 > par0 : func.SetParameter( 0 , abs ( par0 ) )
            par3 = func.GetParameter ( 3 ) 
            if 0 > par3 : func.SetParameter( 3 , abs ( par3 ) )
            st = histo.Fit ( func, opts ,'', _low , _high )
    ##
    slope1 =  func.GetParameter ( 4 )
    curve1 =  func.GetParameter ( 5 )
    ##
    func.ReleaseParameter ( 1 )
    func.ReleaseParameter ( 2 )
    func.FixParameter ( 4 , slope1 )
    func.FixParameter ( 5 , curve1  )
    par0 = func.GetParameter ( 0 ) 
    if 0 > par0 : func.SetParameter( 0 , abs ( par0 ) )
    par3 = func.GetParameter ( 3 ) 
    if 0 > par3 : func.SetParameter( 3 , abs ( par3 ) )
    ## 4 fit it!
    st = histo.Fit ( func, opts ,'', _low , _high )
    
    if 0 == st.Status() :
        func.ReleaseParameter ( 4 )
        func.ReleaseParameter ( 5 )
        par0 = func.GetParameter ( 0 ) 
        if 0 > par0 : func.SetParameter( 0 , abs ( par0 ) )
        par3 = func.GetParameter ( 3 ) 
        if 0 > par3 : func.SetParameter( 3 , abs ( par3 ) )
        st = histo.Fit ( func, opts ,'', _low , _high )
        if 0 != st.Status() :
            func.FixParameter ( 5 , curve1 )
            par0 = func.GetParameter ( 0 ) 
            if 0 > par0 : func.SetParameter( 0 , abs ( par0 ) )
            par3 = func.GetParameter ( 3 ) 
            if 0 > par3 : func.SetParameter( 3 , abs ( par3 ) )
            st = histo.Fit ( func, opts ,'', _low , _high )
            if 0 != st.Status() :
                func.FixParameter ( 4 , slope1 )
                par0 = func.GetParameter ( 0 ) 
                if 0 > par0 : func.SetParameter( 0 , abs ( par0 ) )
                par3 = func.GetParameter ( 3 ) 
                if 0 > par3 : func.SetParameter( 3 , abs ( par3 ) )
                st = histo.Fit ( func, opts ,'', _low , _high )

    ## adjustment (1) 
    sigma1 = func.GetParameter ( 2 )    
    if   sigma1 >  2.0 * sigma0 :
        func.SetParameter( 2 , sigma0 )
        par0 = func.GetParameter ( 0 ) 
        if 0 > par0 : func.SetParameter( 0 , abs ( par0 ) )
        par3 = func.GetParameter ( 3 ) 
        if 0 > par3 : func.SetParameter( 3 , abs ( par3 ) )
        st = histo.Fit ( func, opts ,'', _low , _high )
    elif sigma1 <  0.5 * sigma0 :
        func.SetParameter( 2 , sigma0 )
        par0 = func.GetParameter ( 0 ) 
        if 0 > par0 : func.SetParameter( 0 , abs ( par0 ) )
        par3 = func.GetParameter ( 3 ) 
        if 0 > par3 : func.SetParameter( 3 , abs ( par3 ) )
        st = histo.Fit ( func, opts ,'', _low , _high )

    mass1  = func.GetParameter ( 1 )
    if   mass1 > mass0 + 2 * sigma0 :
        func.SetParameter( 1 , mass0 )
        par0 = func.GetParameter ( 0 ) 
        if 0 > par0 : func.SetParameter( 0 , abs ( par0 ) )
        par3 = func.GetParameter ( 3 ) 
        if 0 > par3 : func.SetParameter( 3 , abs ( par3 ) )
        st = histo.Fit ( func, opts,'', _low , _high )
    elif mass1 < mass0 - 2 * sigma0 :
        func.SetParameter( 1 , mass0 )
        par0 = func.GetParameter ( 0 ) 
        if 0 > par0 : func.SetParameter( 0 , abs ( par0 ) )
        par3 = func.GetParameter ( 3 ) 
        if 0 > par3 : func.SetParameter( 3 , abs ( par3 ) )
        st = histo.Fit ( func, opts,'', _low , _high )

    ## adjustment
    sigma1 = func.GetParameter ( 2 )
    if   sigma1 >  1.5 * sigma0 :
        sigma1 = min ( sigma1 ,  1.5  * sigma0 ) 
        func.FixParameter( 2 , sigma1 )
        par0 = func.GetParameter ( 0 ) 
        if 0 > par0 : func.SetParameter( 0 , abs ( par0 ) )
        par3 = func.GetParameter ( 3 ) 
        if 0 > par3 : func.SetParameter( 3 , abs ( par3 ) )
        st = histo.Fit ( func, opts ,'', _low , _high )
    elif sigma1 <  0.5 * sigma0 :
        sigma1 = max ( sigma1 , 0.5  * sigma0 ) 
        func.FixParameter( 2 , sigma1 )
        par0 = func.GetParameter ( 0 ) 
        if 0 > par0 : func.SetParameter( 0 , abs ( par0 ) )
        par3 = func.GetParameter ( 3 ) 
        if 0 > par3 : func.SetParameter( 3 , abs ( par3 ) )
        st = histo.Fit ( func, opts ,'', _low , _high )
        
    mass1  = func.GetParameter ( 1 )
    if   mass1 > mass0 + 2 * sigma0 :
        mass1  = min ( mass1 , mass0 + 2  * sigma0 ) 
        func.FixParameter( 1 , mass1 )
        par0 = func.GetParameter ( 0 ) 
        if 0 > par0 : func.SetParameter( 0 , abs ( par0 ) )
        par3 = func.GetParameter ( 3 ) 
        if 0 > par3 : func.SetParameter( 3 , abs ( par3 ) )
        st = histo.Fit ( func, opts,'', _low , _high )
    elif mass1 < mass0 - 2 * sigma0 :
        mass1  = max ( mass1 , mass0 - 2  * sigma0 ) 
        func.FixParameter( 1 , mass1 )
        par0 = func.GetParameter ( 0 ) 
        if 0 > par0 : func.SetParameter( 0 , abs ( par0 ) )
        par3 = func.GetParameter ( 3 ) 
        if 0 > par3 : func.SetParameter( 3 , abs ( par3 ) )
        st = histo.Fit ( func, opts,'', _low , _high )
    
    par0 = func.GetParameter ( 0 ) 
    if 0 > par0 : func.SetParameter( 0 , abs ( par0 ) )
    
    if abs ( par0 ) > histo.GetEntries () :
        func.SetParameter ( 0 , histo.GetEntries() ) 
        func.SetParameter ( 3 , backg0 ) 
        func.SetParameter ( 1 , mass1  )
        func.SetParameter ( 2 , sigma1 )
        func.FixParameter ( 4 , slope1 )
        func.FixParameter ( 5 , curve1 )
        par0 = func.GetParameter ( 0 ) 
        if 0 > par0 : func.SetParameter( 0 , abs ( par0 ) )
        par3 = func.GetParameter ( 3 ) 
        if 0 > par3 : func.SetParameter( 3 , abs ( par3 ) )
        st = histo.Fit ( func, opts ,'', _low , _high )
        
    ##
    if 0 != st.Status() :
        func.FixParameter ( 5 , curve1 ) 
        func.SetParameter ( 1 , mass0  ) 
        func.SetParameter ( 2 , sigma0 ) 
        par0 = func.GetParameter ( 0 ) 
        if 0 > par0 : func.SetParameter( 0 , abs ( par0 ) )
        par3 = func.GetParameter ( 3 ) 
        if 0 > par3 : func.SetParameter( 3 , abs ( par3 ) )
        st = histo.Fit ( func, opts ,'', _low , _high )
        if  0 != st.Status() :
            func.FixParameter ( 4 , slope1 ) 
            func.SetParameter ( 1 , mass0  ) 
            func.SetParameter ( 2 , sigma0 ) 
            par0 = func.GetParameter ( 0 ) 
            if 0 > par0 : func.SetParameter( 0 , abs ( par0 ) )
            par3 = func.GetParameter ( 3 ) 
            if 0 > par3 : func.SetParameter( 3 , abs ( par3 ) )
            st = histo.Fit ( func, opts ,'', _low , _high )
            if 0 != st.Status() :                
                func.SetParameter ( 1 , mass0  ) 
                func.FixParameter ( 2 , sigma0 ) 
                par0 = func.GetParameter ( 0 ) 
                if 0 > par0 : func.SetParameter( 0 , abs ( par0 ) )
                par3 = func.GetParameter ( 3 ) 
                if 0 > par3 : func.SetParameter( 3 , abs ( par3 ) )
                st = histo.Fit ( func, opts ,'', _low , _high )
                if 0 != st.Status () : 
                    pars = getPi0Params  ( histo )
                    print 'SIGNAL    : signal=%20s mass =%20s sigma=%20s backg=%20s %8d %25s ' % (
                        pars[1] ,
                        pars[2] ,
                        pars[3] ,
                        pars[4] ,
                        int ( histo.GetEntries () ) , 
                        histo.GetName  () 
                        )
                
    if options :
        par0 = func.GetParameter ( 0 ) 
        if 0 > par0 : func.SetParameter( 0 , abs ( par0 ) )
        par3 = func.GetParameter ( 3 ) 
        if 0 > par3 : func.SetParameter( 3 , abs ( par3 ) )
        st = histo.Fit ( func, opts.replace('0','').replace('Q','') + options ,'', _low , _high )
                
    return  0 == st.Status()

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
## pre-fit historgam set 
def preFitHistoSet ( histoset ) :
    """
    Pre-fit historgam set 
    
    """
    _func = pi0Func
    _func.SetParameter ( 0 , 100 )
    _func.SetParameter ( 1 , 135 )
    _func.SetParameter ( 2 ,  10 )
    _func.SetParameter ( 3 ,  10 )
    _func.SetParameter ( 4 ,   0 )
    _func.SetParameter ( 5 ,   0 )
    
    # start form background historgam
    h3 = histoset [3]
    h4 = histoset [4]
    h5 = histoset [5]
    _preFitBkg ( _func , h3 )
    _preFitBkg ( _func , h4 )
    _preFitBkg ( _func , h5 )

    # go to signal :
    h2 = histoset[2]
    h1 = histoset[1]
    h0 = histoset[0]

    _preFitSignal ( _func , h2 , background = h5 )
    _preFitSignal ( _func , h1 , background = h4 , signal = h2 )
    _preFitSignal ( _func , h0 , background = h3 , signal = h1 )
    
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
    if pars[2].value() > 2.0 * _sigma0          : return False
    if pars[2].value() < 0.5 * _sigma0          : return False
    ##
    return True 

# =============================================================================
## fit the set of histograms
def fitHistoSet ( histoset , set0 , force = False ) :
    """
    Fit historgram set 
    """
    
    _func = pi0Func
    
    h0 = histoset  [0]
    h1 = histoset  [1]
    h2 = histoset  [2]
    h3 = histoset  [3]
    h4 = histoset  [4]
    h5 = histoset  [5]
    
    r0 =      set0 [0]
    r1 =      set0 [1]
    r2 =      set0 [2]
    r3 =      set0 [3]
    r4 =      set0 [4]
    r5 =      set0 [5]

    ## start for background:
    fitBkg ( _func , h5 , background = r5 )
    fitBkg ( _func , h4 , background = r4 )
    fitBkg ( _func , h3 , background = r3 )

    ok0 = 0 
    ## play with signal:

    ## Prs+Prs

    ## use "own" background + reference sygnal
    res0 = fitSignal     ( _func , h2 , background = h5 , signal = r2 )
    if not res0 or not checkHisto ( h2 ) : 
        ok0  = 1 
        ## use "reference signal & background
        res0 = fitSignal ( _func , h2 , background = r5 , signal = r2 )    
    if not res0 or not checkHisto ( h2 ) :
        ok0  = -1
        if not force : return (-2,-2,ok0) 
        
    ## no-Prs +Prs 
    ok1 = 0
    
    ## use "own" background & signal 
    res1 = fitSignal     ( _func , h1 , background = h4 , signal = h2 )
    if not res1 or not checkHisto ( h1 ) :
        ok1  = 1 
        ## use "own" background & reference signal 
        res1 = fitSignal ( _func , h1 , background = h4 , signal = r1 )
    if not res1 or not checkHisto ( h1 ) :
        ok1  = 2 
        ## use "reference" background & signal 
        res1 = fitSignal ( _func , h1 , background = r4 , signal = r1 )
    if not res1 or not checkHisto ( h1 ) :
        ok1  = 3 
        ## use "own" background & reference signal Prs+Prs 
        res1 = fitSignal ( _func , h1 , background = h4 , signal = r2 )
    if not res1 or not checkHisto ( h1 ) :
        ok1  = 4 
        ## use reference background & signal Prs+Prs 
        res1 = fitSignal ( _func , h1 , background = r4 , signal = r2 )
    if not res1 or not checkHisto ( h1 ) :
        ok1  = -1 
        if not force : return (-2,ok1,ok0)
    
    ## no-Prs+no-Prs 
    ok2 = 0
    ## use own background and signal (Prs+no-Prs)
    res2 =  fitSignal    ( _func , h0 , background = h3 , signal = h1 )
    if not res2 or not checkHisto ( h0 ) :
        ok2 = 1 
        ## use reference background and own signal (Prs+Prs)
        res2 = fitSignal ( _func , h0 , background = r3 , signal = h2 )
    if not res2 or not checkHisto ( h0 ) :
        ok2 = 2 
        ## use own background and reference signal (Prs+no-Prs)
        res2 = fitSignal ( _func , h0 , background = h3 , signal = r1 )
    if not res2 or not checkHisto ( h0 ) :
        ok2 = 3 
        ## use reference background and reference signal (Prs+no-Prs)
        res2 = fitSignal ( _func , h0 , background = r3 , signal = r1 )
    if not res2 or not checkHisto ( h0 ) :
        ok2 = 4 
        ## use own background and reference signal (Prs+Prs)
        res2 = fitSignal ( _func , h0 , background = h3 , signal = r2 )
    if not res2 or not checkHisto ( h0 ) :
        ok2 = 5 
        ## use reference background and reference signal (Prs+Prs)
        res2 = fitSignal ( _func , h0 , background = r3 , signal = r2 )
    if not res2 or not checkHisto ( h0 ) :
        ok2 = -1 
        if not force : return (ok2,ok1,ok0) 

    return (ok2,ok1,ok0) 
    

# =============================================================================
if '__main__' == __name__ :

    print '*'*120
    print                      __doc__
    #print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print '*'*120  

# =============================================================================
# The END 
# =============================================================================
