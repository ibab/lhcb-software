########################################################################
#
# Tagging cuts calibrated for Stripping17 Reco12, use default(2011data) cuts
#
# @author Marc Grabalosa & Stefania Vecchi
# @date 2011-12-12
########################################################################

from Gaudi.Configuration import *
from Configurables import BTagging, BTaggingTool, TaggerMuonTool, TaggerElectronTool, TaggerKaonSameTool, TaggerKaonOppositeTool, TaggerPionSameTool, TaggerVertexChargeTool, CombineTaggersProbability

tag = BTagging ('BTagging')
tag.addTool ( BTaggingTool )

tag.BTaggingTool.addTool ( CombineTaggersProbability )
tag.BTaggingTool.CombineTaggersProbability.ProbMin_OS  = 0.
tag.BTaggingTool.CombineTaggersProbability.P0_Cal_OS  = 0.392 #±0.0016
tag.BTaggingTool.CombineTaggersProbability.P1_Cal_OS  = 0.921 #±0.017
tag.BTaggingTool.CombineTaggersProbability.Eta_Cal_OS = 0.363
tag.BTaggingTool.CombineTaggersProbability.ProbMin  = 0.


tag.BTaggingTool.addTool ( TaggerMuonTool )
tag.BTaggingTool.TaggerMuonTool.Muon_ProbMin     = 0.
tag.BTaggingTool.TaggerMuonTool.Muon_P0_Cal      = 0.309 #±0.0038
tag.BTaggingTool.TaggerMuonTool.Muon_P1_Cal      = 1.1953 #±0.061
tag.BTaggingTool.TaggerMuonTool.Muon_Eta_Cal     = 0.304

tag.BTaggingTool.addTool ( TaggerElectronTool )
tag.BTaggingTool.TaggerElectronTool.Ele_Pt_cut    = 1000
tag.BTaggingTool.TaggerElectronTool.Ele_P_cut     = 0.0
tag.BTaggingTool.TaggerElectronTool.Ele_lcs_cut   = 3.85
tag.BTaggingTool.TaggerElectronTool.Ele_IPs_cut   = 1.
tag.BTaggingTool.TaggerElectronTool.Ele_ghost_cut =-999.
tag.BTaggingTool.TaggerElectronTool.Ele_VeloChargeMin = 0.0
tag.BTaggingTool.TaggerElectronTool.Ele_VeloChargeMax = 1.6
tag.BTaggingTool.TaggerElectronTool.Ele_EoverP        = 0.75
tag.BTaggingTool.TaggerElectronTool.Ele_PIDe_cut  = 4.
tag.BTaggingTool.TaggerElectronTool.Ele_ipPU_cut  = 5.
tag.BTaggingTool.TaggerElectronTool.Ele_distPhi_cut  = 0.005
tag.BTaggingTool.TaggerElectronTool.Ele_ProbMin       = 0.
tag.BTaggingTool.TaggerElectronTool.Ele_P0_Cal        = 0.306 #±0.0056
tag.BTaggingTool.TaggerElectronTool.Ele_P1_Cal        = 0.974 #±0.091
tag.BTaggingTool.TaggerElectronTool.Ele_Eta_Cal       = 0.346

tag.BTaggingTool.addTool ( TaggerKaonOppositeTool )
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_Pt_cut   = 700
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_P_cut    = 5000
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_IPs_cut  = 4.3
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_IP_cut   = 1.45
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_lcs_cut  = 2.45
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_PIDk     = 0.75
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_PIDkp    = -3.
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_ghost_cut= -999.
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_ipPU_cut = 7.5
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_distPhi_cut = 0.005
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_ProbMin     = 0.54
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_P0_Cal      = 0.393 #±0.002
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_P1_Cal      = 0.706 #±0.036
tag.BTaggingTool.TaggerKaonOppositeTool.Kaon_Eta_Cal     = 0.354


tag.BTaggingTool.addTool ( TaggerVertexChargeTool )
tag.BTaggingTool.SVertexOneSeedTool.MinSeedProbability    = 0.42
tag.BTaggingTool.TaggerVertexChargeTool.Vtx_PowerK        = 0.55
tag.BTaggingTool.TaggerVertexChargeTool.Vtx_MinimumVCharge= 0.2
tag.BTaggingTool.TaggerVertexChargeTool.Vtx_Ptsum         = 1.55
tag.BTaggingTool.TaggerVertexChargeTool.Vtx_Ptmean        = 0.
tag.BTaggingTool.TaggerVertexChargeTool.Vtx_IPSsum        = 0.
tag.BTaggingTool.TaggerVertexChargeTool.Vtx_DocaMaxsum    = 0.5
tag.BTaggingTool.TaggerVertexChargeTool.Vtx_Psum          = 8.
tag.BTaggingTool.TaggerVertexChargeTool.Vtx_Msum          = 0.6
tag.BTaggingTool.TaggerVertexChargeTool.Vtx_Magsum        = 0.
tag.BTaggingTool.TaggerVertexChargeTool.Vtx_Bdphidir      = 50.
tag.BTaggingTool.TaggerVertexChargeTool.Vtx_ProbMin       = 0.54
tag.BTaggingTool.TaggerVertexChargeTool.Vtx_P0_Cal        = 0.404 #±0.002
tag.BTaggingTool.TaggerVertexChargeTool.Vtx_P1_Cal        = 0.84 #±0.032
tag.BTaggingTool.TaggerVertexChargeTool.Vtx_Eta_Cal       = 0.362
