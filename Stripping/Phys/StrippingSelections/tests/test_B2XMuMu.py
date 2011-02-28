#!/usr/bin/env python

from StrippingUtils import LineBuilderTests
from StrippingSelections.StrippingB2XMuMu import B2XMuMuConf as builder

config_params =  {
    'BVXCHI2NDOF'        : 4.0         
    , 'BIPCHI2'            : 9.0       
    , 'BDIRA'              : 0.999968  
    , 'BFDCHI2'            : 100.0     
    , 'KpiMINIPCHI2'       : 9.0       
    , 'KpiTRACKCHI2'       : 4.0       
    , 'KpiVXCHI2NDOF'      : 9.0       
    , 'MuonMINIPCHI2'      : 16.0      
    , 'MuonTRACKCHI2'      : 4.0       
    , 'MuonPID'            : 0.0       
    , 'DimuonVXCHI2NDOF'   : 9.0       
    , 'DimuonUPPERMASS'    : 5050.0    
    , 'Pi0MINPT'           : 800.0     
    , 'DplusLOWERMASS'     : 1600.0    
    , 'DplusUPPERMASS'     : 2300.0    
    , 'KstarplusWINDOW'    : 300.0     
    }

def test_line_builder() :
    LineBuilderTests.test_line_builder(builder, config_params)

if __name__ == '__main__' :
    
    test_line_builder()
