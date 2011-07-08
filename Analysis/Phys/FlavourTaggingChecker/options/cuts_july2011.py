########################################################################
#
# Tagging cuts calibrated for Stripping13b Reco10, use default(2010data) cuts
#
# @author Marc Grabalosa
# @date 2011-01-24
########################################################################

from Gaudi.Configuration import *
from Configurables import BTagging, BTaggingTool, TaggerMuonTool, TaggerElectronTool, TaggerKaonSameTool, TaggerKaonOppositeTool, TaggerPionSameTool, TaggerVertexChargeTool, CombineTaggersProbability

tag = BTagging ('BTagging')
tag.addTool ( BTaggingTool )

tag.BTaggingTool.addTool ( CombineTaggersProbability )
#tag.BTaggingTool.CombineTaggersProbability.ProbMin_OS  = 0.58 #0.55
tag.BTaggingTool.CombineTaggersProbability.ProbMin_OS  = 0.5 #0.55
tag.BTaggingTool.CombineTaggersProbability.P0_Cal_OS  = 0.376 # +/-0.003
tag.BTaggingTool.CombineTaggersProbability.P1_Cal_OS  = 0.928 #+/-0.035
tag.BTaggingTool.CombineTaggersProbability.Eta_Cal_OS = 0.354
#tag.BTaggingTool.CombineTaggersProbability.ProbMin  = 0.55
tag.BTaggingTool.CombineTaggersProbability.ProbMin  = 0.5

tag.BTaggingTool.addTool ( TaggerMuonTool )
tag.BTaggingTool.TaggerMuonTool.Muon_Pt_cut      = 1200
tag.BTaggingTool.TaggerMuonTool.Muon_P_cut       = 0 
tag.BTaggingTool.TaggerMuonTool.Muon_lcs_cut     = 3.2
tag.BTaggingTool.TaggerMuonTool.Muon_IPs_cut     = 0.0
tag.BTaggingTool.TaggerMuonTool.Muon_IPssign_cut = -9999.0
tag.BTaggingTool.TaggerMuonTool.Muon_PIDm_cut    = 2.5
tag.BTaggingTool.TaggerMuonTool.Muon_ipPU_cut    = 3.0
tag.BTaggingTool.TaggerMuonTool.Muon_distPhi_cut = 0.005 
tag.BTaggingTool.TaggerMuonTool.Muon_cand        = 10.
tag.BTaggingTool.TaggerMuonTool.Muon_ProbMin     = 0.
tag.BTaggingTool.TaggerMuonTool.Muon_P0_Cal      = 0.304 #+/- 0.007
tag.BTaggingTool.TaggerMuonTool.Muon_P1_Cal      = 1.10 #+/- 0.11
tag.BTaggingTool.TaggerMuonTool.Muon_Eta_Cal     = 0.303

tag.BTaggingTool.addTool ( TaggerElectronTool )
tag.BTaggingTool.TaggerElectronTool.Ele_Pt_cut    = 1000
tag.BTaggingTool.TaggerElectronTool.Ele_P_cut     = 0.0
tag.BTaggingTool.TaggerElectronTool.Ele_lcs_cut   = 5.
tag.BTaggingTool.TaggerElectronTool.Ele_IPs_cut   = 2.
tag.BTaggingTool.TaggerElectronTool.Ele_ghost_cut =-999.
tag.BTaggingTool.TaggerElectronTool.Ele_VeloChargeMin = 0.0
tag.BTaggingTool.TaggerElectronTool.Ele_VeloChargeMax = 1.6
tag.BTaggingTool.TaggerElectronTool.Ele_EoverP        = 0.6
tag.BTaggingTool.TaggerElectronTool.Ele_PIDe_cut  = 4.0
tag.BTaggingTool.TaggerElectronTool.Ele_ipPU_cut  = 3.
tag.BTaggingTool.TaggerElectronTool.Ele_distPhi_cut  = 0.03
tag.BTaggingTool.TaggerElectronTool.Ele_ProbMin       = 0.
tag.BTaggingTool.TaggerElectronTool.Ele_P0_Cal        = 0.3305 # +/- 0.01
tag.BTaggingTool.TaggerElectronTool.Ele_P1_Cal        = 1.09 # +/- 0.17
tag.BTaggingTool.TaggerElectronTool.Ele_Eta_Cal       = 0.345 

tag.BTaggingTool.addTool ( TaggerKaonOppositeTool )
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_Pt_cut   = 800
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_P_cut    = 5875
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_IPs_cut  = 4.05
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_IP_cut   = 1.25
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_lcs_cut  = 2.125
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_PIDk     = 6.5
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_PIDkp    = -3.5
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_ghost_cut= -999.
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_ipPU_cut = 4.72
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_distPhi_cut = -0.005
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_ProbMin     = 0.54
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_P0_Cal      = 0.390 # +/- 0.005
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_P1_Cal      = 0.90  # +/- 0.09
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_Eta_Cal     = 0.362 

tag.BTaggingTool.addTool ( TaggerKaonSameTool )
tag.BTaggingTool.TaggerKaonSameTool.KaonSame_Pt_cut  = 750
tag.BTaggingTool.TaggerKaonSameTool.KaonSame_P_cut   = 5250
tag.BTaggingTool.TaggerKaonSameTool.KaonSame_IP_cut  = 4.125
tag.BTaggingTool.TaggerKaonSameTool.KaonSame_Phi_cut = 0.7
tag.BTaggingTool.TaggerKaonSameTool.KaonSame_Eta_cut = 0.525
tag.BTaggingTool.TaggerKaonSameTool.KaonSame_dQ_cut  = 1462.5
tag.BTaggingTool.TaggerKaonSameTool.KaonSame_lcs_cut    = 3.75
tag.BTaggingTool.TaggerKaonSameTool.KaonSame_PIDk_cut  = 4.5
tag.BTaggingTool.TaggerKaonSameTool.KaonSame_PIDkp_cut = -8.5
tag.BTaggingTool.TaggerKaonSameTool.KaonSame_ipPU_cut   = 3.0
tag.BTaggingTool.TaggerKaonSameTool.KaonSame_distPhi_cut   = 0.005
tag.BTaggingTool.TaggerKaonSameTool.KaonSame_ProbMin     = 0.
tag.BTaggingTool.TaggerKaonSameTool.KaonSame_P0_Cal      = 0.0
tag.BTaggingTool.TaggerKaonSameTool.KaonSame_P1_Cal      = 1.0
tag.BTaggingTool.TaggerKaonSameTool.KaonSame_Eta_Cal     = 0.0

tag.BTaggingTool.addTool ( TaggerPionSameTool )
tag.BTaggingTool.TaggerPionSameTool.PionSame_Pt_cut       = 750
tag.BTaggingTool.TaggerPionSameTool.PionSame_P_cut        = 5000
tag.BTaggingTool.TaggerPionSameTool.PionSame_IPs_cut      = 3.5
tag.BTaggingTool.TaggerPionSameTool.PionSame_lcs_cut         = 5.0
tag.BTaggingTool.TaggerPionSameTool.PionSame_dQ_cut       = 2500
tag.BTaggingTool.TaggerPionSameTool.PionSame_dQ_extra_cut = 1500
tag.BTaggingTool.TaggerPionSameTool.PionSame_ghost_cut        = -999.
tag.BTaggingTool.TaggerPionSameTool.PionSame_ipPU_cut     = 3.
tag.BTaggingTool.TaggerPionSameTool.PionSame_distPhi_cut  = 0.005
tag.BTaggingTool.TaggerPionSameTool.PionSame_PIDNoK_cut   = 3.0
tag.BTaggingTool.TaggerPionSameTool.PionSame_PIDNoP_cut   = 10.0
tag.BTaggingTool.TaggerPionSameTool.PionSame_ProbMin      = 0.54
tag.BTaggingTool.TaggerPionSameTool.PionSame_P0_Cal       = 0.470 # +/- 0.007  
tag.BTaggingTool.TaggerPionSameTool.PionSame_P1_Cal       = 0.614 # +/- 0.15    o 0.909 # +/- 0.07
tag.BTaggingTool.TaggerPionSameTool.PionSame_P2_Cal       = -2.94 # +/- 1.1
tag.BTaggingTool.TaggerPionSameTool.PionSame_Eta_Cal      = 0.448

tag.BTaggingTool.addTool ( TaggerVertexChargeTool )
tag.BTaggingTool.TaggerVertexChargeTool.Vtx_PowerK        = 0.4
tag.BTaggingTool.TaggerVertexChargeTool.Vtx_MinimumVCharge= 0.275
tag.BTaggingTool.TaggerVertexChargeTool.Vtx_Ptsum         = 1.5
tag.BTaggingTool.TaggerVertexChargeTool.Vtx_Ptmean        = 0.
tag.BTaggingTool.TaggerVertexChargeTool.Vtx_IPSsum        = 10.
tag.BTaggingTool.TaggerVertexChargeTool.Vtx_DocaMaxsum    = 0.5
tag.BTaggingTool.TaggerVertexChargeTool.Vtx_Psum          = 10.
tag.BTaggingTool.TaggerVertexChargeTool.Vtx_Msum          = 0.5
tag.BTaggingTool.TaggerVertexChargeTool.Vtx_Magsum        = 0.
tag.BTaggingTool.TaggerVertexChargeTool.Vtx_Bdphidir      = 50.
tag.BTaggingTool.TaggerVertexChargeTool.Vtx_ProbMin       = 0.54
tag.BTaggingTool.TaggerVertexChargeTool.Vtx_P0_Cal        = 0.401 # +/- 0.005
tag.BTaggingTool.TaggerVertexChargeTool.Vtx_P1_Cal        = 0.90  # 0.876 # +/- 0.08
tag.BTaggingTool.TaggerVertexChargeTool.Vtx_Eta_Cal       = 0.368
