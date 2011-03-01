#!/usr/bin/env python

from StrippingUtils import LineBuilderTests

config_params = {
  'DpKs_prescale'      : 1,
  'DstarpKs_prescale'  : 1,
  'D0K_prescale'       : 1,
  'DpPim_prescale'     : 1,
  'D0Pip_prescale'     : 1,
  'DstarpPim_prescale' : 1
  }


from StrippingSelections.StrippingCharmedAndCharmedStrangeSpectroscopy import CharmedAndCharmedStrangeSpectroscopyConf as builder

def test_line_builder() :
    LineBuilderTests.test_line_builder(builder, config_params)

if __name__ == '__main__' :
    
    test_line_builder()
