#!/usr/bin/env python
# =============================================================================
## @file KaliPi0/Pi0_3.py
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

> python Pi0_3.py

"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@itep.ru "
__date__    = " 2010-03-20 "
__version__ = " CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $ "
# =============================================================================
import ROOT, math 
c1 = ROOT.TCanvas()

import KaliCalo.Kali as Kali 

from   KaliCalo.Pi0FillHistos    import fillDataBase
from   KaliCalo.Analyse          import analyse
from   GaudiKernel.SystemOfUnits import GeV
    
# =============================================================================
def buildTable ( lams  ,
                 cells ,
                 func  ) :

    results = {}
    for cell in cells :
        newCell = Kali.CellID ( func ( cell ) )
        if lams.has_key ( newCell ) :
            results [ cell ] = lams [ newCell ]
            
    return results 
    
# =============================================================================

def fakeCell ( cell ) :

    nc = Kali.CellID ( cell.calo () ,
                       cell.area () ,
                       cell.row  () , 1 ) 
    return  nc
    
# =============================================================================
if '__main__' == __name__ :
    
    ## make printout of the own documentations 
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print '*'*120  
    
    import KaliCalo.ZipShelve as ZipShelve 
    
    lfns = [ 'LFN:/lhcb/user/a/apuignav/6966/6966779/KaliPi0_Tuples.root',
             'LFN:/lhcb/user/a/apuignav/6966/6966780/KaliPi0_Tuples.root',
             'LFN:/lhcb/user/a/apuignav/6966/6966781/KaliPi0_Tuples.root',
             'LFN:/lhcb/user/a/apuignav/6966/6966782/KaliPi0_Tuples.root'
             ] 

    castor = '/castor/cern.ch/user/i/ibelyaev/Kali/MC09_Miscalib_Tuples_2/'
    
    files = [ '6966779.KaliPi0_Tuples.root',
              '6966780.KaliPi0_Tuples.root',
              '6966781.KaliPi0_Tuples.root',
              '6966782.KaliPi0_Tuples.root'
              ] 

    files = [ castor + f for f in files ]
    
    lambdas = Kali.LambdaMap()

    bad, low = [],[]

    dbase_name_1 = 'details_zdb'
    dbase_name_2 = 'results_zdb'
    
    for niter in range ( 0 , 1 ) :
        
        histos, lambdas , badfiles = fillDataBase (
            lambdas                   ,
            files[:2]                 , 
            dbase_name = 'kali_db.gz' , 
            Unit = GeV 
            )
        
        if badfiles : print ' Bad files: ', badfiles 
        
##         ## bad,low =  analyse ( histos , lambdas )
##         bad,low = [], [] ## analyse ( histos , lambdas )
        
##         if bad      : print 'Bad cells           ', len(bad) 
##         if low      : print 'Low occupancy cells ', len(low) 

##         lams = lambdas.lambdas()
        
##         print 'Calibration Coefficients '
##         keys = lams.keys()
##         keys.sort() 
##         for key in Kali.Zones  :
##             if key in keys : 
##                 keys.remove ( key     )
##                 keys.insert ( 0 , key )
##         for key in keys :
##             if key in Kali.Zones :
##                 hs     = histomap[key].histos() 
##                 r0 = Fit.getPi0Params ( hs[0] )
##                 r1 = Fit.getPi0Params ( hs[1] )
##                 r2 = Fit.getPi0Params ( hs[2] )
##                 print ' MASS : %.20s %.20s %.20s ' % ( r0[1] , r1[1] , r2[1] ) , key 
##                 print ' SIGMA: %.20s %.20s %.20s ' % ( r0[2] , r1[2] , r2[2] ) , key 
##                 print ' NUM0 : %.20s %.20s %.20s ' % ( r0[0] , r1[0] , r2[0] ) , key
##                 print ' S/B  : %.20s %.20s %.20s ' % ( r0[0] / r0[3] ,
##                                                        r1[0] / r1[3] ,
##                                                        r2[0] / r2[3] ) , key
##                 continue
            
##             print ' %-25s %9.4g  ' % ( key , lams[key] ) , [ ' %.3f ' % l for l in lambdas[key] ]

        
##        dbase = ZipShelve.open ( dbase_results_1 )
##        dbase [ 'Ecal'] = lambdas
##        dbase.close()

        
# =============================================================================
# The END 
# =============================================================================
