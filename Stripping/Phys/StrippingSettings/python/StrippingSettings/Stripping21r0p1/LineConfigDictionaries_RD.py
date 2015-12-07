################################################################################
##                          S T R I P P I N G  21r0p1                         ##
##                                                                            ##
##  Configuration for RD WG                                                   ##
##  Contact persons: Pablo Ruiz Valls pruizval@cern.ch                        ##
##                   Guido Andreassi guido.andreassi@cern.ch                  ##
################################################################################

B23Mu = {
    "BUILDERTYPE": "B23MuLinesConf", 
    "CONFIG": {
        "B23MuPrescale": 1, 
        "B23PiPrescale": 1, 
        "B2MueePrescale": 1, 
        "CommonRelInfoTools": [
            {
                "Location": "VtxIsoInfo", 
                "Type": "RelInfoVertexIsolation"
            }, 
            {
                "Location": "VtxIsoInfoBDT", 
                "Type": "RelInfoVertexIsolationBDT"
            }, 
            {
                "Location": "BsMuMuBIsolation", 
                "Type": "RelInfoBs2MuMuBIsolations", 
                "Variables": [], 
                "makeTrackCuts": False, 
                "tracktype": 3
            }
        ], 
        "MaxDoca": 0.3, 
        "MaxIPChi2": 25, 
        "MaxTrChi2Dof": 4.0, 
        "MaxTrGhp": 0.4, 
        "MaxVtxChi2Dof": 9, 
        "MinDira": 0.0, 
        "MinTau": 0.0, 
        "MinTrIPChi2": 25.0, 
        "MinVDChi2": 225, 
        "Postscale": 1, 
        "mDiffBuLoose": 500, 
        "mDiffBuTight": 100
    }, 
    "STREAMS": [ "Leptonic" ], 
    "WGs": [ "RD" ]
}

B2KstTauTau = {
    "BUILDERTYPE": "B2KstTauXConf", 
    "CONFIG": {
        "AMAXDOCA_D": "0.2", 
        "APT_D": "800", 
        "B2DDSL_LinePostscale": 1, 
        "B2DDSL_LinePrescale": 1, 
        "B2KstMuMu_LinePostscale": 1, 
        "B2KstMuMu_LinePrescale": 1, 
        "B2KstMuMu_SameSign_LinePostscale": 1, 
        "B2KstMuMu_SameSign_LinePrescale": 1, 
        "B2KstTauMu_LinePostscale": 1, 
        "B2KstTauMu_LinePrescale": 1, 
        "B2KstTauMu_SameSign_LinePostscale": 1, 
        "B2KstTauMu_SameSign_LinePrescale": 0.5, 
        "B2KstTauTau_LinePostscale": 1, 
        "B2KstTauTau_LinePrescale": 1, 
        "B2KstTauTau_SameSign_LinePostscale": 1, 
        "B2KstTauTau_SameSign_LinePrescale": 1, 
        "B_COSDIRA_KMM": "0.999", 
        "DIRA_D": "0.99", 
        "DOCAMAX_KST_KMM": "0.15", 
        "FDCHI2OWNPV_KST_KMM": "120", 
        "FDCHI2_B": "80", 
        "FDCHI2_D": "16", 
        "FD_B_Max_KTM": "70", 
        "FD_B_Max_KTT": "40", 
        "FD_B_Mu": "3", 
        "FD_B_Mu_KMM": "3", 
        "FD_Kst_Mu_KMM": "3", 
        "IPCHI2_HAD_ALL_FINAL_STATE": "16", 
        "MASS_HIGH_B": "10000", 
        "MASS_HIGH_B_KMM": "10000", 
        "MASS_HIGH_D": "1900", 
        "MASS_HIGH_Ds": "1998", 
        "MASS_HIGH_Kst": "1100", 
        "MASS_LOW_B": "2000", 
        "MASS_LOW_B_KMM": "1500", 
        "MASS_LOW_D": "1840", 
        "MASS_LOW_Ds": "1938", 
        "MASS_LOW_Kst": "700", 
        "MINIPCHI2_KST_KMM": "3", 
        "MINIPCHI2_K_KMM": "4", 
        "MINIPCHI2_MU_KMM": "4", 
        "MINIPCHI2_PI_KMM": "4", 
        "MaxPT_D": "800", 
        "PT_B_KMM": "2000", 
        "PT_B_KTM": "3000", 
        "PT_B_KTT": "3000", 
        "PT_D": "1000", 
        "PT_HAD_ALL_FINAL_STATE": "250", 
        "PT_Kst": "1000", 
        "PT_MU": "1000", 
        "PT_MU_KMM": "800", 
        "P_HAD_ALL_FINAL_STATE": "2000", 
        "RelatedInfoTools": [
            {
                "Location": "B2KstTauTau_MuonIsolationBDT", 
                "Type": "RelInfoBKsttautauMuonIsolationBDT", 
                "Variables": [
                    "BKSTTAUTAUMUONISOBDTFIRSTVALUETAUP", 
                    "BKSTTAUTAUMUONISOBDTSECONDVALUETAUP", 
                    "BKSTTAUTAUMUONISOBDTTHIRDVALUETAUP", 
                    "BKSTTAUTAUMUONISOBDTFIRSTVALUETAUM", 
                    "BKSTTAUTAUMUONISOBDTSECONDVALUETAUM", 
                    "BKSTTAUTAUMUONISOBDTTHIRDVALUETAUM"
                ]
            }, 
            {
                "Location": "B2KstTauTau_TauIsolationBDT", 
                "Type": "RelInfoBKsttautauTauIsolationBDT", 
                "Variables": [
                    "BKSTTAUTAUTAUISOBDTFIRSTVALUETAUP", 
                    "BKSTTAUTAUTAUISOBDTSECONDVALUETAUP", 
                    "BKSTTAUTAUTAUISOBDTTHIRDVALUETAUP", 
                    "BKSTTAUTAUTAUISOBDTFIRSTVALUETAUM", 
                    "BKSTTAUTAUTAUISOBDTSECONDVALUETAUM", 
                    "BKSTTAUTAUTAUISOBDTTHIRDVALUETAUM", 
                    "BKSTTAUTAUTAUISOBDTFIRSTVALUEKST", 
                    "BKSTTAUTAUTAUISOBDTSECONDVALUEKST", 
                    "BKSTTAUTAUTAUISOBDTTHIRDVALUEKST"
                ]
            }, 
            {
                "Location": "B2KstTauTau_TrackIsolationBDT", 
                "Type": "RelInfoBKsttautauTrackIsolationBDT", 
                "Variables": [
                    "BKSTTAUTAUTRKISOBDTFIRSTVALUETAUPPIM", 
                    "BKSTTAUTAUTRKISOBDTSECONDVALUETAUPPIM", 
                    "BKSTTAUTAUTRKISOBDTTHIRDVALUETAUPPIM", 
                    "BKSTTAUTAUTRKISOBDTFIRSTVALUETAUPPIP1", 
                    "BKSTTAUTAUTRKISOBDTSECONDVALUETAUPPIP1", 
                    "BKSTTAUTAUTRKISOBDTTHIRDVALUETAUPPIP1", 
                    "BKSTTAUTAUTRKISOBDTFIRSTVALUETAUPPIP2", 
                    "BKSTTAUTAUTRKISOBDTSECONDVALUETAUPPIP2", 
                    "BKSTTAUTAUTRKISOBDTTHIRDVALUETAUPPIP2", 
                    "BKSTTAUTAUTRKISOBDTFIRSTVALUETAUMPIP", 
                    "BKSTTAUTAUTRKISOBDTSECONDVALUETAUMPIP", 
                    "BKSTTAUTAUTRKISOBDTTHIRDVALUETAUMPIP", 
                    "BKSTTAUTAUTRKISOBDTFIRSTVALUETAUMPIM1", 
                    "BKSTTAUTAUTRKISOBDTSECONDVALUETAUMPIM1", 
                    "BKSTTAUTAUTRKISOBDTTHIRDVALUETAUMPIM1", 
                    "BKSTTAUTAUTRKISOBDTFIRSTVALUETAUMPIM2", 
                    "BKSTTAUTAUTRKISOBDTSECONDVALUETAUMPIM2", 
                    "BKSTTAUTAUTRKISOBDTTHIRDVALUETAUMPIM2", 
                    "BKSTTAUTAUTRKISOBDTFIRSTVALUEKSTK", 
                    "BKSTTAUTAUTRKISOBDTSECONDVALUEKSTK", 
                    "BKSTTAUTAUTRKISOBDTTHIRDVALUEKSTK", 
                    "BKSTTAUTAUTRKISOBDTFIRSTVALUEKSTPI", 
                    "BKSTTAUTAUTRKISOBDTSECONDVALUEKSTPI", 
                    "BKSTTAUTAUTRKISOBDTTHIRDVALUEKSTPI"
                ]
            }, 
            {
                "Location": "B2KstTauTau_CDFIso", 
                "Type": "RelInfoBstautauCDFIso"
            }
        ], 
        "SpdMult": "600", 
        "TRACKCHI2_HAD_ALL_FINAL_STATE": "4", 
        "TRACKCHI2_MU": "4", 
        "TRGHOPROB_HAD_ALL_FINAL_STATE": "0.4", 
        "VCHI2_B": "100", 
        "VCHI2_B_Mu": "150", 
        "VCHI2_B_Mu_KMM": "100", 
        "VCHI2_D": "16", 
        "VCHI2_Kst": "15", 
        "VDRHOmax_D": "7.0", 
        "VDRHOmin_D": "0.1", 
        "VDZ_D": "5.0"
    }, 
    "STREAMS": [ "Bhadron" ], 
    "WGs": [ "RD" ]
}

B2Lambda0Mu = {
    "BUILDERTYPE": "B2Lambda0MuLines", 
    "CONFIG": {
        "BDIRA": 0.99, 
        "BVCHI2DOF": 4.0, 
        "GEC_nLongTrk": 300.0, 
        "Lambda0DaugMIPChi2": 10.0, 
        "Lambda0DaugP": 2000.0, 
        "Lambda0DaugPT": 250.0, 
        "Lambda0DaugTrackChi2": 4.0, 
        "Lambda0PT": 700.0, 
        "Lambda0VertexChi2": 10.0, 
        "LambdaMuMassLowTight": 1500.0, 
        "LambdaZ": 0.0, 
        "MajoranaCutFDChi2": 100.0, 
        "MajoranaCutM": 1500.0, 
        "MuonGHOSTPROB": 0.5, 
        "MuonMINIPCHI2": 12, 
        "MuonP": 3000.0, 
        "MuonPIDK": 0.0, 
        "MuonPIDmu": 0.0, 
        "MuonPIDp": 0.0, 
        "MuonPT": 250.0, 
        "MuonTRCHI2": 4.0, 
        "XMuMassUpperHigh": 6500.0
    }, 
    "STREAMS": [ "Dimuon" ], 
    "WGs": [ "RD" ]
}

B2XLL = {
    "BUILDERTYPE": "B2XLLConf", 
    "CONFIG": {
        "BDIRA": 0.9995, 
        "BFlightCHI2": 100, 
        "BIPCHI2": 25, 
        "BMassWindow": 1500, 
        "BVertexCHI2": 9, 
        "DiHadronMass": 2600, 
        "DiLeptonFDCHI2": 16, 
        "DiLeptonIPCHI2": 0, 
        "DiLeptonPT": 0, 
        "KaonIPCHI2": 9, 
        "KaonPT": 400, 
        "LeptonIPCHI2": 9, 
        "LeptonPT": 300, 
        "PIDe": 0, 
        "TrChi2DOF": 4, 
        "TrGhostProb": 0.4, 
        "UpperMass": 5500, 
        "eeXLinePrescale": 1, 
        "eeXSSLinePrescale": 1, 
        "meXLinePrescale": 1, 
        "meXSSLinePrescale": 1, 
        "mmXLinePrescale": 1, 
        "mmXSSLinePrescale": 1
    }, 
    "STREAMS": [ "Leptonic" ], 
    "WGs": [ "RD" ]
}

B2XMuMu = {
    "BUILDERTYPE": "B2XMuMuConf", 
    "CONFIG": {
        "A1_Comb_MassHigh": 5550.0, 
        "A1_Comb_MassLow": 0.0, 
        "A1_Dau_MaxIPCHI2": 9.0, 
        "A1_FlightChi2": 25.0, 
        "A1_MassHigh": 5500.0, 
        "A1_MassLow": 0.0, 
        "A1_MinIPCHI2": 4.0, 
        "A1_VtxChi2": 10.0, 
        "B_Comb_MassHigh": 7100.0, 
        "B_Comb_MassLow": 4600.0, 
        "B_DIRA": 0.9999, 
        "B_Dau_MaxIPCHI2": 9.0, 
        "B_FlightCHI2": 121.0, 
        "B_IPCHI2": 16.0, 
        "B_MassHigh": 7000.0, 
        "B_MassLow": 4700.0, 
        "B_VertexCHI2": 8.0, 
        "DECAYS": [
            "[B0 -> J/psi(1S) K*(892)0]cc", 
            "[B+ -> J/psi(1S) rho(770)+]cc", 
            "[B+ -> J/psi(1S) K+]cc", 
            "[B+ -> J/psi(1S) K*(892)+]cc", 
            "B0 -> J/psi(1S) pi0"
        ], 
        "Dau_DIRA": -0.9, 
        "Dau_VertexCHI2": 12.0, 
        "Dimu_Dau_MaxIPCHI2": 9.0, 
        "Dimu_FlightChi2": 9.0, 
        "DimuonUPPERMASS": 7100.0, 
        "DimuonWS": True, 
        "DplusLOWERMASS": 1600.0, 
        "DplusUPPERMASS": 2300.0, 
        "HLT1_FILTER": None, 
        "HLT2_FILTER": None, 
        "HadronWS": True, 
        "Hadron_MinIPCHI2": 6.0, 
        "K12OmegaK_CombMassHigh": 2000, 
        "K12OmegaK_CombMassLow": 400, 
        "K12OmegaK_MassHigh": 2100, 
        "K12OmegaK_MassLow": 300, 
        "K12OmegaK_VtxChi2": 10, 
        "KpiVXCHI2NDOF": 9.0, 
        "KsWINDOW": 30.0, 
        "Kstar_Comb_MassHigh": 6200.0, 
        "Kstar_Comb_MassLow": 0.0, 
        "Kstar_Dau_MaxIPCHI2": 9.0, 
        "Kstar_FlightChi2": 9.0, 
        "Kstar_MassHigh": 6200.0, 
        "Kstar_MassLow": 0.0, 
        "Kstar_MinIPCHI2": 0.0, 
        "KstarplusWINDOW": 300.0, 
        "L0DU_FILTER": None, 
        "LambdaWINDOW": 30.0, 
        "LongLivedPT": 0.0, 
        "LongLivedTau": 2, 
        "MuonNoPIDs_PIDmu": 0.0, 
        "MuonPID": -3.0, 
        "Muon_IsMuon": True, 
        "Muon_MinIPCHI2": 9.0, 
        "OmegaChi2Prob": 1e-05, 
        "Omega_CombMassWin": 200, 
        "Omega_MassWin": 100, 
        "Pi0ForOmegaMINPT": 500.0, 
        "Pi0MINPT": 700.0, 
        "RelatedInfoTools": [
            {
                "Location": "ConeIsoInfo", 
                "Type": "RelInfoConeVariables", 
                "Variables": [
                    "CONEANGLE", 
                    "CONEMULT", 
                    "CONEPTASYM", 
                    "CONEPT", 
                    "CONEP", 
                    "CONEPASYM", 
                    "CONEDELTAETA", 
                    "CONEDELTAPHI"
                ]
            }, 
            {
                "ConeSize": 1.0, 
                "Location": "ConeIsoInfoCCNC", 
                "Type": "RelInfoConeIsolation", 
                "Variables": [
                    "CC_ANGLE", 
                    "CC_MULT", 
                    "CC_SPT", 
                    "CC_VPT", 
                    "CC_PX", 
                    "CC_PY", 
                    "CC_PZ", 
                    "CC_PASYM", 
                    "CC_PTASYM", 
                    "CC_PXASYM", 
                    "CC_PYASYM", 
                    "CC_PZASYM", 
                    "CC_DELTAETA", 
                    "CC_DELTAPHI", 
                    "CC_IT", 
                    "CC_MAXPT_Q", 
                    "CC_MAXPT_PT", 
                    "CC_MAXPT_PX", 
                    "CC_MAXPT_PY", 
                    "CC_MAXPT_PZ", 
                    "CC_MAXPT_PE", 
                    "NC_ANGLE", 
                    "NC_MULT", 
                    "NC_SPT", 
                    "NC_VPT", 
                    "NC_PX", 
                    "NC_PY", 
                    "NC_PZ", 
                    "NC_PASYM", 
                    "NC_PTASYM", 
                    "NC_PXASYM", 
                    "NC_PYASYM", 
                    "NC_PZASYM", 
                    "NC_DELTAETA", 
                    "NC_DELTAPHI", 
                    "NC_IT", 
                    "NC_MAXPT_PT", 
                    "NC_MAXPT_PX", 
                    "NC_MAXPT_PY", 
                    "NC_MAXPT_PZ"
                ]
            }, 
            {
                "Location": "VtxIsoInfo", 
                "Type": "RelInfoVertexIsolation", 
                "Variables": [
                    "VTXISONUMVTX", 
                    "VTXISODCHI2ONETRACK", 
                    "VTXISODCHI2MASSONETRACK", 
                    "VTXISODCHI2TWOTRACK", 
                    "VTXISODCHI2MASSTWOTRACK"
                ]
            }, 
            {
                "Location": "VtxIsoBDTInfo", 
                "Type": "RelInfoVertexIsolationBDT"
            }
        ], 
        "SpdMult": 600, 
        "Track_GhostProb": 0.5, 
        "UseNoPIDsHadrons": True
    }, 
    "STREAMS": [ "Leptonic" ], 
    "WGs": [ "RD" ]
}

B2XTauMu = {
    "BUILDERTYPE": "B2XTauMuConf", 
    "CONFIG": {
        "BDIRA_K": 0.99, 
        "BDIRA_K_3pi": 0.999, 
        "BDIRA_kst": 0.95, 
        "BVCHI2DOF_K": 15.0, 
        "BVCHI2DOF_K_3pi": 4.0, 
        "BVCHI2DOF_kst": 25.0, 
        "BVCHI2DOF_phi": 10000000000.0, 
        "B_FDCHI2_K": 400.0, 
        "B_FDCHI2_kst": 80, 
        "B_MAX_MASS": 10000, 
        "B_MAX_MASS_K_3pi": 10000, 
        "B_MIN_MASS": 2000, 
        "B_MIN_MASS_K_3pi": 3000, 
        "B_PT_K": 3000.0, 
        "KMuPT_K": 1000.0, 
        "KMuSumPT_K": 2000.0, 
        "KPiPT_K": 800.0, 
        "KPiPT_kst": 500.0, 
        "KaonPIDK": 4.0, 
        "KaonPIDK_K": 5, 
        "KaonPIDK_K_3pi": 6, 
        "KaonP_K": 3.0, 
        "KaonP_K_3pi": 6.0, 
        "KaonP_kst": 2.0, 
        "KstAMassWin": 180.0, 
        "KstMassWin": 150.0, 
        "KstVCHI2DOF": 15.0, 
        "MINIPCHI2": 16.0, 
        "MINIPCHI2_K": 36.0, 
        "MuTauWS": True, 
        "MuonPT_K": 800.0, 
        "MuonPT_kst": 500.0, 
        "MuonP_K": 5.0, 
        "MuonP_K_3pi": 6.0, 
        "MuonP_kst": 2.0, 
        "PIDmu": 2.0, 
        "PhiAMassWin": 30.0, 
        "PhiMassWin": 25.0, 
        "PhiVCHI2DOF": 20.0, 
        "PionPIDK": 0.0, 
        "PionP_kst": 2.0, 
        "Prescale": 1, 
        "Prescale_WS": 0.5, 
        "RelatedInfoTools": [
            {
                "Location": "VertexIsoInfo", 
                "Type": "RelInfoVertexIsolation"
            }, 
            {
                "Location": "VertexIsoBDTInfo", 
                "Type": "RelInfoVertexIsolationBDT"
            }, 
            {
                "ConeAngle": 1.0, 
                "Location": "ConeVarsInfo", 
                "Type": "RelInfoConeVariables"
            }, 
            {
                "ConeSize": 0.5, 
                "DaughterLocations": {
                    "[Beauty -> (X+ -> (X0 -> X+ ^X-) l+) l+]CC ": "ConeIsoInfoH2", 
                    "[Beauty -> (X+ -> (X0 -> X+ ^X-) l+) l-]CC ": "ConeIsoInfoH2", 
                    "[Beauty -> (X+ -> (X0 -> ^X+ X-) l+) l+]CC ": "ConeIsoInfoH1", 
                    "[Beauty -> (X+ -> (X0 -> ^X+ X-) l+) l-]CC ": "ConeIsoInfoH1", 
                    "[Beauty -> (X+ -> X+ ^l+) l+]CC ": "ConeIsoInfoL1", 
                    "[Beauty -> (X+ -> X+ ^l+) l-]CC ": "ConeIsoInfoL1", 
                    "[Beauty -> (X+ -> X+ l+) ^l+]CC ": "ConeIsoInfoL2", 
                    "[Beauty -> (X+ -> X+ l+) ^l-]CC ": "ConeIsoInfoL2", 
                    "[Beauty -> (X+ -> X0 ^l+) l+]CC ": "ConeIsoInfoL1", 
                    "[Beauty -> (X+ -> X0 ^l+) l-]CC ": "ConeIsoInfoL1", 
                    "[Beauty -> (X+ -> X0 l+) ^l+]CC ": "ConeIsoInfoL2", 
                    "[Beauty -> (X+ -> X0 l+) ^l-]CC ": "ConeIsoInfoL2", 
                    "[Beauty -> (X+ -> ^X+ l+) l+]CC ": "ConeIsoInfoH", 
                    "[Beauty -> (X+ -> ^X+ l+) l-]CC ": "ConeIsoInfoH", 
                    "[Beauty -> (X0 -> X+ ^l-) l+]CC ": "ConeIsoInfoL1", 
                    "[Beauty -> (X0 -> X+ ^l-) l-]CC ": "ConeIsoInfoL1", 
                    "[Beauty -> (X0 -> X+ l-) ^l+]CC ": "ConeIsoInfoL2", 
                    "[Beauty -> (X0 -> X+ l-) ^l-]CC ": "ConeIsoInfoL2", 
                    "[Beauty -> (X0 -> ^X+ l-) l+]CC ": "ConeIsoInfoH", 
                    "[Beauty -> (X0 -> ^X+ l-) l-]CC ": "ConeIsoInfoH", 
                    "[Beauty -> (X0-> X+ X-) ^l+ l+]CC ": "ConeIsoInfoL1", 
                    "[Beauty -> (X0-> X+ X-) ^l+ l-]CC ": "ConeIsoInfoL1", 
                    "[Beauty -> (X0-> X+ X-) l+ ^l+]CC ": "ConeIsoInfoL2", 
                    "[Beauty -> (X0-> X+ X-) l+ ^l-]CC ": "ConeIsoInfoL2", 
                    "[Beauty -> (X0-> X+ ^X-) l+ l+]CC ": "ConeIsoInfoH1", 
                    "[Beauty -> (X0-> X+ ^X-) l+ l-]CC ": "ConeIsoInfoH1", 
                    "[Beauty -> (X0-> ^X+ X-) l+ l+]CC ": "ConeIsoInfoH1", 
                    "[Beauty -> (X0-> ^X+ X-) l+ l-]CC ": "ConeIsoInfoH1"
                }, 
                "IgnoreUnmatchedDescriptors": True, 
                "Type": "RelInfoConeIsolation"
            }, 
            {
                "ConeAngle": 0.5, 
                "DaughterLocations": {
                    "[Beauty -> (X+ -> (X0 -> X+ ^X-) l+) l+]CC ": "ConeVarsInfoH2", 
                    "[Beauty -> (X+ -> (X0 -> X+ ^X-) l+) l-]CC ": "ConeVarsInfoH2", 
                    "[Beauty -> (X+ -> (X0 -> ^X+ X-) l+) l+]CC ": "ConeVarsInfoH1", 
                    "[Beauty -> (X+ -> (X0 -> ^X+ X-) l+) l-]CC ": "ConeVarsInfoH1", 
                    "[Beauty -> (X+ -> X+ ^l+) l+]CC ": "ConeVarsInfoL1", 
                    "[Beauty -> (X+ -> X+ ^l+) l-]CC ": "ConeVarsInfoL1", 
                    "[Beauty -> (X+ -> X+ l+) ^l+]CC ": "ConeVarsInfoL2", 
                    "[Beauty -> (X+ -> X+ l+) ^l-]CC ": "ConeVarsInfoL2", 
                    "[Beauty -> (X+ -> X0 ^l+) l+]CC ": "ConeVarsInfoL1", 
                    "[Beauty -> (X+ -> X0 ^l+) l-]CC ": "ConeVarsInfoL1", 
                    "[Beauty -> (X+ -> X0 l+) ^l+]CC ": "ConeVarsInfoL2", 
                    "[Beauty -> (X+ -> X0 l+) ^l-]CC ": "ConeVarsInfoL2", 
                    "[Beauty -> (X+ -> ^X+ l+) l+]CC ": "ConeVarsInfoH", 
                    "[Beauty -> (X+ -> ^X+ l+) l-]CC ": "ConeVarsInfoH", 
                    "[Beauty -> (X0 -> X+ ^l-) l+]CC ": "ConeVarsInfoL1", 
                    "[Beauty -> (X0 -> X+ ^l-) l-]CC ": "ConeVarsInfoL1", 
                    "[Beauty -> (X0 -> X+ l-) ^l+]CC ": "ConeVarsInfoL2", 
                    "[Beauty -> (X0 -> X+ l-) ^l-]CC ": "ConeVarsInfoL2", 
                    "[Beauty -> (X0 -> ^X+ l-) l+]CC ": "ConeVarsInfoH", 
                    "[Beauty -> (X0 -> ^X+ l-) l-]CC ": "ConeVarsInfoH", 
                    "[Beauty -> (X0-> X+ X-) ^l+ l+]CC ": "ConeVarsInfoL1", 
                    "[Beauty -> (X0-> X+ X-) ^l+ l-]CC ": "ConeVarsInfoL1", 
                    "[Beauty -> (X0-> X+ X-) l+ ^l+]CC ": "ConeVarsInfoL2", 
                    "[Beauty -> (X0-> X+ X-) l+ ^l-]CC ": "ConeVarsInfoL2", 
                    "[Beauty -> (X0-> X+ ^X-) l+ l+]CC ": "ConeVarsInfoH1", 
                    "[Beauty -> (X0-> X+ ^X-) l+ l-]CC ": "ConeVarsInfoH1", 
                    "[Beauty -> (X0-> ^X+ X-) l+ l+]CC ": "ConeVarsInfoH1", 
                    "[Beauty -> (X0-> ^X+ X-) l+ l-]CC ": "ConeVarsInfoH1"
                }, 
                "IgnoreUnmatchedDescriptors": True, 
                "Type": "RelInfoConeVariables"
            }, 
            {
                "DaughterLocations": {
                    "[Beauty -> (X+ -> (X0 -> X+ ^X-) l+) l+]CC ": "TrackIsoBDTInfoH2", 
                    "[Beauty -> (X+ -> (X0 -> X+ ^X-) l+) l-]CC ": "TrackIsoBDTInfoH2", 
                    "[Beauty -> (X+ -> (X0 -> ^X+ X-) l+) l+]CC ": "TrackIsoBDTInfoH1", 
                    "[Beauty -> (X+ -> (X0 -> ^X+ X-) l+) l-]CC ": "TrackIsoBDTInfoH1", 
                    "[Beauty -> (X+ -> X+ ^l+) l+]CC ": "TrackIsoBDTInfoL1", 
                    "[Beauty -> (X+ -> X+ ^l+) l-]CC ": "TrackIsoBDTInfoL1", 
                    "[Beauty -> (X+ -> X+ l+) ^l+]CC ": "TrackIsoBDTInfoL2", 
                    "[Beauty -> (X+ -> X+ l+) ^l-]CC ": "TrackIsoBDTInfoL2", 
                    "[Beauty -> (X+ -> X0 ^l+) l+]CC ": "TrackIsoBDTInfoL1", 
                    "[Beauty -> (X+ -> X0 ^l+) l-]CC ": "TrackIsoBDTInfoL1", 
                    "[Beauty -> (X+ -> X0 l+) ^l+]CC ": "TrackIsoBDTInfoL2", 
                    "[Beauty -> (X+ -> X0 l+) ^l-]CC ": "TrackIsoBDTInfoL2", 
                    "[Beauty -> (X+ -> ^X+ l+) l+]CC ": "TrackIsoBDTInfoH", 
                    "[Beauty -> (X+ -> ^X+ l+) l-]CC ": "TrackIsoBDTInfoH", 
                    "[Beauty -> (X0 -> X+ ^l-) l+]CC ": "TrackIsoBDTInfoL1", 
                    "[Beauty -> (X0 -> X+ ^l-) l-]CC ": "TrackIsoBDTInfoL1", 
                    "[Beauty -> (X0 -> X+ l-) ^l+]CC ": "TrackIsoBDTInfoL2", 
                    "[Beauty -> (X0 -> X+ l-) ^l-]CC ": "TrackIsoBDTInfoL2", 
                    "[Beauty -> (X0 -> ^X+ l-) l+]CC ": "TrackIsoBDTInfoH", 
                    "[Beauty -> (X0 -> ^X+ l-) l-]CC ": "TrackIsoBDTInfoH", 
                    "[Beauty -> (X0-> X+ X-) ^l+ l+]CC ": "TrackIsoBDTInfoL1", 
                    "[Beauty -> (X0-> X+ X-) ^l+ l-]CC ": "TrackIsoBDTInfoL1", 
                    "[Beauty -> (X0-> X+ X-) l+ ^l+]CC ": "TrackIsoBDTInfoL2", 
                    "[Beauty -> (X0-> X+ X-) l+ ^l-]CC ": "TrackIsoBDTInfoL2", 
                    "[Beauty -> (X0-> X+ ^X-) l+ l+]CC ": "TrackIsoBDTInfoH1", 
                    "[Beauty -> (X0-> X+ ^X-) l+ l-]CC ": "TrackIsoBDTInfoH1", 
                    "[Beauty -> (X0-> ^X+ X-) l+ l+]CC ": "TrackIsoBDTInfoH1", 
                    "[Beauty -> (X0-> ^X+ X-) l+ l-]CC ": "TrackIsoBDTInfoH1"
                }, 
                "IgnoreUnmatchedDescriptors": True, 
                "Type": "RelInfoTrackIsolationBDT"
            }, 
            {
                "DaughterLocations": {
                    "[Beauty -> (X+ -> (X0 -> X+ ^X-) l+) l+]CC ": "TrackIsoBsMMInfoH2", 
                    "[Beauty -> (X+ -> (X0 -> X+ ^X-) l+) l-]CC ": "TrackIsoBsMMInfoH2", 
                    "[Beauty -> (X+ -> (X0 -> ^X+ X-) l+) l+]CC ": "TrackIsoBsMMInfoH1", 
                    "[Beauty -> (X+ -> (X0 -> ^X+ X-) l+) l-]CC ": "TrackIsoBsMMInfoH1", 
                    "[Beauty -> (X+ -> X+ ^l+) l+]CC ": "TrackIsoBsMMInfoL1", 
                    "[Beauty -> (X+ -> X+ ^l+) l-]CC ": "TrackIsoBsMMInfoL1", 
                    "[Beauty -> (X+ -> X+ l+) ^l+]CC ": "TrackIsoBsMMInfoL2", 
                    "[Beauty -> (X+ -> X+ l+) ^l-]CC ": "TrackIsoBsMMInfoL2", 
                    "[Beauty -> (X+ -> X0 ^l+) l+]CC ": "TrackIsoBsMMInfoL1", 
                    "[Beauty -> (X+ -> X0 ^l+) l-]CC ": "TrackIsoBsMMInfoL1", 
                    "[Beauty -> (X+ -> X0 l+) ^l+]CC ": "TrackIsoBsMMInfoL2", 
                    "[Beauty -> (X+ -> X0 l+) ^l-]CC ": "TrackIsoBsMMInfoL2", 
                    "[Beauty -> (X+ -> ^X+ l+) l+]CC ": "TrackIsoBsMMInfoH", 
                    "[Beauty -> (X+ -> ^X+ l+) l-]CC ": "TrackIsoBsMMInfoH", 
                    "[Beauty -> (X0 -> X+ ^l-) l+]CC ": "TrackIsoBsMMInfoL1", 
                    "[Beauty -> (X0 -> X+ ^l-) l-]CC ": "TrackIsoBsMMInfoL1", 
                    "[Beauty -> (X0 -> X+ l-) ^l+]CC ": "TrackIsoBsMMInfoL2", 
                    "[Beauty -> (X0 -> X+ l-) ^l-]CC ": "TrackIsoBsMMInfoL2", 
                    "[Beauty -> (X0 -> ^X+ l-) l+]CC ": "TrackIsoBsMMInfoH", 
                    "[Beauty -> (X0 -> ^X+ l-) l-]CC ": "TrackIsoBsMMInfoH", 
                    "[Beauty -> (X0-> X+ X-) ^l+ l+]CC ": "TrackIsoBsMMInfoL1", 
                    "[Beauty -> (X0-> X+ X-) ^l+ l-]CC ": "TrackIsoBsMMInfoL1", 
                    "[Beauty -> (X0-> X+ X-) l+ ^l+]CC ": "TrackIsoBsMMInfoL2", 
                    "[Beauty -> (X0-> X+ X-) l+ ^l-]CC ": "TrackIsoBsMMInfoL2", 
                    "[Beauty -> (X0-> X+ ^X-) l+ l+]CC ": "TrackIsoBsMMInfoH1", 
                    "[Beauty -> (X0-> X+ ^X-) l+ l-]CC ": "TrackIsoBsMMInfoH1", 
                    "[Beauty -> (X0-> ^X+ X-) l+ l+]CC ": "TrackIsoBsMMInfoH1", 
                    "[Beauty -> (X0-> ^X+ X-) l+ l-]CC ": "TrackIsoBsMMInfoH1"
                }, 
                "IgnoreUnmatchedDescriptors": True, 
                "IsoTwoBody": False, 
                "Type": "RelInfoBs2MuMuTrackIsolations", 
                "angle": 0.27, 
                "doca_iso": 0.13, 
                "fc": 0.6, 
                "ips": 3.0, 
                "makeTrackCuts": False, 
                "pvdis": 0.5, 
                "pvdis_h": 40.0, 
                "svdis": -0.15, 
                "svdis_h": 30.0, 
                "tracktype": 3
            }
        ], 
        "TrackCHI2": 3, 
        "TrackGhostProb": 0.5, 
        "XMuVCHI2DOF_K": 9.0, 
        "XMuVCHI2DOF_kst": 15.0, 
        "XMuVCHI2DOF_phi": 20.0
    }, 
    "STREAMS": {
        "Dimuon": [
            "StrippingB2XTauMu_KstLine", 
            "StrippingB2XTauMu_Kst_WSLine", 
            "StrippingB2XTauMu_K_3piLine", 
            "StrippingB2XTauMu_K_3pi_WSLine"
        ], 
        "Leptonic": [
            "StrippingB2XTauMu_PhiLine", 
            "StrippingB2XTauMu_KLine", 
            "StrippingB2XTauMu_Phi_3piLine", 
            "StrippingB2XTauMu_Phi_WSLine", 
            "StrippingB2XTauMu_K_WSLine", 
            "StrippingB2XTauMu_Phi_3pi_WSLine", 
            "StrippingB2XTauMu_K_3pi_looseLine", 
            "StrippingB2XTauMu_K_3pi_loose_WSLine"
        ]
    }, 
    "WGs": [ "RD" ]
}

BLV = {
    "BUILDERTYPE": "BLVLinesConf", 
    "CONFIG": {
        "B2DpiPrescale": 0.1, 
        "B2KhhPrescale": 1, 
        "B2LcmuPrescale": 1, 
        "B2LcpPrescale": 0.2, 
        "Bs2DspiPrescale": 0.1, 
        "CommonRelInfoTools": [
            {
                "Location": "VtxIsoInfo", 
                "Type": "RelInfoVertexIsolation"
            }, 
            {
                "Location": "VtxIsoInfoBDT", 
                "Type": "RelInfoVertexIsolationBDT"
            }, 
            {
                "Location": "BsMuMuBIsolation", 
                "Type": "RelInfoBs2MuMuBIsolations", 
                "Variables": [], 
                "makeTrackCuts": False, 
                "tracktype": 3
            }
        ], 
        "La2KmuPrescale": 1, 
        "Lb2DmuPrescale": 1, 
        "Lb2DsmuPrescale": 1, 
        "Lb2KmuPrescale": 1, 
        "Lb2LcpiPrescale": 0.1, 
        "MaxDoca": 0.3, 
        "MaxIPChi2": 25, 
        "MaxTrChi2Dof": 4.0, 
        "MaxTrGhp": 0.4, 
        "MaxVtxChi2Dof": 9, 
        "MinDira": 0.0, 
        "MinTauLambda": 10.0, 
        "MinTauLoose": 0.0, 
        "MinTauTight": 1.0, 
        "MinTrIPChi2": 25.0, 
        "MinVDChi2Loose": 100, 
        "MinVDChi2Tight": 225, 
        "Postscale": 1, 
        "Xib2KhmuPrescale": 1, 
        "mDiffD": 100.0, 
        "mDiffDs": 100.0, 
        "mDiffLa": 400.0, 
        "mDiffbLoose": 500.0, 
        "mDiffbStand": 400.0, 
        "mDiffbTight": 300.0, 
        "mKst": 1200.0
    }, 
    "STREAMS": [ "Bhadron" ], 
    "WGs": [ "RD" ]
}

Beauty2XGamma = {
    "BUILDERTYPE": "Beauty2XGammaConf", 
    "CONFIG": {
        "B2PhiOmega2pipipi0MPrescale": 1.0, 
        "B2PhiOmega2pipipi0RPrescale": 1.0, 
        "B2XG2pi2KsPrescale": 0.0, 
        "B2XG2piCNVDDPrescale": 0.0, 
        "B2XG2piCNVLLPrescale": 0.0, 
        "B2XG2piGGPrescale": 1.0, 
        "B2XG2piKsPrescale": 0.0, 
        "B2XG2piPrescale": 0.0, 
        "B2XG2pipi0MPrescale": 0.0, 
        "B2XG2pipi0RPrescale": 0.0, 
        "B2XG3piCNVDDPrescale": 0.0, 
        "B2XG3piCNVLLPrescale": 0.0, 
        "B2XG3piGGPrescale": 1.0, 
        "B2XG3piKsPrescale": 0.0, 
        "B2XG3piPrescale": 0.0, 
        "B2XG3pipi0MPrescale": 0.0, 
        "B2XG3pipi0RPrescale": 0.0, 
        "B2XG4piPrescale": 0.0, 
        "B2XGBMaxCorrM": 73000.0, 
        "B2XGBMaxM": 9000.0, 
        "B2XGBMinBPVDIRA": 0.0, 
        "B2XGBMinM2pi": 3280.0, 
        "B2XGBMinM3pi": 2900.0, 
        "B2XGBMinM4pi": 2560.0, 
        "B2XGBMinMLambda": 2560.0, 
        "B2XGBMinPT": 200.0, 
        "B2XGBSumPtMin": 5000, 
        "B2XGBVtxChi2DOF": 9.0, 
        "B2XGBVtxMaxIPChi2": 9.0, 
        "B2XGGammaCL": 0.0, 
        "B2XGGammaCNVPTMin": 1000.0, 
        "B2XGGammaPTMin": 2000.0, 
        "B2XGLambda2piPrescale": 0.0, 
        "B2XGLambda3piPrescale": 0.0, 
        "B2XGLambdapiPrescale": 0.0, 
        "B2XGLbLambdaPrescale": 0.0, 
        "B2XGPhiOmegaMaxMass": 1300.0, 
        "B2XGPhiOmegaMinMass": 700.0, 
        "B2XGResBPVVDCHI2Min": 0.0, 
        "B2XGResDocaMax": 100.5, 
        "B2XGResIPCHI2Min": 0.0, 
        "B2XGResMaxMass": 7900.0, 
        "B2XGResMinMass": 0.0, 
        "B2XGResMinPT": 150.0, 
        "B2XGResSumPtMin": 1500.0, 
        "B2XGResVtxChi2DOF": 10.0, 
        "B2XGTrkChi2DOF": 3.0, 
        "B2XGTrkMinIPChi2": 16.0, 
        "B2XGTrkMinP": 1000, 
        "B2XGTrkMinPT": 300.0, 
        "B2XGpiKsPrescale": 0.0, 
        "Pi0MPMin": 4000.0, 
        "Pi0MPTMin": 700.0, 
        "Pi0MPTReCut": 1200.0, 
        "Pi0RPMin": 4000.0, 
        "Pi0RPTMin": 700.0, 
        "Pi0RPTReCut": 1700.0, 
        "TISTOSLinesDict": {
            "Hlt2IncPhi.*Decision%TIS": 0, 
            "Hlt2IncPhi.*Decision%TOS": 0, 
            "Hlt2Radiative.*Decision%TIS": 0, 
            "Hlt2Radiative.*Decision%TOS": 0, 
            "Hlt2Topo(2|3|4)Body.*Decision%TIS": 0, 
            "Hlt2Topo(2|3|4)Body.*Decision%TOS": 0
        }, 
        "TrackGhostProb": 0.4
    }, 
    "STREAMS": [ "Leptonic" ], 
    "WGs": [ "RD" ]
}

Bs2MuMuLines = {
    "BUILDERTYPE": "Bs2MuMuLinesConf", 
    "CONFIG": {
        "BFDChi2_loose": 100, 
        "BIPChi2_loose": 64, 
        "BPVVDChi2": 121, 
        "B_BPVIPChi2": 25, 
        "B_BPVIPChi2_LTUB": 25, 
        "B_Pt": 350, 
        "B_Pt_LTUB": 500, 
        "B_maximum_decaytime_bsst": 0.2, 
        "B_minimum_decaytime_LTUB": 0.6, 
        "B_minimum_decaytime_bsst": 0.0, 
        "BdPrescale": 0, 
        "Bs2KKLTUBLinePrescale": 0, 
        "Bs2mmLTUBLinePrescale": 0, 
        "Bs2mmWideLinePrescale": 0, 
        "BsPrescale": 0, 
        "Bsst2mmLinePrescale": 1, 
        "BuPrescale": 0, 
        "DOCA": 0.3, 
        "DOCA_LTUB": 0.3, 
        "DOCA_loose": 0.5, 
        "DefaultLinePrescale": 0, 
        "DefaultPostscale": 0, 
        "JPsiLinePrescale": 0, 
        "JPsiLooseLinePrescale": 0.1, 
        "JPsiPromptLinePrescale": 0.005, 
        "LooseLinePrescale": 0.0, 
        "MuIPChi2_loose": 9, 
        "MuTrChi2_loose": 10, 
        "ProbNN": 0.4, 
        "SSPrescale": 0, 
        "SUMPT": 4500, 
        "TrackGhostProb": 0.4, 
        "TrackGhostProb_bsst": 0.3, 
        "VCHI2_VDOF": 9, 
        "VCHI2_VDOF_LTUB": 9, 
        "VCHI2_VDOF_loose": 25, 
        "daughter_IPChi2": 9, 
        "daughter_TrChi2": 4, 
        "daughter_TrChi2_LTUB": 4, 
        "daughter_TrChi2_bsst": 3, 
        "muon_PT_LTUB": 40
    }, 
    "STREAMS": {
        "Dimuon": [
            "StrippingBs2MuMuLinesNoMuIDLine", 
            "StrippingBs2MuMuLinesWideMassLine", 
            "StrippingBs2MuMuLinesBu2JPsiKLine", 
            "StrippingBs2MuMuLinesBsstLine"
        ], 
        "Leptonic": [
            "StrippingBs2MuMuLinesWideMassLine", 
            "StrippingBs2MuMuLinesBs2JPsiPhiLine", 
            "StrippingBs2MuMuLinesBs2KKLTUBLine", 
            "StrippingBs2MuMuLinesNoMuIDLine", 
            "StrippingBs2MuMuLinesSSLine", 
            "StrippingBs2MuMuLinesBd2JPsiKstLine", 
            "StrippingBs2MuMuLinesLTUBLine", 
            "StrippingBs2MuMuLinesBu2JPsiKLine"
        ]
    }, 
    "WGs": [ "RD" ]
}

Bs2st2KKMuX = {
    "BUILDERTYPE": "Bs2st2KKMuXConf", 
    "CONFIG": {
        "Bs2PT": 50.0, 
        "Bs2st2KKJpsiPrescale": 1.0, 
        "Bs2st2KKJpsiWSPrescale": 1.0, 
        "Bs2st2KKMuPrescale": 1.0, 
        "Bs2st2KKMuWSPrescale": 1.0, 
        "DMKKJpsi": 1093.677, 
        "DMKKMu": 713.677, 
        "DZBPV": 1.0, 
        "GEC_nLongTrk": 1000.0, 
        "HLT_FILTER": "", 
        "JpsiMassWindow": 80.0, 
        "K1MinIPChi2": 9.0, 
        "K1PIDK": 16.0, 
        "K1PT": 500.0, 
        "K1PTLoose": 250.0, 
        "K2MinIPChi2": 9.0, 
        "K2P": 3000.0, 
        "K2PIDK": 0.0, 
        "K2PT": 1000.0, 
        "KJpsiFdChi2": 25.0, 
        "KJpsiMassMax": 5500.0, 
        "KJpsiMassMin": 5050.0, 
        "KJpsiVChi2Dof": 4.0, 
        "KMuFdChi2": 100.0, 
        "KMuMassMax": 5500.0, 
        "KMuMassMin": 1800.0, 
        "KMuVChi2Dof": 4.0, 
        "MuMinIPChi2": 9.0, 
        "MuP": 3000.0, 
        "MuPIDmu": 0.0, 
        "MuPT": 1000.0, 
        "RelatedInfoTools": [
            {
                "DaughterLocations": {
                    "[B*_s20 -> (B+ -> K+ [mu-]cc) ^[K-]cc]CC": "K1ISO", 
                    "[B*_s20 -> (B+ -> K+ ^[mu-]cc) [K-]cc]CC": "MuISO", 
                    "[B*_s20 -> (B+ -> ^K+ [mu-]cc) [K-]cc]CC": "K2ISO"
                }, 
                "IsoTwoBody": True, 
                "Type": "RelInfoBs2MuMuTrackIsolations", 
                "Variables": [
                    "BSMUMUTRACKPLUSISO", 
                    "BSMUMUTRACKPLUSISOTWO", 
                    "ISOTWOBODYMASSISOPLUS", 
                    "ISOTWOBODYCHI2ISOPLUS", 
                    "ISOTWOBODYISO5PLUS", 
                    "BSMUMUTRACKID"
                ], 
                "angle": 0.27, 
                "doca_iso": 0.13, 
                "fc": 0.6, 
                "ips": 3.0, 
                "makeTrackCuts": False, 
                "pvdis": 0.5, 
                "pvdis_h": 40.0, 
                "svdis": -0.15, 
                "svdis_h": 30.0, 
                "tracktype": 3
            }
        ]
    }, 
    "STREAMS": [ "Semileptonic" ], 
    "WGs": [ "RD" ]
}

D23Mu = {
    "BUILDERTYPE": "D23MuLinesConf", 
    "CONFIG": {
        "CommonRelInfoTools": [
            {
                "Location": "VtxIsoInfo", 
                "Type": "RelInfoVertexIsolation"
            }, 
            {
                "Location": "VtxIsoInfoBDT", 
                "Type": "RelInfoVertexIsolationBDT"
            }, 
            {
                "Location": "BsMuMuBIsolation", 
                "Type": "RelInfoBs2MuMuBIsolations", 
                "Variables": [], 
                "makeTrackCuts": False, 
                "tracktype": 3
            }
        ], 
        "D23MuPrescale": 1, 
        "D23PiPrescale": 0.01, 
        "D2MueePrescale": 1, 
        "MaxDoca": 0.3, 
        "MaxIPChi2": 25, 
        "MaxTrChi2Dof": 4.0, 
        "MaxTrGhp": 0.4, 
        "MaxVtxChi2Dof": 9, 
        "MinDira": 0.0, 
        "MinTau": 0.1, 
        "MinTrIPChi2": 25.0, 
        "MinVDChi2": 225, 
        "Postscale": 1, 
        "mDiffDLoose": 150, 
        "mDiffDTight": 150
    }, 
    "STREAMS": [ "Leptonic" ], 
    "WGs": [ "RD" ]
}

LFV = {
    "BUILDERTYPE": "LFVLinesConf", 
    "CONFIG": {
        "B2TauMuPrescale": 0, 
        "B2eMuPrescale": 1, 
        "B2eePrescale": 1, 
        "B2hTauMuPrescale": 0, 
        "B2heMuPrescale": 1, 
        "B2pMuPrescale": 0, 
        "Bu2KJPsieePrescale": 1, 
        "Postscale": 1, 
        "RelatedInfoTools_B2eMu": [
            {
                "Location": "BSMUMUVARIABLES", 
                "Type": "RelInfoBs2MuMuBIsolations", 
                "Variables": [
                    "BSMUMUCDFISO", 
                    "BSMUMUOTHERBMAG", 
                    "BSMUMUOTHERBANGLE", 
                    "BSMUMUOTHERBBOOSTMAG", 
                    "BSMUMUOTHERBBOOSTANGLE", 
                    "BSMUMUOTHERBTRACKS"
                ], 
                "makeTrackCuts": False, 
                "tracktype": 3
            }, 
            {
                "DaughterLocations": {
                    "[B_s0 -> ^e+ [mu-]cc]CC": "Electron_ISO", 
                    "[B_s0 -> e+ ^[mu-]cc]CC": "Muon_ISO"
                }, 
                "IsoTwoBody": True, 
                "Type": "RelInfoBs2MuMuTrackIsolations", 
                "Variables": [
                    "BSMUMUTRACKPLUSISO", 
                    "BSMUMUTRACKPLUSISOTWO", 
                    "ISOTWOBODYQPLUS", 
                    "ISOTWOBODYMASSISOPLUS", 
                    "ISOTWOBODYCHI2ISOPLUS", 
                    "ISOTWOBODYISO5PLUS"
                ], 
                "angle": 0.27, 
                "doca_iso": 0.13, 
                "fc": 0.6, 
                "ips": 3.0, 
                "makeTrackCuts": False, 
                "pvdis": 0.5, 
                "pvdis_h": 40.0, 
                "svdis": -0.15, 
                "svdis_h": 30.0, 
                "tracktype": 3
            }, 
            {
                "Location": "VtxIsoInfo", 
                "Type": "RelInfoVertexIsolation"
            }, 
            {
                "Location": "ConeIsoInfo", 
                "Type": "RelInfoConeVariables"
            }, 
            {
                "Location": "VtxIsoInfoBDT", 
                "Type": "RelInfoVertexIsolationBDT"
            }, 
            {
                "Location": "ConeIsoInfoBDT", 
                "Type": "RelInfoTrackIsolationBDT"
            }
        ], 
        "RelatedInfoTools_B2ee": [
            {
                "Location": "BSMUMUVARIABLES", 
                "Type": "RelInfoBs2MuMuBIsolations", 
                "Variables": [
                    "BSMUMUCDFISO", 
                    "BSMUMUOTHERBMAG", 
                    "BSMUMUOTHERBANGLE", 
                    "BSMUMUOTHERBBOOSTMAG", 
                    "BSMUMUOTHERBBOOSTANGLE", 
                    "BSMUMUOTHERBTRACKS"
                ], 
                "makeTrackCuts": False, 
                "tracktype": 3
            }, 
            {
                "DaughterLocations": {
                    "[B_s0 -> ^e+ [e-]cc]CC": "Electron1_ISO", 
                    "[B_s0 -> e+ ^[e-]cc]CC": "Electron2_ISO"
                }, 
                "IsoTwoBody": True, 
                "Type": "RelInfoBs2MuMuTrackIsolations", 
                "Variables": [
                    "BSMUMUTRACKPLUSISO", 
                    "BSMUMUTRACKPLUSISOTWO", 
                    "ISOTWOBODYQPLUS", 
                    "ISOTWOBODYMASSISOPLUS", 
                    "ISOTWOBODYCHI2ISOPLUS", 
                    "ISOTWOBODYISO5PLUS"
                ], 
                "angle": 0.27, 
                "doca_iso": 0.13, 
                "fc": 0.6, 
                "ips": 3.0, 
                "makeTrackCuts": False, 
                "pvdis": 0.5, 
                "pvdis_h": 40.0, 
                "svdis": -0.15, 
                "svdis_h": 30.0, 
                "tracktype": 3
            }, 
            {
                "Location": "VtxIsoInfo", 
                "Type": "RelInfoVertexIsolation"
            }, 
            {
                "Location": "ConeIsoInfo", 
                "Type": "RelInfoConeVariables"
            }, 
            {
                "Location": "VtxIsoInfoBDT", 
                "Type": "RelInfoVertexIsolationBDT"
            }, 
            {
                "Location": "ConeIsoInfoBDT", 
                "Type": "RelInfoTrackIsolationBDT"
            }
        ], 
        "RelatedInfoTools_Bu2KJPsiee": [
            {
                "DaughterLocations": {
                    "[B+ -> ^(J/psi(1S) -> e+ e-) K+]CC": "Jpsi_ISO"
                }, 
                "Location": "BSMUMUVARIABLES", 
                "Type": "RelInfoBs2MuMuBIsolations", 
                "Variables": [
                    "BSMUMUCDFISO", 
                    "BSMUMUOTHERBMAG", 
                    "BSMUMUOTHERBANGLE", 
                    "BSMUMUOTHERBBOOSTMAG", 
                    "BSMUMUOTHERBBOOSTANGLE", 
                    "BSMUMUOTHERBTRACKS"
                ], 
                "makeTrackCuts": False, 
                "tracktype": 3
            }, 
            {
                "DaughterLocations": {
                    "[B+ -> (J/psi(1S) -> ^e+ e-) K+]CC": "Electron1_ISO", 
                    "[B+ -> (J/psi(1S) -> e+ ^e-) K+]CC": "Electron2_ISO", 
                    "[B+ -> (J/psi(1S) -> e+ e-) ^K+]CC": "Kplus_ISO"
                }, 
                "IsoTwoBody": True, 
                "Type": "RelInfoBs2MuMuTrackIsolations", 
                "Variables": [
                    "BSMUMUTRACKPLUSISO", 
                    "BSMUMUTRACKPLUSISOTWO", 
                    "ISOTWOBODYQPLUS", 
                    "ISOTWOBODYMASSISOPLUS", 
                    "ISOTWOBODYCHI2ISOPLUS", 
                    "ISOTWOBODYISO5PLUS"
                ], 
                "angle": 0.27, 
                "doca_iso": 0.13, 
                "fc": 0.6, 
                "ips": 3.0, 
                "makeTrackCuts": False, 
                "pvdis": 0.5, 
                "pvdis_h": 40.0, 
                "svdis": -0.15, 
                "svdis_h": 30.0, 
                "tracktype": 3
            }, 
            {
                "Location": "VtxIsoInfo", 
                "Type": "RelInfoVertexIsolation"
            }, 
            {
                "Location": "ConeIsoInfo", 
                "Type": "RelInfoConeVariables"
            }, 
            {
                "Location": "VtxIsoInfoBDT", 
                "Type": "RelInfoVertexIsolationBDT"
            }, 
            {
                "Location": "ConeIsoInfoBDT", 
                "Type": "RelInfoTrackIsolationBDT"
            }
        ], 
        "RelatedInfoTools_Tau2MuEtaPrime": [
            {
                "Location": "VtxIsoInfo", 
                "Type": "RelInfoVertexIsolation"
            }, 
            {
                "Location": "VtxIsoInfoBDT", 
                "Type": "RelInfoVertexIsolationBDT"
            }
        ], 
        "RelatedInfoTools_Tau2PhiMu": [
            {
                "ConeAngle": 0.5, 
                "DaughterLocations": {
                    "[tau+ -> (phi(1020)->K+ K-) ^mu+]CC": "coneInfoMu05", 
                    "[tau+ -> (phi(1020)->K+ ^K-) mu+]CC": "coneInfoKminus05", 
                    "[tau+ -> (phi(1020)->^K+ K-) mu+]CC": "coneInfoKplus05", 
                    "[tau+ -> ^(phi(1020)->K+ K-) mu+]CC": "coneInfoPhi05"
                }, 
                "Location": "coneInfoTau05", 
                "Type": "RelInfoConeVariables", 
                "Variables": [
                    "CONEANGLE", 
                    "CONEMULT", 
                    "CONEPT", 
                    "CONEPTASYM"
                ]
            }, 
            {
                "ConeAngle": 0.8, 
                "DaughterLocations": {
                    "[tau+ -> (phi(1020)->K+ K-) ^mu+]CC": "coneInfoMu08", 
                    "[tau+ -> (phi(1020)->K+ ^K-) mu+]CC": "coneInfoKminus08", 
                    "[tau+ -> (phi(1020)->^K+ K-) mu+]CC": "coneInfoKplus08", 
                    "[tau+ -> ^(phi(1020)->K+ K-) mu+]CC": "coneInfoPhi08"
                }, 
                "Location": "coneInfoTau08", 
                "Type": "RelInfoConeVariables", 
                "Variables": [
                    "CONEANGLE", 
                    "CONEMULT", 
                    "CONEPT", 
                    "CONEPTASYM"
                ]
            }, 
            {
                "ConeAngle": 1.0, 
                "DaughterLocations": {
                    "[tau+ -> (phi(1020)->K+ K-) ^mu+]CC": "coneInfoMu10", 
                    "[tau+ -> (phi(1020)->K+ ^K-) mu+]CC": "coneInfoKminus10", 
                    "[tau+ -> (phi(1020)->^K+ K-) mu+]CC": "coneInfoKplus10", 
                    "[tau+ -> ^(phi(1020)->K+ K-) mu+]CC": "coneInfoPhi10"
                }, 
                "Location": "coneInfoTau10", 
                "Type": "RelInfoConeVariables", 
                "Variables": [
                    "CONEANGLE", 
                    "CONEMULT", 
                    "CONEPT", 
                    "CONEPTASYM"
                ]
            }, 
            {
                "ConeAngle": 1.2, 
                "DaughterLocations": {
                    "[tau+ -> (phi(1020)->K+ K-) ^mu+]CC": "coneInfoMu12", 
                    "[tau+ -> (phi(1020)->K+ ^K-) mu+]CC": "coneInfoKminus12", 
                    "[tau+ -> (phi(1020)->^K+ K-) mu+]CC": "coneInfoKplus12", 
                    "[tau+ -> ^(phi(1020)->K+ K-) mu+]CC": "coneInfoPhi12"
                }, 
                "Location": "coneInfoTau12", 
                "Type": "RelInfoConeVariables", 
                "Variables": [
                    "CONEANGLE", 
                    "CONEMULT", 
                    "CONEPT", 
                    "CONEPTASYM"
                ]
            }, 
            {
                "Location": "VtxIsoInfo", 
                "Type": "RelInfoVertexIsolation", 
                "Variables": [
                    "VTXISONUMVTX", 
                    "VTXISODCHI2ONETRACK", 
                    "VTXISODCHI2MASSONETRACK", 
                    "VTXISODCHI2TWOTRACK", 
                    "VTXISODCHI2MASSTWOTRACK"
                ]
            }, 
            {
                "DaughterLocations": {
                    "[tau+ -> (phi(1020)->K+ K-) ^mu+]CC": "MuonTrackIsoBDTInfo", 
                    "[tau+ -> (phi(1020)->K+ ^K-) mu+]CC": "KminusTrackIsoBDTInfo", 
                    "[tau+ -> (phi(1020)->^K+ K-) mu+]CC": "KplusTrackIsoBDTInfo"
                }, 
                "Type": "RelInfoTrackIsolationBDT"
            }
        ], 
        "Tau2MuEtaPrimePrescale": 1, 
        "Tau2MuMuePrescale": 0, 
        "TauPrescale": 0, 
        "config_B2eMu": {
            "max_ADAMASS": 1200.0, 
            "max_AMAXDOCA": 0.3, 
            "max_BPVIPCHI2": 25, 
            "max_TRCHI2DV": 3.0, 
            "max_TRGHOSTPROB": 0.3, 
            "min_BPVDIRA": 0, 
            "min_BPVVDCHI2": 225, 
            "min_MIPCHI2DV": 25.0
        }, 
        "config_Tau2MuEtaPrime": {
            "config_EtaPrime2pipigamma": {
                "etap_cuts": "(PT > 500*MeV) & (VFASPF(VCHI2/VDOF) < 6.0)", 
                "etap_mass_window": "(ADAMASS('eta') < 80*MeV) | (ADAMASS('eta_prime') < 80*MeV)", 
                "gamma_cuts": "(PT > 300*MeV) & (CL > 0.1)", 
                "piminus_cuts": "(PROBNNpi > 0.1) & (PT > 250*MeV) & (TRGHOSTPROB < 0.3) & (TRCHI2DOF < 3.0) & (MIPCHI2DV(PRIMARY) > 9.)", 
                "piplus_cuts": "(PROBNNpi > 0.1) & (PT > 250*MeV) & (TRGHOSTPROB < 0.3) & (TRCHI2DOF < 3.0) & (MIPCHI2DV(PRIMARY) > 9.)"
            }, 
            "config_EtaPrime2pipipi": {
                "etap_cuts": "(PT > 500*MeV) & (VFASPF(VCHI2/VDOF) < 6.0)", 
                "etap_mass_window": "(ADAMASS('eta') < 80*MeV) | (ADAMASS('eta_prime') < 80*MeV)", 
                "pi0_cuts": "(PT > 250*MeV)", 
                "piminus_cuts": "(PROBNNpi > 0.1) & (PT > 250*MeV) & (TRGHOSTPROB < 0.3) & (TRCHI2DOF < 3.0) & (MIPCHI2DV(PRIMARY) > 9.)", 
                "piplus_cuts": "(PROBNNpi > 0.1) & (PT > 250*MeV) & (TRGHOSTPROB < 0.3) & (TRCHI2DOF < 3.0) & (MIPCHI2DV(PRIMARY) > 9.)"
            }, 
            "muplus_cuts": "(ISLONG) & (TRCHI2DOF < 3 )  & (MIPCHI2DV(PRIMARY) >  9.) & (PT > 300*MeV) & (TRGHOSTPROB < 0.3)", 
            "tau_cuts": "(BPVIPCHI2()< 100) & (VFASPF(VCHI2/VDOF)<6.) & (BPVLTIME()*c_light > 50.*micrometer) & (BPVLTIME()*c_light < 400.*micrometer) & (PT>500*MeV) & (D2DVVD(2) < 80*micrometer)", 
            "tau_mass_window": "(ADAMASS('tau+')<150*MeV)"
        }
    }, 
    "STREAMS": [ "Leptonic" ], 
    "WGs": [ "RD" ]
}

Lb2L0Gamma = {
    "BUILDERTYPE": "StrippingLb2L0GammaConf", 
    "CONFIG": {
        "HLT1": [], 
        "HLT2": [], 
        "L0": [
            "Photon", 
            "Electron", 
            "Hadron"
        ], 
        "L0_Conv": [], 
        "Lambda0DD_MassWindow": 30.0, 
        "Lambda0LL_IP_Min": 0.05, 
        "Lambda0LL_MassWindow": 20.0, 
        "Lambda0_Pt_Min": 1000.0, 
        "Lambda0_VtxChi2_Max": 9.0, 
        "Lambdab_IPChi2_Max": 25.0, 
        "Lambdab_MTDOCAChi2_Max": 7.0, 
        "Lambdab_MassWindow": 1100.0, 
        "Lambdab_Pt_Min": 1000.0, 
        "Lambdab_SumPt_Min": 5000.0, 
        "Lambdab_VtxChi2_Max": 9.0, 
        "Lb2L0GammaConvertedPrescale": 1.0, 
        "Lb2L0GammaPrescale": 1.0, 
        "PhotonCnv_MM_Max": 100.0, 
        "PhotonCnv_PT_Min": 1000.0, 
        "PhotonCnv_VtxChi2_Max": 9.0, 
        "Photon_CL_Min": 0.2, 
        "Photon_PT_Min": 2500.0, 
        "Pion_P_Min": 2000.0, 
        "Pion_Pt_Min": 300.0, 
        "Proton_P_Min": 7000.0, 
        "Proton_Pt_Min": 800.0, 
        "TrackLL_IPChi2_Min": 16.0, 
        "Track_Chi2ndf_Max": 3.0, 
        "Track_GhostProb_Max": 0.4, 
        "Track_MinChi2ndf_Max": 2.0
    }, 
    "STREAMS": [ "Leptonic" ], 
    "WGs": [ "RD" ]
}

Lc23Mu = {
    "BUILDERTYPE": "Lc23MuLinesConf", 
    "CONFIG": {
        "CommonRelInfoTools": [
            {
                "Location": "VtxIsoInfo", 
                "Type": "RelInfoVertexIsolation"
            }, 
            {
                "Location": "VtxIsoInfoBDT", 
                "Type": "RelInfoVertexIsolationBDT"
            }, 
            {
                "Location": "BsMuMuBIsolation", 
                "Type": "RelInfoBs2MuMuBIsolations", 
                "Variables": [], 
                "makeTrackCuts": False, 
                "tracktype": 3
            }
        ], 
        "Lc23muPrescale": 1, 
        "Lc2mueePrescale": 1, 
        "Lc2pKpiPrescale": 0.01, 
        "Lc2peePrescale": 1, 
        "Lc2pmumuPrescale": 1, 
        "MaxDoca": 0.3, 
        "MaxIPChi2": 100, 
        "MaxTrChi2Dof": 4.0, 
        "MaxTrGhp": 0.4, 
        "MaxVtxChi2": 15, 
        "MinTrIPChi2": 9, 
        "MinTrPT": 300, 
        "MinVD": 70, 
        "Postscale": 1, 
        "mDiffLcLoose": 200, 
        "mDiffLcTight": 150
    }, 
    "STREAMS": [ "Leptonic" ], 
    "WGs": [ "RD" ]
}

RareStrange = {
    "BUILDERTYPE": "RareStrangeLinesConf", 
    "CONFIG": {
        "DiElectronMaxMass": 1000, 
        "DiElectronMinIpChi2": 9, 
        "DiElectronPIDe": 2, 
        "DiElectronVtxChi2": 36, 
        "DiMuonMaxDOCA": 2, 
        "DiMuonMaxMass": 700, 
        "DiMuonMinIpChi2": 9, 
        "DiMuonMinPt": 0, 
        "DiMuonVtxChi2": 36, 
        "KDauMinIpChi2": 9.0, 
        "KDauMinIpChi2Down": 5.0, 
        "KDauMinIpChi2MassMeas": 8.0, 
        "KDauMinIpChi2MassMeasDown": 4.0, 
        "KDauTrChi2": 3.0, 
        "KMassWin": 100.0, 
        "KMassWinDown": 100.0, 
        "KMassWinMassMeas": 50.0, 
        "KMassWinMassMeasDown": 100.0, 
        "KMaxDOCA": 3.0, 
        "KMaxDOCADown": 10.0, 
        "KMaxDOCAMassMeas": 2.0, 
        "KMaxDOCAMassMeasDown": 2.0, 
        "KMaxIpChi2": 25.0, 
        "KMaxIpChi2MassMeas": 25.0, 
        "KMinDIRA": 0.98, 
        "KMinDIRADown": 0.98, 
        "KMinDIRAMassMeas": 0.9998, 
        "KMinDIRAMassMeasDown": 0.999, 
        "KMinPT": 100.0, 
        "KMinPTDown": 0.0, 
        "KMinPTMassMeas": 300.0, 
        "KMinPTMassMeasDown": 250.0, 
        "KMinVDChi2": 36.0, 
        "KMinVDChi2Down": 49.0, 
        "KMinVDChi2MassMeas": 100.0, 
        "KMinVDChi2MassMeasDown": 64.0, 
        "KPiMuMuDownPrescale": 1, 
        "KPiMuMuLFVDownPrescale": 1, 
        "KPiMuMuLFVPrescale": 1, 
        "KPiMuMuPrescale": 1, 
        "KPiPiPiDownPrescale": 0.1, 
        "KPiPiPiMassMeasDownPrescale": 1, 
        "KPiPiPiMassMeasPrescale": 1, 
        "KPiPiPiPrescale": 0.01, 
        "KVDPVMaxDown": 2500.0, 
        "KVDPVMaxMassMeasDown": 2200.0, 
        "KVDPVMinDown": 500.0, 
        "KVDPVMinMassMeasDown": 900.0, 
        "KVtxChi2": 25.0, 
        "KVtxChi2Down": 25.0, 
        "KVtxChi2MassMeas": 10.0, 
        "KVtxChi2MassMeasDown": 20.0, 
        "LambdaMassWin": 500.0, 
        "LambdaMassWinTight": 50.0, 
        "LambdaMaxDOCA": 2.0, 
        "LambdaMaxIpChi2": 36.0, 
        "LambdaMinDIRA": 0.9, 
        "LambdaMinPt": 500.0, 
        "LambdaMinTauPs": 6.0, 
        "LambdaPPiEEPrescale": 1.0, 
        "LambdaPPiPrescale": 0.01, 
        "LambdaVtxChi2": 25.0, 
        "PhiDauMinPT": 400.0, 
        "PhiKMuPrescale": 0.01, 
        "PhiMassMax": 1200, 
        "PhiMassMin": 800, 
        "PhiMaxDOCA": 0.1, 
        "PhiMinDIRA": 0.5, 
        "PhiMinPT": 700, 
        "PhiProbNNk": 0.3, 
        "PhiVtxChi2": 9, 
        "Postscale": 1, 
        "Sigma3DauTrChi2Down": 9.0, 
        "Sigma3MassWin": 500.0, 
        "Sigma3MassWinDown": 500.0, 
        "Sigma3MaxDOCA": 2.0, 
        "Sigma3MaxDOCADown": 2.0, 
        "Sigma3MaxIpChi2": 36.0, 
        "Sigma3MaxIpChi2Down": 100.0, 
        "Sigma3MinDIRA": 0.9, 
        "Sigma3MinDIRADown": 0.1, 
        "Sigma3MinPt": 0.0, 
        "Sigma3MinPtDown": 0.0, 
        "Sigma3MinTauPs": 3, 
        "Sigma3MinTauPsDown": 2, 
        "Sigma3VtxChi2": 36.0, 
        "Sigma3VtxChi2Down": 100.0, 
        "SigmaDauTrChi2Down": 9.0, 
        "SigmaDetVtxChi2": 25, 
        "SigmaMassWin": 500.0, 
        "SigmaMassWinDown": 500.0, 
        "SigmaMaxDOCA": 2.0, 
        "SigmaMaxDOCADown": 10.0, 
        "SigmaMaxIpChi2": 36.0, 
        "SigmaMaxIpChi2Down": 25.0, 
        "SigmaMinDIRA": 0.9, 
        "SigmaMinDIRADown": 0.9, 
        "SigmaMinPt": 500.0, 
        "SigmaMinPtDown": 0.0, 
        "SigmaMinTauPs": 6.0, 
        "SigmaMinTauPsDown": 7.0, 
        "SigmaMuMuMuDownPrescale": 1, 
        "SigmaMuMuMuPrescale": 1, 
        "SigmaPEEDetPrescale": 1, 
        "SigmaPEEDownPrescale": 0.1, 
        "SigmaPEEMassWinDown": 100.0, 
        "SigmaPEEPrescale": 1, 
        "SigmaPEMuPrescale": 1, 
        "SigmaPMuMuDetPrescale": 1, 
        "SigmaPMuMuDownPrescale": 1, 
        "SigmaPMuMuLFVDownPrescale": 0.1, 
        "SigmaPMuMuLFVPrescale": 1, 
        "SigmaPMuMuPrescale": 1, 
        "SigmaPPi0CalPrescale": 1.0, 
        "SigmaPPi0MassWin": 150.0, 
        "SigmaPPi0Prescale": 1, 
        "SigmaVtxChi2": 36.0, 
        "SigmaVtxChi2Down": 25.0, 
        "electronMinIpChi2": 9.0, 
        "electronMinIpChi2Down": 4.0, 
        "electronPIDe": 2.0, 
        "muon3MinIpChi2": 5.0, 
        "muon3MinIpChi2Down": 5.0, 
        "muonMinIpChi2": 9.0, 
        "muonMinIpChi2Down": 9.0, 
        "pMinPt": 500.0, 
        "pi0MinPt": 700.0, 
        "pionMinIpChi2": 9.0, 
        "protonMinIpChi2": 9.0, 
        "protonProbNNp": 0.05, 
        "protonProbNNpTight": 0.5
    }, 
    "STREAMS": [ "Leptonic" ], 
    "WGs": [ "RD" ]
}

RnS = {
    "BUILDERTYPE": "RnSConf", 
    "CONFIG": {
        "DiDOCA": 0.3, 
        "IP": 0.5, 
        "IPChi2Max": 1500, 
        "IPChi2Min": 1.5, 
        "K0s2mmLinePostscale": 1, 
        "K0s2mmLinePrescale": 1, 
        "K0s2mmSBCut": 465, 
        "K0s2mmSBLinePostscale": 1, 
        "K0s2mmSBLinePrescale": 0.1, 
        "KSdira": 0, 
        "KSip": 0.9, 
        "KSlife": 1.610411922, 
        "KSsidebmaxMass": 1000, 
        "KSsidebminMass": 600, 
        "KSsignalmaxMass": 600, 
        "KSsignalminMass": 300, 
        "L0life": 4.734283679999999, 
        "MaxIpDistRatio": 0.016666666666666666, 
        "MaxMass": 450, 
        "MinBPVDira": 0, 
        "MinVDZ": 0, 
        "MultibodyChi2dof": 9, 
        "MultibodyIPChi2": 25, 
        "NoMuIDLinePostscale": 1, 
        "NoMuIDLinePrescale": 0.001, 
        "Rho": 4, 
        "SVZ": 650, 
        "SidebandLinePostscale": 1, 
        "SidebandLinePrescale": 0.2, 
        "SignalLinePostscale": 1, 
        "SignalLinePrescale": 1, 
        "TRACK_TRGHOSTPROB_MAX": 0.3, 
        "TTHits": -1, 
        "VertexChi2": 9, 
        "cosAngle": 0.999998, 
        "eIpChi2": 49, 
        "muIpChi2": 36, 
        "muTrChi2Dof": 5, 
        "piIpChi2": 100, 
        "protonIpChi2": 16
    }, 
    "STREAMS": [ "Dimuon" ], 
    "WGs": [ "RD" ]
}

B2LLXBDT = {
    "BUILDERTYPE": "B2LLXBDTConf", 
    "CONFIG": {
        "BComCuts": "(AM > 4.0*GeV) & (AM < 6.2*GeV)", 
        "BMomCuts": "(VFASPF(VCHI2/VDOF) < 25.) & (BPVDIRA> 0.999) & (BPVIPCHI2()<25) & (BPVDLS>0)", 
        "Bd2LLKstarXmlFile": "$TMVAWEIGHTSROOT/data/Bd2eeKstar_BDTG_v1r0.xml", 
        "Bd2eeKstarMVACut": "-0.98", 
        "Bd2mumuKstarMVACut": "-0.98", 
        "DiElectronCuts": "\n                           (HASVERTEX) & (VFASPF(VCHI2)<16) & (MM<5.0*GeV)\n                           & (INTREE( (ID=='e+') & (PT>200*MeV) & (BPVIPCHI2()>1.) & (PIDe>-2) & (TRGHOSTPROB<0.5) ))\n                           & (INTREE( (ID=='e-') & (PT>200*MeV) & (BPVIPCHI2()>1.) & (PIDe>-2) & (TRGHOSTPROB<0.5) ))\n                          ", 
        "DiMuonCuts": "\n                           (HASVERTEX) & (VFASPF(VCHI2)<16) & (MM<5.0*GeV)\n                           & (INTREE( (ID=='mu+') & (PT>200*MeV) & (BPVIPCHI2()>1.) & (TRGHOSTPROB<0.5) ))\n                           & (INTREE( (ID=='mu-') & (PT>200*MeV) & (BPVIPCHI2()>1.) & (TRGHOSTPROB<0.5) ))\n                          ", 
        "Kaon4BuCuts": "(PROBNNk > 0.3) & (PT > 800*MeV) & (TRGHOSTPROB<0.4) & (BPVIPCHI2()>9.)", 
        "KaonCuts": "(PROBNNk > 0.1)  & (PT > 300*MeV) & (TRGHOSTPROB<0.4)", 
        "KsDDCuts": "(ADMASS('KS0') < 30.*MeV) & (BPVDLS>5)", 
        "KsLLCuts": "(ADMASS('KS0') < 30.*MeV) & (BPVDLS>5)", 
        "KstarCuts": "(VFASPF(VCHI2/VDOF)<16) & (ADMASS('K*(892)0')< 300*MeV)", 
        "LambdaDDCuts": "(ADMASS('Lambda0') < 30.*MeV) & (BPVDLS>5)", 
        "LambdaLLCuts": "(ADMASS('Lambda0') < 30.*MeV) & (BPVDLS>5)", 
        "LambdastarComCuts": "(AM < 5.6*GeV)", 
        "LambdastarCuts": "(VFASPF(VCHI2) < 25.)", 
        "Lb2LLPKXmlFile": "$TMVAWEIGHTSROOT/data/Bd2eeKstar_BDTG_v1r0.xml", 
        "Lb2eePKMVACut": "-0.99", 
        "Lb2mumuPKMVACut": "-0.99", 
        "LbComCuts": "(AM > 4.0*GeV) & (AM < 6.6*GeV)", 
        "LbMomCuts": "(VFASPF(VCHI2/VDOF) < 25.) & (BPVDIRA> 0.999) & (BPVIPCHI2()<25) & (BPVDLS>0)", 
        "PhiCuts": "\n                          (MAXTREE(ABSID=='K+',TRGHOSTPROB) < 0.4)\n                          & (MM<1.05*GeV)\n                          & (PT> 800*MeV)\n                          & (MIPCHI2DV(PRIMARY) > 3.)\n                          & (VFASPF(VCHI2) < 9.)\n                          ", 
        "PionCuts": "(PROBNNpi > 0.2) & (PT > 250*MeV) & (TRGHOSTPROB<0.4)", 
        "ProtonCuts": "(PROBNNp > 0.1)  & (PT > 300*MeV) & (TRGHOSTPROB<0.4)", 
        "RelatedInfoTools": [
            {
                "Location": "VertexIsoInfo", 
                "Type": "RelInfoVertexIsolation"
            }, 
            {
                "Location": "VertexIsoBDTInfo", 
                "Type": "RelInfoVertexIsolationBDT"
            }, 
            {
                "ConeAngle": 0.5, 
                "DaughterLocations": {
                    "[Beauty ->  StableCharged (X0 ->  l+ ^l-)]CC": "TrackIsoInfoL2", 
                    "[Beauty ->  StableCharged (X0 -> ^l+  l-)]CC": "TrackIsoInfoL1", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  StableCharged ^StableCharged)]CC": "TrackIsoInfoL2", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 -> ^StableCharged  StableCharged)]CC": "TrackIsoInfoL1", 
                    "[Beauty -> (X0 ->  X+ ^X-) (X0 ->  StableCharged  StableCharged)]CC": "TrackIsoInfoH2", 
                    "[Beauty -> (X0 -> ^X+  X-) (X0 ->  StableCharged  StableCharged)]CC": "TrackIsoInfoH1", 
                    "[Beauty -> ^StableCharged (X0 ->  l+  l-)]CC": "TrackIsoInfoH"
                }, 
                "IgnoreUnmatchedDescriptors": True, 
                "Type": "RelInfoConeVariables"
            }, 
            {
                "ConeSize": 0.5, 
                "DaughterLocations": {
                    "[Beauty ->  StableCharged (X0 ->  l+ ^l-)]CC": "ConeIsoInfoL2", 
                    "[Beauty ->  StableCharged (X0 -> ^l+  l-)]CC": "ConeIsoInfoL1", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  StableCharged ^StableCharged)]CC": "ConeIsoInfoL2", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 -> ^StableCharged  StableCharged)]CC": "ConeIsoInfoL1", 
                    "[Beauty -> (X0 ->  X+ ^X-) (X0 ->  StableCharged  StableCharged)]CC": "ConeIsoInfoH2", 
                    "[Beauty -> (X0 -> ^X+  X-) (X0 ->  StableCharged  StableCharged)]CC": "ConeIsoInfoH1", 
                    "[Beauty -> ^StableCharged (X0 ->  l+  l-)]CC": "ConeIsoInfoH"
                }, 
                "IgnoreUnmatchedDescriptors": True, 
                "Type": "RelInfoConeIsolation"
            }, 
            {
                "DaughterLocations": {
                    "[Beauty ->  StableCharged (X0 ->  l+ ^l-)]CC": "TrackIsoBDTInfoL2", 
                    "[Beauty ->  StableCharged (X0 -> ^l+  l-)]CC": "TrackIsoBDTInfoL1", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  StableCharged ^StableCharged)]CC": "TrackIsoBDTInfoL2", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 -> ^StableCharged  StableCharged)]CC": "TrackIsoBDTInfoL1", 
                    "[Beauty -> (X0 ->  X+ ^X-) (X0 ->  StableCharged  StableCharged)]CC": "TrackIsoBDTInfoH2", 
                    "[Beauty -> (X0 -> ^X+  X-) (X0 ->  StableCharged  StableCharged)]CC": "TrackIsoBDTInfoH1", 
                    "[Beauty -> ^StableCharged (X0 ->  l+  l-)]CC": "TrackIsoBDTInfoH"
                }, 
                "IgnoreUnmatchedDescriptors": True, 
                "Type": "RelInfoTrackIsolationBDT"
            }, 
            {
                "DaughterLocations": {
                    "[Beauty ->  StableCharged (X0 ->  l+ ^l-)]CC": "TrackIsoBs2MMInfoL2", 
                    "[Beauty ->  StableCharged (X0 -> ^l+  l-)]CC": "TrackIsoBs2MMInfoL1", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  StableCharged ^StableCharged)]CC": "TrackIsoBs2MMInfoL2", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 -> ^StableCharged  StableCharged)]CC": "TrackIsoBs2MMInfoL1", 
                    "[Beauty -> (X0 ->  X+ ^X-) (X0 ->  StableCharged  StableCharged)]CC": "TrackIsoBs2MMInfoH2", 
                    "[Beauty -> (X0 -> ^X+  X-) (X0 ->  StableCharged  StableCharged)]CC": "TrackIsoBs2MMInfoH1", 
                    "[Beauty -> ^StableCharged (X0 ->  l+  l-)]CC": "TrackIsoBs2MMInfoH"
                }, 
                "IgnoreUnmatchedDescriptors": True, 
                "Type": "RelInfoBs2MuMuTrackIsolations"
            }
        ]
    }, 
    "STREAMS": [ "Leptonic" ], 
    "WGs": [ "RD" ]
}

Bu2LLK = {
    "BUILDERTYPE": "Bu2LLKConf", 
    "CONFIG": {
        "BDIRA": 0.9995, 
        "BFlightCHI2": 100, 
        "BIPCHI2": 25, 
        "BMassWindow": 1500, 
        "BMassWindowTau": 5000, 
        "BVertexCHI2": 9, 
        "Bu2eeLine2Prescale": 1, 
        "Bu2eeLine3Prescale": 1, 
        "Bu2eeLinePrescale": 1, 
        "Bu2meLinePrescale": 1, 
        "Bu2meSSLinePrescale": 1, 
        "Bu2mmLinePrescale": 1, 
        "Bu2mtLinePrescale": 1, 
        "Bu2mtSSLinePrescale": 1, 
        "DiHadronMass": 2600, 
        "DiLeptonFDCHI2": 16, 
        "DiLeptonIPCHI2": 0, 
        "DiLeptonPT": 0, 
        "K1_MassWindow_Hi": 6000, 
        "K1_MassWindow_Lo": 0, 
        "K1_SumIPChi2Had": 48.0, 
        "K1_SumPTHad": 800, 
        "K1_VtxChi2": 12, 
        "KaonIPCHI2": 9, 
        "KaonPT": 400, 
        "KstarPADOCACHI2": 30, 
        "KstarPMassWindow": 300, 
        "KstarPVertexCHI2": 25, 
        "LeptonIPCHI2": 9, 
        "LeptonPT": 300, 
        "PIDe": 0, 
        "RelatedInfoTools": [
            {
                "Location": "VertexIsoInfo", 
                "Type": "RelInfoVertexIsolation"
            }, 
            {
                "Location": "VertexIsoBDTInfo", 
                "Type": "RelInfoVertexIsolationBDT"
            }, 
            {
                "ConeAngle": 0.5, 
                "DaughterLocations": {
                    "[Beauty ->  StableCharged (X0 ->  l+ ^l+)]CC": "TrackIsoInfoL2", 
                    "[Beauty ->  StableCharged (X0 ->  l+ ^l-)]CC": "TrackIsoInfoL2", 
                    "[Beauty ->  StableCharged (X0 -> ^l+  l+)]CC": "TrackIsoInfoL1", 
                    "[Beauty ->  StableCharged (X0 -> ^l+  l-)]CC": "TrackIsoInfoL1", 
                    "[Beauty -> (X+ ->  X+  X+  X-) (X0 ->  l+ ^l-)]CC": "TrackIsoInfoL2", 
                    "[Beauty -> (X+ ->  X+  X+  X-) (X0 -> ^l+  l-)]CC": "TrackIsoInfoL1", 
                    "[Beauty -> (X+ ->  X+  X+ ^X-) (X0 ->  l+  l-)]CC": "TrackIsoInfoH3", 
                    "[Beauty -> (X+ ->  X+ (X0 ->  X+  X-)) (X0 ->  l+ ^l+)]CC": "TrackIsoInfoL2", 
                    "[Beauty -> (X+ ->  X+ (X0 ->  X+  X-)) (X0 ->  l+ ^l-)]CC": "TrackIsoInfoL2", 
                    "[Beauty -> (X+ ->  X+ (X0 ->  X+  X-)) (X0 -> ^l+  l+)]CC": "TrackIsoInfoL1", 
                    "[Beauty -> (X+ ->  X+ (X0 ->  X+  X-)) (X0 -> ^l+  l-)]CC": "TrackIsoInfoL1", 
                    "[Beauty -> (X+ ->  X+ (X0 ->  X+ ^X-)) (X0 ->  l+  l+)]CC": "TrackIsoInfoH3", 
                    "[Beauty -> (X+ ->  X+ (X0 ->  X+ ^X-)) (X0 ->  l+  l-)]CC": "TrackIsoInfoH3", 
                    "[Beauty -> (X+ ->  X+ (X0 -> ^X+  X-)) (X0 ->  l+  l+)]CC": "TrackIsoInfoH2", 
                    "[Beauty -> (X+ ->  X+ (X0 -> ^X+  X-)) (X0 ->  l+  l-)]CC": "TrackIsoInfoH2", 
                    "[Beauty -> (X+ ->  X+ ^X+  X-) (X0 ->  l+  l-)]CC": "TrackIsoInfoH2", 
                    "[Beauty -> (X+ -> ^X+  X+  X-) (X0 ->  l+  l-)]CC": "TrackIsoInfoH1", 
                    "[Beauty -> (X+ -> ^X+ (X0 ->  X+  X-)) (X0 ->  l+  l+)]CC": "TrackIsoInfoH1", 
                    "[Beauty -> (X+ -> ^X+ (X0 ->  X+  X-)) (X0 ->  l+  l-)]CC": "TrackIsoInfoH1", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  StableCharged ^StableCharged)]CC": "TrackIsoInfoL2", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  l+  (l+ ->  X+  X- ^X+))]CC": "TrackIsoInfoL23", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  l+  (l+ ->  X+ ^X-  X+))]CC": "TrackIsoInfoL22", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  l+  (l+ -> ^X+  X-  X+))]CC": "TrackIsoInfoL21", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  l+  (l- ->  X-  X- ^X+))]CC": "TrackIsoInfoL23", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  l+  (l- ->  X- ^X-  X+))]CC": "TrackIsoInfoL22", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  l+  (l- -> ^X-  X-  X+))]CC": "TrackIsoInfoL21", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  l+ ^(l+ ->  X+  X-  X+))]CC": "TrackIsoInfoL2", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  l+ ^(l- ->  X-  X-  X+))]CC": "TrackIsoInfoL2", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  l+ ^l+)]CC": "TrackIsoInfoL2", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 -> ^StableCharged  StableCharged)]CC": "TrackIsoInfoL1", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 -> ^l+  (l+ ->  X+  X-  X+))]CC": "TrackIsoInfoL1", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 -> ^l+  (l- ->  X-  X-  X+))]CC": "TrackIsoInfoL1", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 -> ^l+  l+)]CC": "TrackIsoInfoL1", 
                    "[Beauty -> (X0 ->  X+ ^X-) (X0 ->  StableCharged  StableCharged)]CC": "TrackIsoInfoH2", 
                    "[Beauty -> (X0 ->  X+ ^X-) (X0 ->  l+  (l+ ->  X+  X-  X+))]CC": "TrackIsoInfoH2", 
                    "[Beauty -> (X0 ->  X+ ^X-) (X0 ->  l+  (l- ->  X-  X-  X+))]CC": "TrackIsoInfoH2", 
                    "[Beauty -> (X0 ->  X+ ^X-) (X0 ->  l+  l+)]CC": "TrackIsoInfoH2", 
                    "[Beauty -> (X0 -> ^X+  X-) (X0 ->  StableCharged  StableCharged)]CC": "TrackIsoInfoH1", 
                    "[Beauty -> (X0 -> ^X+  X-) (X0 ->  l+  (l+ ->  X+  X-  X+))]CC": "TrackIsoInfoH1", 
                    "[Beauty -> (X0 -> ^X+  X-) (X0 ->  l+  (l- ->  X-  X-  X+))]CC": "TrackIsoInfoH1", 
                    "[Beauty -> (X0 -> ^X+  X-) (X0 ->  l+  l+)]CC": "TrackIsoInfoH1", 
                    "[Beauty -> ^StableCharged (X0 ->  l+  l+)]CC": "TrackIsoInfoH", 
                    "[Beauty -> ^StableCharged (X0 ->  l+  l-)]CC": "TrackIsoInfoH"
                }, 
                "IgnoreUnmatchedDescriptors": True, 
                "Type": "RelInfoConeVariables"
            }, 
            {
                "ConeSize": 0.5, 
                "DaughterLocations": {
                    "[Beauty ->  StableCharged (X0 ->  l+ ^l+)]CC": "ConeIsoInfoL2", 
                    "[Beauty ->  StableCharged (X0 ->  l+ ^l-)]CC": "ConeIsoInfoL2", 
                    "[Beauty ->  StableCharged (X0 -> ^l+  l+)]CC": "ConeIsoInfoL1", 
                    "[Beauty ->  StableCharged (X0 -> ^l+  l-)]CC": "ConeIsoInfoL1", 
                    "[Beauty -> (X+ ->  X+  X+  X-) (X0 ->  l+ ^l-)]CC": "ConeIsoInfoL2", 
                    "[Beauty -> (X+ ->  X+  X+  X-) (X0 -> ^l+  l-)]CC": "ConeIsoInfoL1", 
                    "[Beauty -> (X+ ->  X+  X+ ^X-) (X0 ->  l+  l-)]CC": "ConeIsoInfoH3", 
                    "[Beauty -> (X+ ->  X+ (X0 ->  X+  X-)) (X0 ->  l+ ^l+)]CC": "ConeIsoInfoL2", 
                    "[Beauty -> (X+ ->  X+ (X0 ->  X+  X-)) (X0 ->  l+ ^l-)]CC": "ConeIsoInfoL2", 
                    "[Beauty -> (X+ ->  X+ (X0 ->  X+  X-)) (X0 -> ^l+  l+)]CC": "ConeIsoInfoL1", 
                    "[Beauty -> (X+ ->  X+ (X0 ->  X+  X-)) (X0 -> ^l+  l-)]CC": "ConeIsoInfoL1", 
                    "[Beauty -> (X+ ->  X+ (X0 ->  X+ ^X-)) (X0 ->  l+  l+)]CC": "ConeIsoInfoH3", 
                    "[Beauty -> (X+ ->  X+ (X0 ->  X+ ^X-)) (X0 ->  l+  l-)]CC": "ConeIsoInfoH3", 
                    "[Beauty -> (X+ ->  X+ (X0 -> ^X+  X-)) (X0 ->  l+  l+)]CC": "ConeIsoInfoH2", 
                    "[Beauty -> (X+ ->  X+ (X0 -> ^X+  X-)) (X0 ->  l+  l-)]CC": "ConeIsoInfoH2", 
                    "[Beauty -> (X+ ->  X+ ^X+  X-) (X0 ->  l+  l-)]CC": "ConeIsoInfoH2", 
                    "[Beauty -> (X+ -> ^X+  X+  X-) (X0 ->  l+  l-)]CC": "ConeIsoInfoH1", 
                    "[Beauty -> (X+ -> ^X+ (X0 ->  X+  X-)) (X0 ->  l+  l+)]CC": "ConeIsoInfoH1", 
                    "[Beauty -> (X+ -> ^X+ (X0 ->  X+  X-)) (X0 ->  l+  l-)]CC": "ConeIsoInfoH1", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  StableCharged ^StableCharged)]CC": "ConeIsoInfoL2", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  l+  (l+ ->  X+  X- ^X+))]CC": "ConeIsoInfoL23", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  l+  (l+ ->  X+ ^X-  X+))]CC": "ConeIsoInfoL22", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  l+  (l+ -> ^X+  X-  X+))]CC": "ConeIsoInfoL21", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  l+  (l- ->  X-  X- ^X+))]CC": "ConeIsoInfoL23", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  l+  (l- ->  X- ^X-  X+))]CC": "ConeIsoInfoL22", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  l+  (l- -> ^X-  X-  X+))]CC": "ConeIsoInfoL21", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  l+ ^(l+ ->  X+  X-  X+))]CC": "ConeIsoInfoL2", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  l+ ^(l- ->  X-  X-  X+))]CC": "ConeIsoInfoL2", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  l+ ^l+)]CC": "ConeIsoInfoL2", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 -> ^StableCharged  StableCharged)]CC": "ConeIsoInfoL1", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 -> ^l+  (l+ ->  X+  X-  X+))]CC": "ConeIsoInfoL1", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 -> ^l+  (l- ->  X-  X-  X+))]CC": "ConeIsoInfoL1", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 -> ^l+  l+)]CC": "ConeIsoInfoL1", 
                    "[Beauty -> (X0 ->  X+ ^X-) (X0 ->  StableCharged  StableCharged)]CC": "ConeIsoInfoH2", 
                    "[Beauty -> (X0 ->  X+ ^X-) (X0 ->  l+  (l+ ->  X+  X-  X+))]CC": "ConeIsoInfoH2", 
                    "[Beauty -> (X0 ->  X+ ^X-) (X0 ->  l+  (l- ->  X-  X-  X+))]CC": "ConeIsoInfoH2", 
                    "[Beauty -> (X0 ->  X+ ^X-) (X0 ->  l+  l+)]CC": "ConeIsoInfoH2", 
                    "[Beauty -> (X0 -> ^X+  X-) (X0 ->  StableCharged  StableCharged)]CC": "ConeIsoInfoH1", 
                    "[Beauty -> (X0 -> ^X+  X-) (X0 ->  l+  (l+ ->  X+  X-  X+))]CC": "ConeIsoInfoH1", 
                    "[Beauty -> (X0 -> ^X+  X-) (X0 ->  l+  (l- ->  X-  X-  X+))]CC": "ConeIsoInfoH1", 
                    "[Beauty -> (X0 -> ^X+  X-) (X0 ->  l+  l+)]CC": "ConeIsoInfoH1", 
                    "[Beauty -> ^StableCharged (X0 ->  l+  l+)]CC": "ConeIsoInfoH", 
                    "[Beauty -> ^StableCharged (X0 ->  l+  l-)]CC": "ConeIsoInfoH"
                }, 
                "IgnoreUnmatchedDescriptors": True, 
                "Type": "RelInfoConeIsolation"
            }, 
            {
                "DaughterLocations": {
                    "[Beauty ->  StableCharged (X0 ->  l+ ^l+)]CC": "TrackIsoBDTInfoL2", 
                    "[Beauty ->  StableCharged (X0 ->  l+ ^l-)]CC": "TrackIsoBDTInfoL2", 
                    "[Beauty ->  StableCharged (X0 -> ^l+  l+)]CC": "TrackIsoBDTInfoL1", 
                    "[Beauty ->  StableCharged (X0 -> ^l+  l-)]CC": "TrackIsoBDTInfoL1", 
                    "[Beauty -> (X+ ->  X+  X+  X-) (X0 ->  l+ ^l-)]CC": "TrackIsoBDTInfoL2", 
                    "[Beauty -> (X+ ->  X+  X+  X-) (X0 -> ^l+  l-)]CC": "TrackIsoBDTInfoL1", 
                    "[Beauty -> (X+ ->  X+  X+ ^X-) (X0 ->  l+  l-)]CC": "TrackIsoBDTInfoH3", 
                    "[Beauty -> (X+ ->  X+ (X0 ->  X+  X-)) (X0 ->  l+ ^l+)]CC": "TrackIsoBDTInfoL2", 
                    "[Beauty -> (X+ ->  X+ (X0 ->  X+  X-)) (X0 ->  l+ ^l-)]CC": "TrackIsoBDTInfoL2", 
                    "[Beauty -> (X+ ->  X+ (X0 ->  X+  X-)) (X0 -> ^l+  l+)]CC": "TrackIsoBDTInfoL1", 
                    "[Beauty -> (X+ ->  X+ (X0 ->  X+  X-)) (X0 -> ^l+  l-)]CC": "TrackIsoBDTInfoL1", 
                    "[Beauty -> (X+ ->  X+ (X0 ->  X+ ^X-)) (X0 ->  l+  l+)]CC": "TrackIsoBDTInfoH3", 
                    "[Beauty -> (X+ ->  X+ (X0 ->  X+ ^X-)) (X0 ->  l+  l-)]CC": "TrackIsoBDTInfoH3", 
                    "[Beauty -> (X+ ->  X+ (X0 -> ^X+  X-)) (X0 ->  l+  l+)]CC": "TrackIsoBDTInfoH2", 
                    "[Beauty -> (X+ ->  X+ (X0 -> ^X+  X-)) (X0 ->  l+  l-)]CC": "TrackIsoBDTInfoH2", 
                    "[Beauty -> (X+ ->  X+ ^X+  X-) (X0 ->  l+  l-)]CC": "TrackIsoBDTInfoH2", 
                    "[Beauty -> (X+ -> ^X+  X+  X-) (X0 ->  l+  l-)]CC": "TrackIsoBDTInfoH1", 
                    "[Beauty -> (X+ -> ^X+ (X0 ->  X+  X-)) (X0 ->  l+  l+)]CC": "TrackIsoBDTInfoH1", 
                    "[Beauty -> (X+ -> ^X+ (X0 ->  X+  X-)) (X0 ->  l+  l-)]CC": "TrackIsoBDTInfoH1", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  StableCharged ^StableCharged)]CC": "TrackIsoBDTInfoL2", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  l+  (l+ ->  X+  X- ^X+))]CC": "TrackIsoBDTInfoL23", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  l+  (l+ ->  X+ ^X-  X+))]CC": "TrackIsoBDTInfoL22", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  l+  (l+ -> ^X+  X-  X+))]CC": "TrackIsoBDTInfoL21", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  l+  (l- ->  X-  X- ^X+))]CC": "TrackIsoBDTInfoL23", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  l+  (l- ->  X- ^X-  X+))]CC": "TrackIsoBDTInfoL22", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  l+  (l- -> ^X-  X-  X+))]CC": "TrackIsoBDTInfoL21", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  l+ ^(l+ ->  X+  X-  X+))]CC": "TrackIsoBDTInfoL2", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  l+ ^(l- ->  X-  X-  X+))]CC": "TrackIsoBDTInfoL2", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  l+ ^l+)]CC": "TrackIsoBDTInfoL2", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 -> ^StableCharged  StableCharged)]CC": "TrackIsoBDTInfoL1", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 -> ^l+  (l+ ->  X+  X-  X+))]CC": "TrackIsoBDTInfoL1", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 -> ^l+  (l- ->  X-  X-  X+))]CC": "TrackIsoBDTInfoL1", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 -> ^l+  l+)]CC": "TrackIsoBDTInfoL1", 
                    "[Beauty -> (X0 ->  X+ ^X-) (X0 ->  StableCharged  StableCharged)]CC": "TrackIsoBDTInfoH2", 
                    "[Beauty -> (X0 ->  X+ ^X-) (X0 ->  l+  (l+ ->  X+  X-  X+))]CC": "TrackIsoBDTInfoH2", 
                    "[Beauty -> (X0 ->  X+ ^X-) (X0 ->  l+  (l- ->  X-  X-  X+))]CC": "TrackIsoBDTInfoH2", 
                    "[Beauty -> (X0 ->  X+ ^X-) (X0 ->  l+  l+)]CC": "TrackIsoBDTInfoH2", 
                    "[Beauty -> (X0 -> ^X+  X-) (X0 ->  StableCharged  StableCharged)]CC": "TrackIsoBDTInfoH1", 
                    "[Beauty -> (X0 -> ^X+  X-) (X0 ->  l+  (l+ ->  X+  X-  X+))]CC": "TrackIsoBDTInfoH1", 
                    "[Beauty -> (X0 -> ^X+  X-) (X0 ->  l+  (l- ->  X-  X-  X+))]CC": "TrackIsoBDTInfoH1", 
                    "[Beauty -> (X0 -> ^X+  X-) (X0 ->  l+  l+)]CC": "TrackIsoBDTInfoH1", 
                    "[Beauty -> ^StableCharged (X0 ->  l+  l+)]CC": "TrackIsoBDTInfoH", 
                    "[Beauty -> ^StableCharged (X0 ->  l+  l-)]CC": "TrackIsoBDTInfoH"
                }, 
                "IgnoreUnmatchedDescriptors": True, 
                "Type": "RelInfoTrackIsolationBDT"
            }, 
            {
                "DaughterLocations": {
                    "[Beauty ->  StableCharged (X0 ->  l+ ^l+)]CC": "TrackIsoBs2MMInfoL2", 
                    "[Beauty ->  StableCharged (X0 ->  l+ ^l-)]CC": "TrackIsoBs2MMInfoL2", 
                    "[Beauty ->  StableCharged (X0 -> ^l+  l+)]CC": "TrackIsoBs2MMInfoL1", 
                    "[Beauty ->  StableCharged (X0 -> ^l+  l-)]CC": "TrackIsoBs2MMInfoL1", 
                    "[Beauty -> (X+ ->  X+  X+  X-) (X0 ->  l+ ^l-)]CC": "TrackIsoBs2MMInfoL2", 
                    "[Beauty -> (X+ ->  X+  X+  X-) (X0 -> ^l+  l-)]CC": "TrackIsoBs2MMInfoL1", 
                    "[Beauty -> (X+ ->  X+  X+ ^X-) (X0 ->  l+  l-)]CC": "TrackIsoBs2MMInfoH3", 
                    "[Beauty -> (X+ ->  X+ (X0 ->  X+  X-)) (X0 ->  l+ ^l+)]CC": "TrackIsoBs2MMInfoL2", 
                    "[Beauty -> (X+ ->  X+ (X0 ->  X+  X-)) (X0 ->  l+ ^l-)]CC": "TrackIsoBs2MMInfoL2", 
                    "[Beauty -> (X+ ->  X+ (X0 ->  X+  X-)) (X0 -> ^l+  l+)]CC": "TrackIsoBs2MMInfoL1", 
                    "[Beauty -> (X+ ->  X+ (X0 ->  X+  X-)) (X0 -> ^l+  l-)]CC": "TrackIsoBs2MMInfoL1", 
                    "[Beauty -> (X+ ->  X+ (X0 ->  X+ ^X-)) (X0 ->  l+  l+)]CC": "TrackIsoBs2MMInfoH3", 
                    "[Beauty -> (X+ ->  X+ (X0 ->  X+ ^X-)) (X0 ->  l+  l-)]CC": "TrackIsoBs2MMInfoH3", 
                    "[Beauty -> (X+ ->  X+ (X0 -> ^X+  X-)) (X0 ->  l+  l+)]CC": "TrackIsoBs2MMInfoH2", 
                    "[Beauty -> (X+ ->  X+ (X0 -> ^X+  X-)) (X0 ->  l+  l-)]CC": "TrackIsoBs2MMInfoH2", 
                    "[Beauty -> (X+ ->  X+ ^X+  X-) (X0 ->  l+  l-)]CC": "TrackIsoBs2MMInfoH2", 
                    "[Beauty -> (X+ -> ^X+  X+  X-) (X0 ->  l+  l-)]CC": "TrackIsoBs2MMInfoH1", 
                    "[Beauty -> (X+ -> ^X+ (X0 ->  X+  X-)) (X0 ->  l+  l+)]CC": "TrackIsoBs2MMInfoH1", 
                    "[Beauty -> (X+ -> ^X+ (X0 ->  X+  X-)) (X0 ->  l+  l-)]CC": "TrackIsoBs2MMInfoH1", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  StableCharged ^StableCharged)]CC": "TrackIsoBs2MMInfoL2", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  l+  (l+ ->  X+  X- ^X+))]CC": "TrackIsoBs2MMInfoL23", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  l+  (l+ ->  X+ ^X-  X+))]CC": "TrackIsoBs2MMInfoL22", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  l+  (l+ -> ^X+  X-  X+))]CC": "TrackIsoBs2MMInfoL21", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  l+  (l- ->  X-  X- ^X+))]CC": "TrackIsoBs2MMInfoL23", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  l+  (l- ->  X- ^X-  X+))]CC": "TrackIsoBs2MMInfoL22", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  l+  (l- -> ^X-  X-  X+))]CC": "TrackIsoBs2MMInfoL21", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  l+ ^(l+ ->  X+  X-  X+))]CC": "TrackIsoBs2MMInfoL2", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  l+ ^(l- ->  X-  X-  X+))]CC": "TrackIsoBs2MMInfoL2", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 ->  l+ ^l+)]CC": "TrackIsoBs2MMInfoL2", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 -> ^StableCharged  StableCharged)]CC": "TrackIsoBs2MMInfoL1", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 -> ^l+  (l+ ->  X+  X-  X+))]CC": "TrackIsoBs2MMInfoL1", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 -> ^l+  (l- ->  X-  X-  X+))]CC": "TrackIsoBs2MMInfoL1", 
                    "[Beauty -> (X0 ->  X+  X-) (X0 -> ^l+  l+)]CC": "TrackIsoBs2MMInfoL1", 
                    "[Beauty -> (X0 ->  X+ ^X-) (X0 ->  StableCharged  StableCharged)]CC": "TrackIsoBs2MMInfoH2", 
                    "[Beauty -> (X0 ->  X+ ^X-) (X0 ->  l+  (l+ ->  X+  X-  X+))]CC": "TrackIsoBs2MMInfoH2", 
                    "[Beauty -> (X0 ->  X+ ^X-) (X0 ->  l+  (l- ->  X-  X-  X+))]CC": "TrackIsoBs2MMInfoH2", 
                    "[Beauty -> (X0 ->  X+ ^X-) (X0 ->  l+  l+)]CC": "TrackIsoBs2MMInfoH2", 
                    "[Beauty -> (X0 -> ^X+  X-) (X0 ->  StableCharged  StableCharged)]CC": "TrackIsoBs2MMInfoH1", 
                    "[Beauty -> (X0 -> ^X+  X-) (X0 ->  l+  (l+ ->  X+  X-  X+))]CC": "TrackIsoBs2MMInfoH1", 
                    "[Beauty -> (X0 -> ^X+  X-) (X0 ->  l+  (l- ->  X-  X-  X+))]CC": "TrackIsoBs2MMInfoH1", 
                    "[Beauty -> (X0 -> ^X+  X-) (X0 ->  l+  l+)]CC": "TrackIsoBs2MMInfoH1", 
                    "[Beauty -> ^StableCharged (X0 ->  l+  l+)]CC": "TrackIsoBs2MMInfoH", 
                    "[Beauty -> ^StableCharged (X0 ->  l+  l-)]CC": "TrackIsoBs2MMInfoH"
                }, 
                "IgnoreUnmatchedDescriptors": True, 
                "Type": "RelInfoBs2MuMuTrackIsolations"
            }
        ], 
        "TauPT": 0, 
        "TauVCHI2DOF": 150, 
        "Trk_Chi2": 3, 
        "Trk_GhostProb": 0.4, 
        "UpperMass": 5500
    }, 
    "STREAMS": [ "Leptonic" ], 
    "WGs": [ "RD" ]
}

