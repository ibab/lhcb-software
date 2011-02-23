#!/usr/bin/env python

from StrippingUtils import LineBuilderTests
from StrippingSelections.StrippingBu2D0h_D02KShh_NoPID import StrippingBu2D0h_D02KShh_NoPIDConf as builder
from StrippingSelections.StrippingBu2D0h_D02KShh_NoPID import confdict_Bu2D0h_D02KShh_NoPID_offlinesel as config_params

def test_line_builder() :
    LineBuilderTests.test_line_builder(builder, config_params)

if __name__ == '__main__' :
    
    test_line_builder()
