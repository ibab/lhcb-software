#!/usr/bin/env python

from StrippingUtils import LineBuilderTests

config_params = {
     'LongTrackGEC' : 150
    ,'controlPrescaleLc' : 0.05
    ,'controlPrescaleXic' : 1.0
    ,'signalPrescaleViaLc' : 1.0
    ,'signalPrescaleViaXic' : 1.0
    }


from StrippingSelections.StrippingXicc import XiccBuilder as builder

def test_line_builder() :
    LineBuilderTests.test_line_builder(builder, config_params)

if __name__ == '__main__' :
    
    test_line_builder()
