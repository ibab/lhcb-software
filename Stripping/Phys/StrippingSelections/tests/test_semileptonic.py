#!/usr/bin/env python

from StrippingSelections.StrippingB0q2DplusMuX import B0q2DplusMuXAllLinesConf as builder1

from StrippingSelections.StrippingB0q2DplusMuX import confdict as config_params1

from StrippingSelections.StrippingB2XuMuNu import B2XuMuNuBuilder as builder2

from StrippingSelections.StrippingB2XuMuNu import confdict as config_params2

from StrippingUtils import LineBuilderTests

def test_line_builder() :
    LineBuilderTests.test_line_builder(builder1, config_params1)
    LineBuilderTests.test_line_builder(builder2, config_params2)

if __name__ == '__main__' :
    
    test_line_builder()
