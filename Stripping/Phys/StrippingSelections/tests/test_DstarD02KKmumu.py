#!/usr/bin/env python

from StrippingUtils import LineBuilderTests

config_params = {
      'TrackChi2max' : 5.
    ,'DDauKPTmin'          : 200.
    , 'DDauKPmin'     : 1000.
    , 'DDauMuPTmin'         : 200.
    , 'DDauMuPmin'        : 1000.
    , 'DDauKDelPIDmin'        : -5.
    , 'DDauMuDelPIDmin'            : -10.
    , 'DCombMassWind'             : 60.
    , 'DCombAMAXDOCAmax'            : 0.4
    , 'DMothPTmin'            : 1500.
    , 'DMothBPVDIRAmin'             : 0.999
    , 'DMothMIPDVmax'          :  0.2
    , 'DMothMIPCHI2DVmax'  :  36.
    , 'DMothBPVVDmin'           : 1.0
    , 'DMothBPVDCHI2min'           : 4.
    , 'DMothBPVVDZmin'              : 0.
    , 'DMothVFASPFmax'               :  12.
    , 'DstarCombMassWind'               : 60.
    , 'DstarCombAMAXDOCAmax'                 :  0.4
    , 'DstarMothVFASPFmax'            :  12.
    , 'DstarMothPTmin'             : 1500.
    , 'DstarMothDMmin'               : 138.
    , 'DstarMothDMmax'           : 156.
    , 'DstarMothSlpiMIPDVmax'           : 0.400
    , 'DstarMothSlpiMIPCHI2DVmax'   : 36.
    , 'DstarMothSlpiPTmin'       : 250.
    , 'NTracksLim'               : 150
    , 'LinePrescale'   : 1.
    , 'LinePostscale'  : 1.
    }


from StrippingSelections.StrippingDstarD02KKmumuRegular import  StrippingDstarD02KKmumuRegularConf as builder

def test_line_builder() :
    LineBuilderTests.test_line_builder(builder, config_params)

if __name__ == '__main__' :
    
    test_line_builder()
