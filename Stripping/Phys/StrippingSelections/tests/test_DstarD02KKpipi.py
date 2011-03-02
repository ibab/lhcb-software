#!/usr/bin/env python

from StrippingUtils import LineBuilderTests

config_params = {
    'TrackChi2max' : 5.
    ,'DDauKPTmin'          : 500.
    , 'DDauKPmin'     : 3000.
    , 'DDauPiPTmin'         : 400.
    , 'DDauPiPmin'        : 3000.
    , 'DDauKDelPIDmin'        : -4.
    , 'DDauPiDelPIDmin'            : -5.
    , 'DCombMassWind'             : 60.
    , 'DCombAMAXDOCAmax'            : 0.25
    , 'DMothPTmin'            : 2500.
    , 'DMothBPVDIRAmin'             : 0.9996
    , 'DMothMIPDVmax'          :  0.15
    , 'DMothMIPCHI2DVmax'  :  25.
    , 'DMothBPVVDmin'           : 1.0
    , 'DMothBPVDCHI2min'           : 16.0
    , 'DMothBPVVDZmin'              : 0.
    , 'DMothVFASPFmax'               :  8.
    , 'DstarCombMassWind'               : 60.
    , 'DstarCombAMAXDOCAmax'                 :  0.250
    , 'DstarMothVFASPFmax'            :  8.
    , 'DstarMothPTmin'             : 2500.
    , 'DstarMothDMmin'               : 138.
    , 'DstarMothDMmax'           : 156.
    , 'DstarMothSlpiMIPDVmax'           : 0.250
    , 'DstarMothSlpiMIPCHI2DVmax'   : 25.
    , 'DstarMothSlpiPTmin'       : 250.
    , 'NTracksLim'               : 150
    , 'LinePrescale'   : 1.
    , 'LinePostscale'  : 1.
    }


from StrippingSelections.StrippingDstarD02KKpipiRegular import  StrippingDstarD02KKpipiRegularConf as builder

def test_line_builder() :
    LineBuilderTests.test_line_builder(builder, config_params)

if __name__ == '__main__' :
    
    test_line_builder()
