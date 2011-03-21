########################################################################
#
# Tagging cuts optimized for Stripping12
# Optimization cuts with Bd2DstrarMuNu (OSK optimized with sPlot technique)
#
# @author Marc Grabalosa
# @date 2011-02-10
########################################################################

from Gaudi.Configuration import *
from Configurables import BTagging, BTaggingTool, TaggerMuonTool, TaggerElectronTool, TaggerKaonSameTool, TaggerKaonOppositeTool, TaggerPionSameTool, TaggerVertexChargeTool, CombineTaggersProbability

tag = BTagging ('BTagging')
tag.addTool ( BTaggingTool )

tag.BTaggingTool.IPPU_cut = 3.0
tag.BTaggingTool.distphi_cut = 0.005

tag.BTaggingTool.addTool ( CombineTaggersProbability )
tag.BTaggingTool.CombineTaggersProbability.P0_Cal_OS = 0.
tag.BTaggingTool.CombineTaggersProbability.P1_Cal_OS = 1.
tag.BTaggingTool.CombineTaggersProbability.Eta_Cal_OS = 0.
    
tag.BTaggingTool.addTool ( TaggerMuonTool )
tag.BTaggingTool.TaggerMuonTool.Muon_Pt_cut   = 1100
tag.BTaggingTool.TaggerMuonTool.Muon_P_cut    = 0.0 
tag.BTaggingTool.TaggerMuonTool.Muon_IPs_cut  = 0.0
tag.BTaggingTool.TaggerMuonTool.Muon_lcs_cut  = 2.2
tag.BTaggingTool.TaggerMuonTool.Muon_PIDm_cut = 2.0
tag.BTaggingTool.TaggerMuonTool.Muon_ipPU_cut = 3.0
tag.BTaggingTool.TaggerMuonTool.Muon_distPhi_cut = 0.005
tag.BTaggingTool.TaggerMuonTool.Muon_ProbMin  = 0.0
tag.BTaggingTool.TaggerMuonTool.Muon_AverageOmega  = 0.33
tag.BTaggingTool.TaggerMuonTool.Muon_P0_Cal   = 0.
tag.BTaggingTool.TaggerMuonTool.Muon_P1_Cal   = 1.
tag.BTaggingTool.TaggerMuonTool.Muon_Eta_Cal  = 0.

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
tag.BTaggingTool.TaggerElectronTool.Ele_ProbMin   = 0.0
tag.BTaggingTool.TaggerElectronTool.Ele_AverageOmega  = 0.33
tag.BTaggingTool.TaggerElectronTool.Ele_P0_Cal    = 0.
tag.BTaggingTool.TaggerElectronTool.Ele_P1_Cal    = 1.
tag.BTaggingTool.TaggerElectronTool.Ele_Eta_Cal   = 0.

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
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_ProbMin  = 0.51
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_AverageOmega  = 0.33
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_P0_Cal   = 0.
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_P1_Cal   = 1.
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_Eta_Cal  = 0.

tag.BTaggingTool.addTool ( TaggerKaonSameTool )
tag.BTaggingTool.TaggerKaonSameTool.KaonSame_Pt_cut  = 550
tag.BTaggingTool.TaggerKaonSameTool.KaonSame_P_cut   = 4200
tag.BTaggingTool.TaggerKaonSameTool.KaonSame_IP_cut  = 3.5
tag.BTaggingTool.TaggerKaonSameTool.KaonSame_Phi_cut = 0.8
tag.BTaggingTool.TaggerKaonSameTool.KaonSame_Eta_cut = 0.8
tag.BTaggingTool.TaggerKaonSameTool.KaonSame_dQ_cut  = 2000
tag.BTaggingTool.TaggerKaonSameTool.KaonSame_lcs_cut    = 2.75
tag.BTaggingTool.TaggerKaonSameTool.KaonSame_PIDk_cut  = 6.5
tag.BTaggingTool.TaggerKaonSameTool.KaonSame_PIDkp_cut = -1.
tag.BTaggingTool.TaggerKaonSameTool.KaonSame_ipPU_cut   = 3.0
tag.BTaggingTool.TaggerKaonSameTool.KaonSame_distPhi_cut   = 0.005
tag.BTaggingTool.TaggerKaonSameTool.KaonSame_ProbMin    = 0.0
tag.BTaggingTool.TaggerKaonSameTool.KaonSame_AverageOmega     = 0.33

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
tag.BTaggingTool.TaggerPionSameTool.PionSame_ProbMin           = 0.53
tag.BTaggingTool.TaggerPionSameTool.PionSame_AverageOmega          = 0.41
tag.BTaggingTool.TaggerPionSameTool.PionSame_P0_Cal       = 0.
tag.BTaggingTool.TaggerPionSameTool.PionSame_P1_Cal       = 1.
tag.BTaggingTool.TaggerPionSameTool.PionSame_Eta_Cal      = 0.

tag.BTaggingTool.addTool ( TaggerVertexChargeTool )
tag.BTaggingTool.TaggerVertexChargeTool.Vtx_AverageOmega  = 0.41
tag.BTaggingTool.TaggerVertexChargeTool.Vtx_PowerK        = 0.4
tag.BTaggingTool.TaggerVertexChargeTool.Vtx_MinimumVCharge= 0.17
tag.BTaggingTool.TaggerVertexChargeTool.Vtx_ProbMin   = 0.53
tag.BTaggingTool.TaggerVertexChargeTool.Vtx_P0_Cal    = 0.
tag.BTaggingTool.TaggerVertexChargeTool.Vtx_P1_Cal    = 1.
tag.BTaggingTool.TaggerVertexChargeTool.Vtx_Eta_Cal   = 0.

