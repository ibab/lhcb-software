#!/usr/bin/env python

from StrippingUtils import LineBuilderTests
from StrippingSelections.StrippingForTriggerValidation import TriggerValidationConf as builder
from StrippingSelections.StrippingForTriggerValidation import default_config as config_params

def test_line_builder() :
    LineBuilderTests.test_line_builder(builder, config_params)

if __name__ == '__main__' :
    
    test_line_builder()
