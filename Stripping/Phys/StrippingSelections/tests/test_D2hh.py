#!/usr/bin/env python

from StrippingUtils import LineBuilderTests

config_params = {
    'DaugPtMin': 800.,
    'DaugPtMax': 1500.,
    'DaugP': 5000.,
    'DaugIPChi2': 9.,
    'DaugTrkChi2': 3.,
    'HighPIDK': 5.,
    'LowPIDK': 0.,
    'D0Pt': 2000.,
    'D0MassWindowCentre': 1865.,
    'D0MassWindowWidth': 100.,
    'D0P': 5000.,
    'D0VtxChi2Ndof': 10.,
    'D0FDChi2': 40.,
    'D0BPVDira': 0.9999,
    'D0DOCA': 0.07,
    'Daug_TRCHI2DOF_MAX': 5.,
    'Dstar_AMDiff_MAX': 165.,
    'Dstar_VCHI2VDOF_MAX': 100.,
    'Dstar_MDiff_MAX': 160.,
    'UntaggedCFLinePrescale': 1.,
    'UntaggedCFLinePostscale': 1.,
    'UntaggedSCSLinePrescale': 1.,
    'UntaggedSCSLinePostscale': 1.,
    'TaggedRSLinePrescale': 1.,
    'TaggedRSLinePostscale': 1.,
    'TaggedWSLinePrescale': 1.,
    'TaggedWSLinePostscale': 1.,
    'TaggedSCSLinePrescale': 1.,
    'TaggedSCSLinePostscale': 1.,
  }


from StrippingSelections.StrippingD2hh import D2hhConf as builder


def test_line_builder() :
    LineBuilderTests.test_line_builder(builder, config_params)

if __name__ == '__main__' :
    
    test_line_builder()
