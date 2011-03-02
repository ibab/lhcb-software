#!/usr/bin/env python

from StrippingUtils import LineBuilderTests

config_params = {
   'MuonP'         : 3000. ,    #MeV
   'MuonPT'        : 500.  ,    #MeV
   'MuonMINIPCHI2' : 4     ,    #adminensional
   'MuonTRCHI2'    : 8     ,    #adminensional
   'PionP'         : 2000. ,    #MeV
   'PionPT'        : 300.  ,    #MeV
   'PionMINIPCHI2' : 6     ,    #adminensional
   'PionTRCHI2'    : 8     ,    #adminensional
   'KaonP'         : 2000. ,    #MeV
   'KaonPT'        : 300.  ,    #MeV
   'KaonPIDK'      : -1.   ,    #adimensional
   'KaonMINIPCHI2' : 6     ,    #adminensional
   'KaonTRCHI2'    : 8     ,    #adminensional
   'DimuonMass'    : 250.  ,    #MeV
   'DVCHI2DOF'     : 5    ,    #adminensional                              
   'DDIRA'         : 0.9999,    #adimensional
   'DIPCHI2'       : 30    ,    #adimensional
   'DMassWin'      : 200.  ,    #MeV, mass window
   'DMassLow'      :1763.  ,    #MeV, low-mass veto
   'D2PiMuMuOSLinePrescale'  : 1 ,
   'D2PiMuMuOSLinePostscale' : 1 ,
   'D2PiMuMuSSLinePrescale'  : 1 ,
   'D2PiMuMuSSLinePostscale' : 1 ,
   'D2KMuMuOSLinePrescale'   : 1 ,
   'D2KMuMuOSLinePostscale'  : 1 ,
   'D2KMuMuSSLinePrescale'   : 1 ,
   'D2KMuMuSSLinePostscale'  : 1 ,
   'D2PiPiPiCalLinePrescale' : 0.01 ,
   'D2PiPiPiCalLinePostscale': 1 
   }


from StrippingSelections.StrippingD2XMuMuSS import D2XMuMuConf as builder

def test_line_builder() :
    LineBuilderTests.test_line_builder(builder, config_params)

if __name__ == '__main__' :
    
    test_line_builder()
