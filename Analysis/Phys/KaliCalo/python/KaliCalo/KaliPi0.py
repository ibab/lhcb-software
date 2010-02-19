#!/usr/bin/env python
# =============================================================================
# $Id: KaliPi0.py,v 1.12 2010-02-19 12:03:13 ibelyaev Exp $ 
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
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, version $Revision: 1.12 $"
# =============================================================================
## the basic import
from Gaudi.Configuration    import *
from KaliCalo.Configuration import  KaliPi0Conf

from Configurables import GaudiSequencer

kali = KaliPi0Conf (
    ## example of the first pass 
    ## FirstPass    =  True 
    FirstPass    =  False 
    , UseTracks  =  False
    , UseSpd     =  False     
    , Mirror     =  True
    , Histograms =  True 
    ## 
    , PrintFreq  =  1000
    , EvtMax     =  -1 
    ##
    ##, Simulation = True 
    ##, DataType   = 'MC09' 
    ##
    )

# =============================================================================
## the actual job steering 
if '__main__' == __name__ :
    
    print __doc__
    print __author__
    print __version__

    ## import BenderExample.RealData 
    from GaudiPython.Bindings import AppMgr    
    gaudi = AppMgr()
    
    evtSel = gaudi.evtSel()
    evtSel.open('castor:/castor/cern.ch/user/i/ibelyaev/Kali/Pi0_2009_Reco07.fmDST')
    
    gaudi.run(-1 )
    
##     import GaudiPython.GaudiAlgs
##     import GaudiPython.HistoUtils
    
##     kali = gaudi.algorithm('KaliPi0')
##     histos = kali.Histos()
##     if histos :
##         keys = histos.keys()
##         keys.sort() 
##         for h in keys :
##             histo = histos[h]
##             if hasattr ( histo , 'dump' ) :
##                 print histo.dump(40,20,True) 
                
    gaudi.exit()
    
# =============================================================================
# The END 
# =============================================================================
