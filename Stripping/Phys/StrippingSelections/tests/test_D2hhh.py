#!/usr/bin/env python

from StrippingUtils import LineBuilderTests

config_params = {
    'DaughterPT' : 250.,
    'DaughterP' : 2000.,
    'DaughterIPChi2' : 4.,
    'Daughter2IPChi2' : 10.,
    'D2KKKDaughterIPChi2' : 2.,
    'D2KKKDaughter2IPChi2' : 4.,
    'KPIDK' : 7.,
    'piPIDK' : 3.,
    'PTSum' : 2800.,
    'DOCAChi2' : 50.,
    'DPt' : 1000.,
    'DIPChi2' : 15.,
    'DdcaFDChi2' : 100.,
    'DDIRA' : 0.0,
    'DVXChi2NDOF' : 10.,
    'MinMassPosFit' : 1800.,
    'MaxMassPosFit' : 2040.,
    'D2KPPMaxMassPosFit' : 1940.,
    'Ds2KKPosMinMassPosFit' : 1900.,
    'D2HHHIncMinMassPosFit' : 1100.,
    'D2HHHIncMaxMassPosFit' : 2070.,
    'MaxTracksInEvent' : 500,
    'D2KKPLinePrescale' : 1.0,
    'D2KKPLinePostscale' : 1.0 ,
    'D2KPPLinePrescale' : 1.0,
    'D2KPPLinePostscale' : 1.0,
    'D2PPPLinePrescale' : 1.0,
    'D2PPPLinePostscale' : 1.0,
    'D2KPPosLinePrescale' : 0.4,
    'D2KPPosLinePostscale' : 1.0,
    'D2KKKLinePrescale' : 1.0,
    'D2KKKLinePostscale' : 1.0,
    'D2HHHIncLinePrescale' : 0.04,
    'D2HHHIncLinePostscale' : 1.0
   }


from StrippingSelections.StrippingD2hhh_conf import D2hhhConf as builder

def test_line_builder() :
    LineBuilderTests.test_line_builder(builder, config_params)

if __name__ == '__main__' :
    
    test_line_builder()
