#!/usr/bin/env python

from StrippingConf.StrippingStream import StrippingStream

from StrippingSelections.StrippingB2nbody import B2nbodyConf  as builder

from StrippingSelections.StrippingB2nbody import confdict

from StrippingUtils import LineBuilderTests

def test_line_builder() :
    LineBuilderTests.test_line_builder(builder, confdict['2body'])

if __name__ == '__main__' :
    
    test_line_builder()
