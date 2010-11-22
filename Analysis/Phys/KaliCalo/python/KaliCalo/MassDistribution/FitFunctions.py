#!/usr/bin/env python
# =============================================================================
# @file FitFunctions.py
# @author Albert Puig Navarro (albert.puig@cern.ch)
# @reshuffled from Vanya Belyaev's FitTask.py and Pi0HistoFit.py
# @date 2010-11-13
# =============================================================================
"""
"""

__author__ = "Albert Puig (albert.puig@cern.ch)"
__date__ = "$Date$"
__version__ = "$Revision$"

import KaliCalo.FitUtils as Fit
from KaliCalo.Calo       import pi0Mass

pi0Func       = Fit.pi0Func

# Load pre-fit functionality
_preFitBkg    = Fit._preFitBkg
_preFitSignal = Fit._preFitSignal

# =============================================================================
## pre-fit historgam set 
def preFitHistoSet ( histoset ) :
    """
    Pre-fit historgam set 
    
    """
    _func = pi0Func
    _func.SetParameter ( 0 , 100 )
    _func.SetParameter ( 1 , pi0Mass )
    _func.SetParameter ( 2 ,  10 )
    _func.SetParameter ( 3 ,  10 )
    _func.SetParameter ( 4 ,   0 )
    _func.SetParameter ( 5 ,   0 )
    
    # start from background historgam
    h3 = None
    h4 = None
    h5 = None
    
    nFits = 0

    if len( histoset ) >= 6 :
        
        h3 = histoset [3]
        h4 = histoset [4]
        h5 = histoset [5]

        st , nFit1 = _preFitBkg ( _func , h3 )
        st , nFit2 = _preFitBkg ( _func , h4 )
        st , nFit3 = _preFitBkg ( _func , h5 )
        nFits += nFit1 
        nFits += nFit2 
        nFits += nFit3
    
    # go to signal :
    h2 = histoset[2]
    h1 = histoset[1]
    h0 = histoset[0]

    st1 , nFit1 = _preFitSignal ( _func , h2 , background = h5 )
    st2 , nFit2 = _preFitSignal ( _func , h1 , background = h4 , signal = h2 )
    st3 , nFit3 = _preFitSignal ( _func , h0 , background = h3 , signal = h1 )

    nFits += nFit1 
    nFits += nFit2 
    nFits += nFit3

    return st1,st2,st3, nFits

#  Load fit functionality
fitBkg     = Fit.fitBkg
fitSignal  = Fit.fitSignal
checkHisto = Fit.checkHisto

# =============================================================================
## fit the set of histograms
def fitHistoSet ( histoset , set0 , force = False ) :
    """
    Fit historgram set 
    """
    histoset.result = None 

    _func = pi0Func

    nFits = 0 

    h0 = histoset  [0]
    h1 = histoset  [1]
    h2 = histoset  [2]

    h3 = None
    h4 = None
    h5 = None
    if len ( histoset ) >= 6 : 
        h3 = histoset  [3]
        h4 = histoset  [4]
        h5 = histoset  [5]


    r0 =      set0 [0]
    r1 =      set0 [1]
    r2 =      set0 [2]

    r3 = None
    r4 = None
    r5 = None
    if len ( set0 ) >= 6 : 
        r3 =      set0 [3]
        r4 =      set0 [4]
        r5 =      set0 [5]

    ## start for background:
    if h5 :
        st , nFit1 = fitBkg ( _func , h5 , background = r5 )
        nFits += nFit1
    if h4 :
        st , nFit2 = fitBkg ( _func , h4 , background = r4 )
        nFits += nFit2
    if h3 : 
        st , nFit3 = fitBkg ( _func , h3 , background = r3 )
        nFits += nFit3

    ok0 = 0 
    ## play with signal:

    ## Prs+Prs

    ## use "own" background + reference signal
    res0, nFit = fitSignal     ( _func , h2 , background = h5 , signal = r2 )
    nFits += nFit 
    if ( not res0 or not checkHisto ( h2 ) ) and r5 : 
        ok0  = 1 
        ## use "reference signal & background
        res0, nFit = fitSignal ( _func , h2 , background = r5 , signal = r2 )    
        nFits += nFit 
    if not res0 or not checkHisto ( h2 ) :
        ok0  = -1
        if not force :
            result =  (-2,-2,ok0, nFits)
            histoset.result = result
            return result 

    ## no-Prs +Prs 
    ok1 = 0

    ## use "own" background & signal 
    res1 , nFit = fitSignal     ( _func , h1 , background = h4 , signal = h2 )
    nFits += nFit 
    if not res1 or not checkHisto ( h1 ) :
        ok1  = 1 
        ## use "own" background & reference signal 
        res1 , nFit = fitSignal ( _func , h1 , background = h4 , signal = r1 )
        nFits += nFit 
    if ( not res1 or not checkHisto ( h1 ) ) and r4 :
        ok1  = 2 
        ## use "reference" background & signal 
        res1 , nFit = fitSignal ( _func , h1 , background = r4 , signal = r1 )
        nFits += nFit 
    if not res1 or not checkHisto ( h1 ) :
        ok1  = 3 
        ## use "own" background & reference signal Prs+Prs 
        res1, nFit = fitSignal ( _func , h1 , background = h4 , signal = r2 )
        nFits += nFit 
    if ( not res1 or not checkHisto ( h1 ) ) and r4 :
        ok1  = 4 
        ## use reference background & signal Prs+Prs 
        res1 , nFit = fitSignal ( _func , h1 , background = r4 , signal = r2 )
        nFits += nFit 
    if not res1 or not checkHisto ( h1 ) :
        ok1  = -1 
        if not force :
            result =  (-2,ok1,ok0, nFits)
            histoset.result = result 
            return result 

    ## no-Prs+no-Prs 
    ok2 = 0
    ## use own background and signal (Prs+no-Prs)
    res2 , nFit =  fitSignal    ( _func , h0 , background = h3 , signal = h1 )
    nFits += nFit
    if ( not res2 or not checkHisto ( h0 ) ) and r3 :
        ok2 = 1 
        ## use reference background and own signal (Prs+Prs)
        res2 , nFit = fitSignal ( _func , h0 , background = r3 , signal = h2 )
        nFits += nFit
    if not res2 or not checkHisto ( h0 ) :
        ok2 = 2 
        ## use own background and reference signal (Prs+no-Prs)
        res2 , nFit = fitSignal ( _func , h0 , background = h3 , signal = r1 )
        nFits += nFit
    if ( not res2 or not checkHisto ( h0 ) ) and r3 :
        ok2 = 3 
        ## use reference background and reference signal (Prs+no-Prs)
        res2 , nFit = fitSignal ( _func , h0 , background = r3 , signal = r1 )
        nFits += nFit
    if not res2 or not checkHisto ( h0 ) :
        ok2 = 4 
        ## use own background and reference signal (Prs+Prs)
        res2 , nFit = fitSignal ( _func , h0 , background = h3 , signal = r2 )
        nFits += nFit
    if ( not res2 or not checkHisto ( h0 ) ) and r3 :
        ok2 = 5 
        ## use reference background and reference signal (Prs+Prs)
        res2 , nFit = fitSignal ( _func , h0 , background = r3 , signal = r2 )
        nFits += nFit
    if not res2 or not checkHisto ( h0 ) :
        ok2 = -1 
        if not force :
            result =  (ok2,ok1,ok0, nFits)
            histoset.result = result 
            return result 

    result =  (ok2,ok1,ok0, nFits)
    histoset.result = result 
    return result 

# EOF
