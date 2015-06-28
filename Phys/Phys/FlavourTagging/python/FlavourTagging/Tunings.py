"""
	  Configurable to set Tagger cuts
	 
	  @ Created by M. Grabalosa
	  @ date 2011-03-21
"""
	
__author__ = 'Marc Grabalosa mgrabalo@cern.ch'
__date__ = 'March 2011'
__version__ = '$Revision: 1.0 $'
	
from Gaudi.Configuration import *

tunings = {
    'Stripping23' : { 
        "Personality": "'Reco14'",  # it is then propagated to all other tools
        "IPPU_cut" : 3.0,
        "distphi_cut" : 0.005,

        #"ForceSignalID" :   , #"Bs", "Bd", "Bu" to force SSk or SSpi

        "EnableCharmTagger":    "True", # was disabled in the initial Stripping21
        "EnableProtonTagger":   "True",
        "EnablePionBDTTagger":  "True",
        #Following 2 lines optimize the average and combined (5 cat) performances for B2JpsiK
        #When performing your analisis, please find yourself the cut which maximeze the Eeff
        #Probably if you are using Eeff event per event no cut will be needed
        #"CombineTaggersProbability.ProbMin_OS" : 0.58,
        #"CombineTaggersProbability.ProbMin" : 0.55,
        
        "TaggingUtils.PVReFitter":           "'LoKi::PVReFitter'",
        "TaggingUtils.LifetimeFitter":       "'LoKi::LifetimeFitter'",
        "TaggingUtils.VertexFitter":         "'LoKi::VertexFitter'",
        "TaggingUtils.DistanceCalculator":   "'LoKi::DistanceCalculator'",
        "TaggingUtils.ParticleDescendants":  "'ParticleDescendants'",

        "CombineTaggersProbability.P0_Cal_OS" : 0.390,
        "CombineTaggersProbability.P1_Cal_OS" : 0.899,
        "CombineTaggersProbability.Eta_Cal_OS" : 0.365,

        # calibration for combination with nnetOSK 
        "CombineTaggersProbability.P0_Cal_OSnnet" : 0.4353,
        "CombineTaggersProbability.P1_Cal_OSnnet" : 0.948,
        "CombineTaggersProbability.Eta_Cal_OSnnet" : 0.4356,

        "TaggerMuonTool.Muon_Pt_cut" :     1100.0,
        "TaggerMuonTool.Muon_P_cut" :         0.0,
        "TaggerMuonTool.Muon_lcs_cut" :       3.0,
        "TaggerMuonTool.Muon_IPs_cut" :       0.0,
        "TaggerMuonTool.Muon_PIDm_cut" :   -100.0,  # no cut
        "TaggerMuonTool.Muon_ipPU_cut" :      3.0,
        "TaggerMuonTool.Muon_distPhi_cut"   : 0.005,        
        "TaggerMuonTool.Muon_ghostprob_cut" : 0.4,  # new cut
        "TaggerMuonTool.Muon_PIDNNm_cut"    : 0.35,  # new cut       
        "TaggerMuonTool.Muon_PIDNNpi_cut"   : 0.8,  # new cut       
        "TaggerMuonTool.Muon_PIDNNe_cut"    : 0.8,  # new cut       
        "TaggerMuonTool.Muon_PIDNNk_cut"    : 0.8,  # new cut       
        "TaggerMuonTool.Muon_PIDNNp_cut"    : 0.8,  # new cut       
        "TaggerMuonTool.Muon_P0_Cal" :       0.,
        "TaggerMuonTool.Muon_P1_Cal" :       1., 
        "TaggerMuonTool.Muon_Eta_Cal" :      0.,
        "TaggerMuonTool.Muon_ProbMin" :       0.0,
        "TaggerMuonTool.Muon_AverageOmega" :  0.33,

        "TaggerElectronTool.Ele_Pt_cut" :       1100,
        "TaggerElectronTool.Ele_P_cut" :         0.0,
        "TaggerElectronTool.Ele_lcs_cut" :       3.,
        "TaggerElectronTool.Ele_IPs_cut" :       3.5,
        "TaggerElectronTool.Ele_ghost_cut" :   -999.,
        "TaggerElectronTool.Ele_VeloChargeMin" : 0.0,
        "TaggerElectronTool.Ele_VeloChargeMax" : 1.4,
        "TaggerElectronTool.Ele_EoverP" :       0.85,
        "TaggerElectronTool.Ele_EoverPmax" :      2.,
        "TaggerElectronTool.Ele_PIDe_cut" :     -1.0,
        "TaggerElectronTool.Ele_ipPU_cut" :      4.0,
        "TaggerElectronTool.Ele_distPhi_cut" : 0.005,
        "TaggerElectronTool.Ele_PIDNNe_cut" :    0.1,  # new
        "TaggerElectronTool.Ele_PIDNNk_cut" :    0.8,  # new
        "TaggerElectronTool.Ele_PIDNNp_cut" :    0.8,  # new
        "TaggerElectronTool.Ele_PIDNNpi_cut" :   0.8,  # new
        "TaggerElectronTool.Ele_PIDNNepi_cut" : -0.8,  # new
        "TaggerElectronTool.Ele_ghostProb_cut" : 0.4,  # new       
        "TaggerElectronTool.Ele_P0_Cal" :         0.,
        "TaggerElectronTool.Ele_P1_Cal" :         1., 
        "TaggerElectronTool.Ele_Eta_Cal" :        0.,
        "TaggerElectronTool.Ele_ProbMin" :       0.0,
        "TaggerElectronTool.Ele_AverageOmega" : 0.33,
        
        "TaggerKaonOppositeTool.Kaon_Pt_cut" :   700,
        "TaggerKaonOppositeTool.Kaon_P_cut" :   2000,
        "TaggerKaonOppositeTool.Kaon_lcs_cut" :  3,
        "TaggerKaonOppositeTool.Kaon_IPs_cut" : 4.,
        "TaggerKaonOppositeTool.Kaon_IP_cut" :   1.6,
        "TaggerKaonOppositeTool.Kaon_PIDk_cut" :    -100.,
        "TaggerKaonOppositeTool.Kaon_PIDkp_cut" :   -300.,
        "TaggerKaonOppositeTool.Kaon_ghost_cut" :   -999.,
        "TaggerKaonOppositeTool.Kaon_ipPU_cut" :      6.0,
        "TaggerKaonOppositeTool.Kaon_distPhi_cut" : 0.005,
        "TaggerKaonOppositeTool.Kaon_PIDNNk_cut" :    0.25, # new
        "TaggerKaonOppositeTool.Kaon_PIDNNe_cut" :   0.8, #new
        "TaggerKaonOppositeTool.Kaon_PIDNNm_cut" :   0.8, #new
        "TaggerKaonOppositeTool.Kaon_PIDNNpi_cut" :   0.8, #new
        "TaggerKaonOppositeTool.Kaon_PIDNNp_cut" :    0.8, #new
        "TaggerKaonOppositeTool.Kaon_PIDNNkp_cut" :   0.0, # new
        "TaggerKaonOppositeTool.Kaon_PIDNNkpi_cut" : -0.6, # new
        "TaggerKaonOppositeTool.Kaon_ghostProb_cut" :0.35, # new
        "TaggerKaonOppositeTool.Kaon_P0_Cal" :        0.0,
        "TaggerKaonOppositeTool.Kaon_P1_Cal" :        1.0,
        "TaggerKaonOppositeTool.Kaon_Eta_Cal" :       0.0,
        "TaggerKaonOppositeTool.Kaon_ProbMin" :       0.0,
        "TaggerKaonOppositeTool.Kaon_AverageOmega" : 0.33,

        "TaggerNEWKaonOppositeTool.Kaon_PIDk_cut":     0.75,
        "TaggerNEWKaonOppositeTool.Kaon_PIDkp_cut":   -3.,
        "TaggerNEWKaonOppositeTool.Kaon_distPhi_cut":  0.005,
        "TaggerNEWKaonOppositeTool.Kaon_tsa_cut":   -999.0, # formerly Kaon_ghost_cut
        "TaggerNEWKaonOppositeTool.Kaon_ipPU_cut":     3.0,
        "TaggerNEWKaonOppositeTool.Kaon_ghost_cut":    0.5,
        "TaggerNEWKaonOppositeTool.Kaon_NN1_cut":      0.6,
        "TaggerNEWKaonOppositeTool.Kaon_ProbMin":      0.5,
        "TaggerNEWKaonOppositeTool.Kaon_P0_Cal":       0.4349, #+/- 0.0016
        "TaggerNEWKaonOppositeTool.Kaon_P1_Cal":       1.074, #+/- 0.0292
        "TaggerNEWKaonOppositeTool.Kaon_AverageOmega": 0.4350,

        "TaggerKaonSameTool.KaonSame_Pt_cut" : 850,
        "TaggerKaonSameTool.KaonSame_P_cut"  : 5250,
        "TaggerKaonSameTool.KaonSame_IP_cut" : 4.125,
        "TaggerKaonSameTool.KaonSame_lcs_cut" : 3.75,
        "TaggerKaonSameTool.KaonSame_Phi_cut" : 0.825,
        "TaggerKaonSameTool.KaonSame_Eta_cut" : 0.6,
        "TaggerKaonSameTool.KaonSame_dQ_cut" : 1850.,
        "TaggerKaonSameTool.KaonSame_PIDk_cut" : 3.5,
        "TaggerKaonSameTool.KaonSame_PIDkp_cut" : -8.5,
        "TaggerKaonSameTool.KaonSame_ipPU_cut" : 3.0,
        "TaggerKaonSameTool.KaonSame_distPhi_cut" : 0.005,
        "TaggerKaonSameTool.KaonSame_ProbMin"  : 0.,
        "TaggerKaonSameTool.KaonSame_AverageOmega" : 0.33,
        "TaggerKaonSameTool.KaonSame_P0_Cal"   : 0.350,
        "TaggerKaonSameTool.KaonSame_P1_Cal"   : 0.51,
        "TaggerKaonSameTool.KaonSame_Eta_Cal"  : 0.324,
        # Same tuning as Reco12_2011, different calibration 
        "TaggerNEWKaonSameTool.Kaon_PIDk_cut"   :    0.75,
        "TaggerNEWKaonSameTool.Kaon_PIDkp_cut"  :   -8.5,
        "TaggerNEWKaonSameTool.Kaon_ghost_cut"  :    1000,
        "TaggerNEWKaonSameTool.Kaon_distPhi_cut":    0.005,
        "TaggerNEWKaonSameTool.Kaon_NN1_cut"    :    0.65,
        "TaggerNEWKaonSameTool.Kaon_P0_Cal"     :    0.436,
        "TaggerNEWKaonSameTool.Kaon_P1_Cal"     :    0.900,
        "TaggerNEWKaonSameTool.Kaon_AverageOmega":   0.430,
        

        "TaggerPionSameTool.PionSame_Pt_cut"  : 600,
        "TaggerPionSameTool.PionSame_P_cut"   : 4000,
        "TaggerPionSameTool.PionSame_IPs_cut" : 9.,
        "TaggerPionSameTool.PionSame_lcs_cut" : 5.0,
        "TaggerPionSameTool.PionSame_deta_max_cut" : 0.35,
        "TaggerPionSameTool.PionSame_deta_min_cut" : -0.5,
        "TaggerPionSameTool.PionSame_dphi_cut": 0.75,
        "TaggerPionSameTool.PionSame_dR_cut"  : 0.75,
        "TaggerPionSameTool.PionSame_dQ_cut"  : 2500,
        "TaggerPionSameTool.PionSame_dQ_extra_cut" : 1200,
        "TaggerPionSameTool.PionSame_ghost_cut" : -999.,
        "TaggerPionSameTool.PionSame_PIDNoK_cut" : 4.3,
        "TaggerPionSameTool.PionSame_PIDNoP_cut" : 14.0,
        "TaggerPionSameTool.PionSame_ipPU_cut" : 6.0,
        "TaggerPionSameTool.PionSame_distPhi_cut" : 0.005,
        "TaggerPionSameTool.PionSame_ProbMin" : 0.56,
        "TaggerPionSameTool.PionSame_AverageOmega" : 0.41,
        "TaggerPionSameTool.PionSame_P0_Cal" : 0.4493,
        "TaggerPionSameTool.PionSame_P1_Cal" : 0.7794,
        "TaggerPionSameTool.PionSame_P2_Cal" : -1.8686,
        "TaggerPionSameTool.PionSame_Eta_Cal" : 0.3908,

        "TaggerVertexChargeTool.SVertexOneSeedTool.MinSeedProbability" : 0.42,
        "TaggerVertexChargeTool.SVertexOneSeedTool.MinSeedPtmin"       : 0.1,
        "TaggerVertexChargeTool.SVertexOneSeedTool.MinSeedIPSmin"      : 2.5,
        "TaggerVertexChargeTool.SVertexOneSeedTool.MinSeedDphimin"     : 0.,
        #"TaggerVertexChargeTool.SVertexOneSeedTool.MinSeedDphimax"     : 5,
        "TaggerVertexChargeTool.SVertexOneSeedTool.ghostProb_cut"      : 0.37,  # new
        "TaggerVertexChargeTool.Vtx_PowerK"     : 0.55,
        "TaggerVertexChargeTool.Vtx_MinimumVCharge" : 0.2,
        "TaggerVertexChargeTool.Vtx_Ptsum"      : 2.2,
        "TaggerVertexChargeTool.Vtx_Ptmean"     : 0.0,
        "TaggerVertexChargeTool.Vtx_IPSsum"     : 0., # no cut
        "TaggerVertexChargeTool.Vtx_DocaMaxsum" : 0.5,
        "TaggerVertexChargeTool.Vtx_Psum"       : 8.0,
        "TaggerVertexChargeTool.Vtx_Msum"       : 0.6,
        "TaggerVertexChargeTool.Vtx_P0_Cal"     : 0.0,
        "TaggerVertexChargeTool.Vtx_P1_Cal"     : 1.0,
        "TaggerVertexChargeTool.Vtx_Eta_Cal"    : 0.0,
        "TaggerVertexChargeTool.Vtx_ProbMin"    : 0.0,
        "TaggerVertexChargeTool.Vtx_AverageOmega" : 0.41,

        "TaggerCharmTool.Charm_P0_Cal":  0.0,
        "TaggerCharmTool.Charm_P1_Cal":  1.0,
        "TaggerCharmTool.Charm_Eta_Cal": 0.0,
        "TaggerCharmTool.LifetimeFitterAlgName": "'LoKi::LifetimeFitter'", 

         #// cuts_Moriond2013 trained on CloneKO, lowCorrelation, newNNPID D,N, CE tanh, Split i%3=0,1  
        "TaggerMuonTool.P0_mu_scale"        : -5.47039,
        "TaggerMuonTool.P1_mu_scale"        :  1.23885,
        "TaggerMuonTool.P2_mu_scale"        : -0.00793716,
        "TaggerMuonTool.P3_mu_scale"        :  0.647253,
  
        "TaggerElectronTool.P0_e_scale"     :  -3.04032,
        "TaggerElectronTool.P1_e_scale"     :   1.7055,
        "TaggerElectronTool.P2_e_scale"     :  -0.136785,
        "TaggerElectronTool.P3_e_scale"     :   0.646733,

        "TaggerKaonOppositeTool.P0_k_scale"     :  -5.12122,
        "TaggerKaonOppositeTool.P1_k_scale"     :   1.21427,
        "TaggerKaonOppositeTool.P2_k_scale"     :   0.088854,
        "TaggerKaonOppositeTool.P3_k_scale"     :   0.573943,

        "TaggerVertexChargeTool.P0_vtx_scale"   :-5.77134,
        "TaggerVertexChargeTool.P1_vtx_scale"   : 1.11591,
        "TaggerVertexChargeTool.P2_vtx_scale"   : 0.113682,
        "TaggerVertexChargeTool.P3_vtx_scale"   : 0.570684,

        "TaggerKaonSameTool.P0_ks_scale"        :  1.22418, 
        "TaggerKaonSameTool.P1_ks_scale"        : -1.63297,  
        "TaggerKaonSameTool.P2_ks_scale"        :  0.401361,
        "TaggerKaonSameTool.P3_ks_scale"        :  0.,

        "TaggerPionSameTool.P0_ps_scale"        :  1.22123,
        "TaggerPionSameTool.P1_ps_scale"        :  -1.76027,
        "TaggerPionSameTool.P2_ps_scale"        :  0.651766,
        "TaggerPionSameTool.P3_ps_scale"        :  0.,          

        },

    #use cuts optimized and calibrated with Strip20 (2012 data 2fb-1)
    'Stripping21' : { 
        "Personality": "'Reco14'",  # it is then propagated to all other tools
        "IPPU_cut" : 3.0,
        "distphi_cut" : 0.005,

        #"ForceSignalID" :   , #"Bs", "Bd", "Bu" to force SSk or SSpi

        "EnableCharmTagger": "True", # was disabled in the initial Stripping21
        "EnableProtonTagger": "True",
        "EnablePionBDTTagger": "True",
        
        # choice of fitters and tools
        "TaggingUtils.PVReFitter":           "'AdaptivePVReFitter'",
        "TaggingUtils.LifetimeFitter":       "'PropertimeFitter'",
        "TaggingUtils.VertexFitter":         "'OfflineVertexFitter'",
        "TaggingUtils.DistanceCalculator":   "'LoKi::DistanceCalculator'",
        "TaggingUtils.ParticleDescendants":  "'ParticleDescendants'",
        
        #Following 2 lines optimize the average and combined (5 cat) performances for B2JpsiK
        #When performing your analisis, please find yourself the cut which maximeze the Eeff
        #Probably if you are using Eeff event per event no cut will be needed
        #"CombineTaggersProbability.ProbMin_OS" : 0.58,
        #"CombineTaggersProbability.ProbMin" : 0.55,
 
        "CombineTaggersProbability.P0_Cal_OS" : 0.390,
        "CombineTaggersProbability.P1_Cal_OS" : 0.899,
        "CombineTaggersProbability.Eta_Cal_OS" : 0.365,

        # calibration for combination with nnetOSK 
        "CombineTaggersProbability.P0_Cal_OSnnet" : 0.4353,
        "CombineTaggersProbability.P1_Cal_OSnnet" : 0.948,
        "CombineTaggersProbability.Eta_Cal_OSnnet" : 0.4356,

        "TaggerMuonTool.Muon_Pt_cut" :     1100.0,
        "TaggerMuonTool.Muon_P_cut" :         0.0,
        "TaggerMuonTool.Muon_lcs_cut" :       3.0,
        "TaggerMuonTool.Muon_IPs_cut" :       0.0,
        "TaggerMuonTool.Muon_PIDm_cut" :   -100.0,  # no cut
        "TaggerMuonTool.Muon_ipPU_cut" :      3.0,
        "TaggerMuonTool.Muon_distPhi_cut"   : 0.005,        
        "TaggerMuonTool.Muon_ghostprob_cut" : 0.4,  # new cut
        "TaggerMuonTool.Muon_PIDNNm_cut"    : 0.35,  # new cut       
        "TaggerMuonTool.Muon_PIDNNpi_cut"   : 0.8,  # new cut       
        "TaggerMuonTool.Muon_PIDNNe_cut"    : 0.8,  # new cut       
        "TaggerMuonTool.Muon_PIDNNk_cut"    : 0.8,  # new cut       
        "TaggerMuonTool.Muon_PIDNNp_cut"    : 0.8,  # new cut       
        "TaggerMuonTool.Muon_P0_Cal" :       0.,
        "TaggerMuonTool.Muon_P1_Cal" :       1., 
        "TaggerMuonTool.Muon_Eta_Cal" :      0.,
        "TaggerMuonTool.Muon_ProbMin" :       0.0,
        "TaggerMuonTool.Muon_AverageOmega" :  0.33,

        "TaggerElectronTool.Ele_Pt_cut" :       1100,
        "TaggerElectronTool.Ele_P_cut" :         0.0,
        "TaggerElectronTool.Ele_lcs_cut" :       3.,
        "TaggerElectronTool.Ele_IPs_cut" :       3.5,
        "TaggerElectronTool.Ele_ghost_cut" :   -999.,
        "TaggerElectronTool.Ele_VeloChargeMin" : 0.0,
        "TaggerElectronTool.Ele_VeloChargeMax" : 1.4,
        "TaggerElectronTool.Ele_EoverP" :       0.85,
        "TaggerElectronTool.Ele_EoverPmax" :      2.,
        "TaggerElectronTool.Ele_PIDe_cut" :     -1.0,
        "TaggerElectronTool.Ele_ipPU_cut" :      4.0,
        "TaggerElectronTool.Ele_distPhi_cut" : 0.005,
        "TaggerElectronTool.Ele_PIDNNe_cut" :    0.1,  # new
        "TaggerElectronTool.Ele_PIDNNk_cut" :    0.8,  # new
        "TaggerElectronTool.Ele_PIDNNp_cut" :    0.8,  # new
        "TaggerElectronTool.Ele_PIDNNpi_cut" :   0.8,  # new
        "TaggerElectronTool.Ele_PIDNNepi_cut" : -0.8,  # new
        "TaggerElectronTool.Ele_ghostProb_cut" : 0.4,  # new       
        "TaggerElectronTool.Ele_P0_Cal" :         0.,
        "TaggerElectronTool.Ele_P1_Cal" :         1., 
        "TaggerElectronTool.Ele_Eta_Cal" :        0.,
        "TaggerElectronTool.Ele_ProbMin" :       0.0,
        "TaggerElectronTool.Ele_AverageOmega" : 0.33,
        
        "TaggerKaonOppositeTool.Kaon_Pt_cut" :   700,
        "TaggerKaonOppositeTool.Kaon_P_cut" :   2000,
        "TaggerKaonOppositeTool.Kaon_lcs_cut" :  3,
        "TaggerKaonOppositeTool.Kaon_IPs_cut" : 4.,
        "TaggerKaonOppositeTool.Kaon_IP_cut" :   1.6,
        "TaggerKaonOppositeTool.Kaon_PIDk_cut" :    -100.,
        "TaggerKaonOppositeTool.Kaon_PIDkp_cut" :   -300.,
        "TaggerKaonOppositeTool.Kaon_ghost_cut" :   -999.,
        "TaggerKaonOppositeTool.Kaon_ipPU_cut" :      6.0,
        "TaggerKaonOppositeTool.Kaon_distPhi_cut" : 0.005,
        "TaggerKaonOppositeTool.Kaon_PIDNNk_cut" :    0.25, # new
        "TaggerKaonOppositeTool.Kaon_PIDNNe_cut" :   0.8, #new
        "TaggerKaonOppositeTool.Kaon_PIDNNm_cut" :   0.8, #new
        "TaggerKaonOppositeTool.Kaon_PIDNNpi_cut" :   0.8, #new
        "TaggerKaonOppositeTool.Kaon_PIDNNp_cut" :    0.8, #new
        "TaggerKaonOppositeTool.Kaon_PIDNNkp_cut" :   0.0, # new
        "TaggerKaonOppositeTool.Kaon_PIDNNkpi_cut" : -0.6, # new
        "TaggerKaonOppositeTool.Kaon_ghostProb_cut" :0.35, # new
        "TaggerKaonOppositeTool.Kaon_P0_Cal" :        0.0,
        "TaggerKaonOppositeTool.Kaon_P1_Cal" :        1.0,
        "TaggerKaonOppositeTool.Kaon_Eta_Cal" :       0.0,
        "TaggerKaonOppositeTool.Kaon_ProbMin" :       0.0,
        "TaggerKaonOppositeTool.Kaon_AverageOmega" : 0.33,

        "TaggerNEWKaonOppositeTool.Kaon_PIDk_cut":     0.75,
        "TaggerNEWKaonOppositeTool.Kaon_PIDkp_cut":   -3.,
        "TaggerNEWKaonOppositeTool.Kaon_distPhi_cut":  0.005,
        "TaggerNEWKaonOppositeTool.Kaon_tsa_cut":   -999.0, # formerly Kaon_ghost_cut
        "TaggerNEWKaonOppositeTool.Kaon_ipPU_cut":     3.0,
        "TaggerNEWKaonOppositeTool.Kaon_ghost_cut":    0.5,
        "TaggerNEWKaonOppositeTool.Kaon_NN1_cut":      0.6,
        "TaggerNEWKaonOppositeTool.Kaon_ProbMin":      0.5,
        "TaggerNEWKaonOppositeTool.Kaon_P0_Cal":       0.4349, #+/- 0.0016
        "TaggerNEWKaonOppositeTool.Kaon_P1_Cal":       1.074, #+/- 0.0292
        "TaggerNEWKaonOppositeTool.Kaon_AverageOmega": 0.4350,

        "TaggerKaonSameTool.KaonSame_Pt_cut" : 850,
        "TaggerKaonSameTool.KaonSame_P_cut"  : 5250,
        "TaggerKaonSameTool.KaonSame_IP_cut" : 4.125,
        "TaggerKaonSameTool.KaonSame_lcs_cut" : 3.75,
        "TaggerKaonSameTool.KaonSame_Phi_cut" : 0.825,
        "TaggerKaonSameTool.KaonSame_Eta_cut" : 0.6,
        "TaggerKaonSameTool.KaonSame_dQ_cut" : 1850.,
        "TaggerKaonSameTool.KaonSame_PIDk_cut" : 3.5,
        "TaggerKaonSameTool.KaonSame_PIDkp_cut" : -8.5,
        "TaggerKaonSameTool.KaonSame_ipPU_cut" : 3.0,
        "TaggerKaonSameTool.KaonSame_distPhi_cut" : 0.005,
        "TaggerKaonSameTool.KaonSame_ProbMin"  : 0.,
        "TaggerKaonSameTool.KaonSame_AverageOmega" : 0.33,
        "TaggerKaonSameTool.KaonSame_P0_Cal"   : 0.350,
        "TaggerKaonSameTool.KaonSame_P1_Cal"   : 0.51,
        "TaggerKaonSameTool.KaonSame_Eta_Cal"  : 0.324,
        # Same tuning as Reco12_2011, different calibration 
        "TaggerNEWKaonSameTool.Kaon_PIDk_cut"   :    0.75,
        "TaggerNEWKaonSameTool.Kaon_PIDkp_cut"  :   -8.5,
        "TaggerNEWKaonSameTool.Kaon_ghost_cut"  :    1000,
        "TaggerNEWKaonSameTool.Kaon_distPhi_cut":    0.005,
        "TaggerNEWKaonSameTool.Kaon_NN1_cut"    :    0.65,
        "TaggerNEWKaonSameTool.Kaon_P0_Cal"     :    0.436,
        "TaggerNEWKaonSameTool.Kaon_P1_Cal"     :    0.900,
        "TaggerNEWKaonSameTool.Kaon_AverageOmega":   0.430,
        

        "TaggerPionSameTool.PionSame_Pt_cut"  : 600,
        "TaggerPionSameTool.PionSame_P_cut"   : 4000,
        "TaggerPionSameTool.PionSame_IPs_cut" : 9.,
        "TaggerPionSameTool.PionSame_lcs_cut" : 5.0,
        "TaggerPionSameTool.PionSame_deta_max_cut" : 0.35,
        "TaggerPionSameTool.PionSame_deta_min_cut" : -0.5,
        "TaggerPionSameTool.PionSame_dphi_cut": 0.75,
        "TaggerPionSameTool.PionSame_dR_cut"  : 0.75,
        "TaggerPionSameTool.PionSame_dQ_cut"  : 2500,
        "TaggerPionSameTool.PionSame_dQ_extra_cut" : 1200,
        "TaggerPionSameTool.PionSame_ghost_cut" : -999.,
        "TaggerPionSameTool.PionSame_PIDNoK_cut" : 4.3,
        "TaggerPionSameTool.PionSame_PIDNoP_cut" : 14.0,
        "TaggerPionSameTool.PionSame_ipPU_cut" : 6.0,
        "TaggerPionSameTool.PionSame_distPhi_cut" : 0.005,
        "TaggerPionSameTool.PionSame_ProbMin" : 0.56,
        "TaggerPionSameTool.PionSame_AverageOmega" : 0.41,
        "TaggerPionSameTool.PionSame_P0_Cal" : 0.4493,
        "TaggerPionSameTool.PionSame_P1_Cal" : 0.7794,
        "TaggerPionSameTool.PionSame_P2_Cal" : -1.8686,
        "TaggerPionSameTool.PionSame_Eta_Cal" : 0.3908,

        "TaggerVertexChargeTool.SVertexOneSeedTool.MinSeedProbability" : 0.42,
        "TaggerVertexChargeTool.SVertexOneSeedTool.MinSeedPtmin"       : 0.1,
        "TaggerVertexChargeTool.SVertexOneSeedTool.MinSeedIPSmin"      : 2.5,
        "TaggerVertexChargeTool.SVertexOneSeedTool.MinSeedDphimin"     : 0.,
        #"TaggerVertexChargeTool.SVertexOneSeedTool.MinSeedDphimax"     : 5,
        "TaggerVertexChargeTool.SVertexOneSeedTool.ghostProb_cut"      : 0.37,  # new
        "TaggerVertexChargeTool.Vtx_PowerK"     : 0.55,
        "TaggerVertexChargeTool.Vtx_MinimumVCharge" : 0.2,
        "TaggerVertexChargeTool.Vtx_Ptsum"      : 2.2,
        "TaggerVertexChargeTool.Vtx_Ptmean"     : 0.0,
        "TaggerVertexChargeTool.Vtx_IPSsum"     : 0., # no cut
        "TaggerVertexChargeTool.Vtx_DocaMaxsum" : 0.5,
        "TaggerVertexChargeTool.Vtx_Psum"       : 8.0,
        "TaggerVertexChargeTool.Vtx_Msum"       : 0.6,
        "TaggerVertexChargeTool.Vtx_P0_Cal"     : 0.0,
        "TaggerVertexChargeTool.Vtx_P1_Cal"     : 1.0,
        "TaggerVertexChargeTool.Vtx_Eta_Cal"    : 0.0,
        "TaggerVertexChargeTool.Vtx_ProbMin"    : 0.0,
        "TaggerVertexChargeTool.Vtx_AverageOmega" : 0.41,

        "TaggerCharmTool.Charm_P0_Cal"     : 0.0,
        "TaggerCharmTool.Charm_P1_Cal"     : 1.0,
        "TaggerCharmTool.Charm_Eta_Cal"    : 0.0,


         #// cuts_Moriond2013 trained on CloneKO, lowCorrelation, newNNPID D,N, CE tanh, Split i%3=0,1  
        "TaggerMuonTool.P0_mu_scale"        : -5.47039,
        "TaggerMuonTool.P1_mu_scale"        :  1.23885,
        "TaggerMuonTool.P2_mu_scale"        : -0.00793716,
        "TaggerMuonTool.P3_mu_scale"        :  0.647253,
  
        "TaggerElectronTool.P0_e_scale"     :  -3.04032,
        "TaggerElectronTool.P1_e_scale"     :   1.7055,
        "TaggerElectronTool.P2_e_scale"     :  -0.136785,
        "TaggerElectronTool.P3_e_scale"     :   0.646733,

        "TaggerKaonOppositeTool.P0_k_scale"     :  -5.12122,
        "TaggerKaonOppositeTool.P1_k_scale"     :   1.21427,
        "TaggerKaonOppositeTool.P2_k_scale"     :   0.088854,
        "TaggerKaonOppositeTool.P3_k_scale"     :   0.573943,

        "TaggerVertexChargeTool.P0_vtx_scale"   :-5.77134,
        "TaggerVertexChargeTool.P1_vtx_scale"   : 1.11591,
        "TaggerVertexChargeTool.P2_vtx_scale"   : 0.113682,
        "TaggerVertexChargeTool.P3_vtx_scale"   : 0.570684,

        "TaggerKaonSameTool.P0_ks_scale"        :  1.22418, 
        "TaggerKaonSameTool.P1_ks_scale"        : -1.63297,  
        "TaggerKaonSameTool.P2_ks_scale"        :  0.401361,
        "TaggerKaonSameTool.P3_ks_scale"        :  0.,

        "TaggerPionSameTool.P0_ps_scale"        :  1.22123,
        "TaggerPionSameTool.P1_ps_scale"        :  -1.76027,
        "TaggerPionSameTool.P2_ps_scale"        :  0.651766,
        "TaggerPionSameTool.P3_ps_scale"        :  0.,          

        },

    #use cuts optimized and calibrated with Strip20 (2012 data 2fb-1)
    'Reco14_2012' : { 
        "Personality": "'Reco14'",  # it is then propagated to all other tools
        "IPPU_cut" : 3.0,
        "distphi_cut" : 0.005,

        #"ForceSignalID" :   , #"Bs", "Bd", "Bu" to force SSk or SSpi

        "EnableCharmTagger": "True",
        "EnableProtonTagger": "True",
        "EnablePionBDTTagger": "True",
        
        "TaggingUtils.PVReFitter":           "'AdaptivePVReFitter'",
        "TaggingUtils.LifetimeFitter":       "'PropertimeFitter'",
        "TaggingUtils.VertexFitter":         "'OfflineVertexFitter'",
        "TaggingUtils.DistanceCalculator":   "'LoKi::DistanceCalculator'",
        "TaggingUtils.ParticleDescendants":  "'ParticleDescendants'",
 
        
        #Following 2 lines optimize the average and combined (5 cat) performances for B2JpsiK
        #When performing your analisis, please find yourself the cut which maximeze the Eeff
        #Probably if you are using Eeff event per event no cut will be needed
        #"CombineTaggersProbability.ProbMin_OS" : 0.58,
        #"CombineTaggersProbability.ProbMin" : 0.55,
 
        "CombineTaggersProbability.P0_Cal_OS" : 0.390,
        "CombineTaggersProbability.P1_Cal_OS" : 0.899,
        "CombineTaggersProbability.Eta_Cal_OS" : 0.365,

        # calibration for combination with nnetOSK 
        "CombineTaggersProbability.P0_Cal_OSnnet" : 0.4353,
        "CombineTaggersProbability.P1_Cal_OSnnet" : 0.948,
        "CombineTaggersProbability.Eta_Cal_OSnnet" : 0.4356,

        "TaggerMuonTool.Muon_Pt_cut" :     1100.0,
        "TaggerMuonTool.Muon_P_cut" :         0.0,
        "TaggerMuonTool.Muon_lcs_cut" :       3.0,
        "TaggerMuonTool.Muon_IPs_cut" :       0.0,
        "TaggerMuonTool.Muon_PIDm_cut" :   -100.0,  # no cut
        "TaggerMuonTool.Muon_ipPU_cut" :      3.0,
        "TaggerMuonTool.Muon_distPhi_cut"   : 0.005,        
        "TaggerMuonTool.Muon_ghostprob_cut" : 0.4,  # new cut
        "TaggerMuonTool.Muon_PIDNNm_cut"    : 0.35,  # new cut       
        "TaggerMuonTool.Muon_PIDNNpi_cut"   : 0.8,  # new cut       
        "TaggerMuonTool.Muon_PIDNNe_cut"    : 0.8,  # new cut       
        "TaggerMuonTool.Muon_PIDNNk_cut"    : 0.8,  # new cut       
        "TaggerMuonTool.Muon_PIDNNp_cut"    : 0.8,  # new cut       
        "TaggerMuonTool.Muon_P0_Cal" :       0.,
        "TaggerMuonTool.Muon_P1_Cal" :       1., 
        "TaggerMuonTool.Muon_Eta_Cal" :      0.,
        "TaggerMuonTool.Muon_ProbMin" :       0.0,
        "TaggerMuonTool.Muon_AverageOmega" :  0.33,

        "TaggerElectronTool.Ele_Pt_cut" :       1100,
        "TaggerElectronTool.Ele_P_cut" :         0.0,
        "TaggerElectronTool.Ele_lcs_cut" :       3.,
        "TaggerElectronTool.Ele_IPs_cut" :       3.5,
        "TaggerElectronTool.Ele_ghost_cut" :   -999.,
        "TaggerElectronTool.Ele_VeloChargeMin" : 0.0,
        "TaggerElectronTool.Ele_VeloChargeMax" : 1.4,
        "TaggerElectronTool.Ele_EoverP" :       0.85,
        "TaggerElectronTool.Ele_EoverPmax" :      2.,
        "TaggerElectronTool.Ele_PIDe_cut" :     -1.0,
        "TaggerElectronTool.Ele_ipPU_cut" :      4.0,
        "TaggerElectronTool.Ele_distPhi_cut" : 0.005,
        "TaggerElectronTool.Ele_PIDNNe_cut" :    0.1,  # new
        "TaggerElectronTool.Ele_PIDNNk_cut" :    0.8,  # new
        "TaggerElectronTool.Ele_PIDNNp_cut" :    0.8,  # new
        "TaggerElectronTool.Ele_PIDNNpi_cut" :   0.8,  # new
        "TaggerElectronTool.Ele_PIDNNepi_cut" : -0.8,  # new
        "TaggerElectronTool.Ele_ghostProb_cut" : 0.4,  # new       
        "TaggerElectronTool.Ele_P0_Cal" :         0.,
        "TaggerElectronTool.Ele_P1_Cal" :         1., 
        "TaggerElectronTool.Ele_Eta_Cal" :        0.,
        "TaggerElectronTool.Ele_ProbMin" :       0.0,
        "TaggerElectronTool.Ele_AverageOmega" : 0.33,
        
        "TaggerKaonOppositeTool.Kaon_Pt_cut" :   700,
        "TaggerKaonOppositeTool.Kaon_P_cut" :   2000,
        "TaggerKaonOppositeTool.Kaon_lcs_cut" :  3,
        "TaggerKaonOppositeTool.Kaon_IPs_cut" : 4.,
        "TaggerKaonOppositeTool.Kaon_IP_cut" :   1.6,
        "TaggerKaonOppositeTool.Kaon_PIDk_cut" :    -100.,
        "TaggerKaonOppositeTool.Kaon_PIDkp_cut" :   -300.,
        "TaggerKaonOppositeTool.Kaon_ghost_cut" :   -999.,
        "TaggerKaonOppositeTool.Kaon_ipPU_cut" :      6.0,
        "TaggerKaonOppositeTool.Kaon_distPhi_cut" : 0.005,
        "TaggerKaonOppositeTool.Kaon_PIDNNk_cut" :    0.25, # new
        "TaggerKaonOppositeTool.Kaon_PIDNNe_cut" :   0.8, #new
        "TaggerKaonOppositeTool.Kaon_PIDNNm_cut" :   0.8, #new
        "TaggerKaonOppositeTool.Kaon_PIDNNpi_cut" :   0.8, #new
        "TaggerKaonOppositeTool.Kaon_PIDNNp_cut" :    0.8, #new
        "TaggerKaonOppositeTool.Kaon_PIDNNkp_cut" :   0.0, # new
        "TaggerKaonOppositeTool.Kaon_PIDNNkpi_cut" : -0.6, # new
        "TaggerKaonOppositeTool.Kaon_ghostProb_cut" :0.35, # new
        "TaggerKaonOppositeTool.Kaon_P0_Cal" :        0.0,
        "TaggerKaonOppositeTool.Kaon_P1_Cal" :        1.0,
        "TaggerKaonOppositeTool.Kaon_Eta_Cal" :       0.0,
        "TaggerKaonOppositeTool.Kaon_ProbMin" :       0.0,
        "TaggerKaonOppositeTool.Kaon_AverageOmega" : 0.33,

        "TaggerNEWKaonOppositeTool.Kaon_PIDk_cut":     0.75,
        "TaggerNEWKaonOppositeTool.Kaon_PIDkp_cut":   -3.,
        "TaggerNEWKaonOppositeTool.Kaon_distPhi_cut":  0.005,
        "TaggerNEWKaonOppositeTool.Kaon_tsa_cut":   -999.0, # formerly Kaon_ghost_cut
        "TaggerNEWKaonOppositeTool.Kaon_ipPU_cut":     3.0,
        "TaggerNEWKaonOppositeTool.Kaon_ghost_cut":    0.5,
        "TaggerNEWKaonOppositeTool.Kaon_NN1_cut":      0.6,
        "TaggerNEWKaonOppositeTool.Kaon_ProbMin":      0.5,
        "TaggerNEWKaonOppositeTool.Kaon_P0_Cal":       0.4349, #+/- 0.0016
        "TaggerNEWKaonOppositeTool.Kaon_P1_Cal":       1.074, #+/- 0.0292
        "TaggerNEWKaonOppositeTool.Kaon_AverageOmega": 0.4350,

        "TaggerKaonSameTool.KaonSame_Pt_cut" : 850,
        "TaggerKaonSameTool.KaonSame_P_cut"  : 5250,
        "TaggerKaonSameTool.KaonSame_IP_cut" : 4.125,
        "TaggerKaonSameTool.KaonSame_lcs_cut" : 3.75,
        "TaggerKaonSameTool.KaonSame_Phi_cut" : 0.825,
        "TaggerKaonSameTool.KaonSame_Eta_cut" : 0.6,
        "TaggerKaonSameTool.KaonSame_dQ_cut" : 1850.,
        "TaggerKaonSameTool.KaonSame_PIDk_cut" : 3.5,
        "TaggerKaonSameTool.KaonSame_PIDkp_cut" : -8.5,
        "TaggerKaonSameTool.KaonSame_ipPU_cut" : 3.0,
        "TaggerKaonSameTool.KaonSame_distPhi_cut" : 0.005,
        "TaggerKaonSameTool.KaonSame_ProbMin"  : 0.,
        "TaggerKaonSameTool.KaonSame_AverageOmega" : 0.33,
        "TaggerKaonSameTool.KaonSame_P0_Cal"   : 0.350,
        "TaggerKaonSameTool.KaonSame_P1_Cal"   : 0.51,
        "TaggerKaonSameTool.KaonSame_Eta_Cal"  : 0.324,
        # Same tuning as Reco12_2011, different calibration 
        "TaggerNEWKaonSameTool.Kaon_PIDk_cut"   :    0.75,
        "TaggerNEWKaonSameTool.Kaon_PIDkp_cut"  :   -8.5,
        "TaggerNEWKaonSameTool.Kaon_ghost_cut"  :    1000,
        "TaggerNEWKaonSameTool.Kaon_distPhi_cut":    0.005,
        "TaggerNEWKaonSameTool.Kaon_NN1_cut"    :    0.65,
        "TaggerNEWKaonSameTool.Kaon_P0_Cal"     :    0.436,
        "TaggerNEWKaonSameTool.Kaon_P1_Cal"     :    0.900,
        "TaggerNEWKaonSameTool.Kaon_AverageOmega":   0.430,
        

        "TaggerPionSameTool.PionSame_Pt_cut"  : 600,
        "TaggerPionSameTool.PionSame_P_cut"   : 4000,
        "TaggerPionSameTool.PionSame_IPs_cut" : 9.,
        "TaggerPionSameTool.PionSame_lcs_cut" : 5.0,
        "TaggerPionSameTool.PionSame_deta_max_cut" : 0.35,
        "TaggerPionSameTool.PionSame_deta_min_cut" : -0.5,
        "TaggerPionSameTool.PionSame_dphi_cut": 0.75,
        "TaggerPionSameTool.PionSame_dR_cut"  : 0.75,
        "TaggerPionSameTool.PionSame_dQ_cut"  : 2500,
        "TaggerPionSameTool.PionSame_dQ_extra_cut" : 1200,
        "TaggerPionSameTool.PionSame_ghost_cut" : -999.,
        "TaggerPionSameTool.PionSame_PIDNoK_cut" : 4.3,
        "TaggerPionSameTool.PionSame_PIDNoP_cut" : 14.0,
        "TaggerPionSameTool.PionSame_ipPU_cut" : 6.0,
        "TaggerPionSameTool.PionSame_distPhi_cut" : 0.005,
        "TaggerPionSameTool.PionSame_ProbMin" : 0.56,
        "TaggerPionSameTool.PionSame_AverageOmega" : 0.41,
        "TaggerPionSameTool.PionSame_P0_Cal" : 0.4493,
        "TaggerPionSameTool.PionSame_P1_Cal" : 0.7794,
        "TaggerPionSameTool.PionSame_P2_Cal" : -1.8686,
        "TaggerPionSameTool.PionSame_Eta_Cal" : 0.3908,

        "TaggerVertexChargeTool.SVertexOneSeedTool.MinSeedProbability" : 0.42,
        "TaggerVertexChargeTool.SVertexOneSeedTool.MinSeedPtmin"       : 0.1,
        "TaggerVertexChargeTool.SVertexOneSeedTool.MinSeedIPSmin"      : 2.5,
        "TaggerVertexChargeTool.SVertexOneSeedTool.MinSeedDphimin"     : 0.,
        #"TaggerVertexChargeTool.SVertexOneSeedTool.MinSeedDphimax"     : 5,
        "TaggerVertexChargeTool.SVertexOneSeedTool.ghostProb_cut"      : 0.37,  # new
        "TaggerVertexChargeTool.Vtx_PowerK"     : 0.55,
        "TaggerVertexChargeTool.Vtx_MinimumVCharge" : 0.2,
        "TaggerVertexChargeTool.Vtx_Ptsum"      : 2.2,
        "TaggerVertexChargeTool.Vtx_Ptmean"     : 0.0,
        "TaggerVertexChargeTool.Vtx_IPSsum"     : 0., # no cut
        "TaggerVertexChargeTool.Vtx_DocaMaxsum" : 0.5,
        "TaggerVertexChargeTool.Vtx_Psum"       : 8.0,
        "TaggerVertexChargeTool.Vtx_Msum"       : 0.6,
        "TaggerVertexChargeTool.Vtx_P0_Cal"     : 0.0,
        "TaggerVertexChargeTool.Vtx_P1_Cal"     : 1.0,
        "TaggerVertexChargeTool.Vtx_Eta_Cal"    : 0.0,
        "TaggerVertexChargeTool.Vtx_ProbMin"    : 0.0,
        "TaggerVertexChargeTool.Vtx_AverageOmega" : 0.41,

        "TaggerCharmTool.Charm_P0_Cal"     : 0.0,
        "TaggerCharmTool.Charm_P1_Cal"     : 1.0,
        "TaggerCharmTool.Charm_Eta_Cal"    : 0.0,


         #// cuts_Moriond2013 trained on CloneKO, lowCorrelation, newNNPID D,N, CE tanh, Split i%3=0,1  
        "TaggerMuonTool.P0_mu_scale"        : -5.47039,
        "TaggerMuonTool.P1_mu_scale"        :  1.23885,
        "TaggerMuonTool.P2_mu_scale"        : -0.00793716,
        "TaggerMuonTool.P3_mu_scale"        :  0.647253,
  
        "TaggerElectronTool.P0_e_scale"     :  -3.04032,
        "TaggerElectronTool.P1_e_scale"     :   1.7055,
        "TaggerElectronTool.P2_e_scale"     :  -0.136785,
        "TaggerElectronTool.P3_e_scale"     :   0.646733,

        "TaggerKaonOppositeTool.P0_k_scale"     :  -5.12122,
        "TaggerKaonOppositeTool.P1_k_scale"     :   1.21427,
        "TaggerKaonOppositeTool.P2_k_scale"     :   0.088854,
        "TaggerKaonOppositeTool.P3_k_scale"     :   0.573943,

        "TaggerVertexChargeTool.P0_vtx_scale"   :-5.77134,
        "TaggerVertexChargeTool.P1_vtx_scale"   : 1.11591,
        "TaggerVertexChargeTool.P2_vtx_scale"   : 0.113682,
        "TaggerVertexChargeTool.P3_vtx_scale"   : 0.570684,

        "TaggerKaonSameTool.P0_ks_scale"        :  1.22418, 
        "TaggerKaonSameTool.P1_ks_scale"        : -1.63297,  
        "TaggerKaonSameTool.P2_ks_scale"        :  0.401361,
        "TaggerKaonSameTool.P3_ks_scale"        :  0.,

        "TaggerPionSameTool.P0_ps_scale"        :  1.22123,
        "TaggerPionSameTool.P1_ps_scale"        :  -1.76027,
        "TaggerPionSameTool.P2_ps_scale"        :  0.651766,
        "TaggerPionSameTool.P3_ps_scale"        :  0.,          

        },

    #use cuts optimized and calibrated with Strip17 (2011 data 1fb-1)
    'Reco12_2011' : {
        "Personality": "'Reco12'",  # it is then propagated to all other tools
        "IPPU_cut" : 3.0,
        "distphi_cut" : 0.005,
        #"ForceSignalID" :   , #"Bs", "Bd", "Bu" to force SSk or SSpi

        "EnableCharmTagger": "False",
        "EnableProtonTagger": "False",
        "EnablePionBDTTagger": "False",
        
        "TaggingUtils.PVReFitter":           "'AdaptivePVReFitter'",
        "TaggingUtils.LifetimeFitter":       "'PropertimeFitter'",
        "TaggingUtils.VertexFitter":         "'OfflineVertexFitter'",
        "TaggingUtils.DistanceCalculator":   "'LoKi::DistanceCalculator'",
        "TaggingUtils.ParticleDescendants":  "'ParticleDescendants'",
 
        #Following 2 lines optimize the average and combined (5 cat) performances for B2JpsiK
        #When performing your analisis, please find yourself the cut which maximeze the Eeff
        #Probably if you are using Eeff event per event no cut will be needed
        #"CombineTaggersProbability.ProbMin_OS" : 0.58,
        #"CombineTaggersProbability.ProbMin" : 0.55,
        
        "CombineTaggersProbability.P0_Cal_OS" : 0.392, #+/-0.0016
        "CombineTaggersProbability.P1_Cal_OS" : 0.953, #+/-0.017
        "CombineTaggersProbability.Eta_Cal_OS": 0.362, 

        # calibration for combination with nnetOSK  -- no calibration
        "CombineTaggersProbability.P0_Cal_OSnnet" : 0.,
        "CombineTaggersProbability.P1_Cal_OSnnet" : 1.,
        "CombineTaggersProbability.Eta_Cal_OSnnet" : 0.,

        "TaggerMuonTool.Muon_Pt_cut" : 1200,
        "TaggerMuonTool.Muon_P_cut" : 0.0,
        "TaggerMuonTool.Muon_IPs_cut" : 0.0,
        "TaggerMuonTool.Muon_lcs_cut" : 3.2,
        "TaggerMuonTool.Muon_PIDm_cut" : 2.5,
        "TaggerMuonTool.Muon_ipPU_cut" : 3.0,
        "TaggerMuonTool.Muon_distPhi_cut" : 0.005,
        "TaggerMuonTool.Muon_ProbMin" : 0.0,
        "TaggerMuonTool.Muon_AverageOmega" : 0.33,
        "TaggerMuonTool.Muon_P0_Cal" : 0.309, #+/- 0.0038
        "TaggerMuonTool.Muon_P1_Cal" : 1.1953, #+/- 0.061
        "TaggerMuonTool.Muon_Eta_Cal" : 0.304,
        "TaggerMuonTool.Muon_ghostprob_cut" : 2,  # no cut
        "TaggerMuonTool.Muon_PIDNNm_cut"    :-1.,  # no cut       
        "TaggerMuonTool.Muon_PIDNNpi_cut"   : 2.,  # no cut       
        "TaggerMuonTool.Muon_PIDNNe_cut"    : 2.,  # no cut       
        "TaggerMuonTool.Muon_PIDNNk_cut"    : 2.,  # no cut       
        "TaggerMuonTool.Muon_PIDNNp_cut"    : 2.,  # no cut
        
        "TaggerElectronTool.Ele_Pt_cut" : 1000,
        "TaggerElectronTool.Ele_P_cut" : 0.0,
        "TaggerElectronTool.Ele_IPs_cut" : 1.0,
        "TaggerElectronTool.Ele_lcs_cut" : 3.85,
        "TaggerElectronTool.Ele_ghost_cut" : -999.,
        "TaggerElectronTool.Ele_VeloChargeMin" : 0.0,
        "TaggerElectronTool.Ele_VeloChargeMax" : 1.6,
        "TaggerElectronTool.Ele_EoverP" : 0.75,
        "TaggerElectronTool.Ele_EoverPmax" :  10., # no cut
        "TaggerElectronTool.Ele_PIDe_cut" : 4.0,
        "TaggerElectronTool.Ele_ipPU_cut" : 5.0,
        "TaggerElectronTool.Ele_distPhi_cut" : 0.005,
        "TaggerElectronTool.Ele_ProbMin" : 0.0,
        "TaggerElectronTool.Ele_AverageOmega" : 0.33,
        "TaggerElectronTool.Ele_P0_Cal" : 0.306, #+/-0.0056
        "TaggerElectronTool.Ele_P1_Cal" : 0.974, #+/-0.091
        "TaggerElectronTool.Ele_Eta_Cal" : 0.346,
        "TaggerElectronTool.Ele_PIDNNe_cut" :   -1.,  # new
        "TaggerElectronTool.Ele_PIDNNk_cut" :    2.,  # new
        "TaggerElectronTool.Ele_PIDNNp_cut" :    2.,  # new
        "TaggerElectronTool.Ele_PIDNNpi_cut" :   2,   # new
        "TaggerElectronTool.Ele_PIDNNepi_cut" : -2.,  # new
        "TaggerElectronTool.Ele_ghostProb_cut" : 2.,  # new 

        "TaggerKaonOppositeTool.Kaon_Pt_cut" : 700,
        "TaggerKaonOppositeTool.Kaon_P_cut" : 5000,
        "TaggerKaonOppositeTool.Kaon_IPs_cut" : 4.3,
        "TaggerKaonOppositeTool.Kaon_IP_cut" : 1.45,
        "TaggerKaonOppositeTool.Kaon_lcs_cut" : 2.45,
        "TaggerKaonOppositeTool.Kaon_ghost_cut" : -999.,
        "TaggerKaonOppositeTool.Kaon_PIDk_cut" : 0.75,
        "TaggerKaonOppositeTool.Kaon_PIDkp_cut" : -3.,
        "TaggerKaonOppositeTool.Kaon_ipPU_cut" : 7.5,
        "TaggerKaonOppositeTool.Kaon_distPhi_cut" : 0.005,
        "TaggerKaonOppositeTool.Kaon_ProbMin" : 0.54,
        "TaggerKaonOppositeTool.Kaon_AverageOmega" : 0.33,
        "TaggerKaonOppositeTool.Kaon_P0_Cal" : 0.393, #+/-0.002
        "TaggerKaonOppositeTool.Kaon_P1_Cal" : 0.706, #+/-0.036
        "TaggerKaonOppositeTool.Kaon_Eta_Cal" : 0.354,
        "TaggerKaonOppositeTool.Kaon_PIDNNk_cut" :   -1., # no
        "TaggerKaonOppositeTool.Kaon_PIDNNe_cut" :    2., # no
        "TaggerKaonOppositeTool.Kaon_PIDNNm_cut" :   2., # no
        "TaggerKaonOppositeTool.Kaon_PIDNNpi_cut" :   2., # no
        "TaggerKaonOppositeTool.Kaon_PIDNNp_cut" :    2., # no
        "TaggerKaonOppositeTool.Kaon_PIDNNkp_cut" :  -1,  # no
        "TaggerKaonOppositeTool.Kaon_PIDNNkpi_cut" : -1,  # no
        "TaggerKaonOppositeTool.Kaon_ghostProb_cut" : 2., # no

        "TaggerNEWKaonOppositeTool.Kaon_PIDk_cut":     0.75,
        "TaggerNEWKaonOppositeTool.Kaon_PIDkp_cut":   -3.,
        "TaggerNEWKaonOppositeTool.Kaon_distPhi_cut":  0.005,
        "TaggerNEWKaonOppositeTool.Kaon_tsa_cut":   -999.0, # formerly Kaon_ghost_cut
        "TaggerNEWKaonOppositeTool.Kaon_ipPU_cut":     7.5,
        "TaggerNEWKaonOppositeTool.Kaon_ghost_cut":    0.5, # no
        "TaggerNEWKaonOppositeTool.Kaon_NN1_cut":      0.4,
        "TaggerNEWKaonOppositeTool.Kaon_ProbMin":      0.48,
        "TaggerNEWKaonOppositeTool.Kaon_P0_Cal":       0.4485, #+/- 0.0013
        "TaggerNEWKaonOppositeTool.Kaon_P1_Cal":       1.0638, #+/- 0.0273
        "TaggerNEWKaonOppositeTool.Kaon_AverageOmega": 0.4484,

        "TaggerKaonSameTool.KaonSame_Pt_cut" : 850,
        "TaggerKaonSameTool.KaonSame_P_cut" : 5250,
        "TaggerKaonSameTool.KaonSame_IP_cut" : 4.125,
        "TaggerKaonSameTool.KaonSame_lcs_cut" : 3.75,
        "TaggerKaonSameTool.KaonSame_Phi_cut" : 0.825,
        "TaggerKaonSameTool.KaonSame_Eta_cut" : 0.6,
        "TaggerKaonSameTool.KaonSame_dQ_cut" : 1850.,
        "TaggerKaonSameTool.KaonSame_PIDk_cut" : 3.5,
        "TaggerKaonSameTool.KaonSame_PIDkp_cut" : -8.5,
        "TaggerKaonSameTool.KaonSame_ipPU_cut" : 3.0,
        "TaggerKaonSameTool.KaonSame_distPhi_cut" : 0.005,
        "TaggerKaonSameTool.KaonSame_ProbMin" : 0.,
        "TaggerKaonSameTool.KaonSame_AverageOmega" : 0.33,
        "TaggerKaonSameTool.KaonSame_P0_Cal" : 0.350,
        "TaggerKaonSameTool.KaonSame_P1_Cal" : 0.51,
        "TaggerKaonSameTool.KaonSame_Eta_Cal" : 0.324,

        "TaggerNEWKaonSameTool.Kaon_PIDk_cut"   :    0.75,
        "TaggerNEWKaonSameTool.Kaon_PIDkp_cut"  :   -8.5,
        "TaggerNEWKaonSameTool.Kaon_ghost_cut"  :    100000.,
        "TaggerNEWKaonSameTool.Kaon_distPhi_cut":    0.005,
        "TaggerNEWKaonSameTool.Kaon_NN1_cut"    :    0.65,
        "TaggerNEWKaonSameTool.Kaon_P0_Cal"     :    0.4080,
        "TaggerNEWKaonSameTool.Kaon_P1_Cal"     :    0.86,
        "TaggerNEWKaonSameTool.Kaon_AverageOmega":   0.4302,

        "TaggerPionSameTool.PionSame_Pt_cut" : 600,
        "TaggerPionSameTool.PionSame_P_cut" : 4000,
        "TaggerPionSameTool.PionSame_IPs_cut" : 9.,
        "TaggerPionSameTool.PionSame_lcs_cut" : 5.0,
        "TaggerPionSameTool.PionSame_deta_max_cut" : 0.35,
        "TaggerPionSameTool.PionSame_deta_min_cut" : -0.5,
        "TaggerPionSameTool.PionSame_dphi_cut" : 0.75,
        "TaggerPionSameTool.PionSame_dR_cut" : 0.75,
        "TaggerPionSameTool.PionSame_dQ_cut" : 2500,
        "TaggerPionSameTool.PionSame_dQ_extra_cut" : 1200,
        "TaggerPionSameTool.PionSame_ghost_cut" : -999.,
        "TaggerPionSameTool.PionSame_PIDNoK_cut" : 4.3,
        "TaggerPionSameTool.PionSame_PIDNoP_cut" : 14.0,
        "TaggerPionSameTool.PionSame_ipPU_cut" : 6.0,
        "TaggerPionSameTool.PionSame_distPhi_cut" : 0.005,
        "TaggerPionSameTool.PionSame_ProbMin" : 0.56,
        "TaggerPionSameTool.PionSame_AverageOmega" : 0.41,
        "TaggerPionSameTool.PionSame_P0_Cal" : 0.4049, # final calibration for SSpi 10% of B+ background 
        "TaggerPionSameTool.PionSame_P1_Cal" : 0.83,
        "TaggerPionSameTool.PionSame_P2_Cal" : -1.99,
        "TaggerPionSameTool.PionSame_Eta_Cal" : 0.3908,  

        "TaggerVertexChargeTool.SVertexOneSeedTool.MinSeedPtmin"       : 0.1,
        "TaggerVertexChargeTool.SVertexOneSeedTool.MinSeedIPSmin"      : 2.5,
        "TaggerVertexChargeTool.SVertexOneSeedTool.MinSeedDphimin"     : 0.,
        "TaggerVertexChargeTool.SVertexOneSeedTool.ghostProb_cut"      : 2.,  # no cut
        "TaggerVertexChargeTool.Vtx_PowerK" : 0.55,
        "TaggerVertexChargeTool.Vtx_MinimumVCharge" : 0.2,
        "TaggerVertexChargeTool.Vtx_Ptsum" : 1.55,
        "TaggerVertexChargeTool.Vtx_Ptmean" : 0.,
        "TaggerVertexChargeTool.Vtx_IPSsum" : 0.,   # no cut
        "TaggerVertexChargeTool.Vtx_DocaMaxsum" : 0.5,
        "TaggerVertexChargeTool.Vtx_Psum" : 8.,
        "TaggerVertexChargeTool.Vtx_Msum" : 0.6,
        "TaggerVertexChargeTool.Vtx_ProbMin" : 0.54,
        "TaggerVertexChargeTool.Vtx_AverageOmega" : 0.41,
        "TaggerVertexChargeTool.Vtx_P0_Cal" : 0.404, #+/-0.002
        "TaggerVertexChargeTool.Vtx_P1_Cal" : 0.84, #+/-0.032
        "TaggerVertexChargeTool.Vtx_Eta_Cal" : 0.362,

        "TaggerCharmTool.Charm_P0_Cal"     : 0.0,
        "TaggerCharmTool.Charm_P1_Cal"     : 1.0,
        "TaggerCharmTool.Charm_Eta_Cal"    : 0.0,


        "TaggerMuonTool.NNetTool_MLP.P0_mu_scale"   :  -1.73872,
        "TaggerMuonTool.NNetTool_MLP.P1_mu_scale"   :  12.1473,
        "TaggerMuonTool.NNetTool_MLP.P2_mu_scale"   : -20.966,
        "TaggerMuonTool.NNetTool_MLP.P3_mu_scale"   :  10.9829,
  
        "TaggerElectronTool.NNetTool_MLP.P0_e_scale"   :  1.32902,
        "TaggerElectronTool.NNetTool_MLP.P1_e_scale"   : -2.32922,
        "TaggerElectronTool.NNetTool_MLP.P2_e_scale"   :  1.17439,
        "TaggerElectronTool.NNetTool_MLP.P3_e_scale"   :  0.,
        
        "TaggerKaonOppositeTool.NNetTool_MLP.P0_k_scale"   :   -3.66218,
        "TaggerKaonOppositeTool.NNetTool_MLP.P1_k_scale"   :   20.5552,
        "TaggerKaonOppositeTool.NNetTool_MLP.P2_k_scale"   :  -32.958,
        "TaggerKaonOppositeTool.NNetTool_MLP.P3_k_scale"   :    16.7165,

        "TaggerKaonSameTool.NNetTool_MLP.P0_ks_scale"   :  1.22418, # //dec2011_v2                 
        "TaggerKaonSameTool.NNetTool_MLP.P1_ks_scale"   : -1.63297,
        "TaggerKaonSameTool.NNetTool_MLP.P2_ks_scale"   :  0.401361,
        "TaggerKaonSameTool.NNetTool_MLP.P3_ks_scale"   :  0.,
        
        "TaggerPionSameTool.NNetTool_MLP.P0_ps_scale"   :  1.22123,
        "TaggerPionSameTool.NNetTool_MLP.P1_ps_scale"   : -1.76027,
        "TaggerPionSameTool.NNetTool_MLP.P2_ps_scale"   :  0.651766,
        "TaggerPionSameTool.NNetTool_MLP.P3_ps_scale"   :  0.0,
        
        "TaggerVertexChargeTool.NNetTool_MLP.P0_vtx_scale":   0.945773,
        "TaggerVertexChargeTool.NNetTool_MLP.P1_vtx_scale":  -0.93411,
        "TaggerVertexChargeTool.NNetTool_MLP.P2_vtx_scale":   0.,
        "TaggerVertexChargeTool.NNetTool_MLP.P3_vtx_scale":   0.,          

        },

    #use cuts optimized and calibrated with Strip17 (2011 data 1fb-1)
    'Reco12_MC11' : {
        "Personality": "'Reco12'",  # it is then propagated to all other tools
        "IPPU_cut" : 3.0,
        "distphi_cut" : 0.005,
        #"ForceSignalID" :   , #"Bs", "Bd", "Bu" to force SSk or SSpi

        "EnableCharmTagger": "False",
        "EnableProtonTagger": "False",
        "EnablePionBDTTagger": "False",
        
        "TaggingUtils.PVReFitter":           "'AdaptivePVReFitter'",
        "TaggingUtils.LifetimeFitter":       "'PropertimeFitter'",
        "TaggingUtils.VertexFitter":         "'OfflineVertexFitter'",
        "TaggingUtils.DistanceCalculator":   "'LoKi::DistanceCalculator'",
        "TaggingUtils.ParticleDescendants":  "'ParticleDescendants'",
 
        
        #Following 2 lines optimize the average and combined (5 cat) performances for B2JpsiK
        #When performing your analisis, please find yourself the cut which maximeze the Eeff
        #Probably if you are using Eeff event per event no cut will be needed
        #"CombineTaggersProbability.ProbMin_OS" : 0.58,
        #"CombineTaggersProbability.ProbMin" : 0.55,

        "CombineTaggersProbability.P0_Cal_OS" : 0.392, #+/-0.0016
        "CombineTaggersProbability.P1_Cal_OS" : 0.953, #+/-0.017
        "CombineTaggersProbability.Eta_Cal_OS": 0.362, 
        # calibration for combination with nnetOSK  -- no calibration
        "CombineTaggersProbability.P0_Cal_OSnnet" : 0.,
        "CombineTaggersProbability.P1_Cal_OSnnet" : 1.,
        "CombineTaggersProbability.Eta_Cal_OSnnet" : 0.,

        "TaggerMuonTool.Muon_Pt_cut" : 1200,
        "TaggerMuonTool.Muon_P_cut" : 0.0,
        "TaggerMuonTool.Muon_IPs_cut" : 0.0,
        "TaggerMuonTool.Muon_lcs_cut" : 3.2,
        "TaggerMuonTool.Muon_PIDm_cut" : 2.5,
        "TaggerMuonTool.Muon_ipPU_cut" : 3.0,
        "TaggerMuonTool.Muon_distPhi_cut" : 0.005,
        "TaggerMuonTool.Muon_ProbMin" : 0.0,
        "TaggerMuonTool.Muon_AverageOmega" : 0.33,
        "TaggerMuonTool.Muon_P0_Cal" : 0., 
        "TaggerMuonTool.Muon_P1_Cal" : 1., #+/- 0.061
        "TaggerMuonTool.Muon_Eta_Cal" : 0.,
        "TaggerMuonTool.Muon_ghostprob_cut" : 2,  # no cut
        "TaggerMuonTool.Muon_PIDNNm_cut"    :-1.,  # no cut       
        "TaggerMuonTool.Muon_PIDNNpi_cut"   : 2.,  # no cut       
        "TaggerMuonTool.Muon_PIDNNe_cut"    : 2.,  # no cut       
        "TaggerMuonTool.Muon_PIDNNk_cut"    : 2.,  # no cut       
        "TaggerMuonTool.Muon_PIDNNp_cut"    : 2.,  # no cut
        
        "TaggerElectronTool.Ele_Pt_cut" : 1000,
        "TaggerElectronTool.Ele_P_cut" : 0.0,
        "TaggerElectronTool.Ele_IPs_cut" : 1.0,
        "TaggerElectronTool.Ele_lcs_cut" : 3.85,
        "TaggerElectronTool.Ele_ghost_cut" : -999.,
        "TaggerElectronTool.Ele_VeloChargeMin" : 0.0,
        "TaggerElectronTool.Ele_VeloChargeMax" : 1.6,
        "TaggerElectronTool.Ele_EoverP" : 0.75,
        "TaggerElectronTool.Ele_EoverPmax" :  10., # no cut
        "TaggerElectronTool.Ele_PIDe_cut" : 4.0,
        "TaggerElectronTool.Ele_ipPU_cut" : 5.0,
        "TaggerElectronTool.Ele_distPhi_cut" : 0.005,
        "TaggerElectronTool.Ele_ProbMin" : 0.0,
        "TaggerElectronTool.Ele_AverageOmega" : 0.33,
        "TaggerElectronTool.Ele_P0_Cal" : 0., #+/-0.0056
        "TaggerElectronTool.Ele_P1_Cal" : 1.,
        "TaggerElectronTool.Ele_Eta_Cal" : 0.,
        "TaggerElectronTool.Ele_PIDNNe_cut" :   -1.,  # new
        "TaggerElectronTool.Ele_PIDNNk_cut" :    2.,  # new
        "TaggerElectronTool.Ele_PIDNNp_cut" :    2.,  # new
        "TaggerElectronTool.Ele_PIDNNpi_cut" :   2,   # new
        "TaggerElectronTool.Ele_PIDNNepi_cut" : -2.,  # new
        "TaggerElectronTool.Ele_ghostProb_cut" : 2.,  # new 

        "TaggerKaonOppositeTool.Kaon_Pt_cut" : 700,
        "TaggerKaonOppositeTool.Kaon_P_cut" : 5000,
        "TaggerKaonOppositeTool.Kaon_IPs_cut" : 4.3,
        "TaggerKaonOppositeTool.Kaon_IP_cut" : 1.45,
        "TaggerKaonOppositeTool.Kaon_lcs_cut" : 2.45,
        "TaggerKaonOppositeTool.Kaon_ghost_cut" : -999.,
        "TaggerKaonOppositeTool.Kaon_PIDk_cut" : 0.75,
        "TaggerKaonOppositeTool.Kaon_PIDkp_cut" : -3.,
        "TaggerKaonOppositeTool.Kaon_ipPU_cut" : 7.5,
        "TaggerKaonOppositeTool.Kaon_distPhi_cut" : 0.005,
        "TaggerKaonOppositeTool.Kaon_ProbMin" : 0.54,
        "TaggerKaonOppositeTool.Kaon_AverageOmega" : 0.33,
        "TaggerKaonOppositeTool.Kaon_P0_Cal" : 0., 
        "TaggerKaonOppositeTool.Kaon_P1_Cal" : 1.,
        "TaggerKaonOppositeTool.Kaon_Eta_Cal" : 0.,
        "TaggerKaonOppositeTool.Kaon_PIDNNk_cut" :   -1., # no
        "TaggerKaonOppositeTool.Kaon_PIDNNe_cut" :    2., # no
        "TaggerKaonOppositeTool.Kaon_PIDNNm_cut" :   2., # no
        "TaggerKaonOppositeTool.Kaon_PIDNNpi_cut" :   2., # no
        "TaggerKaonOppositeTool.Kaon_PIDNNp_cut" :    2., # no
        "TaggerKaonOppositeTool.Kaon_PIDNNkp_cut" :  -1,  # no
        "TaggerKaonOppositeTool.Kaon_PIDNNkpi_cut" : -1,  # no
        "TaggerKaonOppositeTool.Kaon_ghostProb_cut" : 2., # no

        "TaggerNEWKaonOppositeTool.Kaon_PIDk_cut":     0.75,
        "TaggerNEWKaonOppositeTool.Kaon_PIDkp_cut":   -3.,
        "TaggerNEWKaonOppositeTool.Kaon_distPhi_cut":  0.005,
        "TaggerNEWKaonOppositeTool.Kaon_tsa_cut":   -999.0, # formerly Kaon_ghost_cut
        "TaggerNEWKaonOppositeTool.Kaon_ipPU_cut":     7.5,
        "TaggerNEWKaonOppositeTool.Kaon_ghost_cut":    0.5, # no
        "TaggerNEWKaonOppositeTool.Kaon_NN1_cut":      0.4,
        "TaggerNEWKaonOppositeTool.Kaon_ProbMin":      0.48,
        "TaggerNEWKaonOppositeTool.Kaon_P0_Cal":       0.4485, #+/- 0.0013
        "TaggerNEWKaonOppositeTool.Kaon_P1_Cal":       1.0638, #+/- 0.0273
        "TaggerNEWKaonOppositeTool.Kaon_AverageOmega": 0.4484,

        "TaggerKaonSameTool.KaonSame_Pt_cut" : 850,
        "TaggerKaonSameTool.KaonSame_P_cut" : 5250,
        "TaggerKaonSameTool.KaonSame_IP_cut" : 4.125,
        "TaggerKaonSameTool.KaonSame_lcs_cut" : 3.75,
        "TaggerKaonSameTool.KaonSame_Phi_cut" : 0.825,
        "TaggerKaonSameTool.KaonSame_Eta_cut" : 0.6,
        "TaggerKaonSameTool.KaonSame_dQ_cut" : 1850.,
        "TaggerKaonSameTool.KaonSame_PIDk_cut" : 3.5,
        "TaggerKaonSameTool.KaonSame_PIDkp_cut" : -8.5,
        "TaggerKaonSameTool.KaonSame_ipPU_cut" : 3.0,
        "TaggerKaonSameTool.KaonSame_distPhi_cut" : 0.005,
        "TaggerKaonSameTool.KaonSame_ProbMin" : 0.,
        "TaggerKaonSameTool.KaonSame_AverageOmega" : 0.33,
        "TaggerKaonSameTool.KaonSame_P0_Cal" : 0.,
        "TaggerKaonSameTool.KaonSame_P1_Cal" : 1.,
        "TaggerKaonSameTool.KaonSame_Eta_Cal" : 0.,

        "TaggerNEWKaonSameTool.Kaon_PIDk_cut"   :    0.75,
        "TaggerNEWKaonSameTool.Kaon_PIDkp_cut"  :   -8.5,
        "TaggerNEWKaonSameTool.Kaon_ghost_cut"  :    100000.,
        "TaggerNEWKaonSameTool.Kaon_distPhi_cut":    0.005,
        "TaggerNEWKaonSameTool.Kaon_NN1_cut"    :    0.65,
        "TaggerNEWKaonSameTool.Kaon_P0_Cal"     :    0.4080,
        "TaggerNEWKaonSameTool.Kaon_P1_Cal"     :    0.86,
        "TaggerNEWKaonSameTool.Kaon_AverageOmega":   0.4302,

        "TaggerPionSameTool.PionSame_Pt_cut" : 600,
        "TaggerPionSameTool.PionSame_P_cut" : 4000,
        "TaggerPionSameTool.PionSame_IPs_cut" : 9.,
        "TaggerPionSameTool.PionSame_lcs_cut" : 5.0,
        "TaggerPionSameTool.PionSame_deta_max_cut" : 0.35,
        "TaggerPionSameTool.PionSame_deta_min_cut" : -0.5,
        "TaggerPionSameTool.PionSame_dphi_cut" : 0.75,
        "TaggerPionSameTool.PionSame_dR_cut" : 0.75,
        "TaggerPionSameTool.PionSame_dQ_cut" : 2500,
        "TaggerPionSameTool.PionSame_dQ_extra_cut" : 1200,
        "TaggerPionSameTool.PionSame_ghost_cut" : -999.,
        "TaggerPionSameTool.PionSame_PIDNoK_cut" : 4.3,
        "TaggerPionSameTool.PionSame_PIDNoP_cut" : 14.0,
        "TaggerPionSameTool.PionSame_ipPU_cut" : 6.0,
        "TaggerPionSameTool.PionSame_distPhi_cut" : 0.005,
        "TaggerPionSameTool.PionSame_ProbMin" : 0.56,
        "TaggerPionSameTool.PionSame_AverageOmega" : 0.41,
        "TaggerPionSameTool.PionSame_P0_Cal" : 0., 
        "TaggerPionSameTool.PionSame_P1_Cal" : 1.,
        "TaggerPionSameTool.PionSame_P2_Cal" : 0.,
        "TaggerPionSameTool.PionSame_Eta_Cal" : 0.,  

        "TaggerVertexChargeTool.SVertexOneSeedTool.MinSeedProbability" : 0.42,
        "TaggerVertexChargeTool.SVertexOneSeedTool.MinSeedPtmin"       : 0.1,
        "TaggerVertexChargeTool.SVertexOneSeedTool.MinSeedIPSmin"      : 2.5,
        "TaggerVertexChargeTool.SVertexOneSeedTool.MinSeedDphimin"     : 0.,
        "TaggerVertexChargeTool.SVertexOneSeedTool.ghostProb_cut"      : 2.,  # no cut
        "TaggerVertexChargeTool.Vtx_PowerK" : 0.55,
        "TaggerVertexChargeTool.Vtx_MinimumVCharge" : 0.2,
        "TaggerVertexChargeTool.Vtx_Ptsum" : 1.55,
        "TaggerVertexChargeTool.Vtx_Ptmean" : 0.,
        "TaggerVertexChargeTool.Vtx_IPSsum" : 0.,   # no cut
        "TaggerVertexChargeTool.Vtx_DocaMaxsum" : 0.5,
        "TaggerVertexChargeTool.Vtx_Psum" : 8.,
        "TaggerVertexChargeTool.Vtx_Msum" : 0.6,
        "TaggerVertexChargeTool.Vtx_ProbMin" : 0.54,
        "TaggerVertexChargeTool.Vtx_AverageOmega" : 0.41,
        "TaggerVertexChargeTool.Vtx_P0_Cal" : 0.,
        "TaggerVertexChargeTool.Vtx_P1_Cal" : 1.,
        "TaggerVertexChargeTool.Vtx_Eta_Cal" : 0.,

        "TaggerCharmTool.Charm_P0_Cal"     : 0.0,
        "TaggerCharmTool.Charm_P1_Cal"     : 1.0,
        "TaggerCharmTool.Charm_Eta_Cal"    : 0.0,


        "TaggerMuonTool.NNetTool_MLP.P0_mu_scale"   :  -1.73872,
        "TaggerMuonTool.NNetTool_MLP.P1_mu_scale"   :  12.1473,
        "TaggerMuonTool.NNetTool_MLP.P2_mu_scale"   : -20.966,
        "TaggerMuonTool.NNetTool_MLP.P3_mu_scale"   :  10.9829,
  
        "TaggerElectronTool.NNetTool_MLP.P0_e_scale"   :  1.32902,
        "TaggerElectronTool.NNetTool_MLP.P1_e_scale"   : -2.32922,
        "TaggerElectronTool.NNetTool_MLP.P2_e_scale"   :  1.17439,
        "TaggerElectronTool.NNetTool_MLP.P3_e_scale"   :  0.,
        
        "TaggerKaonOppositeTool.NNetTool_MLP.P0_k_scale"   :   -3.66218,
        "TaggerKaonOppositeTool.NNetTool_MLP.P1_k_scale"   :   20.5552,
        "TaggerKaonOppositeTool.NNetTool_MLP.P2_k_scale"   :  -32.958,
        "TaggerKaonOppositeTool.NNetTool_MLP.P3_k_scale"   :    16.7165,

        "TaggerKaonSameTool.NNetTool_MLP.P0_ks_scale"   :  1.22418, # //dec2011_v2                 
        "TaggerKaonSameTool.NNetTool_MLP.P1_ks_scale"   : -1.63297,
        "TaggerKaonSameTool.NNetTool_MLP.P2_ks_scale"   :  0.401361,
        "TaggerKaonSameTool.NNetTool_MLP.P3_ks_scale"   :  0.,
        
        "TaggerPionSameTool.NNetTool_MLP.P0_ps_scale"   :  1.22123,
        "TaggerPionSameTool.NNetTool_MLP.P1_ps_scale"   : -1.76027,
        "TaggerPionSameTool.NNetTool_MLP.P2_ps_scale"   :  0.651766,
        "TaggerPionSameTool.NNetTool_MLP.P3_ps_scale"   :  0.0,
        
        "TaggerVertexChargeTool.NNetTool_MLP.P0_vtx_scale":   0.945773,
        "TaggerVertexChargeTool.NNetTool_MLP.P1_vtx_scale":  -0.93411,
        "TaggerVertexChargeTool.NNetTool_MLP.P2_vtx_scale":   0.,
        "TaggerVertexChargeTool.NNetTool_MLP.P3_vtx_scale":   0.,          

        },

    'Xmass_2011' : { 
        "IPPU_cut" : 3.0,
        "distphi_cut" : 0.005,
        #"ForceSignalID" :   , #"Bs", "Bd", "Bu" to force SSk or SSpi

        #Following 2 lines optimize the average and combined (5 cat) performances for B2JpsiK
        #When performing your analisis, please find yourself the cut which maximeze the Eeff
        #Probably if you are using Eeff event per event no cut will be needed
        #"CombineTaggersProbability.ProbMin_OS" : 0.58,
        #"CombineTaggersProbability.ProbMin" : 0.55,
        
        "CombineTaggersProbability.P0_Cal_OS" : 0.392, #+/-0.0016
        "CombineTaggersProbability.P1_Cal_OS" : 0.921, #+/-0.017
        "CombineTaggersProbability.Eta_Cal_OS" : 0.363,

        "TaggerMuonTool.Muon_Pt_cut" : 1200,
        "TaggerMuonTool.Muon_P_cut" : 0.0,
        "TaggerMuonTool.Muon_IPs_cut" : 0.0,
        "TaggerMuonTool.Muon_lcs_cut" : 3.2,
        "TaggerMuonTool.Muon_PIDm_cut" : 2.5,
        "TaggerMuonTool.Muon_ipPU_cut" : 3.0,
        "TaggerMuonTool.Muon_distPhi_cut" : 0.005,
        "TaggerMuonTool.Muon_ProbMin" : 0.0,
        "TaggerMuonTool.Muon_AverageOmega" : 0.33,
        "TaggerMuonTool.Muon_P0_Cal" : 0.309, #+/- 0.0038
        "TaggerMuonTool.Muon_P1_Cal" : 1.1953, #+/- 0.061
        "TaggerMuonTool.Muon_Eta_Cal" : 0.304,

        "TaggerElectronTool.Ele_Pt_cut" : 1000,
        "TaggerElectronTool.Ele_P_cut" : 0.0,
        "TaggerElectronTool.Ele_IPs_cut" : 1.0,
        "TaggerElectronTool.Ele_lcs_cut" : 3.85,
        "TaggerElectronTool.Ele_ghost_cut" : -999.,
        "TaggerElectronTool.Ele_VeloChargeMin" : 0.0,
        "TaggerElectronTool.Ele_VeloChargeMax" : 1.6,
        "TaggerElectronTool.Ele_EoverP" : 0.75,
        "TaggerElectronTool.Ele_PIDe_cut" : 4.0,
        "TaggerElectronTool.Ele_ipPU_cut" : 5.0,
        "TaggerElectronTool.Ele_distPhi_cut" : 0.005,
        "TaggerElectronTool.Ele_ProbMin" : 0.0,
        "TaggerElectronTool.Ele_AverageOmega" : 0.33,
        "TaggerElectronTool.Ele_P0_Cal" : 0.306, #+/-0.0056
        "TaggerElectronTool.Ele_P1_Cal" : 0.974, #+/-0.091
        "TaggerElectronTool.Ele_Eta_Cal" : 0.346,

        "TaggerKaonOppositeTool.Kaon_Pt_cut" : 700,
        "TaggerKaonOppositeTool.Kaon_P_cut" : 5000,
        "TaggerKaonOppositeTool.Kaon_IPs_cut" : 4.3,
        "TaggerKaonOppositeTool.Kaon_IP_cut" : 1.45,
        "TaggerKaonOppositeTool.Kaon_lcs_cut" : 2.45,
        "TaggerKaonOppositeTool.Kaon_ghost_cut" : -999.,
        "TaggerKaonOppositeTool.Kaon_PIDk_cut" : 0.75,
        "TaggerKaonOppositeTool.Kaon_PIDkp_cut" : -3.,
        "TaggerKaonOppositeTool.Kaon_ipPU_cut" : 7.5,
        "TaggerKaonOppositeTool.Kaon_distPhi_cut" : 0.005,
        "TaggerKaonOppositeTool.Kaon_ProbMin" : 0.54,
        "TaggerKaonOppositeTool.Kaon_AverageOmega" : 0.33,
        "TaggerKaonOppositeTool.Kaon_P0_Cal" : 0.393, #+/-0.002
        "TaggerKaonOppositeTool.Kaon_P1_Cal" : 0.706, #+/-0.036
        "TaggerKaonOppositeTool.Kaon_Eta_Cal" : 0.354,

        "TaggerKaonSameTool.KaonSame_Pt_cut" : 850,
        "TaggerKaonSameTool.KaonSame_P_cut" : 5250,
        "TaggerKaonSameTool.KaonSame_IP_cut" : 4.125,
        "TaggerKaonSameTool.KaonSame_lcs_cut" : 3.75,
        "TaggerKaonSameTool.KaonSame_Phi_cut" : 0.825,
        "TaggerKaonSameTool.KaonSame_Eta_cut" : 0.6,
        "TaggerKaonSameTool.KaonSame_dQ_cut" : 1850.,
        "TaggerKaonSameTool.KaonSame_PIDk_cut" : 3.5,
        "TaggerKaonSameTool.KaonSame_PIDkp_cut" : -8.5,
        "TaggerKaonSameTool.KaonSame_ipPU_cut" : 3.0,
        "TaggerKaonSameTool.KaonSame_distPhi_cut" : 0.005,
        "TaggerKaonSameTool.KaonSame_ProbMin" : 0.,
        "TaggerKaonSameTool.KaonSame_AverageOmega" : 0.33,
        "TaggerKaonSameTool.KaonSame_P0_Cal" : 0.,
        "TaggerKaonSameTool.KaonSame_P1_Cal" : 1.,
        "TaggerKaonSameTool.KaonSame_Eta_Cal" : 0.,

        "TaggerPionSameTool.PionSame_Pt_cut" : 600,
        "TaggerPionSameTool.PionSame_P_cut" : 4000,
        "TaggerPionSameTool.PionSame_IPs_cut" : 9.,
        "TaggerPionSameTool.PionSame_lcs_cut" : 5.0,
        "TaggerPionSameTool.PionSame_deta_max_cut" : 0.35,
        "TaggerPionSameTool.PionSame_deta_min_cut" : -0.5,
        "TaggerPionSameTool.PionSame_dphi_cut" : 0.75,
        "TaggerPionSameTool.PionSame_dR_cut" : 0.75,
        "TaggerPionSameTool.PionSame_dQ_cut" : 2500,
        "TaggerPionSameTool.PionSame_dQ_extra_cut" : 1200,
        "TaggerPionSameTool.PionSame_ghost_cut" : -999.,
        "TaggerPionSameTool.PionSame_PIDNoK_cut" : 4.3,
        "TaggerPionSameTool.PionSame_PIDNoP_cut" : 14.0,
        "TaggerPionSameTool.PionSame_ipPU_cut" : 6.0,
        "TaggerPionSameTool.PionSame_distPhi_cut" : 0.005,
        "TaggerPionSameTool.PionSame_ProbMin" : 0.56,
        "TaggerPionSameTool.PionSame_AverageOmega" : 0.41,
        "TaggerPionSameTool.PionSame_P0_Cal" : 0.,
        "TaggerPionSameTool.PionSame_P1_Cal" : 1.,
        "TaggerPionSameTool.PionSame_P2_Cal" : 0.,
        "TaggerPionSameTool.PionSame_Eta_Cal" : 0.,

        "TaggerVertexChargeTool.SVertexOneSeedTool.MinSeedProbability" : 0.42,
        "TaggerVertexChargeTool.Vtx_PowerK" : 0.55,
        "TaggerVertexChargeTool.Vtx_MinimumVCharge" : 0.2,
        "TaggerVertexChargeTool.Vtx_Ptsum" : 1.55,
        "TaggerVertexChargeTool.Vtx_Ptmean" : 0.,
        "TaggerVertexChargeTool.Vtx_IPSsum" : 0., # no cut
        "TaggerVertexChargeTool.Vtx_DocaMaxsum" : 0.5,
        "TaggerVertexChargeTool.Vtx_Psum" : 8.,
        "TaggerVertexChargeTool.Vtx_Msum" : 0.6,
        "TaggerVertexChargeTool.Vtx_ProbMin" : 0.54,
        "TaggerVertexChargeTool.Vtx_AverageOmega" : 0.41,
        "TaggerVertexChargeTool.Vtx_P0_Cal" : 0.404, #+/-0.002
        "TaggerVertexChargeTool.Vtx_P1_Cal" : 0.84, #+/-0.032
        "TaggerVertexChargeTool.Vtx_Eta_Cal" : 0.362
        },
    #use cuts optimized and calibrated with Strip20 (2012 data 2fb-1), nnet and polynomials tuned on MC12 
    'Stripping21_MC' : { 
        "Personality": "'Reco14'",  # it is then propagated to all other tools
        "IPPU_cut" : 3.0,
        "distphi_cut" : 0.005,

        #"ForceSignalID" :   , #"Bs", "Bd", "Bu" to force SSk or SSpi

        "EnableCharmTagger": "True", # was disabled in the initial Stripping21
        "EnableProtonTagger": "True",
        "EnablePionBDTTagger": "True",
        #Following 2 lines optimize the average and combined (5 cat) performances for B2JpsiK
        #When performing your analisis, please find yourself the cut which maximeze the Eeff
        #Probably if you are using Eeff event per event no cut will be needed
        #"CombineTaggersProbability.ProbMin_OS" : 0.58,
        #"CombineTaggersProbability.ProbMin" : 0.55,
 
        "CombineTaggersProbability.P0_Cal_OS" :  0.4004,
        "CombineTaggersProbability.P1_Cal_OS" : 0.8476,
        "CombineTaggersProbability.Eta_Cal_OS" : 0.3892,
        # calibration for combination with nnetOSK  -- no calibration
        "CombineTaggersProbability.P0_Cal_OSnnet" : 0.,
        "CombineTaggersProbability.P1_Cal_OSnnet" : 1.,
        "CombineTaggersProbability.Eta_Cal_OSnnet" : 0.,

        "TaggerMuonTool.Muon_Pt_cut" :     1100.0,
        "TaggerMuonTool.Muon_P_cut" :         0.0,
        "TaggerMuonTool.Muon_lcs_cut" :       3.0,
        "TaggerMuonTool.Muon_IPs_cut" :       0.0,
        "TaggerMuonTool.Muon_PIDm_cut" :   -100.0,  # no cut
        "TaggerMuonTool.Muon_ipPU_cut" :      3.0,
        "TaggerMuonTool.Muon_distPhi_cut"   : 0.005,        
        "TaggerMuonTool.Muon_ghostprob_cut" : 0.4,  # new cut
        "TaggerMuonTool.Muon_PIDNNm_cut"    : 0.35,  # new cut       
        "TaggerMuonTool.Muon_PIDNNpi_cut"   : 0.8,  # new cut       
        "TaggerMuonTool.Muon_PIDNNe_cut"    : 0.8,  # new cut       
        "TaggerMuonTool.Muon_PIDNNk_cut"    : 0.8,  # new cut       
        "TaggerMuonTool.Muon_PIDNNp_cut"    : 0.8,  # new cut       
        "TaggerMuonTool.Muon_P0_Cal" :       0.,
        "TaggerMuonTool.Muon_P1_Cal" :       1., 
        "TaggerMuonTool.Muon_Eta_Cal" :      0.,
        "TaggerMuonTool.Muon_ProbMin" :       0.0,
        "TaggerMuonTool.Muon_AverageOmega" :  0.33,
        "TaggerMuonTool.isMonteCarlo" :       1,
        
        "TaggerElectronTool.Ele_Pt_cut" :       1100,
        "TaggerElectronTool.Ele_P_cut" :         0.0,
        "TaggerElectronTool.Ele_lcs_cut" :       3.,
        "TaggerElectronTool.Ele_IPs_cut" :       3.5,
        "TaggerElectronTool.Ele_ghost_cut" :   -999.,
        "TaggerElectronTool.Ele_VeloChargeMin" : 0.0,
        "TaggerElectronTool.Ele_VeloChargeMax" : 1.4,
        "TaggerElectronTool.Ele_EoverP" :       0.85,
        "TaggerElectronTool.Ele_EoverPmax" :      2.,
        "TaggerElectronTool.Ele_PIDe_cut" :     -1.0,
        "TaggerElectronTool.Ele_ipPU_cut" :      4.0,
        "TaggerElectronTool.Ele_distPhi_cut" : 0.005,
        "TaggerElectronTool.Ele_PIDNNe_cut" :    0.1,  # new
        "TaggerElectronTool.Ele_PIDNNk_cut" :    0.8,  # new
        "TaggerElectronTool.Ele_PIDNNp_cut" :    0.8,  # new
        "TaggerElectronTool.Ele_PIDNNpi_cut" :   0.8,  # new
        "TaggerElectronTool.Ele_PIDNNepi_cut" : -0.8,  # new
        "TaggerElectronTool.Ele_ghostProb_cut" : 0.4,  # new       
        "TaggerElectronTool.Ele_P0_Cal" :         0.,
        "TaggerElectronTool.Ele_P1_Cal" :         1., 
        "TaggerElectronTool.Ele_Eta_Cal" :        0.,
        "TaggerElectronTool.isMonteCarlo" :     1,
        
        "TaggerKaonOppositeTool.Kaon_Pt_cut" :   700,
        "TaggerKaonOppositeTool.Kaon_P_cut" :   2000,
        "TaggerKaonOppositeTool.Kaon_lcs_cut" :  3,
        "TaggerKaonOppositeTool.Kaon_IPs_cut" : 4.,
        "TaggerKaonOppositeTool.Kaon_IP_cut" :   1.6,
        "TaggerKaonOppositeTool.Kaon_PIDk_cut" :    -100.,
        "TaggerKaonOppositeTool.Kaon_PIDkp_cut" :   -300.,
        "TaggerKaonOppositeTool.Kaon_ghost_cut" :   -999.,
        "TaggerKaonOppositeTool.Kaon_ipPU_cut" :      6.0,
        "TaggerKaonOppositeTool.Kaon_distPhi_cut" : 0.005,
        "TaggerKaonOppositeTool.Kaon_PIDNNk_cut" :    0.25, # new
        "TaggerKaonOppositeTool.Kaon_PIDNNe_cut" :   0.8, #new
        "TaggerKaonOppositeTool.Kaon_PIDNNm_cut" :   0.8, #new
        "TaggerKaonOppositeTool.Kaon_PIDNNpi_cut" :   0.8, #new
        "TaggerKaonOppositeTool.Kaon_PIDNNp_cut" :    0.8, #new
        "TaggerKaonOppositeTool.Kaon_PIDNNkp_cut" :   0.0, # new
        "TaggerKaonOppositeTool.Kaon_PIDNNkpi_cut" : -0.6, # new
        "TaggerKaonOppositeTool.Kaon_ghostProb_cut" :0.35, # new
        "TaggerKaonOppositeTool.Kaon_P0_Cal" :        0.0,
        "TaggerKaonOppositeTool.Kaon_P1_Cal" :        1.0,
        "TaggerKaonOppositeTool.Kaon_Eta_Cal" :       0.0,
        "TaggerKaonOppositeTool.Kaon_ProbMin" :       0.0,
        "TaggerKaonOppositeTool.Kaon_AverageOmega" : 0.33,
        "TaggerKaonOppositeTool.isMonteCarlo"      : 1,

        "TaggerNEWKaonOppositeTool.Kaon_PIDk_cut":     0.75,
        "TaggerNEWKaonOppositeTool.Kaon_PIDkp_cut":   -3.,
        "TaggerNEWKaonOppositeTool.Kaon_distPhi_cut":  0.005,
        "TaggerNEWKaonOppositeTool.Kaon_tsa_cut":   -999.0, # formerly Kaon_ghost_cut
        "TaggerNEWKaonOppositeTool.Kaon_ipPU_cut":     3.0,
        "TaggerNEWKaonOppositeTool.Kaon_ghost_cut":    0.5,
        "TaggerNEWKaonOppositeTool.Kaon_NN1_cut":      0.6,
        "TaggerNEWKaonOppositeTool.Kaon_ProbMin":      0.5,
        "TaggerNEWKaonOppositeTool.Kaon_P0_Cal":       0.4072, #+/- 0.0015
        "TaggerNEWKaonOppositeTool.Kaon_P1_Cal":       1.2940, #+/- 0.0250
        "TaggerNEWKaonOppositeTool.Kaon_AverageOmega": 0.4245,

        "TaggerKaonSameTool.KaonSame_Pt_cut" : 850,
        "TaggerKaonSameTool.KaonSame_P_cut"  : 5250,
        "TaggerKaonSameTool.KaonSame_IP_cut" : 4.125,
        "TaggerKaonSameTool.KaonSame_lcs_cut" : 3.75,
        "TaggerKaonSameTool.KaonSame_Phi_cut" : 0.825,
        "TaggerKaonSameTool.KaonSame_Eta_cut" : 0.6,
        "TaggerKaonSameTool.KaonSame_dQ_cut" : 1850.,
        "TaggerKaonSameTool.KaonSame_PIDk_cut" : 3.5,
        "TaggerKaonSameTool.KaonSame_PIDkp_cut" : -8.5,
        "TaggerKaonSameTool.KaonSame_ipPU_cut" : 3.0,
        "TaggerKaonSameTool.KaonSame_distPhi_cut" : 0.005,
        "TaggerKaonSameTool.KaonSame_ProbMin"  : 0.,
        "TaggerKaonSameTool.KaonSame_AverageOmega" : 0.33,
        "TaggerKaonSameTool.KaonSame_P0_Cal"   : 0.,
        "TaggerKaonSameTool.KaonSame_P1_Cal"   : 1.,
        "TaggerKaonSameTool.KaonSame_Eta_Cal"  : 0.,
        "TaggerKaonSameTool.isMonteCarlo"      : 1,
        
        # Same tuning as Reco12_2011, different calibration 
        "TaggerNEWKaonSameTool.Kaon_PIDk_cut"   :    0.75,
        "TaggerNEWKaonSameTool.Kaon_PIDkp_cut"  :   -8.5,
        "TaggerNEWKaonSameTool.Kaon_ghost_cut"  :    1000,
        "TaggerNEWKaonSameTool.Kaon_distPhi_cut":    0.005,
        "TaggerNEWKaonSameTool.Kaon_NN1_cut"    :    0.65,
        "TaggerNEWKaonSameTool.Kaon_P0_Cal"     :    0.429,
        "TaggerNEWKaonSameTool.Kaon_P1_Cal"     :    1.11,
        "TaggerNEWKaonSameTool.Kaon_AverageOmega":   0.433,

        "TaggerPionSameTool.PionSame_Pt_cut"  : 600,
        "TaggerPionSameTool.PionSame_P_cut"   : 4000,
        "TaggerPionSameTool.PionSame_IPs_cut" : 9.,
        "TaggerPionSameTool.PionSame_lcs_cut" : 5.0,
        "TaggerPionSameTool.PionSame_deta_max_cut" : 0.35,
        "TaggerPionSameTool.PionSame_deta_min_cut" : -0.5,
        "TaggerPionSameTool.PionSame_dphi_cut": 0.75,
        "TaggerPionSameTool.PionSame_dR_cut"  : 0.75,
        "TaggerPionSameTool.PionSame_dQ_cut"  : 2500,
        "TaggerPionSameTool.PionSame_dQ_extra_cut" : 1200,
        "TaggerPionSameTool.PionSame_ghost_cut" : -999.,
        "TaggerPionSameTool.PionSame_PIDNoK_cut" : 4.3,
        "TaggerPionSameTool.PionSame_PIDNoP_cut" : 14.0,
        "TaggerPionSameTool.PionSame_ipPU_cut" : 6.0,
        "TaggerPionSameTool.PionSame_distPhi_cut" : 0.005,
        "TaggerPionSameTool.PionSame_ProbMin"   : 0.56,
        "TaggerPionSameTool.PionSame_AverageOmega" : 0.41,
        "TaggerPionSameTool.PionSame_P0_Cal"    : 0.,
        "TaggerPionSameTool.PionSame_P1_Cal"    : 1.,
        "TaggerPionSameTool.PionSame_P2_Cal"    : 0.,
        "TaggerPionSameTool.PionSame_Eta_Cal"   : 0.,
        "TaggerPionSameTool.isMonteCarlo"       : 1,

        "TaggerVertexChargeTool.SVertexOneSeedTool.MinSeedProbability" : 0.42,
        "TaggerVertexChargeTool.SVertexOneSeedTool.MinSeedPtmin"       : 0.1,
        "TaggerVertexChargeTool.SVertexOneSeedTool.MinSeedIPSmin"      : 2.5,
        "TaggerVertexChargeTool.SVertexOneSeedTool.MinSeedDphimin"     : 0.,
        #"TaggerVertexChargeTool.SVertexOneSeedTool.MinSeedDphimax"     : 5,
        "TaggerVertexChargeTool.SVertexOneSeedTool.ghostProb_cut"      : 0.37,  # new
        "TaggerVertexChargeTool.Vtx_PowerK"     : 0.55,
        "TaggerVertexChargeTool.Vtx_MinimumVCharge" : 0.2,
        "TaggerVertexChargeTool.Vtx_Ptsum"      : 2.2,
        "TaggerVertexChargeTool.Vtx_Ptmean"     : 0.0,
        "TaggerVertexChargeTool.Vtx_IPSsum"     : 0., # no cut
        "TaggerVertexChargeTool.Vtx_DocaMaxsum" : 0.5,
        "TaggerVertexChargeTool.Vtx_Psum"       : 8.0,
        "TaggerVertexChargeTool.Vtx_Msum"       : 0.6,
        "TaggerVertexChargeTool.Vtx_P0_Cal"     : 0.0,
        "TaggerVertexChargeTool.Vtx_P1_Cal"     : 1.0,
        "TaggerVertexChargeTool.Vtx_Eta_Cal"    : 0.0,
        "TaggerVertexChargeTool.Vtx_ProbMin"    : 0.0,
        "TaggerVertexChargeTool.Vtx_AverageOmega" : 0.41,
        "TaggerVertexChargeTool.isMonteCarlo"   : 1,

         #// cuts_Moriond2013 trained on CloneKO, lowCorrelation, newNNPID D,N, CE tanh, Split i%3=0,1  
        "TaggerMuonTool.P0_mu_scale"        :  -0.141137,
        "TaggerMuonTool.P1_mu_scale"        :  41.5316,
        "TaggerMuonTool.P2_mu_scale"        : -10.2783,
        "TaggerMuonTool.P3_mu_scale"        :   0.685294,

        "TaggerElectronTool.P0_e_scale"     :  -5.94519,
        "TaggerElectronTool.P1_e_scale"     :   0.81266,
        "TaggerElectronTool.P2_e_scale"     :   0.118091, 
        "TaggerElectronTool.P3_e_scale"     :   0.674805,

        "TaggerKaonOppositeTool.P0_k_scale"     :   -3.52673,
        "TaggerKaonOppositeTool.P1_k_scale"     :    1.59036,
        "TaggerKaonOppositeTool.P2_k_scale"     :   -0.0650637,
        "TaggerKaonOppositeTool.P3_k_scale"     :    0.660806,

        "TaggerVertexChargeTool.P0_vtx_scale"   :  -0.151015,
        "TaggerVertexChargeTool.P1_vtx_scale"   :  41.1709,
        "TaggerVertexChargeTool.P2_vtx_scale"   : -10.1307,
        "TaggerVertexChargeTool.P3_vtx_scale"   :   0.636393,

        "TaggerKaonSameTool.P0_ks_scale"        :  -0.127331,
        "TaggerKaonSameTool.P1_ks_scale"        :  44.3673,
        "TaggerKaonSameTool.P2_ks_scale"        :  -11.,
        "TaggerKaonSameTool.P3_ks_scale"        :  0.715702,

        "TaggerPionSameTool.P0_ps_scale"        :  -0.16746,
        "TaggerPionSameTool.P1_ps_scale"        :  34.4145,
        "TaggerPionSameTool.P2_ps_scale"        :  -8.35356,
        "TaggerPionSameTool.P3_ps_scale"        :   0.574669                  
        },

    #use cuts optimized and calibrated with Strip20 (2012 data 2fb-1), nnet and polynomials tuned on MC12 
    'Reco14_MC12' : { 
        "Personality": "'Reco14'",  # it is then propagated to all other tools
        "IPPU_cut" : 3.0,
        "distphi_cut" : 0.005,

        #"ForceSignalID" :   , #"Bs", "Bd", "Bu" to force SSk or SSpi

        "EnableCharmTagger": "True",
        "EnableProtonTagger": "True",
        "EnablePionBDTTagger": "True",
        #Following 2 lines optimize the average and combined (5 cat) performances for B2JpsiK
        #When performing your analisis, please find yourself the cut which maximeze the Eeff
        #Probably if you are using Eeff event per event no cut will be needed
        #"CombineTaggersProbability.ProbMin_OS" : 0.58,
        #"CombineTaggersProbability.ProbMin" : 0.55,
 
        "CombineTaggersProbability.P0_Cal_OS" :  0.4004,
        "CombineTaggersProbability.P1_Cal_OS" : 0.8476,
        "CombineTaggersProbability.Eta_Cal_OS" : 0.3892,
        # calibration for combination with nnetOSK  -- no calibration
        "CombineTaggersProbability.P0_Cal_OSnnet" : 0.,
        "CombineTaggersProbability.P1_Cal_OSnnet" : 1.,
        "CombineTaggersProbability.Eta_Cal_OSnnet" : 0.,

        "TaggerMuonTool.Muon_Pt_cut" :     1100.0,
        "TaggerMuonTool.Muon_P_cut" :         0.0,
        "TaggerMuonTool.Muon_lcs_cut" :       3.0,
        "TaggerMuonTool.Muon_IPs_cut" :       0.0,
        "TaggerMuonTool.Muon_PIDm_cut" :   -100.0,  # no cut
        "TaggerMuonTool.Muon_ipPU_cut" :      3.0,
        "TaggerMuonTool.Muon_distPhi_cut"   : 0.005,        
        "TaggerMuonTool.Muon_ghostprob_cut" : 0.4,  # new cut
        "TaggerMuonTool.Muon_PIDNNm_cut"    : 0.35,  # new cut       
        "TaggerMuonTool.Muon_PIDNNpi_cut"   : 0.8,  # new cut       
        "TaggerMuonTool.Muon_PIDNNe_cut"    : 0.8,  # new cut       
        "TaggerMuonTool.Muon_PIDNNk_cut"    : 0.8,  # new cut       
        "TaggerMuonTool.Muon_PIDNNp_cut"    : 0.8,  # new cut       
        "TaggerMuonTool.Muon_P0_Cal" :       0.,
        "TaggerMuonTool.Muon_P1_Cal" :       1., 
        "TaggerMuonTool.Muon_Eta_Cal" :      0.,
        "TaggerMuonTool.Muon_ProbMin" :       0.0,
        "TaggerMuonTool.Muon_AverageOmega" :  0.33,
        "TaggerMuonTool.isMonteCarlo" :       1,
        
        "TaggerElectronTool.Ele_Pt_cut" :       1100,
        "TaggerElectronTool.Ele_P_cut" :         0.0,
        "TaggerElectronTool.Ele_lcs_cut" :       3.,
        "TaggerElectronTool.Ele_IPs_cut" :       3.5,
        "TaggerElectronTool.Ele_ghost_cut" :   -999.,
        "TaggerElectronTool.Ele_VeloChargeMin" : 0.0,
        "TaggerElectronTool.Ele_VeloChargeMax" : 1.4,
        "TaggerElectronTool.Ele_EoverP" :       0.85,
        "TaggerElectronTool.Ele_EoverPmax" :      2.,
        "TaggerElectronTool.Ele_PIDe_cut" :     -1.0,
        "TaggerElectronTool.Ele_ipPU_cut" :      4.0,
        "TaggerElectronTool.Ele_distPhi_cut" : 0.005,
        "TaggerElectronTool.Ele_PIDNNe_cut" :    0.1,  # new
        "TaggerElectronTool.Ele_PIDNNk_cut" :    0.8,  # new
        "TaggerElectronTool.Ele_PIDNNp_cut" :    0.8,  # new
        "TaggerElectronTool.Ele_PIDNNpi_cut" :   0.8,  # new
        "TaggerElectronTool.Ele_PIDNNepi_cut" : -0.8,  # new
        "TaggerElectronTool.Ele_ghostProb_cut" : 0.4,  # new       
        "TaggerElectronTool.Ele_P0_Cal" :         0.,
        "TaggerElectronTool.Ele_P1_Cal" :         1., 
        "TaggerElectronTool.Ele_Eta_Cal" :        0.,
        "TaggerElectronTool.isMonteCarlo" :     1,
        
        "TaggerKaonOppositeTool.Kaon_Pt_cut" :   700,
        "TaggerKaonOppositeTool.Kaon_P_cut" :   2000,
        "TaggerKaonOppositeTool.Kaon_lcs_cut" :  3,
        "TaggerKaonOppositeTool.Kaon_IPs_cut" : 4.,
        "TaggerKaonOppositeTool.Kaon_IP_cut" :   1.6,
        "TaggerKaonOppositeTool.Kaon_PIDk_cut" :    -100.,
        "TaggerKaonOppositeTool.Kaon_PIDkp_cut" :   -300.,
        "TaggerKaonOppositeTool.Kaon_ghost_cut" :   -999.,
        "TaggerKaonOppositeTool.Kaon_ipPU_cut" :      6.0,
        "TaggerKaonOppositeTool.Kaon_distPhi_cut" : 0.005,
        "TaggerKaonOppositeTool.Kaon_PIDNNk_cut" :    0.25, # new
        "TaggerKaonOppositeTool.Kaon_PIDNNe_cut" :   0.8, #new
        "TaggerKaonOppositeTool.Kaon_PIDNNm_cut" :   0.8, #new
        "TaggerKaonOppositeTool.Kaon_PIDNNpi_cut" :   0.8, #new
        "TaggerKaonOppositeTool.Kaon_PIDNNp_cut" :    0.8, #new
        "TaggerKaonOppositeTool.Kaon_PIDNNkp_cut" :   0.0, # new
        "TaggerKaonOppositeTool.Kaon_PIDNNkpi_cut" : -0.6, # new
        "TaggerKaonOppositeTool.Kaon_ghostProb_cut" :0.35, # new
        "TaggerKaonOppositeTool.Kaon_P0_Cal" :        0.0,
        "TaggerKaonOppositeTool.Kaon_P1_Cal" :        1.0,
        "TaggerKaonOppositeTool.Kaon_Eta_Cal" :       0.0,
        "TaggerKaonOppositeTool.Kaon_ProbMin" :       0.0,
        "TaggerKaonOppositeTool.Kaon_AverageOmega" : 0.33,
        "TaggerKaonOppositeTool.isMonteCarlo"      : 1,

        "TaggerNEWKaonOppositeTool.Kaon_PIDk_cut":     0.75,
        "TaggerNEWKaonOppositeTool.Kaon_PIDkp_cut":   -3.,
        "TaggerNEWKaonOppositeTool.Kaon_distPhi_cut":  0.005,
        "TaggerNEWKaonOppositeTool.Kaon_tsa_cut":   -999.0, # formerly Kaon_ghost_cut
        "TaggerNEWKaonOppositeTool.Kaon_ipPU_cut":     3.0,
        "TaggerNEWKaonOppositeTool.Kaon_ghost_cut":    0.5,
        "TaggerNEWKaonOppositeTool.Kaon_NN1_cut":      0.6,
        "TaggerNEWKaonOppositeTool.Kaon_ProbMin":      0.5,
        "TaggerNEWKaonOppositeTool.Kaon_P0_Cal":       0.4072, #+/- 0.0015
        "TaggerNEWKaonOppositeTool.Kaon_P1_Cal":       1.2940, #+/- 0.0250
        "TaggerNEWKaonOppositeTool.Kaon_AverageOmega": 0.4245,

        "TaggerKaonSameTool.KaonSame_Pt_cut" : 850,
        "TaggerKaonSameTool.KaonSame_P_cut"  : 5250,
        "TaggerKaonSameTool.KaonSame_IP_cut" : 4.125,
        "TaggerKaonSameTool.KaonSame_lcs_cut" : 3.75,
        "TaggerKaonSameTool.KaonSame_Phi_cut" : 0.825,
        "TaggerKaonSameTool.KaonSame_Eta_cut" : 0.6,
        "TaggerKaonSameTool.KaonSame_dQ_cut" : 1850.,
        "TaggerKaonSameTool.KaonSame_PIDk_cut" : 3.5,
        "TaggerKaonSameTool.KaonSame_PIDkp_cut" : -8.5,
        "TaggerKaonSameTool.KaonSame_ipPU_cut" : 3.0,
        "TaggerKaonSameTool.KaonSame_distPhi_cut" : 0.005,
        "TaggerKaonSameTool.KaonSame_ProbMin"  : 0.,
        "TaggerKaonSameTool.KaonSame_AverageOmega" : 0.33,
        "TaggerKaonSameTool.KaonSame_P0_Cal"   : 0.,
        "TaggerKaonSameTool.KaonSame_P1_Cal"   : 1.,
        "TaggerKaonSameTool.KaonSame_Eta_Cal"  : 0.,
        "TaggerKaonSameTool.isMonteCarlo"      : 1,
        
        # Same tuning as Reco12_2011, different calibration 
        "TaggerNEWKaonSameTool.Kaon_PIDk_cut"   :    0.75,
        "TaggerNEWKaonSameTool.Kaon_PIDkp_cut"  :   -8.5,
        "TaggerNEWKaonSameTool.Kaon_ghost_cut"  :    1000,
        "TaggerNEWKaonSameTool.Kaon_distPhi_cut":    0.005,
        "TaggerNEWKaonSameTool.Kaon_NN1_cut"    :    0.65,
        "TaggerNEWKaonSameTool.Kaon_P0_Cal"     :    0.429,
        "TaggerNEWKaonSameTool.Kaon_P1_Cal"     :    1.11,
        "TaggerNEWKaonSameTool.Kaon_AverageOmega":   0.433,

        "TaggerPionSameTool.PionSame_Pt_cut"  : 600,
        "TaggerPionSameTool.PionSame_P_cut"   : 4000,
        "TaggerPionSameTool.PionSame_IPs_cut" : 9.,
        "TaggerPionSameTool.PionSame_lcs_cut" : 5.0,
        "TaggerPionSameTool.PionSame_deta_max_cut" : 0.35,
        "TaggerPionSameTool.PionSame_deta_min_cut" : -0.5,
        "TaggerPionSameTool.PionSame_dphi_cut": 0.75,
        "TaggerPionSameTool.PionSame_dR_cut"  : 0.75,
        "TaggerPionSameTool.PionSame_dQ_cut"  : 2500,
        "TaggerPionSameTool.PionSame_dQ_extra_cut" : 1200,
        "TaggerPionSameTool.PionSame_ghost_cut" : -999.,
        "TaggerPionSameTool.PionSame_PIDNoK_cut" : 4.3,
        "TaggerPionSameTool.PionSame_PIDNoP_cut" : 14.0,
        "TaggerPionSameTool.PionSame_ipPU_cut" : 6.0,
        "TaggerPionSameTool.PionSame_distPhi_cut" : 0.005,
        "TaggerPionSameTool.PionSame_ProbMin"   : 0.56,
        "TaggerPionSameTool.PionSame_AverageOmega" : 0.41,
        "TaggerPionSameTool.PionSame_P0_Cal"    : 0.,
        "TaggerPionSameTool.PionSame_P1_Cal"    : 1.,
        "TaggerPionSameTool.PionSame_P2_Cal"    : 0.,
        "TaggerPionSameTool.PionSame_Eta_Cal"   : 0.,
        "TaggerPionSameTool.isMonteCarlo"       : 1,

        "TaggerVertexChargeTool.SVertexOneSeedTool.MinSeedProbability" : 0.42,
        "TaggerVertexChargeTool.SVertexOneSeedTool.MinSeedPtmin"       : 0.1,
        "TaggerVertexChargeTool.SVertexOneSeedTool.MinSeedIPSmin"      : 2.5,
        "TaggerVertexChargeTool.SVertexOneSeedTool.MinSeedDphimin"     : 0.,
        #"TaggerVertexChargeTool.SVertexOneSeedTool.MinSeedDphimax"     : 5,
        "TaggerVertexChargeTool.SVertexOneSeedTool.ghostProb_cut"      : 0.37,  # new
        "TaggerVertexChargeTool.Vtx_PowerK"     : 0.55,
        "TaggerVertexChargeTool.Vtx_MinimumVCharge" : 0.2,
        "TaggerVertexChargeTool.Vtx_Ptsum"      : 2.2,
        "TaggerVertexChargeTool.Vtx_Ptmean"     : 0.0,
        "TaggerVertexChargeTool.Vtx_IPSsum"     : 0., # no cut
        "TaggerVertexChargeTool.Vtx_DocaMaxsum" : 0.5,
        "TaggerVertexChargeTool.Vtx_Psum"       : 8.0,
        "TaggerVertexChargeTool.Vtx_Msum"       : 0.6,
        "TaggerVertexChargeTool.Vtx_P0_Cal"     : 0.0,
        "TaggerVertexChargeTool.Vtx_P1_Cal"     : 1.0,
        "TaggerVertexChargeTool.Vtx_Eta_Cal"    : 0.0,
        "TaggerVertexChargeTool.Vtx_ProbMin"    : 0.0,
        "TaggerVertexChargeTool.Vtx_AverageOmega" : 0.41,
        "TaggerVertexChargeTool.isMonteCarlo"   : 1,

         #// cuts_Moriond2013 trained on CloneKO, lowCorrelation, newNNPID D,N, CE tanh, Split i%3=0,1  
        "TaggerMuonTool.P0_mu_scale"        :  -0.141137,
        "TaggerMuonTool.P1_mu_scale"        :  41.5316,
        "TaggerMuonTool.P2_mu_scale"        : -10.2783,
        "TaggerMuonTool.P3_mu_scale"        :   0.685294,

        "TaggerElectronTool.P0_e_scale"     :  -5.94519,
        "TaggerElectronTool.P1_e_scale"     :   0.81266,
        "TaggerElectronTool.P2_e_scale"     :   0.118091, 
        "TaggerElectronTool.P3_e_scale"     :   0.674805,

        "TaggerKaonOppositeTool.P0_k_scale"     :   -3.52673,
        "TaggerKaonOppositeTool.P1_k_scale"     :    1.59036,
        "TaggerKaonOppositeTool.P2_k_scale"     :   -0.0650637,
        "TaggerKaonOppositeTool.P3_k_scale"     :    0.660806,

        "TaggerVertexChargeTool.P0_vtx_scale"   :  -0.151015,
        "TaggerVertexChargeTool.P1_vtx_scale"   :  41.1709,
        "TaggerVertexChargeTool.P2_vtx_scale"   : -10.1307,
        "TaggerVertexChargeTool.P3_vtx_scale"   :   0.636393,

        "TaggerKaonSameTool.P0_ks_scale"        :  -0.127331,
        "TaggerKaonSameTool.P1_ks_scale"        :  44.3673,
        "TaggerKaonSameTool.P2_ks_scale"        :  -11.,
        "TaggerKaonSameTool.P3_ks_scale"        :  0.715702,

        "TaggerPionSameTool.P0_ps_scale"        :  -0.16746,
        "TaggerPionSameTool.P1_ps_scale"        :  34.4145,
        "TaggerPionSameTool.P2_ps_scale"        :  -8.35356,
        "TaggerPionSameTool.P3_ps_scale"        :   0.574669                  
        },


    #same as Reco14_MC12 w/o cut on variables not present on MC upgrade 
    'Reco14_MC12_Upgrade' : { 
        "Personality": "'Reco14'",  # it is then propagated to all other tools
        "IPPU_cut" : 3.0,
        "distphi_cut" : 0.005,
        "ghostprob_cut" : 1000.,

        #"ForceSignalID" :   , #"Bs", "Bd", "Bu" to force SSk or SSpi

        "EnableCharmTagger": "True",
        "EnableProtonTagger": "True",
        "EnablePionBDTTagger": "True",
        #Following 2 lines optimize the average and combined (5 cat) performances for B2JpsiK
        #When performing your analisis, please find yourself the cut which maximeze the Eeff
        #Probably if you are using Eeff event per event no cut will be needed
        #"CombineTaggersProbability.ProbMin_OS" : 0.58,
        #"CombineTaggersProbability.ProbMin" : 0.55,
 
        "CombineTaggersProbability.P0_Cal_OS" :  0.4004,
        "CombineTaggersProbability.P1_Cal_OS" : 0.8476,
        "CombineTaggersProbability.Eta_Cal_OS" : 0.3892,
        # calibration for combination with nnetOSK  -- no calibration
        "CombineTaggersProbability.P0_Cal_OSnnet" : 0.,
        "CombineTaggersProbability.P1_Cal_OSnnet" : 1.,
        "CombineTaggersProbability.Eta_Cal_OSnnet" : 0.,

        "TaggerMuonTool.Muon_Pt_cut" :     1100.0,
        "TaggerMuonTool.Muon_P_cut" :         0.0,
        "TaggerMuonTool.Muon_lcs_cut" :       3.0,
        "TaggerMuonTool.Muon_IPs_cut" :       0.0,
        "TaggerMuonTool.Muon_PIDm_cut" :   -100.0,  # no cut
        "TaggerMuonTool.Muon_ipPU_cut" :      3.0,
        "TaggerMuonTool.Muon_distPhi_cut"   : 0.005,        
        "TaggerMuonTool.Muon_ghostprob_cut" : 1000.,  # new cut
        "TaggerMuonTool.Muon_PIDNNm_cut"    : 0.35,  # new cut       
        "TaggerMuonTool.Muon_PIDNNpi_cut"   : 0.8,  # new cut       
        "TaggerMuonTool.Muon_PIDNNe_cut"    : 0.8,  # new cut       
        "TaggerMuonTool.Muon_PIDNNk_cut"    : 0.8,  # new cut       
        "TaggerMuonTool.Muon_PIDNNp_cut"    : 0.8,  # new cut       
        "TaggerMuonTool.Muon_P0_Cal" :       0.,
        "TaggerMuonTool.Muon_P1_Cal" :       1., 
        "TaggerMuonTool.Muon_Eta_Cal" :      0.,
        "TaggerMuonTool.Muon_ProbMin" :       0.0,
        "TaggerMuonTool.Muon_AverageOmega" :  0.33,
        "TaggerMuonTool.isMonteCarlo" :       1,
        
        "TaggerElectronTool.Ele_Pt_cut" :       1100,
        "TaggerElectronTool.Ele_P_cut" :         0.0,
        "TaggerElectronTool.Ele_lcs_cut" :       3.,
        "TaggerElectronTool.Ele_IPs_cut" :       3.5,
        "TaggerElectronTool.Ele_ghost_cut" :   -999.,
        "TaggerElectronTool.Ele_VeloChargeMin" : 0.0,
        "TaggerElectronTool.Ele_VeloChargeMax" : 1.4,
        "TaggerElectronTool.Ele_EoverP" :       0.85,
        "TaggerElectronTool.Ele_EoverPmax" :      2.,
        "TaggerElectronTool.Ele_PIDe_cut" :     -1.0,
        "TaggerElectronTool.Ele_ipPU_cut" :      4.0,
        "TaggerElectronTool.Ele_distPhi_cut" : 0.005,
        "TaggerElectronTool.Ele_PIDNNe_cut" :    0.1,  # new
        "TaggerElectronTool.Ele_PIDNNk_cut" :    0.8,  # new
        "TaggerElectronTool.Ele_PIDNNp_cut" :    0.8,  # new
        "TaggerElectronTool.Ele_PIDNNpi_cut" :   0.8,  # new
        "TaggerElectronTool.Ele_PIDNNepi_cut" : -0.8,  # new
        "TaggerElectronTool.Ele_ghostProb_cut" : 1000.,  # new       
        "TaggerElectronTool.Ele_P0_Cal" :         0.,
        "TaggerElectronTool.Ele_P1_Cal" :         1., 
        "TaggerElectronTool.Ele_Eta_Cal" :        0.,
        "TaggerElectronTool.isMonteCarlo" :     1,
        
        "TaggerKaonOppositeTool.Kaon_Pt_cut" :   700,
        "TaggerKaonOppositeTool.Kaon_P_cut" :   2000,
        "TaggerKaonOppositeTool.Kaon_lcs_cut" :  3,
        "TaggerKaonOppositeTool.Kaon_IPs_cut" : 4.,
        "TaggerKaonOppositeTool.Kaon_IP_cut" :   1.6,
        "TaggerKaonOppositeTool.Kaon_PIDk_cut" :    -100.,
        "TaggerKaonOppositeTool.Kaon_PIDkp_cut" :   -300.,
        "TaggerKaonOppositeTool.Kaon_ghost_cut" :   -999.,
        "TaggerKaonOppositeTool.Kaon_ipPU_cut" :      6.0,
        "TaggerKaonOppositeTool.Kaon_distPhi_cut" : 0.005,
        "TaggerKaonOppositeTool.Kaon_PIDNNk_cut" :    0.25, # new
        "TaggerKaonOppositeTool.Kaon_PIDNNe_cut" :   0.8, #new
        "TaggerKaonOppositeTool.Kaon_PIDNNm_cut" :   0.8, #new
        "TaggerKaonOppositeTool.Kaon_PIDNNpi_cut" :   0.8, #new
        "TaggerKaonOppositeTool.Kaon_PIDNNp_cut" :    0.8, #new
        "TaggerKaonOppositeTool.Kaon_PIDNNkp_cut" :   0.0, # new
        "TaggerKaonOppositeTool.Kaon_PIDNNkpi_cut" : -0.6, # new
        "TaggerKaonOppositeTool.Kaon_ghostProb_cut" : 1000., # new
        "TaggerKaonOppositeTool.Kaon_P0_Cal" :        0.0,
        "TaggerKaonOppositeTool.Kaon_P1_Cal" :        1.0,
        "TaggerKaonOppositeTool.Kaon_Eta_Cal" :       0.0,
        "TaggerKaonOppositeTool.Kaon_ProbMin" :       0.0,
        "TaggerKaonOppositeTool.Kaon_AverageOmega" : 0.33,
        "TaggerKaonOppositeTool.isMonteCarlo"      : 1,

        "TaggerNEWKaonOppositeTool.Kaon_PIDk_cut":     0.75,
        "TaggerNEWKaonOppositeTool.Kaon_PIDkp_cut":   -3.,
        "TaggerNEWKaonOppositeTool.Kaon_distPhi_cut":  0.005,
        "TaggerNEWKaonOppositeTool.Kaon_tsa_cut":   -999.0, # formerly Kaon_ghost_cut
        "TaggerNEWKaonOppositeTool.Kaon_ipPU_cut":     7.5,
        "TaggerNEWKaonOppositeTool.Kaon_ghost_cut":    1000.,
        "TaggerNEWKaonOppositeTool.Kaon_NN1_cut":      0.3,
        "TaggerNEWKaonOppositeTool.Kaon_ProbMin":      0.48,
        "TaggerNEWKaonOppositeTool.Kaon_P0_Cal":       0.414,
        "TaggerNEWKaonOppositeTool.Kaon_P1_Cal":       1.48,
        "TaggerNEWKaonOppositeTool.Kaon_AverageOmega": 0.434,

        "TaggerKaonSameTool.KaonSame_Pt_cut" : 850,
        "TaggerKaonSameTool.KaonSame_P_cut"  : 5250,
        "TaggerKaonSameTool.KaonSame_IP_cut" : 4.125,
        "TaggerKaonSameTool.KaonSame_lcs_cut" : 3.75,
        "TaggerKaonSameTool.KaonSame_Phi_cut" : 0.825,
        "TaggerKaonSameTool.KaonSame_Eta_cut" : 0.6,
        "TaggerKaonSameTool.KaonSame_dQ_cut" : 1850.,
        "TaggerKaonSameTool.KaonSame_PIDk_cut" : 3.5,
        "TaggerKaonSameTool.KaonSame_PIDkp_cut" : -8.5,
        "TaggerKaonSameTool.KaonSame_ipPU_cut" : 3.0,
        "TaggerKaonSameTool.KaonSame_distPhi_cut" : 0.005,
        "TaggerKaonSameTool.KaonSame_ProbMin"  : 0.,
        "TaggerKaonSameTool.KaonSame_AverageOmega" : 0.33,
        "TaggerKaonSameTool.KaonSame_P0_Cal"   : 0.,
        "TaggerKaonSameTool.KaonSame_P1_Cal"   : 1.,
        "TaggerKaonSameTool.KaonSame_Eta_Cal"  : 0.,
        "TaggerKaonSameTool.isMonteCarlo"      : 1,
        
        # Same tuning as Reco12_2011, different calibration 
        "TaggerNEWKaonSameTool.Kaon_PIDk_cut"   :    0.75,
        "TaggerNEWKaonSameTool.Kaon_PIDkp_cut"  :   -8.5,
        "TaggerNEWKaonSameTool.Kaon_ghost_cut"  :    1000,
        "TaggerNEWKaonSameTool.Kaon_distPhi_cut":    0.005,
        "TaggerNEWKaonSameTool.Kaon_NN1_cut"    :    0.65,
        "TaggerNEWKaonSameTool.Kaon_P0_Cal"     :    0.429,
        "TaggerNEWKaonSameTool.Kaon_P1_Cal"     :    1.11,
        "TaggerNEWKaonSameTool.Kaon_AverageOmega":   0.433,

        "TaggerPionSameTool.PionSame_Pt_cut"  : 600,
        "TaggerPionSameTool.PionSame_P_cut"   : 4000,
        "TaggerPionSameTool.PionSame_IPs_cut" : 9.,
        "TaggerPionSameTool.PionSame_lcs_cut" : 5.0,
        "TaggerPionSameTool.PionSame_deta_max_cut" : 0.35,
        "TaggerPionSameTool.PionSame_deta_min_cut" : -0.5,
        "TaggerPionSameTool.PionSame_dphi_cut": 0.75,
        "TaggerPionSameTool.PionSame_dR_cut"  : 0.75,
        "TaggerPionSameTool.PionSame_dQ_cut"  : 2500,
        "TaggerPionSameTool.PionSame_dQ_extra_cut" : 1200,
        "TaggerPionSameTool.PionSame_ghost_cut" : -999.,
        "TaggerPionSameTool.PionSame_PIDNoK_cut" : 4.3,
        "TaggerPionSameTool.PionSame_PIDNoP_cut" : 14.0,
        "TaggerPionSameTool.PionSame_ipPU_cut" : 6.0,
        "TaggerPionSameTool.PionSame_distPhi_cut" : 0.005,
        "TaggerPionSameTool.PionSame_ProbMin"   : 0.56,
        "TaggerPionSameTool.PionSame_AverageOmega" : 0.41,
        "TaggerPionSameTool.PionSame_P0_Cal"    : 0.,
        "TaggerPionSameTool.PionSame_P1_Cal"    : 1.,
        "TaggerPionSameTool.PionSame_P2_Cal"    : 0.,
        "TaggerPionSameTool.PionSame_Eta_Cal"   : 0.,
        "TaggerPionSameTool.isMonteCarlo"       : 1,

        "TaggerVertexChargeTool.SVertexOneSeedTool.MinSeedProbability" : 0.42,
        "TaggerVertexChargeTool.SVertexOneSeedTool.MinSeedPtmin"       : 0.1,
        "TaggerVertexChargeTool.SVertexOneSeedTool.MinSeedIPSmin"      : 2.5,
        "TaggerVertexChargeTool.SVertexOneSeedTool.MinSeedDphimin"     : 0.,
        #"TaggerVertexChargeTool.SVertexOneSeedTool.MinSeedDphimax"     : 5,
        "TaggerVertexChargeTool.SVertexOneSeedTool.ghostProb_cut"      : 1000.,  # new
        "TaggerVertexChargeTool.Vtx_PowerK"     : 0.55,
        "TaggerVertexChargeTool.Vtx_MinimumVCharge" : 0.2,
        "TaggerVertexChargeTool.Vtx_Ptsum"      : 2.2,
        "TaggerVertexChargeTool.Vtx_Ptmean"     : 0.0,
        "TaggerVertexChargeTool.Vtx_IPSsum"     : 0., # no cut
        "TaggerVertexChargeTool.Vtx_DocaMaxsum" : 0.5,
        "TaggerVertexChargeTool.Vtx_Psum"       : 8.0,
        "TaggerVertexChargeTool.Vtx_Msum"       : 0.6,
        "TaggerVertexChargeTool.Vtx_P0_Cal"     : 0.0,
        "TaggerVertexChargeTool.Vtx_P1_Cal"     : 1.0,
        "TaggerVertexChargeTool.Vtx_Eta_Cal"    : 0.0,
        "TaggerVertexChargeTool.Vtx_ProbMin"    : 0.0,
        "TaggerVertexChargeTool.Vtx_AverageOmega" : 0.41,
        "TaggerVertexChargeTool.isMonteCarlo"   : 1,

         #// cuts_Moriond2013 trained on CloneKO, lowCorrelation, newNNPID D,N, CE tanh, Split i%3=0,1  
        "TaggerMuonTool.P0_mu_scale"        :  -0.141137,
        "TaggerMuonTool.P1_mu_scale"        :  41.5316,
        "TaggerMuonTool.P2_mu_scale"        : -10.2783,
        "TaggerMuonTool.P3_mu_scale"        :   0.685294,

        "TaggerElectronTool.P0_e_scale"     :  -5.94519,
        "TaggerElectronTool.P1_e_scale"     :   0.81266,
        "TaggerElectronTool.P2_e_scale"     :   0.118091, 
        "TaggerElectronTool.P3_e_scale"     :   0.674805,

        "TaggerKaonOppositeTool.P0_k_scale"     :   -3.52673,
        "TaggerKaonOppositeTool.P1_k_scale"     :    1.59036,
        "TaggerKaonOppositeTool.P2_k_scale"     :   -0.0650637,
        "TaggerKaonOppositeTool.P3_k_scale"     :    0.660806,

        "TaggerVertexChargeTool.P0_vtx_scale"   :  -0.151015,
        "TaggerVertexChargeTool.P1_vtx_scale"   :  41.1709,
        "TaggerVertexChargeTool.P2_vtx_scale"   : -10.1307,
        "TaggerVertexChargeTool.P3_vtx_scale"   :   0.636393,

        "TaggerKaonSameTool.P0_ks_scale"        :  -0.127331,
        "TaggerKaonSameTool.P1_ks_scale"        :  44.3673,
        "TaggerKaonSameTool.P2_ks_scale"        :  -11.,
        "TaggerKaonSameTool.P3_ks_scale"        :  0.715702,

        "TaggerPionSameTool.P0_ps_scale"        :  -0.16746,
        "TaggerPionSameTool.P1_ps_scale"        :  34.4145,
        "TaggerPionSameTool.P2_ps_scale"        :  -8.35356,
        "TaggerPionSameTool.P3_ps_scale"        :   0.574669                  
        },


    #use cuts optimized and calibrated with Strip17 (Xmass2011)
    'MC10' : { 
        "IPPU_cut" : 3.0,
        "distphi_cut" : 0.005,
        #"ForceSignalID" :   , #"Bs", "Bd", "Bu" to force SSk or SSpi

        #Following 2 lines optimize the average and combined (5 cat) performances for B2JpsiK
        #When performing your analisis, please find yourself the cut which maximeze the Eeff
        #Probably if you are using Eeff event per event no cut will be needed
        #"CombineTaggersProbability.ProbMin_OS" : 0.58,
        #"CombineTaggersProbability.ProbMin" : 0.55,
        
        "CombineTaggersProbability.P0_Cal_OS" : 0.392, #+/-0.0016
        "CombineTaggersProbability.P1_Cal_OS" : 0.921, #+/-0.017
        "CombineTaggersProbability.Eta_Cal_OS" : 0.363,
        # calibration for combination with nnetOSK  -- no calibration
        "CombineTaggersProbability.P0_Cal_OSnnet" : 0.,
        "CombineTaggersProbability.P1_Cal_OSnnet" : 1.,
        "CombineTaggersProbability.Eta_Cal_OSnnet" : 0.,

        "TaggerMuonTool.Muon_Pt_cut" : 1200,
        "TaggerMuonTool.Muon_P_cut" : 0.0,
        "TaggerMuonTool.Muon_IPs_cut" : 0.0,
        "TaggerMuonTool.Muon_lcs_cut" : 3.2,
        "TaggerMuonTool.Muon_PIDm_cut" : 2.5,
        "TaggerMuonTool.Muon_ipPU_cut" : 3.0,
        "TaggerMuonTool.Muon_distPhi_cut" : 0.005,
        "TaggerMuonTool.Muon_ProbMin" : 0.0,
        "TaggerMuonTool.Muon_AverageOmega" : 0.33,
        "TaggerMuonTool.Muon_P0_Cal" : 0.,
        "TaggerMuonTool.Muon_P1_Cal" : 1.,
        "TaggerMuonTool.Muon_Eta_Cal" : 0.,

        "TaggerElectronTool.Ele_Pt_cut" : 1000,
        "TaggerElectronTool.Ele_P_cut" : 0.0,
        "TaggerElectronTool.Ele_IPs_cut" : 1.0,
        "TaggerElectronTool.Ele_lcs_cut" : 3.85,
        "TaggerElectronTool.Ele_ghost_cut" : -999.,
        "TaggerElectronTool.Ele_VeloChargeMin" : 0.0,
        "TaggerElectronTool.Ele_VeloChargeMax" : 1.6,
        "TaggerElectronTool.Ele_EoverP" : 0.75,
        "TaggerElectronTool.Ele_PIDe_cut" : 4.0,
        "TaggerElectronTool.Ele_ipPU_cut" : 5.0,
        "TaggerElectronTool.Ele_distPhi_cut" : 0.005,
        "TaggerElectronTool.Ele_ProbMin" : 0.0,
        "TaggerElectronTool.Ele_AverageOmega" : 0.33,
        "TaggerElectronTool.Ele_P0_Cal" : 0.,
        "TaggerElectronTool.Ele_P1_Cal" : 1.,
        "TaggerElectronTool.Ele_Eta_Cal" : 0.,

        "TaggerKaonOppositeTool.Kaon_Pt_cut" : 700,
        "TaggerKaonOppositeTool.Kaon_P_cut" : 5000,
        "TaggerKaonOppositeTool.Kaon_IPs_cut" : 4.3,
        "TaggerKaonOppositeTool.Kaon_IP_cut" : 1.45,
        "TaggerKaonOppositeTool.Kaon_lcs_cut" : 2.45,
        "TaggerKaonOppositeTool.Kaon_ghost_cut" : -999.,

        "TaggerKaonOppositeTool.Kaon_PIDk_cut" : 0.75,
        "TaggerKaonOppositeTool.Kaon_PIDkp_cut" : -3.,
        "TaggerKaonOppositeTool.Kaon_ipPU_cut" : 7.5,
        "TaggerKaonOppositeTool.Kaon_distPhi_cut" : 0.005,
        "TaggerKaonOppositeTool.Kaon_ProbMin" : 0.54,
        "TaggerKaonOppositeTool.Kaon_AverageOmega" : 0.33,
        "TaggerKaonOppositeTool.Kaon_P0_Cal" : 0.,
        "TaggerKaonOppositeTool.Kaon_P1_Cal" : 1.,
        "TaggerKaonOppositeTool.Kaon_Eta_Cal" : 0.,

        "TaggerKaonSameTool.KaonSame_Pt_cut" : 850,
        "TaggerKaonSameTool.KaonSame_P_cut" : 5250,
        "TaggerKaonSameTool.KaonSame_IP_cut" : 4.125,
        "TaggerKaonSameTool.KaonSame_lcs_cut" : 3.75,
        "TaggerKaonSameTool.KaonSame_Phi_cut" : 0.825,
        "TaggerKaonSameTool.KaonSame_Eta_cut" : 0.6,
        "TaggerKaonSameTool.KaonSame_dQ_cut" : 1850.,
        "TaggerKaonSameTool.KaonSame_PIDk_cut" : 3.5,
        "TaggerKaonSameTool.KaonSame_PIDkp_cut" : -8.5,
        "TaggerKaonSameTool.KaonSame_ipPU_cut" : 3.0,
        "TaggerKaonSameTool.KaonSame_distPhi_cut" : 0.005,
        "TaggerKaonSameTool.KaonSame_ProbMin" : 0.,
        "TaggerKaonSameTool.KaonSame_AverageOmega" : 0.33,
        "TaggerKaonSameTool.KaonSame_P0_Cal" : 0.,
        "TaggerKaonSameTool.KaonSame_P1_Cal" : 1.,
        "TaggerKaonSameTool.KaonSame_Eta_Cal" : 0.,

        "TaggerPionSameTool.PionSame_Pt_cut" : 600,
        "TaggerPionSameTool.PionSame_P_cut" : 4000,
        "TaggerPionSameTool.PionSame_IPs_cut" : 9.,
        "TaggerPionSameTool.PionSame_lcs_cut" : 5.0,
        "TaggerPionSameTool.PionSame_deta_max_cut" : 0.35,
        "TaggerPionSameTool.PionSame_deta_min_cut" : -0.5,
        "TaggerPionSameTool.PionSame_dphi_cut" : 0.75,
        "TaggerPionSameTool.PionSame_dR_cut" : 0.75,
        "TaggerPionSameTool.PionSame_dQ_cut" : 2500,
        "TaggerPionSameTool.PionSame_dQ_extra_cut" : 1200,
        "TaggerPionSameTool.PionSame_ghost_cut" : -999.,
        "TaggerPionSameTool.PionSame_PIDNoK_cut" : 4.3,
        "TaggerPionSameTool.PionSame_PIDNoP_cut" : 14.0,
        "TaggerPionSameTool.PionSame_ipPU_cut" : 6.0,
        "TaggerPionSameTool.PionSame_distPhi_cut" : 0.005,
        "TaggerPionSameTool.PionSame_ProbMin" : 0.56,
        "TaggerPionSameTool.PionSame_AverageOmega" : 0.41,
        "TaggerPionSameTool.PionSame_P0_Cal" : 0.,
        "TaggerPionSameTool.PionSame_P1_Cal" : 1.,
        "TaggerPionSameTool.PionSame_P2_Cal" : 0.,
        "TaggerPionSameTool.PionSame_Eta_Cal" : 0.,

        "TaggerVertexChargeTool.SVertexOneSeedTool.MinSeedProbability" : 0.42,
        "TaggerVertexChargeTool.Vtx_PowerK" : 0.55,
        "TaggerVertexChargeTool.Vtx_MinimumVCharge" : 0.2,
        "TaggerVertexChargeTool.Vtx_Ptsum" : 1.55,
        "TaggerVertexChargeTool.Vtx_Ptmean" : 0.,
        "TaggerVertexChargeTool.Vtx_IPSsum" : 0.,  # no cut
        "TaggerVertexChargeTool.Vtx_DocaMaxsum" : 0.5,
        "TaggerVertexChargeTool.Vtx_Psum" : 8.,
        "TaggerVertexChargeTool.Vtx_Msum" : 0.6,
        "TaggerVertexChargeTool.Vtx_ProbMin" : 0.54,
        "TaggerVertexChargeTool.Vtx_AverageOmega" : 0.41,
        "TaggerVertexChargeTool.Vtx_P0_Cal" : 0.,
        "TaggerVertexChargeTool.Vtx_P1_Cal" : 1.,
        "TaggerVertexChargeTool.Vtx_Eta_Cal" : 0.
        },


    ############################################################
    #From here, you need to use the nn in extrann/LP2011
    
    #use cuts optimized and calibrated with B2JpsiK real data (july2011)
    'LP2011' : { 
        "IPPU_cut" : 3.0,
        "distphi_cut" : 0.005,
        #"ForceSignalID" :   , #"Bs", "Bd", "Bu" to force SSk or SSpi

        #Following 2 lines optimize the average and combined (5 cat) performances for B2JpsiK
        #When performing your analisis, please find yourself the cut which maximeze the Eeff
        #Probably if you are using Eeff event per event no cut will be needed
        #"CombineTaggersProbability.ProbMin_OS" : 0.58,
        #"CombineTaggersProbability.ProbMin" : 0.55,
        
        "CombineTaggersProbability.P0_Cal_OS" : 0.376,
        "CombineTaggersProbability.P1_Cal_OS" : 0.928,
        "CombineTaggersProbability.Eta_Cal_OS" : 0.354,
        # calibration for combination with nnetOSK  -- no calibration
        "CombineTaggersProbability.P0_Cal_OSnnet" : 0.,
        "CombineTaggersProbability.P1_Cal_OSnnet" : 1.,
        "CombineTaggersProbability.Eta_Cal_OSnnet" : 0.,

        "TaggerMuonTool.Muon_Pt_cut" : 1200,
        "TaggerMuonTool.Muon_P_cut" : 0.0,
        "TaggerMuonTool.Muon_IPs_cut" : 0.0,
        "TaggerMuonTool.Muon_lcs_cut" : 3.2,
        "TaggerMuonTool.Muon_PIDm_cut" : 2.5,
        "TaggerMuonTool.Muon_ipPU_cut" : 3.0,
        "TaggerMuonTool.Muon_distPhi_cut" : 0.005,
        "TaggerMuonTool.Muon_ProbMin" : 0.0,
        "TaggerMuonTool.Muon_AverageOmega" : 0.33,
        "TaggerMuonTool.Muon_P0_Cal" : 0.304,
        "TaggerMuonTool.Muon_P1_Cal" : 1.10,
        "TaggerMuonTool.Muon_Eta_Cal" : 0.303,

        "TaggerElectronTool.Ele_Pt_cut" : 1000,
        "TaggerElectronTool.Ele_P_cut" : 0.0,
        "TaggerElectronTool.Ele_IPs_cut" : 2.0,
        "TaggerElectronTool.Ele_lcs_cut" : 5.0,
        "TaggerElectronTool.Ele_ghost_cut" : -999.,
        "TaggerElectronTool.Ele_VeloChargeMin" : 0.0,
        "TaggerElectronTool.Ele_VeloChargeMax" : 1.6,
        "TaggerElectronTool.Ele_EoverP" : 0.6,
        "TaggerElectronTool.Ele_PIDe_cut" : 4.0,
        "TaggerElectronTool.Ele_ipPU_cut" : 3.0,
        "TaggerElectronTool.Ele_distPhi_cut" : 0.03,
        "TaggerElectronTool.Ele_ProbMin" : 0.0,
        "TaggerElectronTool.Ele_AverageOmega" : 0.33,
        "TaggerElectronTool.Ele_P0_Cal" : 0.3305,
        "TaggerElectronTool.Ele_P1_Cal" : 1.09,
        "TaggerElectronTool.Ele_Eta_Cal" : 0.345,

        "TaggerKaonOppositeTool.Kaon_Pt_cut" : 800,
        "TaggerKaonOppositeTool.Kaon_P_cut" : 5875,
        "TaggerKaonOppositeTool.Kaon_IPs_cut" : 4.05,
        "TaggerKaonOppositeTool.Kaon_IP_cut" : 1.25,
        "TaggerKaonOppositeTool.Kaon_lcs_cut" : 2.125,
        "TaggerKaonOppositeTool.Kaon_ghost_cut" : -999.,
        "TaggerKaonOppositeTool.Kaon_PIDk_cut" : 6.5,
        "TaggerKaonOppositeTool.Kaon_PIDkp_cut" : -3.5,
        "TaggerKaonOppositeTool.Kaon_ipPU_cut" : 4.72,
        "TaggerKaonOppositeTool.Kaon_distPhi_cut" : -0.005,
        "TaggerKaonOppositeTool.Kaon_ProbMin" : 0.51,
        "TaggerKaonOppositeTool.Kaon_AverageOmega" : 0.33,
        "TaggerKaonOppositeTool.Kaon_P0_Cal" : 0.390,
        "TaggerKaonOppositeTool.Kaon_P1_Cal" : 0.90,
        "TaggerKaonOppositeTool.Kaon_Eta_Cal" : 0.362,

        "TaggerKaonSameTool.KaonSame_Pt_cut" : 750,
        "TaggerKaonSameTool.KaonSame_P_cut" : 5250,
        "TaggerKaonSameTool.KaonSame_IP_cut" : 4.125,
        "TaggerKaonSameTool.KaonSame_lcs_cut" : 3.75,
        "TaggerKaonSameTool.KaonSame_Phi_cut" : 0.7,
        "TaggerKaonSameTool.KaonSame_Eta_cut" : 0.525,
        "TaggerKaonSameTool.KaonSame_dQ_cut" : 1462.5,
        "TaggerKaonSameTool.KaonSame_PIDk_cut" : 4.5,
        "TaggerKaonSameTool.KaonSame_PIDkp_cut" : -8.5,
        "TaggerKaonSameTool.KaonSame_ipPU_cut" : 3.0,
        "TaggerKaonSameTool.KaonSame_distPhi_cut" : 0.005,
        "TaggerKaonSameTool.KaonSame_ProbMin" : 0.51,
        "TaggerKaonSameTool.KaonSame_AverageOmega" : 0.33,
        "TaggerKaonSameTool.KaonSame_P0_Cal" : 0.,
        "TaggerKaonSameTool.KaonSame_P1_Cal" : 1.,
        "TaggerKaonSameTool.KaonSame_Eta_Cal" : 0.,

        "TaggerPionSameTool.PionSame_Pt_cut" : 750,
        "TaggerPionSameTool.PionSame_P_cut" : 5000,
        "TaggerPionSameTool.PionSame_IPs_cut" : 3.5,
        "TaggerPionSameTool.PionSame_lcs_cut" : 5.0,
        "TaggerPionSameTool.PionSame_deta_max_cut" :  5.,
        "TaggerPionSameTool.PionSame_deta_min_cut" : -5.,
        "TaggerPionSameTool.PionSame_dphi_cut" : 50.,
        "TaggerPionSameTool.PionSame_dR_cut" : 50.,
        "TaggerPionSameTool.PionSame_dQ_cut" : 2500,
        "TaggerPionSameTool.PionSame_dQ_extra_cut" : 1500,
        "TaggerPionSameTool.PionSame_ghost_cut" : -999.,
        "TaggerPionSameTool.PionSame_PIDNoK_cut" : 3.0,
        "TaggerPionSameTool.PionSame_PIDNoP_cut" : 10.0,
        "TaggerPionSameTool.PionSame_ipPU_cut" : 3.0,
        "TaggerPionSameTool.PionSame_distPhi_cut" : 0.005,
        "TaggerPionSameTool.PionSame_ProbMin" : 0.54,
        "TaggerPionSameTool.PionSame_AverageOmega" : 0.41,
        "TaggerPionSameTool.PionSame_P0_Cal" : 0.470,
        "TaggerPionSameTool.PionSame_P1_Cal" : 0.614,
        "TaggerPionSameTool.PionSame_P2_Cal" : -2.94,
        "TaggerPionSameTool.PionSame_Eta_Cal" : 0.448,

        "TaggerVertexChargeTool.Vtx_PowerK" : 0.4,
        "TaggerVertexChargeTool.Vtx_MinimumVCharge" : 0.275,
        "TaggerVertexChargeTool.Vtx_Ptsum" : 1.5,
        "TaggerVertexChargeTool.Vtx_Ptmean" : 0.,
        "TaggerVertexChargeTool.Vtx_IPSsum" : 10.,
        "TaggerVertexChargeTool.Vtx_DocaMaxsum" : 0.5,
        "TaggerVertexChargeTool.Vtx_Psum" : 10.,
        "TaggerVertexChargeTool.Vtx_Msum" : 0.5,
        "TaggerVertexChargeTool.Vtx_ProbMin" : 0.54,
        "TaggerVertexChargeTool.Vtx_AverageOmega" : 0.41,
        "TaggerVertexChargeTool.Vtx_P0_Cal" : 0.401,
        "TaggerVertexChargeTool.Vtx_P1_Cal" : 0.90,
        "TaggerVertexChargeTool.Vtx_Eta_Cal" : 0.368
        },


    #use cuts optimized for B2JpsiK real data (july2011) calibration is ok from NNoutput
    'MC10_LP2011' : { 
        "IPPU_cut" : 3.0,
        "distphi_cut" : 0.005,
        #"ForceSignalID" :   , #"Bs", "Bd", "Bu" to force SSk or SSpi

        #Following 2 lines optimize the average and combined (5 cat) performances for B2JpsiK
        #When performing your analisis, please find yourself the cut which maximeze the Eeff
        #Probably if you are using Eeff event per event no cut will be needed
        #"CombineTaggersProbability.ProbMin_OS" : 0.58,
        #"CombineTaggersProbability.ProbMin" : 0.55,
        
        "CombineTaggersProbability.P0_Cal_OS" : 0.376,
        "CombineTaggersProbability.P1_Cal_OS" : 0.928,
        "CombineTaggersProbability.Eta_Cal_OS" : 0.354,
        # calibration for combination with nnetOSK  -- no calibration
        "CombineTaggersProbability.P0_Cal_OSnnet" : 0.,
        "CombineTaggersProbability.P1_Cal_OSnnet" : 1.,
        "CombineTaggersProbability.Eta_Cal_OSnnet" : 0.,

        "TaggerMuonTool.Muon_Pt_cut" : 1200,
        "TaggerMuonTool.Muon_P_cut" : 0.0,
        "TaggerMuonTool.Muon_IPs_cut" : 0.0,
        "TaggerMuonTool.Muon_lcs_cut" : 3.2,
        "TaggerMuonTool.Muon_PIDm_cut" : 2.5,
        "TaggerMuonTool.Muon_ipPU_cut" : 3.0,
        "TaggerMuonTool.Muon_distPhi_cut" : 0.005,
        "TaggerMuonTool.Muon_ProbMin" : 0.0,
        "TaggerMuonTool.Muon_AverageOmega" : 0.33,
        "TaggerMuonTool.Muon_P0_Cal" : 0.,
        "TaggerMuonTool.Muon_P1_Cal" : 1.,
        "TaggerMuonTool.Muon_Eta_Cal" : 0.,

        "TaggerElectronTool.Ele_Pt_cut" : 1000,
        "TaggerElectronTool.Ele_P_cut" : 0.0,
        "TaggerElectronTool.Ele_IPs_cut" : 2.0,
        "TaggerElectronTool.Ele_lcs_cut" : 5.0,
        "TaggerElectronTool.Ele_ghost_cut" : -999.,
        "TaggerElectronTool.Ele_VeloChargeMin" : 0.0,
        "TaggerElectronTool.Ele_VeloChargeMax" : 1.6,
        "TaggerElectronTool.Ele_EoverP" : 0.6,
        "TaggerElectronTool.Ele_PIDe_cut" : 4.0,
        "TaggerElectronTool.Ele_ipPU_cut" : 3.0,
        "TaggerElectronTool.Ele_distPhi_cut" : 0.03,
        "TaggerElectronTool.Ele_ProbMin" : 0.0,
        "TaggerElectronTool.Ele_AverageOmega" : 0.33,
        "TaggerElectronTool.Ele_P0_Cal" : 0.,
        "TaggerElectronTool.Ele_P1_Cal" : 1.,
        "TaggerElectronTool.Ele_Eta_Cal" : 0.,

        "TaggerKaonOppositeTool.Kaon_Pt_cut" : 800,
        "TaggerKaonOppositeTool.Kaon_P_cut" : 5875,
        "TaggerKaonOppositeTool.Kaon_IPs_cut" : 4.05,
        "TaggerKaonOppositeTool.Kaon_IP_cut" : 1.25,
        "TaggerKaonOppositeTool.Kaon_lcs_cut" : 2.125,
        "TaggerKaonOppositeTool.Kaon_ghost_cut" : -999.,
        "TaggerKaonOppositeTool.Kaon_PIDk_cut" : 6.5,
        "TaggerKaonOppositeTool.Kaon_PIDkp_cut" : -3.5,
        "TaggerKaonOppositeTool.Kaon_ipPU_cut" : 4.72,
        "TaggerKaonOppositeTool.Kaon_distPhi_cut" : -0.005,
        "TaggerKaonOppositeTool.Kaon_ProbMin" : 0.51,
        "TaggerKaonOppositeTool.Kaon_AverageOmega" : 0.33,
        "TaggerKaonOppositeTool.Kaon_P0_Cal" : 0.,
        "TaggerKaonOppositeTool.Kaon_P1_Cal" : 1.,
        "TaggerKaonOppositeTool.Kaon_Eta_Cal" : 0.,

        "TaggerKaonSameTool.KaonSame_Pt_cut" : 750,
        "TaggerKaonSameTool.KaonSame_P_cut" : 5250,
        "TaggerKaonSameTool.KaonSame_IP_cut" : 4.125,
        "TaggerKaonSameTool.KaonSame_lcs_cut" : 3.75,
        "TaggerKaonSameTool.KaonSame_Phi_cut" : 0.7,
        "TaggerKaonSameTool.KaonSame_Eta_cut" : 0.525,
        "TaggerKaonSameTool.KaonSame_dQ_cut" : 1462.5,
        "TaggerKaonSameTool.KaonSame_PIDk_cut" : 4.5,
        "TaggerKaonSameTool.KaonSame_PIDkp_cut" : -8.5,
        "TaggerKaonSameTool.KaonSame_ipPU_cut" : 3.0,
        "TaggerKaonSameTool.KaonSame_distPhi_cut" : 0.005,
        "TaggerKaonSameTool.KaonSame_ProbMin" : 0.51,
        "TaggerKaonSameTool.KaonSame_AverageOmega" : 0.33,
        "TaggerKaonSameTool.KaonSame_P0_Cal" : 0.,
        "TaggerKaonSameTool.KaonSame_P1_Cal" : 1.,
        "TaggerKaonSameTool.KaonSame_Eta_Cal" : 0.,

        "TaggerPionSameTool.PionSame_Pt_cut" : 750,
        "TaggerPionSameTool.PionSame_P_cut" : 5000,
        "TaggerPionSameTool.PionSame_IPs_cut" : 3.5,
        "TaggerPionSameTool.PionSame_lcs_cut" : 5.0,
        "TaggerPionSameTool.PionSame_deta_max_cut" :  5.,
        "TaggerPionSameTool.PionSame_deta_min_cut" : -5.,
        "TaggerPionSameTool.PionSame_dphi_cut" : 50.,
        "TaggerPionSameTool.PionSame_dR_cut" : 50.,
        "TaggerPionSameTool.PionSame_dQ_cut" : 2500,
        "TaggerPionSameTool.PionSame_dQ_extra_cut" : 1500,
        "TaggerPionSameTool.PionSame_ghost_cut" : -999.,
        "TaggerPionSameTool.PionSame_PIDNoK_cut" : 3.0,
        "TaggerPionSameTool.PionSame_PIDNoP_cut" : 10.0,
        "TaggerPionSameTool.PionSame_ipPU_cut" : 3.0,
        "TaggerPionSameTool.PionSame_distPhi_cut" : 0.005,
        "TaggerPionSameTool.PionSame_ProbMin" : 0.54,
        "TaggerPionSameTool.PionSame_AverageOmega" : 0.41,
        "TaggerPionSameTool.PionSame_P0_Cal" : 0.,
        "TaggerPionSameTool.PionSame_P1_Cal" : 1.,
        "TaggerPionSameTool.PionSame_P2_Cal" : 0.,
        "TaggerPionSameTool.PionSame_Eta_Cal" : 0.,

        "TaggerVertexChargeTool.Vtx_PowerK" : 0.4,
        "TaggerVertexChargeTool.Vtx_MinimumVCharge" : 0.275,
        "TaggerVertexChargeTool.Vtx_Ptsum" : 1.5,
        "TaggerVertexChargeTool.Vtx_Ptmean" : 0.,
        "TaggerVertexChargeTool.Vtx_IPSsum" : 10.,
        "TaggerVertexChargeTool.Vtx_DocaMaxsum" : 0.5,
        "TaggerVertexChargeTool.Vtx_Psum" : 10.,
        "TaggerVertexChargeTool.Vtx_Msum" : 0.5,
        "TaggerVertexChargeTool.Vtx_ProbMin" : 0.54,
        "TaggerVertexChargeTool.Vtx_AverageOmega" : 0.41,
        "TaggerVertexChargeTool.Vtx_P0_Cal" : 0.,
        "TaggerVertexChargeTool.Vtx_P1_Cal" : 1.,
        "TaggerVertexChargeTool.Vtx_Eta_Cal" : 0.
        },


    ############################################################
    #From here, you need to use the nn in extrann/Moriond2011
    
    #use cuts optimized for real data but calibration done in mc10 (nu=2.5)
    'MC10_Moriond2011' : { 
        "IPPU_cut" : 3.0,
        "distphi_cut" : 0.005,
        #"ForceSignalID" :   , #"Bs", "Bd", "Bu" to force SSk or SSpi
        
        "CombineTaggersProbability.P0_Cal_OS" : 0.366,
        "CombineTaggersProbability.P1_Cal_OS" : 0.933,
        "CombineTaggersProbability.Eta_Cal_OS" : 0.339,
        # calibration for combination with nnetOSK  -- no calibration
        "CombineTaggersProbability.P0_Cal_OSnnet" : 0.,
        "CombineTaggersProbability.P1_Cal_OSnnet" : 1.,
        "CombineTaggersProbability.Eta_Cal_OSnnet" : 0.,

        "TaggerMuonTool.Muon_Pt_cut" : 1100,
        "TaggerMuonTool.Muon_P_cut" : 0.0,
        "TaggerMuonTool.Muon_IPs_cut" : 0.0,
        "TaggerMuonTool.Muon_lcs_cut" : 2.2,
        "TaggerMuonTool.Muon_PIDm_cut" : 2.0,
        "TaggerMuonTool.Muon_ipPU_cut" : 3.0,
        "TaggerMuonTool.Muon_distPhi_cut" : 0.005,
        "TaggerMuonTool.Muon_ProbMin" : 0.0,
        "TaggerMuonTool.Muon_AverageOmega" : 0.33,
        "TaggerMuonTool.Muon_P0_Cal" : 0.313,
        "TaggerMuonTool.Muon_P1_Cal" : 0.700,
        "TaggerMuonTool.Muon_Eta_Cal" : 0.311,

        "TaggerElectronTool.Ele_Pt_cut" : 1000,
        "TaggerElectronTool.Ele_P_cut" : 0.0,
        "TaggerElectronTool.Ele_IPs_cut" : 2.0,
        "TaggerElectronTool.Ele_lcs_cut" : 5.0,
        "TaggerElectronTool.Ele_ghost_cut" : -999.,
        "TaggerElectronTool.Ele_VeloChargeMin" : 0.0,
        "TaggerElectronTool.Ele_VeloChargeMax" : 1.6,
        "TaggerElectronTool.Ele_EoverP" : 0.6,
        "TaggerElectronTool.Ele_PIDe_cut" : 4.0,
        "TaggerElectronTool.Ele_ipPU_cut" : 3.0,
        "TaggerElectronTool.Ele_distPhi_cut" : 0.03,
        "TaggerElectronTool.Ele_ProbMin" : 0.0,
        "TaggerElectronTool.Ele_AverageOmega" : 0.33,
        "TaggerElectronTool.Ele_P0_Cal" : 0.348,
        "TaggerElectronTool.Ele_P1_Cal" : 0.866,
        "TaggerElectronTool.Ele_Eta_Cal" : 0.331,

        "TaggerKaonOppositeTool.Kaon_Pt_cut" : 800,
        "TaggerKaonOppositeTool.Kaon_P_cut" : 5875,
        "TaggerKaonOppositeTool.Kaon_IPs_cut" : 4.05,
        "TaggerKaonOppositeTool.Kaon_IP_cut" : 1.25,
        "TaggerKaonOppositeTool.Kaon_lcs_cut" : 2.125,
        "TaggerKaonOppositeTool.Kaon_ghost_cut" : -999.,
        "TaggerKaonOppositeTool.Kaon_PIDk_cut" : 6.5,
        "TaggerKaonOppositeTool.Kaon_PIDkp_cut" : -3.5,
        "TaggerKaonOppositeTool.Kaon_ipPU_cut" : 4.72,
        "TaggerKaonOppositeTool.Kaon_distPhi_cut" : -0.005,
        "TaggerKaonOppositeTool.Kaon_ProbMin" : 0.51,
        "TaggerKaonOppositeTool.Kaon_AverageOmega" : 0.33,
        "TaggerKaonOppositeTool.Kaon_P0_Cal" : 0.3315,
        "TaggerKaonOppositeTool.Kaon_P1_Cal" : 0.912,
        "TaggerKaonOppositeTool.Kaon_Eta_Cal" : 0.355,

        "TaggerKaonSameTool.KaonSame_Pt_cut" : 550,
        "TaggerKaonSameTool.KaonSame_P_cut" : 4200,
        "TaggerKaonSameTool.KaonSame_IP_cut" : 3.5,
        "TaggerKaonSameTool.KaonSame_lcs_cut" : 2.75,
        "TaggerKaonSameTool.KaonSame_Phi_cut" : 0.8,
        "TaggerKaonSameTool.KaonSame_Eta_cut" : 0.8,
        "TaggerKaonSameTool.KaonSame_dQ_cut" : 2000,
        "TaggerKaonSameTool.KaonSame_PIDk_cut" : 6.5,
        "TaggerKaonSameTool.KaonSame_PIDkp_cut" : -1.,
        "TaggerKaonSameTool.KaonSame_ipPU_cut" : 3.0,
        "TaggerKaonSameTool.KaonSame_distPhi_cut" : 0.005,
        "TaggerKaonSameTool.KaonSame_ProbMin" : 0.51,
        "TaggerKaonSameTool.KaonSame_AverageOmega" : 0.33,
        "TaggerKaonSameTool.KaonSame_P0_Cal" : 0.,
        "TaggerKaonSameTool.KaonSame_P1_Cal" : 1.,
        "TaggerKaonSameTool.KaonSame_Eta_Cal" : 0.,

        "TaggerPionSameTool.PionSame_Pt_cut" : 750,
        "TaggerPionSameTool.PionSame_P_cut" : 5000,
        "TaggerPionSameTool.PionSame_IPs_cut" : 3.5,
        "TaggerPionSameTool.PionSame_lcs_cut" : 5.0,
        "TaggerPionSameTool.PionSame_deta_max_cut" :  5.,
        "TaggerPionSameTool.PionSame_deta_min_cut" : -5.,
        "TaggerPionSameTool.PionSame_dphi_cut" : 50.,
        "TaggerPionSameTool.PionSame_dR_cut" : 50.,
        "TaggerPionSameTool.PionSame_dQ_cut" : 2500,
        "TaggerPionSameTool.PionSame_dQ_extra_cut" : 1500,
        "TaggerPionSameTool.PionSame_ghost_cut" : -999.,
        "TaggerPionSameTool.PionSame_PIDNoK_cut" : 3.0,
        "TaggerPionSameTool.PionSame_PIDNoP_cut" : 10.0,
        "TaggerPionSameTool.PionSame_ipPU_cut" : 3.0,
        "TaggerPionSameTool.PionSame_distPhi_cut" : 0.005,
        "TaggerPionSameTool.PionSame_ProbMin" : 0.53,
        "TaggerPionSameTool.PionSame_AverageOmega" : 0.41,
        "TaggerPionSameTool.PionSame_P0_Cal" : 0.414,
        "TaggerPionSameTool.PionSame_P1_Cal" : 1.002,
        "TaggerPionSameTool.PionSame_Eta_Cal" : 0.40685,

        "TaggerVertexChargeTool.Vtx_PowerK" : 0.4,
        "TaggerVertexChargeTool.Vtx_MinimumVCharge" : 0.17,
        "TaggerVertexChargeTool.Vtx_ProbMin" : 0.53,
        "TaggerVertexChargeTool.Vtx_AverageOmega" : 0.41,
        "TaggerVertexChargeTool.Vtx_P0_Cal" : 0.391,
        "TaggerVertexChargeTool.Vtx_P1_Cal" : 0.660,
        "TaggerVertexChargeTool.Vtx_Eta_Cal" : 0.3989
        },
    
    # data: optimization with Bd2DstarMuNu and Bu2JpsiK+(OSK) and calibration (Bu2JpsiK+)
    'Moriond2011' : { 
        "IPPU_cut" : 3.0,
        "distphi_cut" : 0.005,
        
        "CombineTaggersProbability.P0_Cal_OS" : 0.363,
        "CombineTaggersProbability.P1_Cal_OS" : 1.09,
        "CombineTaggersProbability.Eta_Cal_OS" : 0.335,
        # calibration for combination with nnetOSK  -- no calibration
        "CombineTaggersProbability.P0_Cal_OSnnet" : 0.,
        "CombineTaggersProbability.P1_Cal_OSnnet" : 1.,
        "CombineTaggersProbability.Eta_Cal_OSnnet" : 0.,

        "TaggerMuonTool.Muon_Pt_cut" : 1100,
        "TaggerMuonTool.Muon_P_cut" : 0.0,
        "TaggerMuonTool.Muon_IPs_cut" : 0.0,
        "TaggerMuonTool.Muon_lcs_cut" : 2.2,
        "TaggerMuonTool.Muon_PIDm_cut" : 2.0,
        "TaggerMuonTool.Muon_ipPU_cut" : 3.0,
        "TaggerMuonTool.Muon_distPhi_cut" : 0.005,
        "TaggerMuonTool.Muon_ProbMin" : 0.0,
        "TaggerMuonTool.Muon_AverageOmega" : 0.33,
        "TaggerMuonTool.Muon_P0_Cal" : 0.311,
        "TaggerMuonTool.Muon_P1_Cal" : 1.36,
        "TaggerMuonTool.Muon_Eta_Cal" : 0.316,

        "TaggerElectronTool.Ele_Pt_cut" : 1000,
        "TaggerElectronTool.Ele_P_cut" : 0.0,
        "TaggerElectronTool.Ele_IPs_cut" : 2.0,
        "TaggerElectronTool.Ele_lcs_cut" : 5.0,
        "TaggerElectronTool.Ele_ghost_cut" : -999.,
        "TaggerElectronTool.Ele_VeloChargeMin" : 0.0,
        "TaggerElectronTool.Ele_VeloChargeMax" : 1.6,
        "TaggerElectronTool.Ele_EoverP" : 0.6,
        "TaggerElectronTool.Ele_PIDe_cut" : 4.0,
        "TaggerElectronTool.Ele_ipPU_cut" : 3.0,
        "TaggerElectronTool.Ele_distPhi_cut" : 0.03,
        "TaggerElectronTool.Ele_ProbMin" : 0.0,
        "TaggerElectronTool.Ele_AverageOmega" : 0.33,
        "TaggerElectronTool.Ele_P0_Cal" : 0.291,
        "TaggerElectronTool.Ele_P1_Cal" : 1.47,
        "TaggerElectronTool.Ele_Eta_Cal" : 0.341,

        "TaggerKaonOppositeTool.Kaon_Pt_cut" : 800,
        "TaggerKaonOppositeTool.Kaon_P_cut" : 5875,
        "TaggerKaonOppositeTool.Kaon_IPs_cut" : 4.05,
        "TaggerKaonOppositeTool.Kaon_IP_cut" : 1.25,
        "TaggerKaonOppositeTool.Kaon_lcs_cut" : 2.125,
        "TaggerKaonOppositeTool.Kaon_ghost_cut" : -999.,
        "TaggerKaonOppositeTool.Kaon_PIDk_cut" : 6.5,
        "TaggerKaonOppositeTool.Kaon_PIDkp_cut" : -3.5,
        "TaggerKaonOppositeTool.Kaon_ipPU_cut" : 4.72,
        "TaggerKaonOppositeTool.Kaon_distPhi_cut" : -0.005,
        "TaggerKaonOppositeTool.Kaon_ProbMin" : 0.51,
        "TaggerKaonOppositeTool.Kaon_AverageOmega" : 0.33,
        "TaggerKaonOppositeTool.Kaon_P0_Cal" : 0.359,
        "TaggerKaonOppositeTool.Kaon_P1_Cal" : 0.89,
        "TaggerKaonOppositeTool.Kaon_Eta_Cal" : 0.359,

        "TaggerKaonSameTool.KaonSame_Pt_cut" : 550,
        "TaggerKaonSameTool.KaonSame_P_cut" : 4200,
        "TaggerKaonSameTool.KaonSame_IP_cut" : 3.5,
        "TaggerKaonSameTool.KaonSame_lcs_cut" : 2.75,
        "TaggerKaonSameTool.KaonSame_Phi_cut" : 0.8,
        "TaggerKaonSameTool.KaonSame_Eta_cut" : 0.8,
        "TaggerKaonSameTool.KaonSame_dQ_cut" : 2000,
        "TaggerKaonSameTool.KaonSame_PIDk_cut" : 6.5,
        "TaggerKaonSameTool.KaonSame_PIDkp_cut" : -1.,
        "TaggerKaonSameTool.KaonSame_ipPU_cut" : 3.0,
        "TaggerKaonSameTool.KaonSame_distPhi_cut" : 0.005,
        "TaggerKaonSameTool.KaonSame_ProbMin" : 0.51,
        "TaggerKaonSameTool.KaonSame_AverageOmega" : 0.33,
        "TaggerKaonSameTool.KaonSame_P0_Cal" : 0.,
        "TaggerKaonSameTool.KaonSame_P1_Cal" : 1.,
        "TaggerKaonSameTool.KaonSame_Eta_Cal" : 0.,

        "TaggerPionSameTool.PionSame_Pt_cut" : 750,
        "TaggerPionSameTool.PionSame_P_cut" : 5000,
        "TaggerPionSameTool.PionSame_IPs_cut" : 3.5,
        "TaggerPionSameTool.PionSame_lcs_cut" : 5.0,
        "TaggerPionSameTool.PionSame_deta_max_cut" :  5.,
        "TaggerPionSameTool.PionSame_deta_min_cut" : -5.,
        "TaggerPionSameTool.PionSame_dphi_cut" : 50.,
        "TaggerPionSameTool.PionSame_dR_cut" : 50.,
        "TaggerPionSameTool.PionSame_dQ_cut" : 2500,
        "TaggerPionSameTool.PionSame_dQ_extra_cut" : 1500,
        "TaggerPionSameTool.PionSame_ghost_cut" : -999.,
        "TaggerPionSameTool.PionSame_PIDNoK_cut" : 3.0,
        "TaggerPionSameTool.PionSame_PIDNoP_cut" : 10.0,
        "TaggerPionSameTool.PionSame_ipPU_cut" : 3.0,
        "TaggerPionSameTool.PionSame_distPhi_cut" : 0.005,
        "TaggerPionSameTool.PionSame_ProbMin" : 0.53,
        "TaggerPionSameTool.PionSame_AverageOmega" : 0.41,
        "TaggerPionSameTool.PionSame_P0_Cal" : 0.425,
        "TaggerPionSameTool.PionSame_P1_Cal" : 0.89,
        "TaggerPionSameTool.PionSame_Eta_Cal" : 0.415,

        "TaggerVertexChargeTool.Vtx_PowerK" : 0.4,
        "TaggerVertexChargeTool.Vtx_MinimumVCharge" : 0.17,
        "TaggerVertexChargeTool.Vtx_ProbMin" : 0.53,
        "TaggerVertexChargeTool.Vtx_AverageOmega" : 0.41,
        "TaggerVertexChargeTool.Vtx_P0_Cal" : 0.451,
        "TaggerVertexChargeTool.Vtx_P1_Cal" : 0.358,
        "TaggerVertexChargeTool.Vtx_Eta_Cal" : 0.398
        },
    
    # data: optimization with Bd2DstarMuNu and Bu2JpsiK+(OSK), no calibration
    'Moriond2011_nocal' : { 
        "IPPU_cut" : 3.0,
        "distphi_cut" : 0.005,
        
        "CombineTaggersProbability.P0_Cal_OS" : 0.,
        "CombineTaggersProbability.P1_Cal_OS" : 1.,
        "CombineTaggersProbability.Eta_Cal_OS" : 0.,
        # calibration for combination with nnetOSK  -- no calibration
        "CombineTaggersProbability.P0_Cal_OSnnet" : 0.,
        "CombineTaggersProbability.P1_Cal_OSnnet" : 1.,
        "CombineTaggersProbability.Eta_Cal_OSnnet" : 0.,

        "TaggerMuonTool.Muon_Pt_cut" : 1100,
        "TaggerMuonTool.Muon_P_cut" : 0.0,
        "TaggerMuonTool.Muon_IPs_cut" : 0.0,
        "TaggerMuonTool.Muon_lcs_cut" : 2.2,
        "TaggerMuonTool.Muon_PIDm_cut" : 2.0,
        "TaggerMuonTool.Muon_ipPU_cut" : 3.0,
        "TaggerMuonTool.Muon_distPhi_cut" : 0.005,
        "TaggerMuonTool.Muon_ProbMin" : 0.0,
        "TaggerMuonTool.Muon_AverageOmega" : 0.33,
        "TaggerMuonTool.Muon_P0_Cal" : 0.,
        "TaggerMuonTool.Muon_P1_Cal" : 1.,
        "TaggerMuonTool.Muon_Eta_Cal" : 0.,

        "TaggerElectronTool.Ele_Pt_cut" : 1000,
        "TaggerElectronTool.Ele_P_cut" : 0.0,
        "TaggerElectronTool.Ele_IPs_cut" : 2.0,
        "TaggerElectronTool.Ele_lcs_cut" : 5.0,
        "TaggerElectronTool.Ele_ghost_cut" : -999.,
        "TaggerElectronTool.Ele_VeloChargeMin" : 0.0,
        "TaggerElectronTool.Ele_VeloChargeMax" : 1.6,
        "TaggerElectronTool.Ele_EoverP" : 0.6,
        "TaggerElectronTool.Ele_PIDe_cut" : 4.0,
        "TaggerElectronTool.Ele_ipPU_cut" : 3.0,
        "TaggerElectronTool.Ele_distPhi_cut" : 0.03,
        "TaggerElectronTool.Ele_ProbMin" : 0.0,
        "TaggerElectronTool.Ele_AverageOmega" : 0.33,
        "TaggerElectronTool.Ele_P0_Cal" : 0.,
        "TaggerElectronTool.Ele_P1_Cal" : 1.,
        "TaggerElectronTool.Ele_Eta_Cal" : 0.,

        "TaggerKaonOppositeTool.Kaon_Pt_cut" : 800,
        "TaggerKaonOppositeTool.Kaon_P_cut" : 5875,
        "TaggerKaonOppositeTool.Kaon_IPs_cut" : 4.05,
        "TaggerKaonOppositeTool.Kaon_IP_cut" : 1.25,
        "TaggerKaonOppositeTool.Kaon_lcs_cut" : 2.125,
        "TaggerKaonOppositeTool.Kaon_ghost_cut" : -999.,
        "TaggerKaonOppositeTool.Kaon_PIDk_cut" : 6.5,
        "TaggerKaonOppositeTool.Kaon_PIDkp_cut" : -3.5,
        "TaggerKaonOppositeTool.Kaon_ipPU_cut" : 4.72,
        "TaggerKaonOppositeTool.Kaon_distPhi_cut" : -0.005,
        "TaggerKaonOppositeTool.Kaon_ProbMin" : 0.51,
        "TaggerKaonOppositeTool.Kaon_AverageOmega" : 0.33,
        "TaggerKaonOppositeTool.Kaon_P0_Cal" : 0.,
        "TaggerKaonOppositeTool.Kaon_P1_Cal" : 1.,
        "TaggerKaonOppositeTool.Kaon_Eta_Cal" : 0.,

        "TaggerKaonSameTool.KaonSame_Pt_cut" : 550,
        "TaggerKaonSameTool.KaonSame_P_cut" : 4200,
        "TaggerKaonSameTool.KaonSame_IP_cut" : 3.5,
        "TaggerKaonSameTool.KaonSame_lcs_cut" : 2.75,
        "TaggerKaonSameTool.KaonSame_Phi_cut" : 0.8,
        "TaggerKaonSameTool.KaonSame_Eta_cut" : 0.8,
        "TaggerKaonSameTool.KaonSame_dQ_cut" : 2000,
        "TaggerKaonSameTool.KaonSame_PIDk_cut" : 6.5,
        "TaggerKaonSameTool.KaonSame_PIDkp_cut" : -1.,
        "TaggerKaonSameTool.KaonSame_ipPU_cut" : 3.0,
        "TaggerKaonSameTool.KaonSame_distPhi_cut" : 0.005,
        "TaggerKaonSameTool.KaonSame_ProbMin" : 0.51,
        "TaggerKaonSameTool.KaonSame_AverageOmega" : 0.33,
        "TaggerKaonSameTool.KaonSame_P0_Cal" : 0.,
        "TaggerKaonSameTool.KaonSame_P1_Cal" : 1.,
        "TaggerKaonSameTool.KaonSame_Eta_Cal" : 0.,

        "TaggerPionSameTool.PionSame_Pt_cut" : 750,
        "TaggerPionSameTool.PionSame_P_cut" : 5000,
        "TaggerPionSameTool.PionSame_IPs_cut" : 3.5,
        "TaggerPionSameTool.PionSame_lcs_cut" : 5.0,
        "TaggerPionSameTool.PionSame_deta_max_cut" :  5.,
        "TaggerPionSameTool.PionSame_deta_min_cut" : -5.,
        "TaggerPionSameTool.PionSame_dphi_cut" : 50.,
        "TaggerPionSameTool.PionSame_dR_cut" : 50.,
        "TaggerPionSameTool.PionSame_dQ_cut" : 2500,
        "TaggerPionSameTool.PionSame_dQ_extra_cut" : 1500,
        "TaggerPionSameTool.PionSame_ghost_cut" : -999.,
        "TaggerPionSameTool.PionSame_PIDNoK_cut" : 3.0,
        "TaggerPionSameTool.PionSame_PIDNoP_cut" : 10.0,
        "TaggerPionSameTool.PionSame_ipPU_cut" : 3.0,
        "TaggerPionSameTool.PionSame_distPhi_cut" : 0.005,
        "TaggerPionSameTool.PionSame_ProbMin" : 0.53,
        "TaggerPionSameTool.PionSame_AverageOmega" : 0.41,
        "TaggerPionSameTool.PionSame_P0_Cal" : 0.,
        "TaggerPionSameTool.PionSame_P1_Cal" : 1.,
        "TaggerPionSameTool.PionSame_Eta_Cal" : 0.,

        "TaggerVertexChargeTool.Vtx_PowerK" : 0.4,
        "TaggerVertexChargeTool.Vtx_MinimumVCharge" : 0.17,
        "TaggerVertexChargeTool.Vtx_ProbMin" : 0.53,
        "TaggerVertexChargeTool.Vtx_AverageOmega" : 0.41,
        "TaggerVertexChargeTool.Vtx_P0_Cal" : 0.,
        "TaggerVertexChargeTool.Vtx_P1_Cal" : 1.,
        "TaggerVertexChargeTool.Vtx_Eta_Cal" : 0.
        },
    
    # data: optimization with Bd2DstarMuNu (also OSK) and calibration (Bu2JpsiK+)
    'Moriond2011_oldOSK' : { 
        "IPPU_cut" : 3.0,
        "distphi_cut" : 0.005,
        
        "CombineTaggersProbability.P0_Cal_OS" : 0.374,
        "CombineTaggersProbability.P1_Cal_OS" : 1.08,
        "CombineTaggersProbability.Eta_Cal_OS" : 0.355,
        # calibration for combination with nnetOSK  -- no calibration
        "CombineTaggersProbability.P0_Cal_OSnnet" : 0.,
        "CombineTaggersProbability.P1_Cal_OSnnet" : 1.,
        "CombineTaggersProbability.Eta_Cal_OSnnet" : 0.,

        "TaggerMuonTool.Muon_Pt_cut" : 1100,
        "TaggerMuonTool.Muon_P_cut" : 0.0,
        "TaggerMuonTool.Muon_IPs_cut" : 0.0,
        "TaggerMuonTool.Muon_lcs_cut" : 2.2,
        "TaggerMuonTool.Muon_PIDm_cut" : 2.0,
        "TaggerMuonTool.Muon_ipPU_cut" : 3.0,
        "TaggerMuonTool.Muon_distPhi_cut" : 0.005,
        "TaggerMuonTool.Muon_ProbMin" : 0.0,
        "TaggerMuonTool.Muon_AverageOmega" : 0.33,
        "TaggerMuonTool.Muon_P0_Cal" : 0.311,
        "TaggerMuonTool.Muon_P1_Cal" : 1.36,
        "TaggerMuonTool.Muon_Eta_Cal" : 0.316,

        "TaggerElectronTool.Ele_Pt_cut" : 1000,
        "TaggerElectronTool.Ele_P_cut" : 0.0,
        "TaggerElectronTool.Ele_IPs_cut" : 2.0,
        "TaggerElectronTool.Ele_lcs_cut" : 5.0,
        "TaggerElectronTool.Ele_ghost_cut" : -999.,
        "TaggerElectronTool.Ele_VeloChargeMin" : 0.0,
        "TaggerElectronTool.Ele_VeloChargeMax" : 1.6,
        "TaggerElectronTool.Ele_EoverP" : 0.6,
        "TaggerElectronTool.Ele_PIDe_cut" : 4.0,
        "TaggerElectronTool.Ele_ipPU_cut" : 3.0,
        "TaggerElectronTool.Ele_distPhi_cut" : 0.03,
        "TaggerElectronTool.Ele_ProbMin" : 0.0,
        "TaggerElectronTool.Ele_AverageOmega" : 0.33,
        "TaggerElectronTool.Ele_P0_Cal" : 0.291,
        "TaggerElectronTool.Ele_P1_Cal" : 1.47,
        "TaggerElectronTool.Ele_Eta_Cal" : 0.341,

        "TaggerKaonOppositeTool.Kaon_Pt_cut" : 800,
        "TaggerKaonOppositeTool.Kaon_P_cut" : 4000,
        "TaggerKaonOppositeTool.Kaon_IPs_cut" : 4.,
        "TaggerKaonOppositeTool.Kaon_IP_cut" : 1.5,
        "TaggerKaonOppositeTool.Kaon_lcs_cut" : 2.7,
        "TaggerKaonOppositeTool.Kaon_ghost_cut" : -999.,
        "TaggerKaonOppositeTool.Kaon_PIDk_cut" : 5.,
        "TaggerKaonOppositeTool.Kaon_PIDkp_cut" : -4.,
        "TaggerKaonOppositeTool.Kaon_ipPU_cut" : 6.,
        "TaggerKaonOppositeTool.Kaon_distPhi_cut" : 0.005,
        "TaggerKaonOppositeTool.Kaon_ProbMin" : 0.51,
        "TaggerKaonOppositeTool.Kaon_AverageOmega" : 0.33,
        "TaggerKaonOppositeTool.Kaon_P0_Cal" : 0.421,
        "TaggerKaonOppositeTool.Kaon_P1_Cal" : 0.89,
        "TaggerKaonOppositeTool.Kaon_Eta_Cal" : 0.364,

        "TaggerKaonSameTool.KaonSame_Pt_cut" : 550,
        "TaggerKaonSameTool.KaonSame_P_cut" : 4200,
        "TaggerKaonSameTool.KaonSame_IP_cut" : 3.5,
        "TaggerKaonSameTool.KaonSame_lcs_cut" : 2.75,
        "TaggerKaonSameTool.KaonSame_Phi_cut" : 0.8,
        "TaggerKaonSameTool.KaonSame_Eta_cut" : 0.8,
        "TaggerKaonSameTool.KaonSame_dQ_cut" : 2000,
        "TaggerKaonSameTool.KaonSame_PIDk_cut" : 6.5,
        "TaggerKaonSameTool.KaonSame_PIDkp_cut" : -1.,
        "TaggerKaonSameTool.KaonSame_ipPU_cut" : 3.0,
        "TaggerKaonSameTool.KaonSame_distPhi_cut" : 0.005,
        "TaggerKaonSameTool.KaonSame_ProbMin" : 0.51,
        "TaggerKaonSameTool.KaonSame_AverageOmega" : 0.33,
        "TaggerKaonSameTool.KaonSame_P0_Cal" : 0.,
        "TaggerKaonSameTool.KaonSame_P1_Cal" : 1.,
        "TaggerKaonSameTool.KaonSame_Eta_Cal" : 0.,

        "TaggerPionSameTool.PionSame_Pt_cut" : 750,
        "TaggerPionSameTool.PionSame_P_cut" : 5000,
        "TaggerPionSameTool.PionSame_IPs_cut" : 3.5,
        "TaggerPionSameTool.PionSame_lcs_cut" : 5.0,
        "TaggerPionSameTool.PionSame_deta_max_cut" :  5.,
        "TaggerPionSameTool.PionSame_deta_min_cut" : -5.,
        "TaggerPionSameTool.PionSame_dphi_cut" : 50.,
        "TaggerPionSameTool.PionSame_dR_cut" : 50.,
        "TaggerPionSameTool.PionSame_dQ_cut" : 2500,
        "TaggerPionSameTool.PionSame_dQ_extra_cut" : 1500,
        "TaggerPionSameTool.PionSame_ghost_cut" : -999.,
        "TaggerPionSameTool.PionSame_PIDNoK_cut" : 3.0,
        "TaggerPionSameTool.PionSame_PIDNoP_cut" : 10.0,
        "TaggerPionSameTool.PionSame_ipPU_cut" : 3.0,
        "TaggerPionSameTool.PionSame_distPhi_cut" : 0.005,
        "TaggerPionSameTool.PionSame_ProbMin" : 0.53,
        "TaggerPionSameTool.PionSame_AverageOmega" : 0.41,
        "TaggerPionSameTool.PionSame_P0_Cal" : 0.425,
        "TaggerPionSameTool.PionSame_P1_Cal" : 0.89,
        "TaggerPionSameTool.PionSame_Eta_Cal" : 0.415,

        "TaggerVertexChargeTool.Vtx_PowerK" : 0.4,
        "TaggerVertexChargeTool.Vtx_MinimumVCharge" : 0.17,
        "TaggerVertexChargeTool.Vtx_ProbMin" : 0.53,
        "TaggerVertexChargeTool.Vtx_AverageOmega" : 0.41,
        "TaggerVertexChargeTool.Vtx_P0_Cal" : 0.451,
        "TaggerVertexChargeTool.Vtx_P1_Cal" : 0.358,
        "TaggerVertexChargeTool.Vtx_Eta_Cal" : 0.398
        },
    
    # data: optimization with Bd2DstarMuNu (also OSK), no calibration
    'Moriond2011_oldOSK_nocal' : { 
        "IPPU_cut" : 3.0,
        "distphi_cut" : 0.005,
        
        "CombineTaggersProbability.P0_Cal_OS" : 0.,
        "CombineTaggersProbability.P1_Cal_OS" : 1.,
        "CombineTaggersProbability.Eta_Cal_OS" : 0.,
        # calibration for combination with nnetOSK  -- no calibration
        "CombineTaggersProbability.P0_Cal_OSnnet" : 0.,
        "CombineTaggersProbability.P1_Cal_OSnnet" : 1.,
        "CombineTaggersProbability.Eta_Cal_OSnnet" : 0.,

        "TaggerMuonTool.Muon_Pt_cut" : 1100,
        "TaggerMuonTool.Muon_P_cut" : 0.0,
        "TaggerMuonTool.Muon_IPs_cut" : 0.0,
        "TaggerMuonTool.Muon_lcs_cut" : 2.2,
        "TaggerMuonTool.Muon_PIDm_cut" : 2.0,
        "TaggerMuonTool.Muon_ipPU_cut" : 3.0,
        "TaggerMuonTool.Muon_distPhi_cut" : 0.005,
        "TaggerMuonTool.Muon_ProbMin" : 0.0,
        "TaggerMuonTool.Muon_AverageOmega" : 0.33,
        "TaggerMuonTool.Muon_P0_Cal" : 0.,
        "TaggerMuonTool.Muon_P1_Cal" : 1.,
        "TaggerMuonTool.Muon_Eta_Cal" : 0.,

        "TaggerElectronTool.Ele_Pt_cut" : 1000,
        "TaggerElectronTool.Ele_P_cut" : 0.0,
        "TaggerElectronTool.Ele_IPs_cut" : 2.0,
        "TaggerElectronTool.Ele_lcs_cut" : 5.0,
        "TaggerElectronTool.Ele_ghost_cut" : -999.,
        "TaggerElectronTool.Ele_VeloChargeMin" : 0.0,
        "TaggerElectronTool.Ele_VeloChargeMax" : 1.6,
        "TaggerElectronTool.Ele_EoverP" : 0.6,
        "TaggerElectronTool.Ele_PIDe_cut" : 4.0,
        "TaggerElectronTool.Ele_ipPU_cut" : 3.0,
        "TaggerElectronTool.Ele_distPhi_cut" : 0.03,
        "TaggerElectronTool.Ele_ProbMin" : 0.0,
        "TaggerElectronTool.Ele_AverageOmega" : 0.33,
        "TaggerElectronTool.Ele_P0_Cal" : 0.,
        "TaggerElectronTool.Ele_P1_Cal" : 1.,
        "TaggerElectronTool.Ele_Eta_Cal" : 0.,

        "TaggerKaonOppositeTool.Kaon_Pt_cut" : 800,
        "TaggerKaonOppositeTool.Kaon_P_cut" : 4000,
        "TaggerKaonOppositeTool.Kaon_IPs_cut" : 4.,
        "TaggerKaonOppositeTool.Kaon_IP_cut" : 1.5,
        "TaggerKaonOppositeTool.Kaon_lcs_cut" : 2.7,
        "TaggerKaonOppositeTool.Kaon_ghost_cut" : -999.,
        "TaggerKaonOppositeTool.Kaon_PIDk_cut" : 5.,
        "TaggerKaonOppositeTool.Kaon_PIDkp_cut" : -4.,
        "TaggerKaonOppositeTool.Kaon_ipPU_cut" : 6.,
        "TaggerKaonOppositeTool.Kaon_distPhi_cut" : 0.005,
        "TaggerKaonOppositeTool.Kaon_ProbMin" : 0.51,
        "TaggerKaonOppositeTool.Kaon_AverageOmega" : 0.33,
        "TaggerKaonOppositeTool.Kaon_P0_Cal" : 0.,
        "TaggerKaonOppositeTool.Kaon_P1_Cal" : 1.,
        "TaggerKaonOppositeTool.Kaon_Eta_Cal" : 0.,

        "TaggerKaonSameTool.KaonSame_Pt_cut" : 550,
        "TaggerKaonSameTool.KaonSame_P_cut" : 4200,
        "TaggerKaonSameTool.KaonSame_IP_cut" : 3.5,
        "TaggerKaonSameTool.KaonSame_lcs_cut" : 2.75,
        "TaggerKaonSameTool.KaonSame_Phi_cut" : 0.8,
        "TaggerKaonSameTool.KaonSame_Eta_cut" : 0.8,
        "TaggerKaonSameTool.KaonSame_dQ_cut" : 2000,
        "TaggerKaonSameTool.KaonSame_PIDk_cut" : 6.5,
        "TaggerKaonSameTool.KaonSame_PIDkp_cut" : -1.,
        "TaggerKaonSameTool.KaonSame_ipPU_cut" : 3.0,
        "TaggerKaonSameTool.KaonSame_distPhi_cut" : 0.005,
        "TaggerKaonSameTool.KaonSame_ProbMin" : 0.51,
        "TaggerKaonSameTool.KaonSame_AverageOmega" : 0.33,
        "TaggerKaonSameTool.KaonSame_P0_Cal" : 0.,
        "TaggerKaonSameTool.KaonSame_P1_Cal" : 1.,
        "TaggerKaonSameTool.KaonSame_Eta_Cal" : 0.,

        "TaggerPionSameTool.PionSame_Pt_cut" : 750,
        "TaggerPionSameTool.PionSame_P_cut" : 5000,
        "TaggerPionSameTool.PionSame_IPs_cut" : 3.5,
        "TaggerPionSameTool.PionSame_lcs_cut" : 5.0,
        "TaggerPionSameTool.PionSame_deta_max_cut" :  5.,
        "TaggerPionSameTool.PionSame_deta_min_cut" : -5.,
        "TaggerPionSameTool.PionSame_dphi_cut" : 50.,
        "TaggerPionSameTool.PionSame_dR_cut" : 50.,
        "TaggerPionSameTool.PionSame_dQ_cut" : 2500,
        "TaggerPionSameTool.PionSame_dQ_extra_cut" : 1500,
        "TaggerPionSameTool.PionSame_ghost_cut" : -999.,
        "TaggerPionSameTool.PionSame_PIDNoK_cut" : 3.0,
        "TaggerPionSameTool.PionSame_PIDNoP_cut" : 10.0,
        "TaggerPionSameTool.PionSame_ipPU_cut" : 3.0,
        "TaggerPionSameTool.PionSame_distPhi_cut" : 0.005,
        "TaggerPionSameTool.PionSame_ProbMin" : 0.53,
        "TaggerPionSameTool.PionSame_AverageOmega" : 0.41,
        "TaggerPionSameTool.PionSame_P0_Cal" : 0.,
        "TaggerPionSameTool.PionSame_P1_Cal" : 1.,
        "TaggerPionSameTool.PionSame_Eta_Cal" : 0.,

        "TaggerVertexChargeTool.Vtx_PowerK" : 0.4,
        "TaggerVertexChargeTool.Vtx_MinimumVCharge" : 0.17,
        "TaggerVertexChargeTool.Vtx_ProbMin" : 0.53,
        "TaggerVertexChargeTool.Vtx_AverageOmega" : 0.41,
        "TaggerVertexChargeTool.Vtx_P0_Cal" : 0.,
        "TaggerVertexChargeTool.Vtx_P1_Cal" : 1.,
        "TaggerVertexChargeTool.Vtx_Eta_Cal" : 0.
        }
    
    }

def TuneTool(tool, version, fttoolname = 'BTaggingTool'):

    from Configurables import BTagging, BTaggingTool, TaggerMuonTool, TaggerElectronTool, TaggerKaonSameTool, TaggerNEWKaonOppositeTool, TaggerKaonOppositeTool, TaggerPionSameTool, TaggerNEWKaonSameTool, TaggerCharmTool, TaggerVertexChargeTool, CombineTaggersProbability, SVertexOneSeedTool,NNetTool_MLP, TaggingUtils
        


    tool.addTool ( BTaggingTool, name=fttoolname)
    fttool = eval('tool.%s' % fttoolname)
    fttool.addTool(TaggingUtils, name = 'TaggingUtils')
    fttool.addTool ( CombineTaggersProbability )
    fttool.addTool ( TaggerMuonTool )
    fttool.TaggerMuonTool.addTool(NNetTool_MLP)
    fttool.TaggerMuonTool.addTool(TaggingUtils, name = 'TaggingUtils')
    fttool.addTool ( TaggerElectronTool )
    fttool.TaggerElectronTool.addTool(NNetTool_MLP)
    fttool.TaggerElectronTool.addTool(TaggingUtils, name = 'TaggingUtils')
    fttool.addTool ( TaggerKaonOppositeTool )
    fttool.TaggerKaonOppositeTool.addTool(NNetTool_MLP)
    fttool.TaggerKaonOppositeTool.addTool(TaggingUtils, name = 'TaggingUtils')
    fttool.addTool ( TaggerKaonSameTool )
    fttool.TaggerKaonSameTool.addTool(NNetTool_MLP)
    fttool.TaggerKaonSameTool.addTool(TaggingUtils, name = 'TaggingUtils')
    fttool.addTool ( TaggerPionSameTool )
    fttool.TaggerPionSameTool.addTool(NNetTool_MLP)
    fttool.TaggerPionSameTool.addTool(TaggingUtils, name = 'TaggingUtils')
    fttool.addTool ( TaggerVertexChargeTool )
    fttool.TaggerVertexChargeTool.addTool(NNetTool_MLP)
    fttool.TaggerVertexChargeTool.addTool(TaggingUtils, name = 'TaggingUtils')
    fttool.TaggerVertexChargeTool.addTool( SVertexOneSeedTool )
    fttool.TaggerVertexChargeTool.SVertexOneSeedTool.addTool(TaggingUtils, name = 'TaggingUtils')
    fttool.addTool ( TaggerNEWKaonOppositeTool )
    fttool.TaggerNEWKaonOppositeTool.addTool(TaggingUtils, name = 'TaggingUtils')
    fttool.addTool ( TaggerNEWKaonSameTool )
    fttool.TaggerNEWKaonSameTool.addTool(TaggingUtils, name = 'TaggingUtils')
    fttool.addTool ( TaggerCharmTool )
    fttool.TaggerCharmTool.addTool(NNetTool_MLP)
    fttool.TaggerCharmTool.addTool(TaggingUtils, name = 'TaggingUtils')
    fttool.addTool ( TaggingUtils )

    for cut in tunings[version]:
        exec("fttool."+cut+'='+str(tunings[version][cut]))

    """
    toollist = [TaggerMuonTool, TaggerElectronTool, TaggerKaonOppositeTool,  TaggerVertexChargeTool, TaggerNEWKaonOppositeTool, TaggerNEWKaonSameTool, TaggerPionSameTool, TaggerCharmTool]
    for tool in toollist :
        exec("fttool."+tool+".addTool(TaggingUtils, name = 'TaggingUtils')")
        exec("fttool."+tool+".TaggingUtils.Personality = fttool.Personality")
        print   "fttool."+tool+".TaggingUtils.Personality, " = ", fttool.Personality"
    """
    fttool.TaggerMuonTool.Personality = fttool.Personality
    fttool.TaggerElectronTool.Personality = fttool.Personality
    fttool.TaggerKaonOppositeTool.Personality = fttool.Personality
    #fttool.TaggerKaonSameTool.Personality = fttool.Personality  # not yet implemented
    #fttool.TaggerPionSameTool.Personality = fttool.Personality  # not yet implemented
    #fttool.TaggerNEWKaonSameTool.Personality = fttool.Personality # not yet implemented
    fttool.TaggerNEWKaonOppositeTool.Personality = fttool.Personality
    fttool.TaggerVertexChargeTool.Personality = fttool.Personality
    fttool.TaggerVertexChargeTool.SVertexOneSeedTool.Personality = fttool.Personality
    #
    fttool.TaggingUtils.Personality = fttool.Personality
    fttool.TaggerMuonTool.TaggingUtils.Personality = fttool.Personality
    fttool.TaggerElectronTool.TaggingUtils.Personality = fttool.Personality
    fttool.TaggerKaonOppositeTool.TaggingUtils.Personality = fttool.Personality
    fttool.TaggerKaonSameTool.TaggingUtils.Personality = fttool.Personality
    fttool.TaggerPionSameTool.TaggingUtils.Personality = fttool.Personality
    fttool.TaggerNEWKaonSameTool.TaggingUtils.Personality = fttool.Personality
    fttool.TaggerNEWKaonOppositeTool.TaggingUtils.Personality = fttool.Personality
    fttool.TaggerVertexChargeTool.TaggingUtils.Personality = fttool.Personality
    fttool.TaggerVertexChargeTool.SVertexOneSeedTool.TaggingUtils.Personality = fttool.Personality
    fttool.TaggerCharmTool.TaggingUtils.Personality = fttool.Personality
