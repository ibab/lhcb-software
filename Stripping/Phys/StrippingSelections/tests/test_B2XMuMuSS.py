#!/usr/bin/env python

from StrippingUtils import LineBuilderTests

config_params = {
    'MuonP'         : 3000. ,    #MeV
    'MuonPT'        : 500.  ,    #MeV
    'MuonMINIPCHI2' : 4     ,    #adminensional
    'PionP'         : 2000. ,    #MeV
    'PionPT'        : 500.  ,    #MeV
    'PionMINIPCHI2' : 4     ,    #adminensional
    'KaonP'         : 2000. ,    #MeV
    'KaonPT'        : 300.  ,    #MeV
    'KaonPIDK'      : -1.   ,    #adimensional
    'KaonMINIPCHI2' : 4     ,    #adminensional
    'DimuonMass'    : 250.  ,    #MeV
    'BVCHI2DOF'     : 7    ,    #adminensional                              
    'BDIRA'         : 0.9995 ,    #adimensional
    'BIPCHI2'       : 30    ,    #adimensional
    'BMassWin'      : 400.  ,    #MeV, mass window
    'B2PiMuMuOSLinePrescale'  : 1 ,
    'B2PiMuMuOSLinePostscale' : 1 ,
    'B2PiMuMuSSLinePrescale'  : 1 ,
    'B2PiMuMuSSLinePostscale' : 1 ,
    'B2KMuMuOSLinePrescale'   : 1 ,
    'B2KMuMuOSLinePostscale'  : 1 ,
    'B2KMuMuSSLinePrescale'   : 1 ,
    'B2KMuMuSSLinePostscale'  : 1
    }


from StrippingSelections.StrippingB2XMuMuSS import B2XMuMuConf as builder

def test_line_builder() :
    LineBuilderTests.test_line_builder(builder, config_params)

if __name__ == '__main__' :
    
    test_line_builder()
