#!/usr/bin/env python

from StrippingUtils import LineBuilderTests
from StrippingSelections.StrippingBc2JpsiMuXNew import Bc2JpsiMuXConf as builder
from StrippingSelections.StrippingBc2JpsiMuXNew import config_default as config_params

def test_line_builder() :
    LineBuilderTests.test_line_builder(builder, config_params)

if __name__ == '__main__' :
    
    test_line_builder()
