#!/usr/bin/env python

from StrippingUtils import LineBuilderTests
from StrippingSelections.StrippingBeamGas import BeamGasConf as builder

config_params = {'Prescale' : 0.05, 'Postscale' : 1.0}

def test_line_builder() :
    LineBuilderTests.test_line_builder(builder, config_params)

if __name__ == '__main__' :
    
    test_line_builder()
