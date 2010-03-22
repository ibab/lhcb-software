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
__version__ = " CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $ "
# =============================================================================

import KaliCalo.Kali        as Kali
import KaliCalo.Pi0HistoFit as Fit 

# =============================================================================
## Analyse the histograms 
def  analyse ( histomap , lambdamap ) :    
    """
    Analyse the histograms 
    """
    if issubclass ( type ( histomap ) , str ) :
        
        histos  = Kali.HistoMap()
        print ' Read DBASE ' ,  histos.read ( histomap )
        histomap = histos
                   
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

    import sets

    badCells = sets.Set()
    lowCells = sets.Set()
    
    for key in keys :

        if min ( histomap[key].entries() ) < 30 : 
            ## print ' too low statistics for key', key
            lowCells.add  ( key ) 
            continue
        
        hs     = histomap[key].histos() 
        result = (0,0,0)
        
        if key in Kali.Zones :
            
            r0 = Fit.getPi0Params ( hs[0] )
            r1 = Fit.getPi0Params ( hs[1] )
            r2 = Fit.getPi0Params ( hs[2] )
            
            print ' MASS : %.20s %.20s %.20s ' % ( r0[1] , r1[1] , r2[1] ) , key 
            print ' SIGMA: %.20s %.20s %.20s ' % ( r0[2] , r1[2] , r2[2] ) , key 
            print ' NUM0 : %.20s %.20s %.20s ' % ( r0[0] , r1[0] , r2[0] ) , key
            print ' S/B  : %.20s %.20s %.20s ' % ( Fit.s2b ( hs[0] ) ,
                                                   Fit.s2b ( hs[1] ) ,
                                                   Fit.s2b ( hs[2] ) ) , key
            print '  '
            continue 
        
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
            ## print ' no reliable fits for ',key
            badCells.add ( key ) 
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
        
        lams = lambdamap[key]

        m = r0[1]
        if  0.001 > m.error() :
            m = Kali.VE( 135, 135**2 ) 
        
        corr = Kali.VE( 1 , 1 )
        if corr1.cov2() > min ( corr2.cov2() , corr3.cov2() ) :
            corr = corr.mean ( corr1 )
        
        if abs ( m.value() - 135 ) > 10 or not 0.0001 < corr.error() <= 0.01  :
            ##corr2.setCovariance (  1.2 * corr2.cov2() ) 
            corr = corr.mean ( corr2 )
        m = m.mean( r1[1] ) 
        if abs ( m.value() - 135 ) > 10 or not 0.0001 < corr.error() <= 0.01  :
            ##corr3.setCovariance (  2.0 * corr3.cov2() ) 
            corr = corr.mean ( corr3 )
        m = m.mean( r2[1] )

        ## do not allow too large corrections per one step 
        if corr.value() < 0.90 : corr.setValue ( 0.90 )
        if corr.value() > 1.10 : corr.setValue ( 1.10 )
        
        lam  = corr*lams[-1]
        lams.append ( lam.value() )

        if len ( histomap ) - 4 < 25 :  
            print ' NUM0: %.20s %.20s %.20s ' % ( r0[0] , r1[0] , r2[0] ) , key
        if len ( histomap ) - 4 < 55 :              
            print ' MASS: %.20s %.20s %.20s ' % ( r0[1] , r1[1] , r2[1] ) , key, corr , [  '%.3f' % l for l in lambdamap[key] ] 


    ##print " Bad Cells: " , len( badCells) , badCells
    ##print " Low Cells: " , len( lowCells) , lowCells
    return (badCells,lowCells)  


# =============================================================================
# The END 
# =============================================================================
