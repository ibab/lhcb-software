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

> python Pi0.py

niter is the desired number of iterations (10 by default)
alam   is the dictionary of the calibration coefficients, cell IDs are used as
       the dictionary keys
pi0mas is the dictionary of the pi0 mass histograms, cell IDs are used as the
       dictionary keys
"""
# =============================================================================
import ROOT
c1 = ROOT.TCanvas()

import Kali , math 

def fillHistos ( tree                          ,
                 histomap  = Kali.HistoMap  () ,
                 lambdamap = Kali.LambdaMap () ,
                 cellFunc  = lambda s : s      ) :
    
    from Pi0FillHistos import FillPi0
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

    import Pi0HistoFit as Fit 

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
    Fit.fitHistoSet ( hI , hA )
    Fit.fitHistoSet ( hM , hA )
    Fit.fitHistoSet ( hO , hA )

    
    keys = histomap.keys()
    keys.sort()
    
    for key in Kali.Zones  :
        keys.remove ( key     )
        keys.insert ( 0 , key ) 
        
    for key in keys :


        if min ( histomap[key].entries() ) < 50 : 
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
                
        if   0 == result[0] :  ## use the sample with no Prs 
            r = Fit.getPi0Params( hs[0] )
        elif 0 == result[1] :  ## use the sample with only 1 Prs 
            r = Fit.getPi0Params( hs[1] )
        elif 0 == result[2] :  ## use the sample with two Prs 
            r = Fit.getPi0Params( hs[2] )
        else :
            continue

        r = Fit.getPi0Params( hs[2] )

        lams = lambdas[key]
        corr = 1.0 - (r[1]-135.0)/r[1]
        lam  = corr*lams[-1] 
        lams.append ( lam )

        r0 = Fit.getPi0Params ( hs[0] )
        r1 = Fit.getPi0Params ( hs[1] )
        r2 = Fit.getPi0Params ( hs[2] )
                
        print  ( " %9.5g %9.5g %9.5g" % ( r0[1] , r1[1] , r2[1] ) ,  
                 " %9.5g %9.5g %9.5g" % ( r0[0] , r1[0] , r2[0] ) ,
                 " %9g %9g %9g"       % ( hs[0].GetEntries() ,
                                          hs[1].GetEntries() ,
                                          hs[2].GetEntries() ) , result , key ) , lam  
        


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
    
    f=ROOT.TFile ( "KaliPi0_Tuples.root" )
    
    bamboo = f.Get ("KaliPi0/Pi0-Tuple" )
    
    histos, lambdas = fillHistos ( bamboo , cellFunc = fakeCell )
    
    analyse ( histos , lambdas )
    
    ## iterate ?
    for n in range  ( 0 , 5 ) :
        
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
    for key in lams :
        print ' %-25s %9.4g ' % ( key , lams[key] )
        
        
# =============================================================================
# The END 
# =============================================================================
