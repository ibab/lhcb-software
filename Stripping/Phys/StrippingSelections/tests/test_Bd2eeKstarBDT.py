#!/usr/bin/env python

from StrippingUtils import LineBuilderTests
from StrippingSelections.StrippingBd2eeKstarBDT import Bd2eeKstarBDTConf as builder
from StrippingSelections.StrippingBd2eeKstarBDT import defaulSettings as config_params

def test_line_builder() :
    LineBuilderTests.test_line_builder(builder, config_params)

if __name__ == '__main__' :
    
    test_line_builder()
