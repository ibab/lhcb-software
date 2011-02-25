#!/usr/bin/env python

from StrippingConf.StrippingStream import StrippingStream

from StrippingSelections.StrippingDstarD02Kpipi0 import DstarD02Kpipi0Conf as builder

config = {
  "RSPrescale" : 0.25, 
  "WSPrescale" : 0.0025
}

from StrippingUtils import LineBuilderTests

def test_line_builder() :
    LineBuilderTests.test_line_builder(builder, config)

if __name__ == '__main__' :
    
    test_line_builder()
