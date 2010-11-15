#!/usr/bin/env python

from StrippingConf.StrippingStream import StrippingStream

from StrippingSelections.StrippingBs2JpsiPhiPrescaledAndDetatched import Bs2JpsiPhiPrescaledAndDetatchedConf as builder

config_params = {'PhiPT' : 500.,
                 'BsMassWin' : 300.,
                 'BsLTFITCHI2' : 14.,
                 'BsVCHI2' : 20.,
                 'DetatchedBsLTIME' : 15.,
                 'PrescaledLinePrescale' : 1.0,
                 'PrescaledLinePostscale' : 0.1,
                 'DetatchedLinePrescale' : 1.0,
                 'DetatchedLinePostscale' : 1.0}

from StrippingUtils import LineBuilderTests

def test_line_builder() :
    LineBuilderTests.test_line_builder(builder, config_params)

if __name__ == '__main__' :
    
    test_line_builder()
