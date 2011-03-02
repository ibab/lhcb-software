#!/usr/bin/env python

from StrippingUtils import LineBuilderTests
from GaudiKernel.SystemOfUnits import *

config_params = {
    'LongTrackGEC'          : 150      ## Global event cut on number of long tracks
    ,'KSLLCutDIRA'          : 0.9997   ## unitless
    ,'KSDDCutDIRA'          : 0.99995  ## unitless
    ,'KSLLCutMass'          : 11.4     ## in MeV
    ,'KSDDCutMass'          : 24.9     ## in MeV
    ,'KSLLCutFDChi2'        : 100      ## unitless
    ,'KSDDCutFDChi2'        : 100      ## unitless
    ,'trackFromDCutP'       : 1500.0   ## in MeV
    ,'trackFromDCutPIDe'    : 10.0     ## unitless -- note that this means (DLLe - DLLx) < 10 for x=K,pi
    ,'trackFromDCutPIDp'    : 15.0     ## unitless -- note that this means (DLLp - DLLx) < 15 for x=K,pi
    ,'pionFromDCutPIDK'     : -5.0     ## unitless -- note that this means (DLLpi - DLLK) < -5, i.e. (DLLK-DLLpi) > +5
    ,'kaonFromDCutPIDpi'    : 0.0      ## unitless -- note that this means (DLLK - DLLpi) < 0
    ,'pionFromDCutTRCHI2DOF': 4.0      ## unitless
    ,'kaonFromDCutTRCHI2DOF': 5.0      ## unitless
    ,'DCutDIRA'             : 0.0      ## unitless -- remove "backwards-going" D0
    ,'DCutTau'              : 0.3      ## ps
    ,'DCutVtxChi2_KK'       : 21.0     ## unitless
    ,'DCutVtxChi2_KP'       : 15.0     ## unitless
    ,'DCutVtxChi2_PP'       : 13.0     ## unitless
    ,'preFitDCutPT'         : 1500.0   ## in MeV
    ,'preFitDMassCut_LL'    : 130      ## in MeV
    ,'preFitDMassCut_DD'    : 270      ## in MeV
    ,'wideDMassCut_KKLL'    : 57.2     ## in MeV
    ,'wideDMassCut_KPLL'    : 92.0     ## in MeV
    ,'wideDMassCut_PPLL'    : 110.0    ## in MeV
    ,'wideDMassCut_KKDD'    : 72.0     ## in MeV
    ,'wideDMassCut_KPDD'    : 120.0    ## in MeV
    ,'wideDMassCut_PPDD'    : 140.0    ## in MeV
    ,'preFitDstarMassCut'   : 200.0    ## in MeV; make sure it's well above the largest D mass window
    ,'wideDMCutLower'       : 0.0      ## in MeV
    ,'wideDMCutUpper'       : 15.0     ## in MeV
    ,'DstarCutPT_KK'        : 2500.0   ## in MeV
    ,'DstarCutPT_KP'        : 3000.0   ## in MeV
    ,'DstarCutPT_PP'        : 3000.0   ## in MeV
    ,'DstarCutChi2NDOF_KK'  : 60.0     ## unitless
    ,'DstarCutChi2NDOF_KP'  : 20.0     ## unitless
    ,'DstarCutChi2NDOF_PP'  : 20.0     ## unitless
    ,'SoftPionCutPIDe'      : 5.0      ## unitless
    ,'KKLLPrescale'         : 1.0
    ,'KKLLPostscale'        : 1.0
    ,'KPLLPrescale'         : 1.0
    ,'KPLLPostscale'        : 1.0
    ,'PPLLPrescale'         : 1.0
    ,'PPLLPostscale'        : 1.0
    ,'KKDDPrescale'         : 1.0
    ,'KKDDPostscale'        : 1.0
    ,'KPDDPrescale'         : 1.0
    ,'KPDDPostscale'        : 1.0
    ,'PPDDPrescale'         : 1.0
    ,'PPDDPostscale'        : 1.0
    }


from StrippingSelections.StrippingDstarD2KShh import DstarD2KShhBuilder as builder

def test_line_builder() :
    LineBuilderTests.test_line_builder(builder, config_params)

if __name__ == '__main__' :
    
    test_line_builder()
