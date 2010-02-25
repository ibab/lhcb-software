#!/usr/bin/env python
# =============================================================================
"""
A module for fitting the histograms with pi0-mass
"""
# =============================================================================
from ROOT import TH1F, TF1
from math import sqrt, pi,exp
from array import array

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
    def __init__ ( self ) :
        
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
_weight_s = 0.3
_weight_b = 1.0 - _weight_s

## get the fitte pi0 parameters 
def getPi0Params ( histo ) :
    """
    Get the fitted pi0 parameters 
    """
    _func = histo.GetFunction('Pi0Fit')
    if not _func :
        print 'No function has been found', histo.GetName()
        return ( 100.0   ,
                 _mass0  ,
                 _sigma0 , 
                 100.0   ,
                 _slope0 ,
                 _curve0 ) 
    return ( _func.GetParameter ( 0 ) ,
             _func.GetParameter ( 1 ) ,
             _func.GetParameter ( 2 ) ,
             _func.GetParameter ( 3 ) ,
             _func.GetParameter ( 4 ) ,
             _func.GetParameter ( 5 ) )


## pre-fit background histogram 
def _preFitBkg ( func , histo , background = None ) :
    """
    pre-fit background histogram
    """

    entries = histo.GetEntries()
    
    func.FixParameter ( 0 , 0       )
    func.FixParameter ( 1 , _mass0  )
    func.FixParameter ( 2 , _sigma0 )
    ##
    func.ReleaseParameter(3)
    ##
    if entries > 20 : 
        func.ReleaseParameter(4) 
        func.ReleaseParameter(5)
    else :
        func.FixParameter ( 4 , _slope0 )
        func.FixParameter ( 5 , _curve0 )
    ##
    slope0 = _slope0
    curve0 = _curve0 
    if background :
        b = getPi0Params ( background )
        slope0 = b[4]
        curve0 = b[5]
    ##
    func.SetParameter ( 4 , slope0 )
    func.SetParameter ( 5 , curve0 )
    ##
    iL     = histo.FindBin ( _mass0 - 3.0 * _sigma0 )
    iR     = histo.FindBin ( _mass0 + 3.0 * _sigma0 )
    backg0 = 0.5 *  ( histo.GetBinContent ( iL ) +
                      histo.GetBinContent ( iR ) ) 
                     
    func.SetParameter ( 3 , backg0 )
    
    st = histo.Fit( func, '0QLS','',_low,_high)
    if 0 != st.Status() :
        func.FixParameter ( 5 , curve0 ) 
        st = histo.Fit( func, '0QLS','',_low,_high)
        if 0 != st.Status() : func.FixParameter( 4 , slope0 )
    ##
    st = histo.Fit ( func, '0QLS','',_low,_high )
    ## 
    func.ReleaseParameter(3)
    ##
    func.ReleaseParameter(4) 
    func.ReleaseParameter(5)
    ##
    if 0 != st.Status() :
        pars = getPi0Params  ( histo )
        print 'BACKGROUBND: %s %9d %9.5g %9.5g %9g %9g ' % (
            histo.GetName  () ,
            int ( pars[0] ) ,
            pars[1] ,
            pars[2] ,
            pars[3] ,
            histo.GetEntries ( )
            )
    ##
    return  0 == st.Status()


def sumHisto ( histo , iL , iH ) :
    """
    """
    sum  = 0 
    for iB in range ( 1 , min ( iH + 1 , histo.GetNbinsX() + 1 ) ) :
        if iL <= iB <= iH :
            sum += histo.GetBinContent( iB )
    return sum 
    
## pre-fit background histogram 
def fitBkg ( func , histo , background = None ) :
    """
    Fit background histogram
    """
    return _preFitBkg ( func       ,
                        histo      ,
                        background ) 

## pre-fit signal histogram 
def _preFitSignal ( func  ,
                    histo ,
                    background = None ,
                    signal     = None ) :
    """
    pre-fit background histogram
    """
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

    slope0 = _slope0 
    curve0 = _curve0

    if signal and background : 
        s = getPi0Params ( signal     )
        b = getPi0Params ( background )
        num0    = s[0]
        mass0   = s[1]
        sigma0  = s[2]
        ## bckgr   = _weight_s * s[3]  + _weight_b * b[3]
        slope0  = _weight_s * s[4]  + _weight_b * b[4]  
        curve0  = _weight_s * s[5]  + _weight_b * b[5]  
    elif signal :
        s = getPi0Params( signal )
        num0    = s[0]
        mass0   = s[1]
        sigma0  = s[2]
        ## bckgr0  = s[3]
        slope0  = s[4]
        curve0  = s[5] 
    elif  background :
        b = getPi0Params ( background )
        ## bckgr0  = b[3]  
        slope0  = b[4]
        curve0  = b[5]
        
    ##     
    func.SetParameter ( 0 , num0   )
    
    ## 1. fix signal & background
    func.FixParameter ( 1 , mass0  )
    func.FixParameter ( 2 , sigma0 )
    func.FixParameter ( 4 , slope0 )
    func.FixParameter ( 5 , curve0 )
    st = histo.Fit ( func, '0QLS','', _low , _high )
    ## 2. Release signal parameter 
    func.ReleaseParameter ( 1 )
    func.ReleaseParameter ( 2 )
    st = histo.Fit ( func, '0QLS','', _low , _high )
    if 0 != st.Status() :
        func.FixParameter ( 2 , sigma0 )
        st = histo.Fit ( func, '0QLS','', _low , _high )
    ##
    par1 = func.GetParameter ( 1 )
    if par1 < mass0 - 2 * sigma0 :
        func.SetParameter ( 1 , mass0 - 2*sigma0 )
    if par1 > mass0 + 2 * sigma0 :
        func.SetParameter ( 1 , mass0 + 2*sigma0 )
    st = histo.Fit ( func, '0QLS','', _low , _high )    
    ##
    par0 = func.GetParameter ( 0 ) 
    if 0 > par0 : func.SetParameter( 0 , abs ( par0 ) ) 
    ## 3. fix the background:
    mass1  = func.GetParameter ( 1 )
    sigma1 = func.GetParameter ( 2 )
    ## adjustment 
    mass1  = min ( mass1 , mass0 + 2  * sigma0 ) 
    mass2  = max ( mass1 , mass0 - 2  * sigma0 ) 
    sigma1 = min ( sigma1 ,   2  * sigma0 ) 
    sigma1 = max ( sigma1 , 0.2  * sigma0 ) 
    ##
    func.FixParameter ( 1 , mass1  )
    func.FixParameter ( 2 , sigma1 )
    func.ReleaseParameter ( 4 ) 
    func.ReleaseParameter ( 5 ) 
    st = histo.Fit ( func, '0QLS','', _low , _high )
    if 0 != st.Status() :
        func.FixParameter ( 5 , curve0 )
        st = histo.Fit ( func, '0QLS','', _low , _high )
        if 0 != st.Status() :
            func.FixParameter ( 4 , slope0 )
            st = histo.Fit ( func, '0QLS','', _low , _high )
    ##
    slope1 =  func.GetParameter ( 4 )
    curve1 =  func.GetParameter ( 5 )
    ##
    func.ReleaseParameter ( 1 )
    func.ReleaseParameter ( 2 )
    func.FixParameter ( 4 , slope1 )
    func.FixParameter ( 5 , curve1  )
    ## 4 fit it!
    st = histo.Fit ( func, '0QLS','', _low , _high )
    if 0 == st.Status() :
        func.ReleaseParameter ( 4 )
        func.ReleaseParameter ( 5 )
        st = histo.Fit ( func, '0QLS','', _low , _high )
        if 0 != st.Status() :
            func.FixParameter ( 5 , curve1 )
            st = histo.Fit ( func, '0QLS','', _low , _high )
            if 0 != st.Status() :
                func.FixParameter ( 4 , slope1 )
                st = histo.Fit ( func, '0QLS','', _low , _high )

    ## adjustment
    sigma1 = func.GetParameter ( 2 )
    if   sigma1 >  2.0 * sigma0 :
        sigma1 = min ( sigma1 ,   2  * sigma0 ) 
        func.FixParameter( 2 , sigma1 )
        st = histo.Fit ( func, '0QLS','', _low , _high )
    elif sigma1 <  0.5 * sigma0 :
        sigma1 = max ( sigma1 , 0.2  * sigma0 ) 
        func.FixParameter( 2 , sigma1 )
        st = histo.Fit ( func, '0QLS','', _low , _high )
        
    mass1  = func.GetParameter ( 1 )
    if   mass1 > mass0 + 2 * sigma0 :
        mass1  = min ( mass1 , mass0 + 2  * sigma0 ) 
        func.FixParameter( 1 , mass1 )
        st = histo.Fit ( func, '0QLS','', _low , _high )
    elif mass1 < mass0 + 2 * sigma0 :
        mass1  = max ( mass1 , mass0 - 2  * sigma0 ) 
        func.FixParameter( 1 , mass1 )
        st = histo.Fit ( func, '0QLS','', _low , _high )
    
    par0 = func.GetParameter ( 0 ) 
    if 0 > par0 : func.SetParameter( 0 , abs ( par0 ) )
    
    if abs ( par0 ) > histo.GetEntries () :
        func.SetParameter ( 0 , histo.GetEntries() ) 
        func.SetParameter ( 3 , backg0 ) 
        func.SetParameter ( 1 , mass1  )
        func.SetParameter ( 2 , sigma1 )
        func.FixParameter ( 4 , slope1 )
        func.FixParameter ( 5 , curve1 )
        st = histo.Fit ( func, '0QLS','', _low , _high )
        
    ##
    if 0 != st.Status() :
        func.FixParameter ( 5 , curve1 ) 
        st = histo.Fit ( func, '0QLS','', _low , _high )
        if  0 != st.Status() :
            func.FixParameter ( 4 , slope1 ) 
            st = histo.Fit ( func, '0QLS','', _low , _high )
            if 0 != st.Status() :
                pars = getPi0Params  ( histo )
                print 'SIGNAL     : %s %9d %9.5g %9.5g %9g %9g ' % (
                    histo.GetName  () ,
                    int( pars[0] ) ,
                    pars[1] ,
                    pars[2] ,
                    pars[3] ,
                    histo.GetEntries ( )
                    )

    return  0 == st.Status()

## pre-fit signal histogram 
def fitSignal ( func  ,
                histo ,
                background = None ,
                signal     = None ) :
    
    return _preFitSignal ( func       ,
                           histo      ,
                           background ,
                           signal     )

    
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
    

def checkHisto ( histo ) :
    
    pars = getPi0Params ( histo )
    if pars[0] > histo.GetEntries()   : return False
    if pars[1] > _mass0 + 2 * _sigma0 : return False
    if pars[1] < _mass0 - 2 * _sigma0 : return False
    if pars[2] > 2.0 * _sigma0        : return False
    if pars[2] < 0.5 * _sigma0        : return False
    ##
    return True 
    
## fit historgam set 
def fitHistoSet ( histoset , set0 ) :
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
    res0 = fitSignal     ( _func , h2 , background = h5 , signal = r2 )
    if not res0 or not checkHisto ( h2 ) : 
        ok0  = 1 
        res0 = fitSignal ( _func , h2 , background = r5 , signal = r2 )    
    if not res0 or not checkHisto ( h2 ) : ok0 = 2 
        
    ok1 = 0  
    res1 = fitSignal     ( _func , h1 , background = h4 , signal = h2 )
    if not res1 or not checkHisto ( h1 ) :
        ok1  = 1 
        res1 = fitSignal ( _func , h1 , background = h4 , signal = r1 )
    if not res1 or not checkHisto ( h1 ) :
        ok1  = 2 
        res1 = fitSignal ( _func , h1 , background = r4 , signal = r1 )
    if not res1 or not checkHisto ( h1 ) :
        ok1  = 3 
        res1 = fitSignal ( _func , h1 , background = h4 , signal = r2 )
    if not res1 or not checkHisto ( h1 ) :
        ok1  = 4 
        res1 = fitSignal ( _func , h1 , background = r4 , signal = r2 )
    if not res1 or not checkHisto ( h1 ) : ok1 = 2 
    
    ok2 = 0 
    res2 =  fitSignal    ( _func , h0 , background = h3 , signal = h1 )
    if not res2 or not checkHisto ( h0 ) :
        ok2 = 1 
        res2 = fitSignal ( _func , h0 , background = r3 , signal = h2 )
    if not res2 or not checkHisto ( h0 ) :
        ok2 = 2 
        res2 = fitSignal ( _func , h0 , background = h3 , signal = r1 )
    if not res2 or not checkHisto ( h0 ) :
        ok2 = 3 
        res2 = fitSignal ( _func , h0 , background = r3 , signal = r1 )
    if not res2 or not checkHisto ( h0 ) :
        ok2 = 4 
        res2 = fitSignal ( _func , h0 , background = h3 , signal = r2 )
    if not res2 or not checkHisto ( h0 ) :
        ok2 = 5 
        res2 = fitSignal ( _func , h0 , background = r3 , signal = r2 )
    if not res2 or not checkHisto ( h0 ) : ok2 = 6 

    return (ok2,ok1,ok0) 
    
