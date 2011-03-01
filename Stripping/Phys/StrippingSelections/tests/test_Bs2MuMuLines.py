#!/usr/bin/env python

from StrippingUtils import LineBuilderTests

config_params = {
    'DefaultLinePrescale'    : 1,
    'DefaultLinePostscale'   : 1,
    'Bs2mmWideLinePrescale'  : 1,
    'Bs2mmWideLinePostscale'  : 1,
    'LooseLinePrescale'      : 0.001,
    'LooseLinePostscale'     : 1,
    'JPsiLinePrescale'       : 1,
    'JPsiLinePostscale'      : 1,
    'JPsiLooseLinePrescale'  : 0.1,
    'JPsiLooseLinePostscale' : 1,
    'JPsiPromptLinePrescale' : 0.001,
    'JPsiPromptLinePostscale': 1,
    'MuIPChi2_loose'        :  9,
    'MuTrChi2_loose'        : 10,
    'BIPChi2_loose'         : 64,
    'BFDChi2_loose'         : 100
    }


from StrippingSelections.StrippingBs2MuMuLines import Bs2MuMuLinesConf as builder


def test_line_builder() :
    LineBuilderTests.test_line_builder(builder, config_params)

if __name__ == '__main__' :
    
    test_line_builder()
