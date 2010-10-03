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
__version__ = " CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.10 $ "
# =============================================================================
__all__     = (
    "analyse" ,
    )
# =============================================================================
import KaliCalo.Kali        as     Kali
import KaliCalo.Pi0HistoFit as     Fit
import random

# =============================================================================
## bad parameters ? 
def badParam ( mass  , sigma , num , corr ) :
    """
    Bad parameters ?
    """
    
    # bad mass ?
    if 10 < abs ( mass.value() - 135 )    : return True 

    # bad sigma? 
    if not 7 <= sigma.value() <= 18       : return True
    
    # bad number ?
    if  num.value() < 25                  : return True
    
    # bad number ?
    if  num.value() / num.error () < 3.5  : return True
    
    # bad precision ?
    if not 0.00001 < corr.error() <= 0.01 : return True

    return False 

# =============================================================================
## get the histograms fomr data base (databases)
def  getHistosFromDB ( dbases ) :
    """
    Get the histograms from data base (databases)
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
               manager   = None  ,
               nHistos   = 60    ) :    
    """
    Analyse the histograms 
    """
    histomap = getHistosFromDB ( histomap )
    
    if not fitted :
        import KaliCalo.FitTask as Task 
        histomap = Task.fitHistos (
            histomap ,
            manager  ,
            nHistos  )                 

    keys = histomap.keys()    
    keys.sort()
    for key in Kali.Zones  :
        if key in keys : keys.remove ( key     )
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
        
        if max ( histomap[key].entries() ) < 100 :
            lowCells.add  ( key ) 
            continue
        
        hs     = histomap[key]
        
        if key in Kali.Zones :
            
            r0 = Fit.getPi0Params ( hs[0] )
            r1 = Fit.getPi0Params ( hs[1] )
            r2 = Fit.getPi0Params ( hs[2] )

            if r0 and r1 and r2 :  
                print ' MASS : %-24.24s %-24.24s %-24.24s ' % ( r0[1] , r1[1] , r2[1] ) , key 
                print ' SIGMA: %-24.24s %-24.24s %-24.24s ' % ( r0[2] , r1[2] , r2[2] ) , key 
                print ' NUM0 : %-24.24s %-24.24s %-24.24s ' % ( r0[0] , r1[0] , r2[0] ) , key
                print ' S/B  : %-24.24s %-24.24s %-24.24s ' % ( Fit.s2b ( hs[0] ) ,
                                                                Fit.s2b ( hs[1] ) ,
                                                                Fit.s2b ( hs[2] ) ) , key
                print '  '
            else :
                print 'No fit information available for ZONE: ', key 
                
            continue 

        result = None
        if   hasattr ( hs , 'result' ) : result = hs.result
        
        if not result : 
            ## print ' no reliable fits for ',key
            notFitCells.add ( key ) 
            continue 

        iCnt  += result[-1] 
        
        r0 = Fit.getPi0Params ( hs[0] )
        r1 = Fit.getPi0Params ( hs[1] )
        r2 = Fit.getPi0Params ( hs[2] )

        r0 = r0 if 0 <= result[0] else None
        r1 = r1 if 0 <= result[1] else None
        r2 = r2 if 0 <= result[2] else None
        
        if r0 and r0[2].value() < 5 :
            print r0 , hs[0].GetName() 

        if not r0 and not r1 and not r2 :
            notFitCells.add ( key ) 
            continue 
        
        alpha = 1.0

        mass  = Kali.VE ( 135 , 135**2 )
        sigma = Kali.VE (  12 ,  12**2 ) 
        corr  = Kali.VE (   1 ,   1**2 )
        num   = Kali.VE (   1 ,   1**2 )

        Used = False
        
        if r0 and ( 6 <= r0[2].value() <= 20 ) and  ( 15 <= abs ( r0[0].value() ) ) :
            m      = r0[1]
            deltam = r0[1]-135.0 
            corr1  = 1.0 - alpha*deltam/r0[1]
            s      = r0[2]
            if 0.00001 < corr1.error() < 0.50 :  
                corr   = corr.mean  ( corr1 )
                mass   = mass.mean  ( m     )
                if s.error() > 0.00001     : sigma  = sigma.mean ( s )
                else                       : sigma.setValue ( s.value() ) 
                num    = abs ( r0[0] )
                Used   = True 

        use = badParam  ( mass , sigma , num , corr ) or not Used 
        if r1 and use : 
            m      = r1[1]
            deltam = r1[1]-135.0 
            corr2  = 1.0 - alpha*deltam/r1[1]
            s      = r1[2]
            if 0.00001 < corr2.error() < 0.50 :  
                corr   = corr.mean  ( corr2 )
                mass   = mass.mean  ( m     )
                if s.error() > 0.00001 : sigma  = sigma.mean ( s )  
                num   += abs ( r1[0] ) 
                Used   = True
                
        use = badParam  ( mass , sigma , num , corr ) or not Used 
        if r2 and use : 
            m      = r2[1]
            deltam = r2[1]-135.0 
            hc     = hs.kappas() 
            kappa  = hc[1]
            corr3  = 1.0 - alpha*deltam/r2[1]/(1.0-kappa) 
            s      = r2[2]
            if 0.00001 < corr3.error() < 0.50 :  
                corr   = corr.mean ( corr3 )
                mass   = mass.mean ( m     )
                if s.error() > 0.00001 : sigma  = sigma.mean ( s )  
                num   += abs ( r2[0] )

        if not Used :
            badCells.add ( key ) 
            continue 
            
        lams = lambdamap[key]

        ## do not allow too large corrections per one step 
        if corr.value() < 0.90 : corr.setValue ( 0.90 )
        if corr.value() > 1.10 : corr.setValue ( 1.10 )
        
        lam  = corr*lams[-1]
        lams.append ( lam.value() )

        nFits = Kali.VE( iCnt.flagMean() , iCnt.flagRMS() )
        
        mx = 100.0 / len ( histomap )
        if random.uniform( 0 , 1.0 ) < mx and r0 and r1 and r2 :
            print ' ' 
            print ' KEY   :' , key , ikey , len ( histomap ) , result , iCnt.flag() , nFits 
            print ' NUM0  : %-24.24s %-24.24s %-24.24s ' % ( r0[0] , r1[0] , r2[0] ) 
            print ' MASS  : %-24.24s %-24.24s %-24.24s ' % ( r0[1] , r1[1] , r2[1] ) , corr , [  '%.3f' % l for l in lambdamap[key] ] 
            print ' SIGMA : %-24.24s %-24.24s %-24.24s ' % ( r0[2] , r1[2] , r2[2] ) 
            print ' S/B   : %-24.24s %-24.24s %-24.24s ' % ( Fit.s2b ( hs[0] ) ,
                                                             Fit.s2b ( hs[1] ) ,
                                                             Fit.s2b ( hs[2] ) ) 
            
    return (badCells,lowCells,notFitCells)  




# =============================================================================
# The END 
# =============================================================================
