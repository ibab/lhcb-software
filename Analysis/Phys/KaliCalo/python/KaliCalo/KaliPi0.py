#!/usr/bin/env python
# =============================================================================
# $Id: KaliPi0.py,v 1.14 2010-03-22 18:24:00 ibelyaev Exp $ 
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
__version__ = " CVS Tag $Name: not supported by cvs2svn $, version $Revision: 1.14 $"
# =============================================================================
## the basic import
from   Gaudi.Configuration       import *
from   GaudiKernel.SystemOfUnits import MeV 
from   KaliCalo.Configuration    import firstPass, secondPass
import KaliCalo.Kali             as Kali 
import KaliCalo.ZipShelve        as ZipShelve 

## get Prs correction coefficients

dbase = ZipShelve.open ('/afs/cern.ch/lhcb/group/calo/ecal/vol10/ECAL-2k+9/prs-2k+9_db') 
prs   = dbase['Prs-2k+9']

cnt = Kali.Counter()
for key in prs : cnt += prs[key]
    
print 'Prs_coefficients : MEAN ( %.3f+-%.3f) '         % ( cnt.flagMean    ()  ,
                                                           cnt.flagMeanErr ()  )  
print 'Prs_coefficients : Min/Max/RMS %.3f/%.3f/%.3f ' % ( cnt.flagMin () ,
                                                           cnt.flagMax () ,
                                                           cnt.flagRMS () ) 

kali = firstPass (
    ## ``Physics''
    PtGamma          = 250 * MeV ,
    PtPi0            = 400 * MeV ,
    ## ``Calibration''
    PrsCoefficients  = prs ,
    ## IO 
    NTuple           = "KaliPi0_Tuples_2k+9-Loose.root" , 
    FemtoDST         = "KaliPi0_2k+9-Loose.fmDST"       ,
    ## general 
    DataType         = '2009',
    Simulation       = False ,
    PrintFreq        =  5000 ,
    EvtMax           =  -1 
    )

## ecal = dbase['Ecal-2k+9']
## prs  = dbase['Prs-2k+9']

## dbase = shelve.open('Ecal_2k+10_Mar_18_Calib-01_db','r')

## ecal = dbase['Ecal-2k+9']
## prs  = {}


## print ' PRESHOWER ', len(prs), len(ecal) 

## for key in ecal :
##     print key, ecal[key]
    
## kali = secondPass (
##     Coefficients    = ecal, 
##     PrsCoefficients = prs   , 
##     PrintFreq       =  1000 ,
##     EvtMax          =  -1   ,
##     NTuple   = "KaliPi0_Tuples_Calib-02.root" , 
##     FemtoDST = "KaliPi0_Calib-02.fmDST"  
##     )

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
    
    import KaliCalo.Data2009Reco07
    
    from GaudiPython.Bindings import AppMgr    
    gaudi = AppMgr()
    
    ##evtSel = gaudi.evtSel()
    ## evtSel.open('castor:/castor/cern.ch/user/i/ibelyaev/Kali/Pi0_2009_Reco07.fmDST')
    ##evtSel.open('castor:/castor/cern.ch/user/i/ibelyaev/Kali/KaliPi0_Calib-01.fmDST')
    
    gaudi.run( -1 )

    from   KaliCalo.Pi0HistoFit import fitPi0 , getPi0Params  
    
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
                print 'Fit: ', st, getPi0Params ( histo )
                
    gaudi.exit()
    
# =============================================================================
# The END 
# =============================================================================
