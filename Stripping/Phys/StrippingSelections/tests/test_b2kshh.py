#!/usr/bin/env python

from StrippingUtils import LineBuilderTests
from StrippingSelections.StrippingB2KShh import B2KShhConf as builder
from StrippingSelections.StrippingB2KShh import default_config as config_params

#config_params  = {'Trk_Chi2'         : 4.0, 'KS_DD_MassWindow' : 30.0}

def test_line_builder() :
    LineBuilderTests.test_line_builder(builder, config_params)

if __name__ == '__main__' :
    
    test_line_builder()
