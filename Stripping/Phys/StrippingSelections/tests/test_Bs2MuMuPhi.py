#!/usr/bin/env python

from StrippingUtils import LineBuilderTests

config_params = {
      'BsIPCHI2'             : 9.0      
    , 'BsLT'               : 0.0002     
    , 'BsVertexCHI2'       : 40.0       
    , 'KaonPIDK'           : 0          
    , 'KaonMINIPCHI2'      : 9.0        
    , 'MuonMINIPCHI2'      : 9.0        
    }


from StrippingSelections.StrippingBs2MuMuPhi import Bs2MuMuPhiConf as builder

def test_line_builder() :
    LineBuilderTests.test_line_builder(builder, config_params)

if __name__ == '__main__' :
    
    test_line_builder()
