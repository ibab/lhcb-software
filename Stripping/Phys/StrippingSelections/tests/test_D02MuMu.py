#!/usr/bin/env python

from StrippingUtils import LineBuilderTests

config_params = {
    'MinDauPT'                 : 1.0, # GeV
    'MinDauIPCHI2'             : 1.0,
    'MinD0LT'                  : 0.1, # ps
    'D02MuMuPrescale'          : 1.0,
    'DstD02MuMuPrescale'       : 1.0,
    'D02PiPiPrescale'          : 0.1,
    'DstD02PiPiPrescale'       : 0.1
  }

from StrippingSelections.StrippingD02MuMu import D02MuMuConf as builder

def test_line_builder() :
    LineBuilderTests.test_line_builder(builder, config_params)

if __name__ == '__main__' :
    
    test_line_builder()
