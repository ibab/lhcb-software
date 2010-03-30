#!/usr/bin/env python
# =============================================================================
# $Id: KaliPi0_200M_SP.py,v 1.1 2010-03-30 15:40:58 ibelyaev Exp $ 
# =============================================================================
## @file  KaliCalo/KaliPi0_200M_SP.py
#  The basic configuration to (re)run Ecal pi0-calibration:
#    - reprocess miscalibrated 200M of fmDST, produced by Albert Puig 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date   2010-03-28
# =============================================================================
"""
The basic configuration to (re)run Ecal pi0-calibration:
- reporcess miscalibrated 200M of fmDST, produced by Albert Puig 

"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@nikhef.nl "
__date__    = " 2010-03-28 "
__version__ = " CVS Tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $"
# =============================================================================
## the basic import
from   Gaudi.Configuration       import *
from   GaudiKernel.SystemOfUnits import MeV 
from   KaliCalo.Configuration    import secondPass


kali = secondPass (
    ## IO 
    NTuple           = "KaliPi0_SP.root"  , 
    FemtoDST         = "KaliPi0_SP.fmDST" ,
    ## general 
    DataType         = 'MC09',
    Simulation       =  True ,
    PrintFreq        = 10000 ,
    EvtMax           =    -1 
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
    evtSel.open('castor:/castor/cern.ch/grid/lhcb/user/a/apuignav/7318/7318685/KaliPi0.fmDST')
    
    gaudi.run( 100000 )

    import KaliCalo.Pi0HistoFit as Fit
    
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
                st = Fit.fitPi0 ( histo ) 
                print 'Fit: ', st, Fit.getPi0Params ( histo )
                
    gaudi.exit()
    
    
# =============================================================================
# The END 
# =============================================================================
