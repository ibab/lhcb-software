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
__version__ = " CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $ "
# =============================================================================
import ROOT, math 
c1 = ROOT.TCanvas()

import KaliCalo.Kali        as Kali 
import KaliCalo.ZipShelve   as ZipShelve 

from KaliCalo.Pi0FillHistos import fillHistos
from KaliCalo.Analyse       import analyse

    
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

    
    fname      = "KaliPi0_Tuples_2k+9-Loose.root" 

    f    = ROOT.TFile( fname )
    tree = f.Get ("KaliPi0/Pi0-Tuple" )
    
    histos, lambdas = fillHistos ( tree , cellFunc = fakeCell )
    
    analyse ( histos , lambdas )

    import shelve

    bad = []
    low = [] 
    ## iterate ?
    for n in range  ( 0 , 10 ) :
    ##for n in range  ( 0 , 0 ) :
        
        print ' ITERATION : ', n
        histos.reset() 
        histos, lambdas = fillHistos (
            tree     , 
            histos   ,
            lambdas  ,
            fakeCell 
            )
        
        bad, low = analyse ( histos , lambdas )
        print " Bad/low cells: %d/%d " % ( len ( bad ) , len ( low ) )  

        lams = lambdas.lambdas()

        print 'Calibration Coefficients '
        keys = lams.keys()
        keys.sort() 
        for key in Kali.Zones  :
            if key in keys : 
                keys.remove ( key     )
                keys.insert ( 0 , key )
            
        for key in keys :
            if key in Kali.Zones :
                hs     = histomap[key].histos() 
                r0 = Fit.getPi0Params ( hs[0] )
                r1 = Fit.getPi0Params ( hs[1] )
                r2 = Fit.getPi0Params ( hs[2] )
                print ' MASS : %.20s %.20s %.20s ' % ( r0[1] , r1[1] , r2[1] ) , key 
                print ' SIGMA: %.20s %.20s %.20s ' % ( r0[2] , r1[2] , r2[2] ) , key 
                print ' NUM0 : %.20s %.20s %.20s ' % ( r0[0] , r1[0] , r2[0] ) , key
                print ' S/B  : %.20s %.20s %.20s ' % ( r0[0] / r0[3] ,
                                                       r1[0] / r1[3] ,
                                                       r2[0] / r2[3] ) , key
                continue
            
            print ' %-25s %9.4g  ' % ( key , lams[key] ) , [ ' %.3f ' % l for l in lambdas[key] ]



    dbase_name = 'Ecal_2k+9-Loose-details-zdb' 
    histos.save  ( dbase_name )
    lambdas.save ( dbase_name )
    
    
    
    celldb = shelve.open ( 'cells_db', 'r' )
    cells  = celldb['AllCells']
    
    lams   = lambdas.lambdas()
    map    = buildTable ( lams     ,
                          cells    ,
                          fakeCell )         
    prsdbase = ZipShelve.open ('prs-2k+9_zdb') 
    prs = prsdbase['Prs-2k+9']
    
    dbase_r_name = 'Ecal_2k+9-Loose-results-zdb'
    dbase = ZipShelve.open ( dbase_r_name )
    dbase [ 'Ecal'   ] = map
    dbase [ 'Prs'    ] = prs
    dbase ['BadCells'] = bad
    dbase ['LowCells'] = low
    
    
# =============================================================================
# The END 
# =============================================================================
