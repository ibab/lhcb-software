#!/usr/bin/env python
# =============================================================================
# $Id$ 
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
__version__ = " CVS Tag $Name: not supported by cvs2svn $, version $Revision$"
# =============================================================================
## the basic import
from   Gaudi.Configuration       import *
from   GaudiKernel.SystemOfUnits import MeV 
from   KaliCalo.Configuration    import firstPass, secondPass
import KaliCalo.Kali             as Kali 
import KaliCalo.ZipShelve        as ZipShelve 

##kali = firstPass (
##    ## ``Physics''
##    PtGamma          = 250 * MeV ,
##    Pi0Cut           = "PT > 550 * MeV" ,
##    ## IO 
##    NTuple           = "KaliPi0_Tuples_2k+10.root" , 
##    FemtoDST         = "KaliPi0_2k+10.fmDST"       ,
##    ## general 
##    DataType         = '2010',
##    PrintFreq        =  5000 ,
##    EvtMax           =  -1
##    )

kali = secondPass (
    ## 
    DestroyTES = False , 
    ## ``Physics''
    PtGamma          = 250 * MeV ,
    Pi0Cut           = "PT > 550 * MeV",
    ## IO 
    NTuple           = "KaliPi0_Tuples_2k+11.root" , 
    FemtoDST         = "KaliPi0_2k+11.fmDST"   ,
    ## general 
    DataType         = '2010',
    PrintFreq        =  1000 ,
    EvtMax           =  -1
    )

from Configurables import CondDB
CondDB ( UseLatestTags = ['2011'] )


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
    
    #### === run with test SDSTs as an input
    ##from Configurables import DaVinci
    ##DaVinci().InputType = 'SDST'                            # use SDSTs as an input
    #### SDSTs for Stripping tests, 2010 data, mu = 2.7
    ##importOptions('$STRIPPINGSELECTIONSROOT/tests/data/RUN_81430_RealData+Reco08-Stripping12_90000000_SDST.py')

    
    from GaudiConf.IOHelper import IOHelper
    
    ioh = IOHelper()
    
    eos      = 'root://eoslhcb.cern.ch//eos'
    pattern  = '/lhcb/LHCb/Collision11/FMDST/00011385/0000/00011385_00000%03d_1.fmdst'
    ioh.inputFiles ( [ eos+pattern%i for i in range(1,10) ] )
    
    #evtSel.open ( 'KaliPi0_2k+11.fmdst_test')

    from GaudiPython.Bindings import AppMgr    
    gaudi = AppMgr()
    
    ## === OR with test fmDSTs
    
    evtSel = gaudi.evtSel()
        
    ## run 100 events
    gaudi.run(5000)
    
    from   KaliCalo.FitUtils import fitPi0 , getPi0Params, s2b   
    import GaudiPython.GaudiAlgs 
    pi0    = gaudi.algorithm('KaliPi0')
    histos = pi0.Histos()
    if histos :
        keys = histos.keys()
        keys.sort () 
        for h in keys :
            histo = histos[h]
            if hasattr ( histo , 'dump' ) :
                print histo.dump(60,25,True) 
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
