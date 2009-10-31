#!/usr/bin/env python
# =============================================================================
## @file KaliCalo/Pi0HistoFit.py
#  the helper module which allows the fit of the histogfram with pi0-mass
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-05
# =============================================================================
"""
The helper module which allows the fit of the histogfram with pi0-mass
"""
# =============================================================================
__all__ = (
    'fitPi0Histo' ,
    )
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@nikhef.nl'
__version__ = 'CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $'
# =============================================================================
import  ROOT 
from    math import exp
import  GaudiPython.HistoUtils
from    GaudiPython.Bindings   import gbl as cpp
import  LHCbMath.Types 

Gaudi  = cpp.Gaudi

# =============================================================================
## @class Pi0FitFun
#  Simple class to represent the shape of pi0-mass distribution
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-05
class Pi0FitFun ( object ) :
    """    
    Simple class to represent the shape of pi0-mass distribution
    """
    def __init__ ( self ) :
        
        from math import sqrt,pi
        self.norm = 0.001/sqrt(2.0*pi)   ## the trivial normalization 
        self.x0   = 0.5*(0.250-0.050)    ## the normalization point
    
    def __call__ ( self , x , par ) :
        """
        The major method for function evaluation
        """
        mass = par[1] 
        dm  = x[0] - mass 
        dm2 = dm*dm
        sigma  = par[2]
        sigma2 = sigma*sigma
        number = par[0]*self.norm/sigma 
        signal = number*exp(-0.5*dm2/sigma2)
        
        dm1    = x[0] - self.x0 
        backgr = par[3]*(1.0+dm1*(par[4]+ dm1*(par[5]+dm1*par[6])))
        
        return signal + backgr

    
pi0FitFun = ROOT.TF1('pi0Fit', Pi0FitFun() , 0.05 , 0.20 , 7 )
pi0FitFun.SetParNames  (
    "Signal"     ,
    "Mass"       ,
    "Sigma"      ,
    "Background" ,
    "Slope"      ,
    "Curvature"  ,
    "ThirdOrder"
    )  

# =============================================================================
## fit pi0-histogram 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-05
def fitPi0Histo ( histo , low = None , high = None ) :
    """
    Perform the fit of diphoton mass distribution and return
    tuple with number of event in signal peak and the position of mass-peak

    >>> histo = ...
    >>> num,mass = fitPi0Histo ( histo )
    
    """
    ## ROOT versus AIDA ?
    if not hasattr ( histo , 'Fit' ) or not hasattr ( histo , 'GetEntries' ) :
        if hasattr ( histo , 'toROOT' ) : 
            _root = histo.toROOT() 
            return fitPi0Histo ( _root , low , high )
    
    if 0 == histo.GetEntries() :
        print 'No fit: Empty histogram'
        return ( 0.0 , 0.135 )        ## RETURN 

    fitFun = pi0FitFun
    
    fitFun.SetParameter ( 0 , 0.14 / 1.5 * histo.GetEntries() )
    
    axis  = histo.GetXaxis()
    nbins = axis.GetNbins()

    if not low or not high :
        low  = axis.GetXmin()
        high = axis.GetXmax()

    ilow  = axis.FindBin ( low )
    if 0     >= ilow :
        ilow  = 1
        low   = axis.GetXmin()
        
    ihigh = axis.FindBin ( high )
    if nbins < ihigh :
        ihigh = nbins
        high  = axis.GetXmax()

    ## the final adjustment 
    if ihigh <= ilow :
        ilow  = 1
        ihigh = nbins
        low   = axis.GetXmin()
        high  = axis.GetXmax() 
                
    first = histo.GetBinContent( ilow  )
    last  = histo.GetBinContent( ihigh )
    size  = high - low  
    level = 0.5 * ( first + last )

    print ilow, ihigh, low, high
    
    if 0 == level :
        print 'No FIT, Level = 0 '
        return ( 0.0 , 135 )       ## RETURN 
    
    slope = 0.5 * ( last  - first ) / size / level 
    
    fitFun.SetParameter ( 3 , level )   ## intial guess for background level
    fitFun.SetParameter ( 4 , slope )   ## guess for bakcground slope 
    
    fitFun.FixParameter ( 1 , 0.135 ) 
    fitFun.FixParameter ( 2 , 0.012 ) 
    fitFun.FixParameter ( 5 , 0.0   )  # fix the curvature  
    fitFun.FixParameter ( 6 , 0.0   )  # fix the 3rd order parameter

    histo.Fit( fitFun , '0Q' , '' , low , high )
        
    fitFun.ReleaseParameter ( 1 )
    histo.Fit( fitFun , '0Q' , '' , low , high )
        
    fitFun.ReleaseParameter ( 5 )
    histo.Fit( fitFun , '0Q' , '' , low , high )

    fitFun.FixParameter ( 5 , fitFun.GetParameters()[5] ) 
    fitFun.ReleaseParameter ( 2 )
    histo.Fit( fitFun , '0Q' , '' , low , high )

    fitFun.FixParameter ( 2 , fitFun.GetParameters()[2] ) 

    if 500 < histo.GetEntries() :
        fitFun.ReleaseParameter ( 6 )
        histo.Fit( fitFun , '0Q' , '' , low , high )
    else :
        fitFun.FixParameter ( 6 , 0.0   )  # fix the 3rd order parameter

    fitFun.FixParameter ( 6 , fitFun.GetParameters()[6] )
    
    fitFun.ReleaseParameter ( 2 )
    histo.Fit( fitFun , '0Q' , '' , low , high )
    fitFun.FixParameter ( 2 , fitFun.GetParameters()[2] ) 

    fitFun.ReleaseParameter ( 5 )
    histo.Fit( fitFun , '0Q' , '' , low , high )
    fitFun.FixParameter ( 5 , fitFun.GetParameters()[5] ) 

    fitFun.ReleaseParameter ( 2 )
    histo.Fit( fitFun , '0Q' , '' , low , high )

    fitFun.ReleaseParameter ( 5 )
    histo.Fit( fitFun , '0Q' , '' , low , high )

    ## the final fit 
    histo.Fit( fitFun , '' , '' , low , high )
        

    par0 = Gaudi.Math.ValueWithError (
        fitFun.GetParameter(0) ,
        fitFun.GetParError (0) * fitFun.GetParError (0) )    
    par1 = Gaudi.Math.ValueWithError (
        fitFun.GetParameter(1) ,
        fitFun.GetParError (1) * fitFun.GetParError (1) )
                                   
    return (par0,par1)



    
# =============================================================================
if '__main__' == __name__ :
    print __doc__
    print __author__ 
    print __version__ 
    
# =============================================================================
# The END 
# =============================================================================
