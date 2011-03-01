#!/usr/bin/env python

from StrippingUtils import LineBuilderTests

config_params = {
    'D0Prescale'      : 1.00,
    'DstarPrescale'   : 1.00,
    'DsPrescale'      : 1.00,
    'DplusPrescale'   : 1.00,
    'LamCPrescale'    : 1.00
  }

from StrippingSelections.StrippingPromptCharm import StrippingPromptCharmConf as builder

def test_line_builder() :
    LineBuilderTests.test_line_builder(builder, config_params)

if __name__ == '__main__' :
    
    test_line_builder()
