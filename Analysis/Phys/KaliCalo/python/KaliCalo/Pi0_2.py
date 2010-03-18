#!/usr/bin/env python
# =============================================================================
## @file KaliPi0/Pi0_2.py
# 
# A script for iterative (pi0->gamma gamma) ECAL calibration (primary iterations)
# 
# Run:
# 
# > python Pi0.py
#
# =============================================================================
"""
A script for iterative (pi0->gamma gamma) ECAL calibration (primary iterations)

Run:

> python Pi0_2.py

niter is the desired number of iterations (10 by default)
alam   is the dictionary of the calibration coefficients, cell IDs are used as
       the dictionary keys
pi0mas is the dictionary of the pi0 mass histograms, cell IDs are used as the
       dictionary keys
"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@itep.ru "
__date__    = " 2010-02-?? "
__version__ = " CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $ "
# =============================================================================
import ROOT, math 
c1 = ROOT.TCanvas()

import KaliCalo.Kali as Kali 

def fillHistos ( tree                          ,
                 histomap  = Kali.HistoMap  () ,
                 lambdamap = Kali.LambdaMap () ,
                 cellFunc  = lambda s : s      ) :
    
    from KaliCalo.Pi0FillHistos import FillPi0
    selector = FillPi0 ( histomap  ,
                         lambdamap ,
                         cellFunc  )
    
    bamboo.Process( selector )

    lambdas = selector.lambdas ()
    histos  = selector.histos  ()
    
    print ' histos  : ', len ( histos  )
    print ' lambdas : ', len ( lambdas )
    
    return ( histos , lambdas )  


def  analyse ( histomap , lambdamap ) :

    import KaliCalo.Pi0HistoFit as Fit 

    ## get 'All-Ecal' histoigrams 
    hA = histomap [ Kali.EcalZone   ].histos()

    ## (pre) fit them! 
    Fit.preFitHistoSet ( hA )
    
    ## inner area 
    hI = histomap  [ Kali.InnerZone  ].histos()
    ## middle area 
    hM = histomap  [ Kali.MiddleZone ].histos()
    ## outer area 
    hO = histomap  [ Kali.OuterZone  ].histos() 
    
    ## fit them!!
    print 'FitInner  : ' , Fit.fitHistoSet ( hI , hA , True )
    print 'FitMiddle : ' , Fit.fitHistoSet ( hM , hA , True )
    print 'FitOuter  : ' , Fit.fitHistoSet ( hO , hA , True )
    
    keys = histomap.keys()
    keys.sort()
    
    for key in Kali.Zones  :
        keys.remove ( key     )
        keys.insert ( 0 , key ) 

    for key in keys :

        if min ( histomap[key].entries() ) < 30 : 
            print ' too low statistics for key', key
            continue
        
        hs     = histomap[key].histos() 
        result = (0,0,0)
        
        if not key in Kali.Zones :
            
            if   key.area() == Kali.InnerZone.area() : 
                result = Fit.fitHistoSet ( hs , hI )
            elif key.area() == Kali.MiddleZone.area() : 
                result = Fit.fitHistoSet ( hs , hM )
            elif key.area() == Kali.OuterZone.area() : 
                result = Fit.fitHistoSet ( hs , hO )
            else :
                result = Fit.fitHistoSet ( hs , hA )
                
        if   0 <= result[0] :  ## use the sample with no Prs 
            r = Fit.getPi0Params ( hs[0] )
        elif 0 <= result[1] :  ## use the sample with only 1 Prs 
            r = Fit.getPi0Params ( hs[1] )
        elif 0 <= result[2] :  ## use the sample with two Prs 
            r = Fit.getPi0Params ( hs[2] )
        else :
            print ' no reliable fits for ',key
            continue 
            ##r = Fit.getPi0Params( hs[2] )
        
        hc     = histomap[key].kappas() 
        
##         mass = r[1]

##         lams = lambdas[key]
##         corr = 135.0/mass 
##         lam  = corr*lams[-1] 
##         lams.append ( lam.value() )
        
        r0 = Fit.getPi0Params ( hs[0] )
        r1 = Fit.getPi0Params ( hs[1] )
        r2 = Fit.getPi0Params ( hs[2] )

        alpha = 1.0
        
        deltam = r0[1]-135.0 
        corr1  = 1.0 - alpha*deltam/r0[1]  

        deltam = r1[1]-135.0 
        corr2  = 1.0 - alpha*deltam/r1[1]  
        
        kappa  = hc[1]
        deltam = r2[1]-135.0 
        corr3  = 1.0 - alpha*deltam/r2[1]/(1.0-kappa)
        
        lams = lambdas[key]

        corr = corr1
        if not 0.0001 < corr.error() <= 0.01  : corr = corr.mean ( corr2 )
        if not 0.0001 < corr.error() <= 0.01  : corr = corr.mean ( corr3 )

        print 'corr: ' , corr1, corr2, corr3, corr 
            
        lam  = corr*lams[-1]
        lams.append ( lam.value() )
        
        print ' MASS: %20s %20s %20s ' % ( r0[1] , r1[1] , r2[1] ) , key, corr , [  '%.3f' % l for l in lambdas[key] ] 
        
    
# =============================================================================

## Define 'fake' cell (useful) for grouping 
def fakeCell ( c ) :
    """
    Define 'fake' cell (useful) for grouping 
    """
    i = c.row()
    j = c.col()
    
    z   = math.sqrt ( (i-31.5)**2 + (j-31.5)**2 )
    z  -= 5
    z   = abs ( z )
    z   = int ( z / 4 )
    
    nc  = Kali.CellID ( c.calo () , c.area () , z , 1 )
    
    return nc 

# =============================================================================
if '__main__' == __name__ :
    
    ## make printout of the own documentations 
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print '*'*120  
    
    f=ROOT.TFile ( "KaliPi0_Tuples.root" )
    
    bamboo = f.Get ("KaliPi0/Pi0-Tuple" )
    
    histos, lambdas = fillHistos ( bamboo , cellFunc = fakeCell )
    
    analyse ( histos , lambdas )
    
    ## iterate ?
    for n in range  ( 0 , 12 ) :
    ##for n in range  ( 0 , 0 ) :
        
        print ' ITERATION : ', n
        histos, lambdas = fillHistos (
            bamboo   ,
            histos   ,
            lambdas  ,
            fakeCell 
            )
        
        analyse ( histos , lambdas )

        lams = lambdas.lambdas()

        print 'Calibration Coefficients '
        keys = lams.keys()
        keys.sort() 
        for key in Kali.Zones  :
            keys.remove ( key     )
            keys.insert ( 0 , key ) 
        for key in keys :
            print ' %-25s %9.4g  ' % ( key , lams[key] ) , [ ' %.3f ' % l for l in lambdas[key] ]
        
        
# =============================================================================
# The END 
# =============================================================================
