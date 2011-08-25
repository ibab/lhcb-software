#!/usr/bin/env python
# =============================================================================
# @file FitFunctions.py
# @author Albert Puig Navarro (albert.puig@cern.ch)
# @date 2010-11-13
# =============================================================================
"""
"""

__author__ = "Albert Puig (albert.puig@cern.ch)"
__date__ = "$Date$"
__version__ = "$Revision$"

import KaliCalo.FitUtils as Fit
from KaliCalo.Calo       import pi0Mass

pi0Func           = Fit.pi0Func
pi0SubtractedFunc = Fit.pi0SubtractedFunc

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
    
    nFits = 0

    # prefit background histograms
    hAllBkg     = histoset [2]
    hPrsLT10Bkg = histoset [3]
    
    st , nFit1 = _preFitBkg ( _func , hAllBkg )
    st , nFit2 = _preFitBkg ( _func , hPrsLT10Bkg )
    nFits += nFit1 
    nFits += nFit2 

    # now prefit signal
    hAll     = histoset[2]
    hPrsLT10 = histoset[1]

    st1 , nFit1 = _preFitSignal ( _func , hAll , background = hAllBkg )
    st2 , nFit2 = _preFitSignal ( _func , hPrsLT10 , background = hPrsLT10Bkg , signal = hAll )

    nFits += nFit1 
    nFits += nFit2 

    return st1 , st2 , nFits

#  Load fit functionality
fitBkg     = Fit.fitBkg
fitSignal  = Fit.fitSignal
checkHisto = Fit.checkHisto

# =============================================================================
## fit the set of histograms
def fitHistoSet ( histoset , set0 , force = False ) :
    """
    Fit historgram set.
    
    set0 = helper set to give starting values
    """
    histoset.result = None 
    _func    = pi0Func
    _funcSub = pi0SubtractedFunc
    ## initialize number of hits
    nFits = 0 
    ## initialize histos
    hAll        = histoset [0]
    hAllBkg     = histoset [2]
    hAllSub     = histoset [4]
    hPrsLT10    = histoset [1]
    hPrsLT10Bkg = histoset [3]
    hPrsLT10Sub = histoset [5]
    # initialize reference histos
    refAll        = set0 [0]
    refAllBkg     = set0 [2]
    refAllSub     = set0 [4]
    refPrsLT10    = set0 [1]
    refPrsLT10Bkg = set0 [3]
    refPrsLT10Sub = set0 [5]

    ## start with background
    if hAllBkg :
        st , nFit1 = fitBkg ( _func , hAllBkg , background = refAllBkg )
        nFits += nFit1
    if hPrsLT10Bkg :
        st , nFit2 = fitBkg ( _func , hPrsLT10Bkg , background = refPrsLT10Bkg )
        nFits += nFit2

    ## now the signal: all
    sc0 = 0 # Status of the fit
    ## use "own" background + reference signal
    res0, nFit = fitSignal     ( _func , hAll , background = hAllBkg , signal = hAll )
    nFits += nFit 
    if ( not res0 or not checkHisto ( hAll ) ) and refAllBkg : 
        sc0  = 1
        ## use "reference signal & background
        res0, nFit = fitSignal ( _func , hAll , background = refAllBkg , signal = refAll )    
        nFits += nFit 
    if not res0 or not checkHisto ( hAll ) :
        sc0  = -1

    ## prs < 10
    sc1 = 0
    ## use "own" background & signal prefits
    res1 , nFit = fitSignal ( _func , hPrsLT10 , background = hPrsLT10Bkg , signal = hPrsLT10 )
    nFits += nFit 
    if not res1 or not checkHisto ( hPrsLT10 ) :
        sc1  = 1 
        ## use "own" background & reference signal 
        res1 , nFit = fitSignal ( _func , hPrsLT10 , background = hPrsLT10Bkg , signal = refPrsLT10 )
        nFits += nFit 
    if ( not res1 or not checkHisto ( hPrsLT10 ) ) and refPrsLT10Bkg :
        sc1  = 2 
        ## use "reference" background & signal 
        res1 , nFit = fitSignal ( _func , hPrsLT10 , background = refPrsLT10Bkg , signal = refPrsLT10 )
        nFits += nFit 
    if not res1 or not checkHisto ( hPrsLT10 ) :
        sc1  = 3 
        ## use "own" background & reference signal Prs+Prs 
        res1, nFit = fitSignal ( _func , hPrsLT10 , background = hPrsLT10Bkg , signal = refAll )
        nFits += nFit 
    if ( not res1 or not checkHisto ( hPrsLT10 ) ) and r4 :
        sc1  = 4 
        ## use reference background & signal Prs+Prs 
        res1 , nFit = fitSignal ( _func , hPrsLT10 , background = refPrsLT10Bkg , signal = refAll )
        nFits += nFit 
    if not res1 or not checkHisto ( hPrsLT10 ) :
        sc1 = -1 
            
    # Subtracted all
    sc2 = 0
    res2 , nFit = fitSignal ( _funcSub , hAllSub , background = None, signal = hAllSub )
    nFits += nFit
    if not res2 or not checkHisto ( hAllSub ):
        sc2 = 1
        res1 , nFit = fitSignal ( _funcSub , hAllSub , background = None, signal = refAllSub )
        nFits += nFit
    if not res2 or not checkHisto ( hAllSub ):
        sc2 = -1
        
    # Subtracted Prs<10
    sc3 = 0
    res3 , nFit = fitSignal ( _funcSub , hPrsLT10Sub , background = None, signal = hPrsLT10Sub )
    nFits += nFit
    if not res3 or not checkHisto ( hPrsLT10Sub ):
        sc3 = 1
        res1 , nFit = fitSignal ( _funcSub , hPrsLT10Sub , background = None, signal = refPrsLT10Sub )
        nFits += nFit
    if not res3 or not checkHisto ( hPrsLT10Sub ):
        sc3 = -1
        
    # Return results
    result =  ( sc0 , sc1 , sc2 , sc3 , nFits )
    histoset.result = result 
    return result 

# EOF
