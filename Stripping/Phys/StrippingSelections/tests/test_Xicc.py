#!/usr/bin/env python

from StrippingUtils import LineBuilderTests

config_params = {
     'LongTrackGEC' : 150
    ,'controlPrescaleLc' : 0.05
    ,'controlPrescaleXic' : 1.0
    , 'controlPrescaleDp' : 0.001
    , 'controlPrescaleD0' : 0.001
    , 'controlPrescaleDsp' : 0.00
    , 'signalPrescaleViaLc' : 1.0
    , 'signalPrescaleViaXic' : 1.0
    , 'signalPrescaleViaDp'  : 1.0
    , 'signalPrescaleViaDsp' : 0.0
    , 'signalPrescaleViaD0'  : 1.0
    , 'LcHlt2TisTosSpec' : { 'Hlt2.*CharmHadLambdaC2KPPi.*Decision%TOS' : 0 }
    , 'DzHlt2TisTosSpec' : { 'Hlt2.*CharmHadD02.*Decision%TOS' : 0 }
    , 'DpHlt2TisTosSpec' : { 'Hlt2.*CharmHadD2HHH.*Decision%TOS' : 0 }
    , 'XiHlt2TisTosSpec' : { 'Hlt2.*ChargedHyperon.*Decision%TOS' : 0 }
    }


from StrippingSelections.StrippingXicc import XiccBuilder as builder

def test_line_builder() :
    LineBuilderTests.test_line_builder(builder, config_params)

if __name__ == '__main__' :
    
    test_line_builder()
