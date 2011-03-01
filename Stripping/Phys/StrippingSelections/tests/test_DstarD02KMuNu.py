#!/usr/bin/env python

from StrippingUtils import LineBuilderTests

config_params = {
  'TRACK_Chi2' :  3., 
  'TRACK_MinIPChi2' :  7.,
  'KAON_PIDK' : 5.,
  'KAON_MinPT' : 600., 
  'KAON_MinP' : 5200.,
  'MUON_MinPT' :  630., 
  'PION_PIDmu' :  -1., 
  'PION_MaxP'  :  25000.,
  'PION_IPMin' : 20.,
  'D0_MassMax' : 2100.,
  'D0_MassMin' : 800., 
  'D0_MinPT'  : 1600.,
  'D0_DOCA' :  0.10,
  'D0_MinP' : 20000.,
  'D0_BPVVDZ' : 0.8,
  'D0_VtxChi2' : 9.,
  'D0_DIRA'  : 0.9996,
  'D0_MIP'  :  0.14,
  'Dstar_MassMin'  : 800.,
  'Dstar_DOCA'  : 0.4,
  'Dstar_VtxChi2' : 9.,
  'Dstar_DeltaMass'  :  200.,
  'PrescaledLinePrescale'  : 1.0,
  'PrescaledLinePostscale' : 1.0
  }

from StrippingSelections.StrippingDstarD02KMuNu import DstarD02KMuNuConf as builder

def test_line_builder() :
    LineBuilderTests.test_line_builder(builder, config_params)

if __name__ == '__main__' :
    
    test_line_builder()
