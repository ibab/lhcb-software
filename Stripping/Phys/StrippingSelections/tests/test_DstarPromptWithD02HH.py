#!/usr/bin/env python

from StrippingUtils import LineBuilderTests

config_params = {
    'DaugPt'            : 0.9,        ## GeV
    'DaugP'             : 5.,         ## GeV
    'DaugTrkChi2'       : 5.,         ## unitless
    'D0Pt'              : 3.3,       ## GeV
    'D0MassWindowCentre': 1.865,     ## GeV
    'D0MassWindowWidth' : 0.075,     ## GeV 
    'D0VtxChi2Ndof'     : 13,         ## unitless
    'D0PVDisp'          : 0.9,        ## mm
    'SlowPiPt'          : 0.260,      ## GeV
    'DstarPt'           : 3.6,        ## GeV
    'DstarVtxChi2Ndof'  : 13,         ## unitless
    'RSLinePrescale'    : 1.,         ## unitless
    'RSLinePostscale'   : 1.,         ## unitless
    'WSLinePrescale'    : 1.,         ## unitless
    'WSLinePostscale'   : 1.,         ## unitless
    'HHLinePrescale'    : 1.,         ## unitless
    'HHLinePostscale'   : 1.,         ## unitless
   }


from StrippingSelections.StrippingDstarPromptWithD02HH import StrippingDstarPromptWithD02HHConf as builder

def test_line_builder() :
    LineBuilderTests.test_line_builder(builder, config_params)

if __name__ == '__main__' :
    
    test_line_builder()
