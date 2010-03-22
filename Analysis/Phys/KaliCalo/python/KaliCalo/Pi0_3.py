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

niter is the desired number of iterations (10 by default)
alam   is the dictionary of the calibration coefficients, cell IDs are used as
       the dictionary keys
pi0mas is the dictionary of the pi0 mass histograms, cell IDs are used as the
       dictionary keys
"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@itep.ru "
__date__    = " 2010-03-20 "
__version__ = " CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $ "
# =============================================================================
import ROOT, math 
c1 = ROOT.TCanvas()

import KaliCalo.Kali as Kali 

from KaliCalo.Pi0FillHistos    import fillDataBase
from KaliCalo.Analyse          import analyse
from GaudiKernel.SystemOfUnits import GeV
    
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


# =============================================================================
if '__main__' == __name__ :
    
    ## make printout of the own documentations 
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print '*'*120  
    
    castor = "castor:/castor/cern.ch/user/i/ibelyaev/Kali/MC09_Miscalib_Tuples_2/"
    fname      = "6966779.KaliPi0_Tuples.root" 
    fname2     = "6966780.KaliPi0_Tuples.root" 
    dbase_name = 'MC09_1_db' 

##     f    = ROOT.TFile().Open( castor + fname )
##     tree = f.Get ("KaliPi0/Pi0-Tuple" )
    
##     fakeCell = lambda s : s
    
    lambdas = Kali.LambdaMap()
    
    ##[ castor + fname  ,
    ##  castor + fname2 ] ,
    
    histos, lambdas , badfiles = fillDataBase (
        lambdas        , 
        [ 'LFN:/lhcb/user/a/apuignav/6966/6966779/KaliPi0_Tuples.root',
          'LFN:/lhcb/user/a/apuignav/6966/6966780/KaliPi0_Tuples.root',
          'LFN:/lhcb/user/a/apuignav/6966/6966781/KaliPi0_Tuples.root',
          'LFN:/lhcb/user/a/apuignav/6966/6966782/KaliPi0_Tuples.root'
          ] ,
        Unit = GeV 
        )
    
    print ' AFTER fill dbase '


#    histos, lambdas = fillHistos ( tree                ,
#                                   cellFunc = fakeCell ,
#                                   Unit     = GeV      )
#

    bad,low =  0 , 0  ## analyse ( histos , lambdas )
    if bad :
        print 'Bad cells           ', len(bad) 
    if low :
        print 'Low occupancy cells ', len(low) 
            
##     import shelve
    
##     celldb = shelve.open ( 'cells_db', 'r' )
##     cells  = celldb['AllCells']
    
##     ## iterate ?
##     ##for n in range  ( 0 , 4 ) :
##     for n in range  ( 0 , 0 ) :
        
## ##         print ' ITERATION : ', n
## ##         histos, lambdas = fillHistos (
## ##             tree       ,   
## ##             histos     ,
## ##             lambdas    ,
## ##             fakeCell   ,
## ##             Unit = GeV  
## ##             )
        
##         bas, low = analyse ( histos , lambdas )

##         if bad :
##             print 'Bad cells           ', len(bad) 
##         if low :
##             print 'Low occupancy cells ', len(low) 

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

##         map = buildTable ( lams     ,
##                            cells    ,
##                            fakeCell )         
        
##         dbase = shelve.open ( dbase_name )
##         dbase [ 'Ecal_map_iter_%d'     % n ] = map
##         dbase.close()
        
    
##     dbase = shelve.open ( dbase_name )
##     dbase [ 'Ecal-2k+9' ] = map
##     dbase [ 'Cells'     ] = cells 
##     dbase [ 'Histos'    ] = histos 
##     dbase [ 'Lambdas'   ] = lambdas
        
##     ##prsdb = shelve.open('prs_db')
##     ##prs = prsdb['prs-2009']
##     ##dbase['Prs-2k+9'] = prs

##     for key in dbase :
##         print key

        
# =============================================================================
# The END 
# =============================================================================
