#!/usr/bin/env python

from StrippingUtils import LineBuilderTests

config_params = {
   'B2MuMuMuMuLinePrescale'    : 1,
   'B2MuMuMuMuLinePostscale'   : 1
    }


from StrippingSelections.StrippingB2MuMuMuMuLines import B2MuMuMuMuLinesConf as builder


def test_line_builder() :
    LineBuilderTests.test_line_builder(builder, config_params)

if __name__ == '__main__' :
    
    test_line_builder()
