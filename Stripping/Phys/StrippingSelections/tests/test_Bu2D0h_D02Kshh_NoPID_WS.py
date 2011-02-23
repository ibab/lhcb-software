#!/usr/bin/env python

from StrippingUtils import LineBuilderTests
from StrippingSelections.StrippingBu2D0h_D02KShh_NoPID_WS import StrippingBu2D0h_D02KShh_NoPID_WSConf as builder
from StrippingSelections.StrippingBu2D0h_D02KShh_NoPID_WS import confdict_Bu2D0h_D02KShh_NoPID_offlinesel_WS as config_params

def test_line_builder() :
    LineBuilderTests.test_line_builder(builder, config_params)

if __name__ == '__main__' :
    
    test_line_builder()
