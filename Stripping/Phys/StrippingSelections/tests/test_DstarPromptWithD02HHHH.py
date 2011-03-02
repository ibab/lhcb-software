#!/usr/bin/env python

from StrippingUtils import LineBuilderTests
from GaudiKernel.SystemOfUnits import *

config_params = {
    'CombDelmLower'     : -8.25*MeV
    ,'CombDelmUpper'    : 16.5*MeV
    ,'DelmLower'        : -7.5*MeV
    ,'DelmUpper'        : 15*MeV
    ,'CombD0MassWin'    : 82.5*MeV
    ,'D0MassWin'        : 75*MeV
    ,'DstarDOCA'        : 0.45*mm
    ,'D0MaxDOCA'        : 0.5*mm
    ,'D0DauPt'          : 300*MeV
    ,'D0DauP'           : 3*GeV
    ,'D0Pt'             : 3*GeV
    ,'DstarPt'          : 3*GeV
    ,'SlowPionPt'       : 70*MeV
    ,'DstarVtxChi2DOF'  : 20
    ,'D0VtxChi2DOF'     : 10
    ,'D0DauMaxIPChi2'   : 30
    ,'D0DauIPChi2'      : 1.7
    ,'D0FDChi2'         : 48
    ,'D0IPChi2'         : 30
    ,'D0DIRA'           : 0.9998
    ,'TrackChi2DOF'     : 5
    ,'ApplyKaonPIDK'    : False 
    ,'KaonPIDK'         : -100
    ,'ApplyPionPIDK'    : False 
    ,'PionPIDK'         : 100 # not applied to the slow pion
    ,'CheckPV'          : True # reject events with no PVs
    ,'ApplyGECs'        : True # set to False to apply no GECs
    ,'MaxLongTracks'    : 150
    ,'MaxVeloTracks'    : None
    ,'MaxSpdDigits'     : None
    ,'MaxITClusters'    : None
    ,'Prescale'         : 0.10
    ,'Postscale'        : 1
    }


from StrippingSelections.StrippingDstarPromptWithD02HHHH import DstarPromptWithD02HHHHConf as builder

def test_line_builder() :
    LineBuilderTests.test_line_builder(builder, config_params)

if __name__ == '__main__' :
    
    test_line_builder()
