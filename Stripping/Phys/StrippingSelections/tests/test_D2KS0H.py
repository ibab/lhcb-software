#!/usr/bin/env python

from StrippingUtils import LineBuilderTests

config_params = {
  'KS0DaugP' : 2000       # Cut in common particles
  ,'KS0DaugPT' : 300        # Cut in trigger
  ,'KS0DaugTrackChi2' : 4  
  ,'KS0DaugMIPChi2' : 100
  ,'KS0DaugPIDK' : 10
  ,'KS0MassWindow' :35   # Done by stdLoose KS0s
  ,'KS0MIPChi2'  : 9  # taken away   
  ,'KS0VertexChi2' : 10  
  ,'KS0PT'       : 900   
  ,'BachPionP' : 5000               
  ,'BachPionPT'    : 500     
  ,'BachPionTrackChi2': 3
  ,'BachPionMIPChi2'  : 30
  ,'BachPionPIDK'     : 0
  ,'BachKaonP' : 5000                 
  ,'BachKaonPT'   : 500     
  ,'BachKaonTrackChi2' : 3
  ,'BachKaonMIPChi2'  : 30 
  ,'BachKaonPIDK'      : 0
  ,'DMesonComboLowMass' : 1760          
  ,'DMesonComboHighMass'  : 2080
  ,'DMesonComboDOCA'      : 0.6
  ,'DMesonMotherLowMass' : 1770 
  ,'DMesonMotherHighMass'   : 2070
  ,'DMesonMotherVertexChi2' : 10
  ,'DMesonMotherMIPChi2'    : 20
  ,'DMesonMotherPT' : 2000
  ,'PionLinePrescale' : 1 
  ,'KaonLinePrescale' : 1 
  ,'PionLinePostscale' : 1 
  ,'KaonLinePostscale' : 1
  }


from StrippingSelections.StrippingD2KS0H_conf import D2KS0HConf as builder


def test_line_builder() :
    LineBuilderTests.test_line_builder(builder, config_params)

if __name__ == '__main__' :
    
    test_line_builder()
