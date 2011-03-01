#!/usr/bin/env python

from StrippingUtils import LineBuilderTests

config_params =  {
    'UseNoPIDsHadrons'          : True,
    'Prescale_BdToKstarMuMu'    : 1.0,
    'Postscale_BdToKstarMuMu'   : 1.0,
    'Prescale_BdToKstarMuMuSS'  : 1.0,
    'Postscale_BdToKstarMuMuSS' : 1.0,
    'Prescale_BuToKMuMu'        : 1.0,
    'Postscale_BuToKMuMu'       : 1.0,
    'Prescale_BuToKMuMuSS'      : 1.0,
    'Postscale_BuToKMuMuSS'     : 1.0,
    'B_Comb_MassLow'            : 4600.0,
    'B_Comb_MassHigh'           : 6000.0,
    'B_MassLow'                 : 4850.0,
    'B_MassHigh'                : 5780.0,
    'B_VertexCHI2'              : 6.0,
    'B_IPCHI2'                  : 16.0,
    'B_DIRA'                    : 0.014,
    'B_FlightCHI2'              : 121.0,
    'Dau_VertexCHI2'            : 12.0,
    'Dau_DIRA'                  : -0.9,
    'Kstar_Comb_MassLow'        : 550.0,
    'Kstar_Comb_MassHigh'       : 2200.0,
    'Kstar_MassLow'             : 600.0,
    'Kstar_MassHigh'            : 2000.0,
    'Kstar_MinIPCHI2'           : 4.0,
    'Kstar_FlightChi2'          : 25.0,
    'Dimu_FlightChi2'           : 81.0,
    'Track_CHI2nDOF'            : 5.0,
    'Hadron_MinIPCHI2'          : 9.0,
    'Muon_MinIPCHI2'            : 9.0,
    'Muon_IsMuon'               : False,
    'B_Dau_MaxIPCHI2'           : 9.0,
    'Kstar_Dau_MaxIPCHI2'       : 9.0,
    'Dimu_Dau_MaxIPCHI2'        : 9.0
    }

from StrippingSelections.StrippingBd2KstarMuMu import StrippingBdToKstarMuMuConf as builder

def test_line_builder() :
    LineBuilderTests.test_line_builder(builder, config_params)

if __name__ == '__main__' :
    
    test_line_builder()
