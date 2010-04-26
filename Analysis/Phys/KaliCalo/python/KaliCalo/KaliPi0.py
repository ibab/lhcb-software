#!/usr/bin/env python
# =============================================================================
# $Id: KaliPi0.py,v 1.15 2010-04-26 11:42:38 ibelyaev Exp $ 
# =============================================================================
## @file  KaliCalo/KaliCaloPi0.py
#  The basic configuration to (re)run Ecal pi0-calibration
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date   2009-09-17
# =============================================================================
"""
The basic configuration to (re)run Ecal pi0-calibration

One reads the input data (presumably DST or fmDST),
(re) recontruct Calorimeter, and produces two major outputs:

 -  root file with N-tuple for 'pi0-calibration'
 - 'femtoDST' which contains only 'interesting' digits

Usage:
 
  > gaudirun.py KaliPi0.py InputDataFiles.py

Or (suitable for tests)

  > python ./KaliPi0.py

Or even:

  > ./KaliPi0.py

"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@nikhef.nl "
__date__    = " 2009-09-17 "
__version__ = " CVS Tag $Name: not supported by cvs2svn $, version $Revision: 1.15 $"
# =============================================================================
## the basic import
from   Gaudi.Configuration       import *
from   GaudiKernel.SystemOfUnits import MeV 
from   KaliCalo.Configuration    import firstPass, secondPass
import KaliCalo.Kali             as Kali 
import KaliCalo.ZipShelve        as ZipShelve 


## kali = firstPass (
##     ## ``Physics''
##     PtGamma          = 250 * MeV ,
##     PtPi0            = 400 * MeV ,
##     ## IO 
##     NTuple           = "KaliPi0_Tuples_2k+10.root" , 
##     FemtoDST         = "KaliPi0_2k+10.fmDST"       ,
##     ## general 
##     DataType         = '2010',
##     PrintFreq        =  5000 ,
##     EvtMax           =  -1 
##     )

kali = secondPass (
    ## ``Physics''
    PtGamma          = 250 * MeV ,
    PtPi0            = 400 * MeV ,
    ## IO 
    NTuple           = "KaliPi0_Tuples_2k+10.root" , 
    FemtoDST         = "KaliPi0_2k+10.fmDST"       ,
    ## general 
    DataType         = '2010',
    PrintFreq        =  5000 ,
    EvtMax           =  -1 
    )

# =============================================================================
## the actual job steering 
if '__main__' == __name__ :
    
    ## make printout of the own documentations 
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print '*'*120  
    
    
    from GaudiPython.Bindings import AppMgr    
    gaudi = AppMgr()
    
    evtSel = gaudi.evtSel()
    
    castor   =  'castor:/castor/cern.ch/user/d/dsavrina/Real2010/'
    pattern  =  'KaliPi0_20100404_%d.fmDST'
    evtSel.open ( [ castor + pattern % i for i in range(1,3) ] )

    gaudi.run(20000)
    
    from   KaliCalo.Pi0HistoFit import fitPi0 , getPi0Params, s2b   
    import GaudiPython.GaudiAlgs 
    pi0    = gaudi.algorithm('KaliPi0')
    histos = pi0.Histos()
    if histos :
        keys = histos.keys()
        keys.sort () 
        for h in keys :
            histo = histos[h]
            if hasattr ( histo , 'dump' ) :
                print histo.dump(40,20,True) 
                st = fitPi0 ( histo ) 
                print 'Fit    : ' , st
                print 'N(pi0) : ' , getPi0Params ( histo )[0] 
                print 'Mass   : ' , getPi0Params ( histo )[1] 
                print 'Sigma  : ' , getPi0Params ( histo )[2] 
                print 'S/B    : ' , s2b ( histo.toROOT() ) 
                
    gaudi.exit()
    
# =============================================================================
# The END 
# =============================================================================
