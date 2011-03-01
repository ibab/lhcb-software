#!/usr/bin/env python

from StrippingUtils import LineBuilderTests

config_params = {
     'PrescalepipiBox'     : 0.5
    , 'PrescalemumuBox'   : 1.
    , 'PrescaleKpiBox'    : 0.3
    , 'PrescaleemuBox'    : 1.
    , 'PrescaleeKBox'     : 1.
    , 'PrescaleepiBox'    : 1.
    , 'PrescalepimuBox'   : 1.
    , 'PrescaleKmuBox'    : 1.
    , 'Prescalepipi_untagged_Box'   : 0.2
    , 'Prescalemumu_untagged_Box'   : 1.
    , 'PrescaleKpi_untagged_Box'    : 0.2
    , 'Prescalepimu_untagged_Box'   : 1.
    , 'PrescaleKmu_untagged_Box'    : 0.2  
    ,'DMassWin'           : 70.       # MeV
    ,'DMassWinMuMu'       : 300       #MeV
    ,'DMassWinEMu'        : 300
    ,'doca'               : 0.1        # mm
    ,'XminPT'             : 750.       # MeV
    ,'XmaxPT'             : 1100.      # MeV
    ,'XminP'              : 4000.      # MeV
    ,'XTrackChi2'         : 5.        # adimensional
    ,'XTrackChi2Pi'       : 7.        # adimensional
    ,'XminIPChi2'         : 3        # adimensional
    ,'XmaxIPChi2'         : 8        # adimensional
    ,'DMinFlightChi2'     :  20.
    ,'DDira'              : 0.9997     # adimensional
    ,'D0MinPT'            : 1800.      # MeV
    ,'DMaxIPChi2'         : 15.
    ,'DVChi2'             : 10. 
    ,'PiMinPT'            : 110.       # MeV
    ,'PiMaxIPCHI2'        : 10.         # adimensional
    ,'DstMassWin'         : 110.       # MeV
    ,'DstD0DMWin'         : 10.        # MeV
    ,'DstD0DMWinMuMu'      : 30.        # MeV  
    ,'RequireHlt'         : 1          # 
     ,'prefix'         : ''  
    }

from StrippingSelections.StrippingDstarD02xx import StrippingDstarD02xxConf as builder

def test_line_builder() :
    LineBuilderTests.test_line_builder(builder, config_params)

if __name__ == '__main__' :
    
    test_line_builder()
