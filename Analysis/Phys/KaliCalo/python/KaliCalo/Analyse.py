#!/usr/bin/env python
# =============================================================================
## @file KaliCalo/Analyse.py
#  The main ``analyse'' for Kali
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2010-03-20
# =============================================================================
"""
The main ``analyse'' for Kali
"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyev@itep.ru "
__date__    = " 2010-03-20 "
__version__ = " CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.7 $ "
# =============================================================================
__all__     = (
    "analyse" ,
    )
# =============================================================================
import KaliCalo.Kali        as     Kali
import KaliCalo.Pi0HistoFit as     Fit
import random

# =============================================================================
## get the histograms fomr data base (databases)
def  getHistosFromDB ( dbases ) :
    """
    Get the histograms fomr data base (databases)
    """
    if issubclass ( type ( dbases ) , Kali.HistoMap ) : return dbases
    histos = Kali.HistoMap()
    histos.updateFromDB ( dbases )
    return histos


# =============================================================================
## Analyse the histograms 
def  analyse ( histomap          ,
               lambdamap         ,
               fitted    = False , 
               parallel  = True  ,
               ppservers = []    ) :    
    """
    Analyse the histograms 
    """
    histomap = getHistosFromDB ( histomap )
    
    if not fitted :
        from   KaliCalo.FitTask import fitHistos
        histomap = fitHistos (
            histomap  ,
            parallel  ,
            ppservers )                 

    keys = histomap.keys()    
    keys.sort()
    for key in Kali.Zones  :
        keys.remove ( key     )
        keys.insert ( 0 , key ) 
        
    import sets
    
    badCells    = sets.Set()
    notFitCells = sets.Set()
    lowCells    = sets.Set()
    
    ikey  = 0
    nFits = 0
    iCnt  = Kali.Counter() 
    for key in keys :

        ikey += 1
        
        if min ( histomap[key].entries() ) < 30 : 
            lowCells.add  ( key ) 
            continue
        
        hs     = histomap[key]
        
        if key in Kali.Zones :
            
            r0 = Fit.getPi0Params ( hs[0] )
            r1 = Fit.getPi0Params ( hs[1] )
            r2 = Fit.getPi0Params ( hs[2] )

            if r0 and r1 and r2 :  
                print ' MASS : %-20.20s %-20.20s %-20.20s ' % ( r0[1] , r1[1] , r2[1] ) , key 
                print ' SIGMA: %-20.20s %-20.20s %-20.20s ' % ( r0[2] , r1[2] , r2[2] ) , key 
                print ' NUM0 : %-20.20s %-20.20s %-20.20s ' % ( r0[0] , r1[0] , r2[0] ) , key
                print ' S/B  : %-20.20s %-20.20s %-20.20s ' % ( Fit.s2b ( hs[0] ) ,
                                                                Fit.s2b ( hs[1] ) ,
                                                                Fit.s2b ( hs[2] ) ) , key
                print '  '
            else :
                print 'No fit information available for ZONE: ', key 
                
            continue 

        result = None
        if   hasattr ( hs , 'result' ) : result = hs.result 
        else : 
            ## print ' no reliable fits for ',key
            notFitCells.add ( key ) 
            continue 

        iCnt  += result[-1] 
        
        r0 = Fit.getPi0Params ( hs[0] )
        r1 = Fit.getPi0Params ( hs[1] )
        r2 = Fit.getPi0Params ( hs[2] )

        if not r0 and not r1 and not r2 :
            badCells.add ( key ) 
            continue 
        
        alpha = 1.5

        mass  = Kali.VE(135,135**2)
        sigma = Kali.VE( 12, 12**2) 
        corr  = Kali.VE(  1,  1**2)

        if r0 :
            m      = r0[1]
            deltam = r0[1]-135.0 
            corr1  = 1.0 - alpha*deltam/r0[1]
            if 0.00001 < corr1.error() < 0.50 :  
                corr   = corr.mean ( corr1 )
                mass   = mass.mean ( m     )

        dm = abs  ( mass.value() - 135 ) 
        if r1 and ( dm > 10 or  not 0.00001 < corr.error() <= 0.01 ) :
            m      = r1[1]
            deltam = r1[1]-135.0 
            corr2  = 1.0 - alpha*deltam/r1[1]
            if 0.00001 < corr2.error() < 0.50 :  
                corr   = corr.mean ( corr2 )
                mass   = mass.mean ( m     )
                
        dm = abs  ( mass.value() - 135 ) 
        if r2 and ( dm > 10 or  not 0.00001 < corr.error() <= 0.01 ) :
            m      = r2[1]
            deltam = r2[1]-135.0 
            hc     = hs.kappas() 
            kappa  = hc[1]
            corr3  = 1.0 - alpha*deltam/r2[1]/(1.0-kappa)
            if 0.00001 < corr3.error() < 0.50 :  
                corr   = corr.mean ( corr3 )
                mass   = mass.mean ( m     )
    
        lams = lambdamap[key]

        ## do not allow too large corrections per one step 
        if corr.value() < 0.90 : corr.setValue ( 0.90 )
        if corr.value() > 1.10 : corr.setValue ( 1.10 )
        
        lam  = corr*lams[-1]
        lams.append ( lam.value() )

        nFits = Kali.VE( iCnt.flagMean() , iCnt.flagRMS() )
        
        mx = 100.0 / len ( histomap )
        if random.uniform( 0 , 1.0 ) < mx and r0 and r1 and r2 : 
            print ' KEY   :' , key , ikey , len ( histomap ) , result , iCnt.flag() , nFits 
            print ' NUM0  : %-20.20s %-20.20s %-20.20s ' % ( r0[0] , r1[0] , r2[0] ) 
            print ' MASS  : %-20.20s %-20.20s %-20.20s ' % ( r0[1] , r1[1] , r2[1] ) , corr , [  '%.3f' % l for l in lambdamap[key] ] 
            print ' SIGMA : %-20.20s %-20.20s %-20.20s ' % ( r0[2] , r1[2] , r2[2] ) 
            print ' S/B   : %-20.20s %-20.20s %-20.20s ' % ( Fit.s2b ( hs[0] ) ,
                                                             Fit.s2b ( hs[1] ) ,
                                                             Fit.s2b ( hs[2] ) ) 
            
    return (badCells,lowCells,notFitCells)  


    
# =============================================================================
# The END 
# =============================================================================
