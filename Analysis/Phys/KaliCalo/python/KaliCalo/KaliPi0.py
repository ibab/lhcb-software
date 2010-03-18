#!/usr/bin/env python
# =============================================================================
# $Id: KaliPi0.py,v 1.13 2010-03-18 11:14:23 ibelyaev Exp $ 
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
__version__ = " CVS Tag $Name: not supported by cvs2svn $, version $Revision: 1.13 $"
# =============================================================================
## the basic import
from Gaudi.Configuration    import *
from KaliCalo.Configuration import firstPass, secondPass  

##kali = firstPass ( 
##    PrintFreq  =  1000 ,
##    EvtMax     =  -1 
##    )


import CaloUtils.CellID
from GaudiPython.Bindings import gbl as cpp
LHCb       = cpp.LHCb
## decorate CaloCellID class 
import CaloUtils.CellID
CellID     = LHCb.CaloCellID

import shelve
dbase = shelve.open('prs_db','r')
prs   = dbase['prs-2009']

##prs = { "(0,2,49,52)" : 0.93999999999999995 } 
##prs = { LHCb.CaloCellID('Prs','Inner',49,52) : 0.93999999999999995 } 

print ' PRESHOWER ', len(prs) 

kali = secondPass (
    PrsCoefficients = prs   , 
    PrintFreq       =  1000 ,
    EvtMax          =  -1 
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
    
    ##import KaliCalo.Data2009Reco07
    
    from GaudiPython.Bindings import AppMgr    
    gaudi = AppMgr()
    
    evtSel = gaudi.evtSel()
    evtSel.open('castor:/castor/cern.ch/user/i/ibelyaev/Kali/Pi0_2009_Reco07.fmDST')
    
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
